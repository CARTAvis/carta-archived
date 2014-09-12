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

#pragma once

#include "PixelType.h"
#include "Nullable.h"
#include "misc.h"
#include "Slice.h"
#include "CoordinateFormatter.h"
#include "CoordinateGridPlotter.h"
#include "PlotLabelGenerator.h"
#include <QObject>
#include <functional>
#include <initializer_list>
#include <cstdint>

/// description of a unit
class Unit {
};


/// classes related to n-dimensional views into n-dimensional arrays
namespace NdArray {

/// \brief Interface for reading elements of multidimensional array (n-dimensional array)
/// for arbitrary types. The intentded use of this class is the lowest interface
/// needed to be implemented to read an array. A conversion adapter/mixin would be more
/// convenenient to actually access the arrays.
class RawViewInterface {
public:
    typedef std::vector<int> VI;
    typedef Image::PixelType PixelType;

    /// get the pixel type stored in this array accessor
    virtual PixelType pixelType() = 0;
    /// get the max. dimensions allowed in this accessor
    virtual const VI & dims() = 0;
    /// get the raw data for the given pixel (at acoordinates 'pos')
    virtual const char * get( const VI & pos) = 0;
    /// a way to access each element via function pointer
    virtual void forEach( std::function<void(const char*)>) = 0;

    /// virtual destructor
    virtual ~RawViewInterface() {}
};

/// utility class - wraps a raw view into a typed view
template <typename Type> class TypedView {
public:
    typedef std::vector<int> VI;

    TypedView(RawViewInterface* rawView, bool keepOwnership = false)
        : m_rawView(rawView), m_keepOwnership(keepOwnership)
    {
        Q_ASSERT(rawView != nullptr);

        // figure out which converter to use
        m_converterFunc = getConverter<Type>(rawView->pixelType());
    }

    /// extract the data and convert it to double
    const Type& get(const VI& pos)
    {
        return m_converterFunc(m_rawView->get(pos));
    }

    /// forEach
    /// \todo investigate performance of std::function vs raw function pointer
    void forEach(std::function<void(const Type&)> func)
    {
        auto wrapper = [ this, &func ](const char * ptr)->void
        {
            func(m_converterFunc(ptr));
        };
        m_rawView->forEach(wrapper);
    }

    ~TypedView()
    {
        if (m_keepOwnership) {
            Q_ASSERT(m_rawView != nullptr);
            delete m_rawView;
        }
    }

protected:

    /// pointer to the raw view
    RawViewInterface * m_rawView;

    /// are we keeping ownership of m_rawView
    bool m_keepOwnership;

    /// classic c-style function pointer to the converter
    /// \todo is this faster than std::function?
    const Type & (* m_converterFunc)( const char *);
};

/// convenience types
typedef TypedView<double> Double;
typedef TypedView<float> Float;
typedef TypedView<uint8_t> Byte;
typedef TypedView<int16_t> Int16;
typedef TypedView<int32_t> Int32;
typedef TypedView<int64_t> Int64;

} // namespace NDArrayView

namespace Image {

/// \brief Interface description for accessing various metadata associated with an image.
/// For example: beam related information, miscellaneous records.
///
/// The idea is to present this as an algorithm provider, rather than some predefined
/// set of data.
///
/// \warning This class is still evolving.
class MetaDataInterface {
public:
    /// clone yourself
    virtual MetaDataInterface* clone() = 0;

    /// get a coordinate formatter algorithm
    virtual CoordinateFormatterInterface& coordinateFormatter() = 0;

    /// get a grid plotter algorithm
    virtual CoordinateGridPlotterInterface& coordinateGridPlotter() = 0;

    /// get a labeler algorithm
    virtual PlotLabelGeneratorInterface & plotLabelGenerator() = 0;

    /// title if any (eg. TITLE in FITS)
    virtual QString title(TextFormat format = TextFormat::Plain) = 0;

