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

#include <QObject>

class ImageInfo {
    //Beam related information
    //Miscellaneous records.
};

class Unit {

};

class Slicer {};

class NDArray {};

class NDBoolArray {};


class ImageI : public QObject
{
    Q_OBJECT

public:

    enum class WhichSlice { Data, Mask, Errors, Both };

    /// similar to BITPIX
    enum class PixelType { Int8, Int16, Int32, Int64, Real32, Real64, Other };

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
    virtual void getDataSlice( const Slicer & sliceInfo, NDArray & result) = 0;
    virtual void getMaskSlice( const Slicer & sliceInfo, NDBoolArray & result) = 0;
    virtual void getErrorSlice( const Slicer & sliceInfo, NDArray & result) = 0;

    /// The ImageInfo object contains miscellaneous information about the image
    virtual const ImageInfo & imageInfo() const = 0;


};

