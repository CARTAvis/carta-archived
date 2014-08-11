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
#include <QObject>
#include <functional>

class ImageInfo {
    //Beam related information
    //Miscellaneous records.
};

class Unit {

};

class Slicer {};

class NDArray {};

class NDBoolArray {};

/// \brief Interface for reading elements of multidimensional array (n-dimensional array)
/// for arbitrary types. The intentded use of this class is the lowest interface
/// needed to be implemented to read an array. A conversion adapter/mixin would be more
/// convenenient to actually access the arrays.
class NdArrayViewRawI {
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
    /// a way to access each element via lambda
    virtual void forEach( std::function<void(const char*)>) = 0;

    /// virtual destructor
    virtual ~NdArrayViewRawI() {}
};


/// utility class - wraps a raw view into a double view
class NdArrayViewDouble
{

public:
    typedef std::vector<int> VI;

    NdArrayViewDouble( NdArrayViewRawI * rawView, bool keepOwnership = false)
        : m_rawView( rawView)
        , m_keepOwnership( keepOwnership)
    {
        Q_ASSERT( rawView != nullptr);

        // figure out which converter to use
        m_converterFunc = getConverter<double>( rawView->pixelType());
    }

    /// extract the data and convert it to double
    const double & get( const VI & pos) {
        return m_converterFunc( m_rawView-> get(pos));
    }

    /// forEach
    void forEach( std::function<void(const double &)> func) {
        auto wrapper = [this, & func]( const char * ptr) -> void {
            func( m_converterFunc(ptr));
        };
        m_rawView->forEach( wrapper);
    }

    ~NdArrayViewDouble() {
        if( m_keepOwnership) {
            Q_ASSERT( m_rawView != nullptr);
            delete m_rawView;
        }
    }

protected:

    /// pointer to the raw view
    NdArrayViewRawI * m_rawView;

    /// are we keeping ownership of m_rawView
    bool m_keepOwnership;

    /// classic c-style function pointer to the converter
    // TODO: is this faster than std::function?
    const double & (* m_converterFunc)( const char *);
};

class ImageI : public QObject
{
    Q_OBJECT

public:

    enum class WhichSlice { Data, Mask, Errors, Both };

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

    ///
    /// \brief get slice of data
    /// \param sliceInfo which slice to get
    /// \param result where to store result
    ///
    virtual void getDataSlice( const Slicer & sliceInfo, NdArrayViewRawI & result) = 0;

    virtual void getDataSlice( const Slicer & sliceInfo, NDArray & result) = 0;
    virtual void getMaskSlice( const Slicer & sliceInfo, NDBoolArray & result) = 0;
    virtual void getErrorSlice( const Slicer & sliceInfo, NDArray & result) = 0;

    /// The ImageInfo object contains miscellaneous information about the image
    virtual const ImageInfo & imageInfo() const = 0;
};


static void test_apis()
{
    // get an image....
    ImageI *ii;

    // describe the slice we want to extract
    // note: uninitialized slice should denote 'entire array view'
    Slicer si;
    // get the raw view of the data
    NdArrayViewRawI * rawView;
    ii->getDataSlice( si, * rawView);

    // access a single pixel in the data, returned in raw binary form
    const char * ptr = rawView->get( { 1, 2, 3});

    // access all pixels of the view, one by one
    u_int64_t count = 0;
    rawView-> forEach( [ & count] (const char * /*ptr*/) { count ++; });

    // make a double view (from the raw view)
    NdArrayViewDouble doubleReader( rawView);
    // extract a single pixel
    double x = doubleReader.get({ 1, 2, 3});

    // iterate over all pixels
    double sum = 0.0;
    doubleReader.forEach( [& sum]( const double & x) { sum += x; });

    // access a single piece of data and convert it to double
//    NdArrayDoubleReader dr( ndview);
//    double x = dr.get({ 1, 2, 3});

//    // let's loop over the raw
//    const char * ptr = ndview-> forEach( [](const char * ptr) {});

}

