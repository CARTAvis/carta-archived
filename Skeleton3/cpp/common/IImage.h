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

/// contains parameters needed to create a view
/// this is a collection of methods to create such a description and then to access it
class ViewParams {

public:

    typedef int64_t Index;

    /// description of a 1D slice to extract
    /// this is motivated by the python slice object (start:end:step)
    /// Slice also supports storing a single index (to indicate the result should be
    /// a scalar, rather than array)
    struct Slice {

        Slice()
            : m_singleIndex( false)
        {}

        Slice(Index ind)
            : m_singleIndex( true)
            , m_start( ind)
        {}


        /// set start (very first element)
        Slice & start( Index s) {
            m_start = s;
            return * this;
        }

        /// set stop (the last element, but this is not included)
        Slice & end( Index e) {
            m_end = e;
            return * this;
        }

        /// set step value
        Slice & step( Index s) {
            m_step = s;
            return * this;
        }

        /// set single index
        /// note we do not want chaining here, so we return void
        void index( Index s) {
            m_start = s;
            m_singleIndex = true;
        }

        /// does this represent single index
        bool isSingleIndex() const { return m_singleIndex; }

        /// is the range null (default cosntructed)
        bool isNull() const {
            return m_start.isNull() && m_end.isNull() && m_step.isNull();
        }

        /// return starting index, count and step for the given input size n
        struct AppplyResult {
            /// index of the first element to extract
            /// -1 means bad result/error
            Index start;
            /// how many elements to extract (could be 0)
            /// if count == -1, then the extraction is of a single index
            Index count;
            /// the step
            Index step;
            /// convenience test for error
            bool isError() const {
                return start < 0;
            }
            /// convenience test for single value
            /// note, the result could still be an error...
            bool isSingle() const {
                return count == -1;
            }
        };

        AppplyResult apply( Index n)
        {
            AppplyResult res;

            // extract step or assign default value of "1"
            // this is the same for all cases
            res.step = m_step.isSet() ? m_step.val() : 1;

            // handle special case for single index
            if( isSingleIndex()) {
                res.start = m_start.val();
                if( res.start < 0) {
                    res.start += n;
                }
                // if single index is out of bounds, report error
                if( res.start < 0 || res.start >= n) {
                    res.start = -1;
                }
                res.count = -1;
                return res;
            }

            // special case for step=0, which is an error
            if( res.step == 0) {
                res.start = -1;
                res.count = 0;
                return res;
            }

            // special case for n=0, return empty result
            if( n == 0) {
                res.start = 0;
                res.count = 0;
                return res;
            }

            // handle positive step
            if( res.step > 0) {
                positiveCase( res, n);
            }
            // handle case where step is negative
            else {
                std::swap( m_start, m_end);
                res.step = - res.step;
                positiveCase( res, n, 1);
                std::swap( m_start, m_end);
                res.start += res.count -1;
                res.step = - res.step;
            }
            // we are done
            return res;
        }

        // this is adaptation of cpython's PySlice_GetIndicesEx()
        // https://docs.python.org/2/c-api/slice.html
        AppplyResult cpythonApply( Index n)
        {
            AppplyResult res;

            // extract step or assign default value of "1"
            // this is the same for all cases
            res.step = m_step.isSet() ? m_step.val() : 1;

            // handle special case for single index
            if( isSingleIndex()) {
                res.start = m_start.val();
                if( res.start < 0) {
                    res.start += n;
                }
                // if single index is out of bounds, report error
                if( res.start < 0 || res.start >= n) {
                    res.start = -1;
                }
                res.count = -1;
                return res;
            }

            // special case for step=0, which is an error
            if( res.step == 0) {
                res.start = -1;
                res.count = 0;
                return res;
            }

            Index defstart = res.step < 0 ? n-1 : 0;
            Index defstop = res.step < 0 ? -1 : n;

            if( m_start.isNull()) {
                res.start = defstart;
            }
            else {
                res.start = m_start.val();
                if (res.start < 0) res.start += n;
                if (res.start < 0) res.start = (res.step < 0) ? -1 : 0;
                if (res.start >= n)
                    res.start = (res.step < 0) ? n - 1 : n;
            }

            Index stop;
            if( m_end.isNull()) {
                stop = defstop;
            }
            else {
                stop = m_end.val();
                if (stop < 0) stop += n;
                if (stop < 0) stop = (res.step < 0) ? -1 : 0;
                if (stop >= n)
                    stop = (res.step < 0) ? n - 1 : n;
            }

            if ((res.step < 0 && stop >= res.start)
                    || (res.step > 0 && res.start >= stop)) {
                res.count = 0;
            }
            else if (res.step < 0) {
                res.count = (stop-res.start+1)/(res.step)+1;
            }
            else {
                res.count = (stop-res.start-1)/(res.step)+1;
            }

            return res;
        }


