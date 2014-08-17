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

#include "PixelType.h"
#include "Nullable.h"
#include "misc.h"
#include "Slice.h"
#include <QObject>
#include <functional>
#include <initializer_list>

class ImageInfo {
    //Beam related information
    //Miscellaneous records.
};

/// description of a unit
class Unit {
};


/// classes related to n-dimensional views into n-dimensional arrays
namespace NDArrayView {

/// \brief Interface for reading elements of multidimensional array (n-dimensional array)
/// for arbitrary types. The intentded use of this class is the lowest interface
/// needed to be implemented to read an array. A conversion adapter/mixin would be more
/// convenenient to actually access the arrays.
class RawInterface {
public:
    typedef std::vector<int> VI;
    typedef Image::PixelType PixelType;
//    enum class DataType { Int8 = 0, Int16, Int32, Int64, Real32, Real64, Other };

    /// get the pixel type stored in this array accessor
    virtual PixelType pixelType() = 0;
    /// get the max. dimensions allowed in this accessor
    virtual const VI & dims() = 0;
    /// get the raw data for the given pixel (at acoordinates 'pos')
    virtual const char * get( const VI & pos) = 0;
    /// a way to access each element via function pointer
    virtual void forEach( std::function<void(const char*)>) = 0;

    /// virtual destructor
    virtual ~RawInterface() {}
};

/// utility class - wraps a raw view into a typed view
template < typename Type >
class Typed
{

public:
    typedef std::vector<int> VI;

    Typed( RawInterface * rawView, bool keepOwnership = false)
        : m_rawView( rawView)
        , m_keepOwnership( keepOwnership)
    {
        Q_ASSERT( rawView != nullptr);

        // figure out which converter to use
        m_converterFunc = getConverter<Type>( rawView->pixelType());
    }

    /// extract the data and convert it to double
    const Type & get( const VI & pos) {
        return m_converterFunc( m_rawView-> get(pos));
    }

    /// forEach
    /// TODO: investigate performance of std::function vs raw function pointer
    void forEach( std::function<void(const Type &)> func) {
        auto wrapper = [this, & func]( const char * ptr) -> void {
            func( m_converterFunc(ptr));
        };
        m_rawView->forEach( wrapper);
    }

    ~Typed() {
        if( m_keepOwnership) {
            Q_ASSERT( m_rawView != nullptr);
            delete m_rawView;
        }
    }

protected:

    /// pointer to the raw view
    RawInterface * m_rawView;

    /// are we keeping ownership of m_rawView
    bool m_keepOwnership;

    /// classic c-style function pointer to the converter
    // TODO: is this faster than std::function?
    const Type & (* m_converterFunc)( const char *);
};

typedef Typed<double> Double;
typedef Typed<float> Float;
typedef Typed<int8_t> Int8;
typedef Typed<int16_t> Int16;
typedef Typed<int32_t> Int32;
typedef Typed<int64_t> Int64;

} // namespace NDArrayView


class ImageI : public QObject
{
    Q_OBJECT

public:

//    enum class WhichSlice { Data, Mask, Errors, Both };

    /// similar to BITPIX
    typedef Image::PixelType PixelType;
//    enum class PixelType { Int8, Int16, Int32, Int64, Real32, Real64, Other };

    ImageI() : QObject() {}

    /// virtual destructor to make sure we can delete arbitrary images
    virtual ~ImageI() {}

    /// get a unit of pixels (similar to BUNIT)
    virtual const Unit & getPixelUnit( ) const = 0;

    /// return dimensions of the image
    virtual const std::vector<int> & dims() const = 0;

    /// Used to determine the type of the image (raster, contour, vector).
    /// May need to evolve.
    virtual const QString & imageType() const = 0;

    /// Textual representation of the image.
    /// TODO: what is this for again? Something the user wants to see in the title?
//    virtual const QString & name( bool fullName ) const = 0;

    /// does the image have a mask attached?
    virtual bool hasMask() const = 0;

    /// does the image have errors attached?
    /// TODO: are errors always per pixel? Or could they be per frame, region, etc?
    virtual bool hasErrorsInfo() const = 0;

    /// get the data type for pixels
    /// note: getDataSlice will return views with this type
    virtual PixelType pixelType() const = 0;

    /// get the data type for errors
    /// note: getErrorSlice will return views with this type
    virtual PixelType errorType() const = 0;

    ///
    /// \brief get slice of data
    /// \param sliceInfo which slice to get
    /// \param result where to store result
    ///
    virtual void getDataSlice( const SliceND & sliceInfo, NDArrayView::RawInterface & result) = 0;

//    virtual void getDataSlice( const Slicer & sliceInfo, NDArray & result) = 0;

    /// get the mask
    /// TODO: booleans as bytes is wasting resources, we should specialize typed view for bools
    virtual void getMaskSlice( const SliceND & sliceInfo, NDArrayView::Int8 & result) = 0;

    virtual void getErrorSlice( const SliceND & sliceInfo, NDArrayView::RawInterface & result) = 0;

    /// The ImageInfo object contains miscellaneous information about the image
    virtual const ImageInfo & imageInfo() const = 0;
};


// API testing
// this is never executed, only compiled for API testing
__attribute__ ((unused))
static void test_apis()
{
    // get an image....
    ImageI *ii;

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
    NDArrayView::RawInterface * rawView;
    ii-> getDataSlice( si, * rawView);

    // access a single pixel in the data, returned in raw binary form
    const char * ptr = rawView-> get( { 1, 2, 3});
    Q_UNUSED( ptr);

    // access all pixels of the view, one by one
    u_int64_t count = 0;
    rawView-> forEach( [ & count] (const char * /*ptr*/) { count ++; });

    // make a double view (from the raw view)
    NDArrayView::Typed<double> doubleReader( rawView);
    // or using the typedef
    NDArrayView::Double doubleReader2( rawView);

    // extract a single pixel
    double x = doubleReader2.get({ 1, 2, 3});
    Q_UNUSED( x);

    // iterate over all pixels
    double sum = 0.0;
    doubleReader.forEach( [& sum]( const double & x) { sum += x; });

}

