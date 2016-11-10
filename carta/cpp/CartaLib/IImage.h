/***
 *
 * Generic image interface. Core and plugins rely on all images to have this API.
 *
 * The image interface should provide enough API to load and work with arbitrary
 * image types (e.g. retrieve pixel data, support pixel data in different formats,
 * access to WCS metadata information, access to other metadata)
 *
 * Notes:
 *
 * Currently inherits from QObject because I anticipate we may want to downcast it
 * inside plugins... This makes it not really an interface so we should probably
 * investigate if QObject is needed. If QObject is needed, maybe we can split this up
 * into a pure interface and a base class, but would there be an advantage?
 *
 */

/// @todo move everything here Carta::Lib namespace

#pragma once

#include "PixelType.h"
#include "Nullable.h"
#include "Slice.h"
#include "ICoordinateFormatter.h"
#include "IPlotLabelGenerator.h"
#include "Regions/ICoordSystem.h"
#include <QObject>
#include <functional>
#include <initializer_list>
#include <cstdint>
#include <memory>

namespace Carta
{
namespace Lib
{
/// description of a unit
/// this will hopefully evolve a lot...
/// for now it's essentially an alias for QString
class Unit
{
public:

    QString
    toStr() const { return m_string; }

    Unit( const QString & str = "" ) : m_string( str ) { }

protected:

    QString m_string;
};

/// classes related to n-dimensional views into n-dimensional arrays
namespace NdArray
{
/// \brief Interface for reading elements of multidimensional array (n-dimensional array)
/// for arbitrary types. The intentded use of this class is the lowest interface
/// needed to be implemented to read an array. A conversion adapter/mixin would be more
/// convenenient to actually access the arrays, see TypedView<> as an example
class RawViewInterface
{
    CLASS_BOILERPLATE( RawViewInterface );

public:

    typedef std::vector < int > VI;
    typedef Carta::Lib::Image::PixelType PixelType;

    /// traversal order
    enum class Traversal
    {
        Sequential, ///< sequential order (ie. row-major, or C-style)
        Optimal ///< whatever order that is most optimal
    };

    /// get the pixel type stored in this array accessor
    virtual PixelType
    pixelType() = 0;

    /// get the max. dimensions allowed in this accessor
    virtual const VI &
    dims() = 0;



    /// get the raw data for the given pixel (at coordinates 'pos')
    /// note: this is meant for random access, but is slow
    virtual const char *
    get( const VI & pos ) = 0;

    /// visit each element via function pointer
    /// \param func function that will be invoked on each data
    /// \param traversal determines the order in which the data is traversed
    /// \note this is faster than the get() random access method
    /// \todo investigate whether this is faster/slower than iterator approach?
    /// \todo investigate performance of std::function vs raw function pointer
    virtual void
    forEach( std::function < void (const char *) > func,
             Traversal traversal = Traversal::Sequential ) = 0;

    /// returns the current coordinate of the traversed element (for use with
    /// the forEach() method).
    /// The address of the returned reference will remain the same throughout the
    /// traversal, so for maximum efficiency you only need to call this once
    /// before the traversal starts and store the pointer (or reference)
    virtual const VI &
    currentPos() = 0;

    /// return a unique ID for this instance
    /// \todo motivation for this is a cheap comparison, but maybe we could implement
    /// a real comparison as well?
//    quint64 uniqueId() const;

    /// virtual destructor
    virtual
    ~RawViewInterface() { }

    /// \brief create a view into this view
    /// \param sliceInfo which view to get
    /// \return a new view
    /// \warning for efficiency reasons we are not doing shared pointers, etc... since the returned
    /// view needs to have pointer to the instance of the image interface, the image interface
    /// needs to remain valid while accessing the view! i.e. don't delete it
    /// \todo the above warning I think we should treat as a bug! I think we can do this safely
    /// with shared pointers while maintaining speed (look at qimage plugin)
    virtual RawViewInterface *
    getView( const SliceND & sliceInfo ) = 0;

    // ===-----------------------------------------------------------------------===
    // experimental APIs below, not yet finalized and definitely not yet implemented
    // Probably we'll only implement one of these, not all of them.
    // ===-----------------------------------------------------------------------===

    /// \brief High performance data accessor #1, motivated by unix's read().
    /// \param buffSize max number of bytes to read in
    /// \param buff result will be stored here
    /// \param traversal if sequential, c-order traversal is used, if optimal, the pixels
    /// will be stored in the buffer in some arbitrary order, but fast
    /// \return number of bytes that were placed into buffer, 0 indicates
    /// there were no more bytes to read
    ///
    virtual int64_t
    read( int64_t buffSize, char * buff,
          Traversal traversal = Traversal::Sequential ) = 0;

    /// reset the position for the next read()
    virtual void
    seek( int64_t ind = 0 ) = 0;

    /// Another High performance accessor to data, motivated by unix's read(), except
    /// it's stateless
    /// the view will have (width*height*pixel_size_in_bytes) bytes in them
    /// therefore there will be ceil(n_pix/buffSize) chunks
    virtual int64_t
    read( int64_t chunk, int64_t buffSize, char * buff,
          Traversal traversal = Traversal::Sequential ) = 0;