    protected:

        /// positive step case calculation
        void positiveCase( AppplyResult & res, Index n, Index off = 0) {
            // assuming we already have step>0, n>0 and not single index, and
            // also res.step has already been extracted

            // extract start value if defined, and clamp it to be valid
            if( m_start.isSet()) {
                res.start = m_start.val();
                // adjust negative values
                if( res.start < 0) res.start = n + res.start;
                res.start += off;
                res.start = clamp<Index>( res.start, 0, n);
            }
            // if start was not defined, assign a default
            else {
                res.start = 0;
            }
            // figure out the number of elements
            Index end;
            if( m_end.isSet()) {
                end = m_end.val();
                if( end < 0) end = n + end;
                end += off;
                // make sure start <= end
                end = clamp<Index>( end, res.start, n);
            }
            // if end was not defined, assign default
            else {
                end = n;
            }
            // at this point we have start and end such that
            //      0 <= start <= end <= n
            // to get the count, we do a simple integer division...
            res.count = (end - res.start + res.step - 1) / res.step;
        }


        /// do we represent single index (true) or not
        bool m_singleIndex;

        /// if not single index, these are the start/end/step values
        /// if single index, then start is the index
        Nullable < Index > m_start, m_end, m_step;

    };

    /// number of explicitly defined dimensions
    int dims();

    /// default constructor - creates a [:] slice (ie. everything)
    ViewParams() {}

    /// constructor with initializer list
    ViewParams( std::initializer_list<ViewParams::Slice> list) {
         m_slices = list;
    }

    /// get slice at position pos
    Slice & axis( Index pos) {
        if( pos >= m_dims) {
            m_slices.resize( pos + 1);
        }
        return m_slices[ pos];
    }

    /// forward 1d slice start
    ViewParams & start( Index start) {
        axis( m_currentSlice).start( start);
        return * this;
    }

    /// forward 1d slice end
    ViewParams & end( Index end) {
        axis( m_currentSlice).end( end);
        return * this;
    }

    /// forward 1d slice step
    ViewParams & step( Index step) {
        axis( m_currentSlice).end( step);
        return * this;
    }

    /// forward 1d slice index
    ViewParams & index( Index index) {
        axis( m_currentSlice).index( index);
        return * this;
    }

    /// moves to the next index
    ViewParams & next() {
        m_currentSlice ++;
        axis( m_currentSlice);
        return * this;
    }


protected:

    int m_dims = 0;
    std::vector<Slice> m_slices;
    int m_currentSlice = 0;

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
    virtual void getDataSlice( const ViewParams & sliceInfo, NDArrayView::RawInterface & result) = 0;

//    virtual void getDataSlice( const Slicer & sliceInfo, NDArray & result) = 0;

    /// get the mask
    /// TODO: booleans as bytes is wasting resources, we should specialize typed view for bools
    virtual void getMaskSlice( const ViewParams & sliceInfo, NDArrayView::Int8 & result) = 0;

    virtual void getErrorSlice( const ViewParams & sliceInfo, NDArrayView::RawInterface & result) = 0;

    /// The ImageInfo object contains miscellaneous information about the image
    virtual const ImageInfo & imageInfo() const = 0;
};

__attribute__ ((unused))
static void test_apis()
{
    // get an image....
    ImageI *ii;

    ViewParams::Slice r;
    r.start( 10).start(20).step(8);

    /// simulate [3::-1,:,3]
    ViewParams vp;
    vp.start(3).step(-1)
            .next()
            .next().index(3);
    // or
    vp.axis(0).start(3).step(-1);
    vp.axis(2).index(3);
    // or
    typedef ViewParams::Slice S;
    //    ViewParams vp2({ S().start(3).step(-1), S(), S().index(3) });
    ViewParams vp2({ S().start(3).step(-1), S(), S(3) });

    // describe the slice we want to extract
    // note: uninitialized slice should denote 'entire array view'
    ViewParams si;

    // get the raw view of the data
    NDArrayView::RawInterface * rawView;
    ii->getDataSlice( si, * rawView);

    // access a single pixel in the data, returned in raw binary form
    const char * ptr = rawView->get( { 1, 2, 3});
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