    /// other data users might want to display
    /// (eg. complete FITS header)
    virtual QStringList otherInfo(TextFormat format = TextFormat::Plain) = 0;
};

/// Main interface class for representing an image inside the viewer. This is used to pass
/// around images between core and plugins. For example, a plugin that can load
/// an image would have to implement this interface.
class ImageInterface : public QObject
{
    Q_OBJECT
public:

    /// similar to BITPIX
    typedef Image::PixelType PixelType;

    ImageInterface() : QObject() {}

    /// virtual destructor to make sure we can delete arbitrary images
    virtual ~ImageInterface() {}

    /// get a unit of pixels (similar to BUNIT)
    virtual const Unit & getPixelUnit( ) const = 0;

    /// return dimensions of the image
    virtual const std::vector<int> & dims() const = 0;

    /// Used to determine the type of the image (raster, contour, vector).
    /// May need to evolve.
    virtual const QString & imageType() const = 0;

    /// does the image have a mask attached?
    virtual bool hasMask() const = 0;

    /// does the image have errors attached?
    /// \todo are errors always per pixel? Or could they be per frame, region, etc?
    virtual bool hasErrorsInfo() const = 0;

    /// get the data type for pixels
    /// note: getDataSlice will return views with this type
    virtual PixelType pixelType() const = 0;

    /// get the data type for errors
    /// note: getErrorSlice will return views with this type
    virtual PixelType errorType() const = 0;

    /// \brief get slice of data
    /// \param sliceInfo which slice to get
    /// \param result where to store result
    virtual void getDataSlice( const SliceND & sliceInfo, NdArray::RawViewInterface & result) = 0;

    /// get the mask
    /// \todo booleans as bytes is wasting resources, we should specialize
    /// the NdArray::TypedView for bools
    virtual void getMaskSlice( const SliceND & sliceInfo, NdArray::Byte & result) = 0;

    /// get the errors
    virtual void getErrorSlice( const SliceND & sliceInfo, NdArray::RawViewInterface & result) = 0;

    /// The ImageInfo object contains miscellaneous information about the image
    virtual Image::MetaDataInterface & metaData() = 0;
};

} // namespace Image


/// API testing
/// this is never executed, only compiled for API testing
__attribute__ ((unused))
static void test_apis()
{
    // get an image.... (pretend)
    Image::ImageInterface * ii;

    Slice1D r;
    r.start(10).start(20).step(8);

    /// simulate [3::-1,:,3]
    SliceND vp;
    vp.start(3).step(-1)
            .next()
            .next().index(3);
    // or
    vp.slice(0).start(3).step(-1);
    vp.slice(2).index(3);
    // or
    typedef Slice1D S;
    SliceND vp2({ S().start(3).step(-1), S(), S(3) });

    // describe the slice we want to extract
    // note: uninitialized slice should denote 'entire array view'
    SliceND si;

    // get the raw view of the data
    NdArray::RawViewInterface * rawView;
    ii-> getDataSlice(si, *rawView);

    // access a single pixel in the data, returned in raw binary form
    const char * ptr = rawView-> get( { 1, 2, 3});
    Q_UNUSED( ptr);

    // access all pixels of the view, one by one
    u_int64_t count = 0;
    rawView-> forEach( [ & count] (const char * /*ptr*/) { count ++; });

    // make a double view (from the raw view)
    NdArray::TypedView<double> doubleReader( rawView);
    // or using the typedef
    NdArray::Double doubleReader2( rawView);

    // extract a single pixel
    double x = doubleReader2.get({ 1, 2, 3});
    Q_UNUSED( x);

    // iterate over all pixels
    double sum = 0.0;
    doubleReader.forEach( [& sum]( const double & x) { sum += x; });

    // META DATA API tests:
    // ===========================

    // get info about axis 3
    auto & meta = ii-> metaData();
    AxisInfo axis3 = meta.coordinateFormatter().axisInfo( 3);
    Q_UNUSED( axis3);


}


