    /// yet another high performance accessor... similar to forEach above,
    /// but the supplied function gets called with multiple pixel data
    /// (however many fit into the buffer)
    ///
    /// I think I like this one the most.
    virtual void
    forEach( int64_t buffSize,
             std::function < void (const char *, int64_t count) > func,
             char * buff = nullptr,
             Traversal traversal = Traversal::Sequential ) = 0;
};

/// Utility class that wraps a raw view into a typed view.
template < typename Type >
class TypedView
{
    CLASS_BOILERPLATE( TypedView );

public:

    typedef std::vector < int > VI;

    /// \brief Construct a typed view from raw view.
    /// \details The raw view must exist for the duration of existance of the TypedView.
    /// \param [in] rawView pointer to the raw view
    /// \param [in] keepOwnership whether to delete the raw view when destructing this instance
    TypedView( RawViewInterface * rawView, bool keepOwnership = false )
        : m_rawView( rawView ), m_keepOwnership( keepOwnership )
    {
        Q_ASSERT( rawView != nullptr );

        // figure out which converter to use
        m_converterFunc = getConverter < Type > ( rawView->pixelType() );
    }

    /// get the max. dimensions allowed in this accessor
    const VI &
    dims()
    {
        return m_rawView-> dims();
    }

    /// extract the data and convert it to double
    /// \param pos position of the element to extract (in destination coordinates)
    /// \return the extracted value at the given position pos
    const Type &
    get( const VI & pos )
    {
        return m_converterFunc( m_rawView->get( pos ) );
    }

    /// equivalent to RawViewInterface::forEach but with a typed parameter
    /// \param func function to invoke on each element
    /// \param traversal order of traversal
    void
    forEach(
        std::function < void (const Type &) > func,
        RawViewInterface::Traversal traversal = RawViewInterface::Traversal::Sequential )
    {
        auto wrapper = [this, & func] ( const char * ptr )->void
        {
            func( m_converterFunc( ptr ) );
        };
        m_rawView->forEach( wrapper, traversal );
    }

    ~TypedView()
    {
        if ( m_keepOwnership ) {
            Q_ASSERT( m_rawView != nullptr );
            delete m_rawView;
        }
    }

    /// return the associated raw view
    RawViewInterface *
    rawView()
    {
        return m_rawView;
    }

protected:

    /// pointer to the raw view
    RawViewInterface * m_rawView;

    /// are we keeping ownership of m_rawView
    bool m_keepOwnership;

    /// classic c-style function pointer to the converter
    /// \todo is this faster than std::function?
    const Type & ( * m_converterFunc )(const char *);
};

/// convenience types
typedef TypedView < double > Double;
typedef TypedView < float > Float;
typedef TypedView < uint8_t > Byte;
typedef TypedView < int16_t > Int16;
typedef TypedView < int32_t > Int32;
typedef TypedView < int64_t > Int64;
} // namespace NDArray

namespace Image
{
/// \brief Interface description for accessing various metadata associated with an image.
/// For example: beam related information, miscellaneous records.
///
/// The idea is to present this as an algorithm provider, rather than some predefined
/// set of data.
///
/// \warning This class is still evolving.
class MetaDataInterface
{
    CLASS_BOILERPLATE( MetaDataInterface );

public:

    /// \todo we can remove this once we put this class into carta namespace
    typedef Carta::Lib::TextFormat TextFormat;

    /// clone yourself
    virtual MetaDataInterface *
    clone() = 0;

    /// create a coordinate formatter algorithm
    /// caller assumes ownership
    virtual CoordinateFormatterInterface::SharedPtr
    coordinateFormatter() = 0;

    /// get a grid plotter algorithm
    /// \warning this is probably going to get removed for a plugin-based solution
//    virtual CoordinateGridPlotterInterface::SharedPtr
//    coordinateGridPlotter() = 0;

    ///Return the rest frequency and units; an empty string and a negative value
    ///will be returned if the rest frequency cannot be found.
    virtual std::pair<double,QString> getRestFrequency() const = 0;

    /// get a labeler algorithm
    /// \note this is not being used anywhere, and maybe it won't be used ever
    virtual PlotLabelGeneratorInterface::SharedPtr
    plotLabelGenerator() = 0;

    /// title if any (eg. TITLE in FITS)
    virtual QString
    title( TextFormat format = TextFormat::Plain ) = 0;

    /// other data users might want to display
    /// (eg. complete FITS header)
    virtual QStringList
    otherInfo( TextFormat format = TextFormat::Plain ) = 0;

    virtual
    ~MetaDataInterface();

    /// experimental - return coordinate system converter for the image
    /// the converter's srcCS will be pixel coordinate system, and dstCS will be world
    /// coordinate system if it's there, otherwise it'll be also pixel
    /// \todo change this to pure virtual
    virtual Regions::ICoordSystemConverter::SharedPtr
    getCSConv() = 0;
};

/// Main interface class for representing an image inside the viewer. This is used to pass
/// around images between core and plugins. For example, a plugin that can load
/// an image would have to implement this interface.
class ImageInterface
{
    CLASS_BOILERPLATE( ImageInterface );

public:

    /// similar to BITPIX
    typedef Image::PixelType PixelType;
    typedef std::vector < int > VI;

    ImageInterface() { }

    /// virtual destructor to make sure we can delete arbitrary images
    virtual
    ~ImageInterface() { }

    /// get a unit of pixels (similar to BUNIT)
    virtual const Unit &
    getPixelUnit() const = 0;

    /// Return the image with the axes permuted in the order given by indices.
    /// Passing in [1,2,0] for an image with 3 axes would result in the axis map
    /// 0 -> 1
    /// 1 -> 2
    /// 2 -> 0
    /// That is, the second axis in this image would become the first axis in the
    /// returned image.
    virtual std::shared_ptr<Image::ImageInterface>
    getPermuted(const std::vector<int> & indices) = 0;

    /// return dimensions of the image
    virtual const VI &
    dims() const = 0;

    /// Used to determine the type of the image (raster, contour, vector).
    /// May need to evolve.
    /// \todo I am not sure if this belongs here... This interface has been
    /// designed for nd arrays, most methods here would probably not make
    /// sense for other types of images...
    ///    virtual const QString & imageType() const = 0;

    /// does the image have a mask attached?
    virtual bool
    hasMask() const = 0;

    /// does the image have errors attached?
    /// \todo are errors always per pixel? Or could they be per frame, region, etc?
    virtual bool
    hasErrorsInfo() const = 0;

    /// get the data type for pixels
    /// note: getDataSlice will return views with this type
    virtual PixelType
    pixelType() const = 0;

    /// get the data type for errors
    /// note: getErrorSlice will return views with this type
    virtual PixelType
    errorType() const = 0;

    /// \brief get slice of data (a view into the data, to be precise)
    /// \param sliceInfo which slice to get
    /// \return a new view
    /// \warning for efficiency reasons we are not doing shared pointers, etc... since the returned
    /// view needs to have pointer to the instance of the image interface, the image interface
    /// needs to remain valid while accessing the view! i.e. don't delete it
    virtual NdArray::RawViewInterface *
    getDataSlice( const SliceND & sliceInfo ) = 0;

    /// get the mask
    /// \todo booleans as bytes is wasting resources, we should specialize
    /// the NdArray::TypedView for bools
    virtual NdArray::Byte *
    getMaskSlice( const SliceND & sliceInfo ) = 0;

    /// get the errors
    virtual NdArray::RawViewInterface *
    getErrorSlice( const SliceND & sliceInfo ) = 0;

    /// return a pointer to a meta data object, which is essentially a collection
    /// of algorithms that allows us to do useful things with metadata stored with
    /// the image
    virtual Image::MetaDataInterface::SharedPtr
    metaData() = 0;
};
} // namespace Image
}
}

/// API testing
/// this is never executed, only compiled for API testing
__attribute__ ( ( unused ) )
static void
test_apis()
{
    // get an image.... (pretend)
    Carta::Lib::Image::ImageInterface * ii = nullptr;

    Slice1D r;
    r.start( 10 ).start( 20 ).step( 8 );

    /// simulate [3::-1,:,3]
    SliceND vp;
    vp.start( 3 ).step( - 1 )
        .next()
        .next().index( 3 );

    // or
    vp.slice( 0 ).start( 3 ).step( - 1 );
    vp.slice( 2 ).index( 3 );

    // or
    typedef Slice1D S;
    SliceND vp2( { S().start( 3 ).step( - 1 ), S(), S( 3 ) }
                 );

    // describe the slice we want to extract
    // note: uninitialized slice should denote 'entire array view'
    SliceND si;

    // get the raw view of the data
    Carta::Lib::NdArray::RawViewInterface * rawView = ii-> getDataSlice( si );

    // access a single pixel in the data, returned in raw binary form
    const char * ptr = rawView-> get( { 1, 2, 3 }
                                      );
    Q_UNUSED( ptr );

    // access all pixels of the view, one by one
    u_int64_t count = 0;
    rawView-> forEach([& count] ( const char * /*ptr*/ ) { count++;
                      }
                      );

    // make a double view (from the raw view)
    Carta::Lib::NdArray::TypedView < double > doubleReader( rawView );

    // or using the typedef
    Carta::Lib::NdArray::Double doubleReader2( rawView );

    // extract a single pixel
    double x = doubleReader2.get( { 1, 2, 3 }
                                  );
    Q_UNUSED( x );

    // iterate over all pixels
    double sum = 0.0;
    doubleReader.forEach([& sum] ( const double & x ) { sum += x;
                         }
                         );

    // META DATA API tests:
    // ===========================

    // get info about axis 3
    auto meta = ii-> metaData();
    Carta::Lib::AxisInfo axis3 = meta-> coordinateFormatter()-> axisInfo( 3 );
    Q_UNUSED( axis3 );
} // test_apis
