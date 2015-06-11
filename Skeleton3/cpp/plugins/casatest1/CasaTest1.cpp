#include "CasaTest1.h"
#include <QDebug>
#include <QPainter>
#include <QTime>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/images/Images/FITSImage.h>
#include <casacore/images/Images/MIRIADImage.h>
#include <casacore/images/Images/HDF5Image.h>
#include <casacore/images/Images/ImageExpr.h>
#include <casacore/images/Images/ImageExprParse.h>
#include <casacore/images/Images/ImageOpener.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Quanta.h>
#include <memory>
#include <algorithm>

CasaTest1::CasaTest1(QObject *parent) :
    QObject(parent)
{
}

bool CasaTest1::handleHook(BaseHook & hookData)
{
    qDebug() << "CasaTest Plugin is handling hook #" << hookData.hookId();
    if( hookData.hookId() == Initialize::staticId ) {
        // Register FITS and Miriad image types
        casa::FITSImage::registerOpenFunction();
        casa::MIRIADImage::registerOpenFunction();
        return true;
    }

    if( hookData.hookId() == LoadImage::staticId) {
        LoadImage & hook = static_cast<LoadImage &>( hookData);
        auto fname = hook.paramsPtr->fileName;
        auto channel = hook.paramsPtr->frame;
        qDebug()<<"CasaTest1 loading image "<<fname;
        return loadImage( fname, channel, hook.result );
    }

    qWarning() << "Sorrry, dont' know how to handle this hook";
    return false;
}

std::vector<HookId> CasaTest1::getInitialHookList()
{
    return {
        Initialize::staticId,
        LoadImage::staticId
    };
}

///
/// \brief tries to open a given image using any of the supported formats
/// \param fname filename of the image to open
/// \return on success a pointer to casa::ImageInterface<casa::Float>
///
static
casa::ImageInterface<casa::Float> * openCasaImageF( const QString & fname)
{
    qDebug() << "Opening (casa)image " << fname;
    casa::LatticeBase * lat = 0;

    // try using image opener & paged array
    casa::FITSImage::registerOpenFunction ();
    try {
        lat = casa::ImageOpener::openPagedImage ( fname.toStdString());
        qDebug() << "\t-opened as paged image";
    } catch ( ... ) {
        qDebug() << "\t-paged image open failed";
    }

    // try using image opener & unpaged array
    if( lat == 0 ) {
        try {
            lat = casa::ImageOpener::openImage ( fname.toStdString());
            qDebug() << "\t-opened as unpaged image";
        } catch ( ... ) {
            qDebug() << "\t-unpaged image open failed";
        }
    }
    // if we still failed to open the lattice, we are done
    if( lat == 0 ) {
        qDebug() << "\t-out of ideas, bailing out";
        return 0;
    }

    // try to convert to float image interface
    casa::ImageInterface<casa::Float> * img = 0;

    // see if we can convert to ImageInterface<float> directly
    if( lat->dataType () == casa::TpFloat) {
        try {
            img = dynamic_cast<casa::ImageInterface<casa::Float>*> ( lat );
            qDebug() << "\t-simple conversion to image interface float successful";
        } catch ( ... ) {}
    }
    // if the initial conversion attempt failed, try a LEL expression
    if( img == 0) {
        try {
            qDebug() << "\t-simple conversion to image interface failed";
            std::string expr = "float('" + fname.toStdString() + "')";
            casa::LatticeExpr<casa::Float> le ( casa::ImageExprParse::command( expr ));
            img = new casa::ImageExpr<casa::Float> ( le, expr );
            qDebug() << "\t-LEL conversion successful";
        } catch ( ... ) {}
    }
    if( img == 0) {
        qDebug() << "\t-could not convert to float image, bailing out";
    }

    return img;
}


///
/// \brief Attempts to load an image using casacore library, namely the very first
/// frame of it. Then converts the frame to a QImage using 95% histogram clip values.
/// \param fname file name with the image
/// \param result where to store the result
/// \return true if successful, false otherwise
///
bool CasaTest1::loadImage( const QString & fname, int channel, QImage & result)
{
    qDebug() << "CasaTest Plugin trying to load image: " << fname<<" channel="<<channel;

    // define data type that we are using per pixel
    typedef float Pixel;

    // alias shortcut to casa image
    typedef casa::ImageInterface<Pixel> CImage;

    // auto-release casa image pointer
    std::unique_ptr< CImage > imgptr;

    // try to open the image using casacore's fits implementation
    try {
        CImage * img = nullptr;
        // img = new casa::FITSImage( fname.toStdString());
        img = openCasaImageF( fname);
        imgptr.reset( img);
    } catch (...) {
        qDebug() << "Casa failed to load the image.";
    }

    // if we failed, we are done
    if( ! imgptr) {
        return false;
    }

    // make sure we have at least 2 dimensions
    int dims = imgptr-> ndim();
    if( dims < 2 ) {
        qDebug() << "Casa opened the image but it has less than 2 dimensions...";
        return false;
    }

    int width = imgptr-> shape()[0];
    int height = imgptr-> shape()[1];
    int channelCount = 0;
    if ( dims >= 3 ){
    	channelCount = imgptr->shape()[2];
    }
    qDebug() << "Channel count="<<channelCount;
    qDebug() << "Image opened using casacore" << width << "x" << height << "pixels"<<" channels="<<channelCount;

    // figure out 95% clip values
    std::vector< Pixel> allValues;

    //
    // extract all pixels into an array so that we can compute clip values
    //

    if( false)
    {
        // the SLOOOOOW way
        casa::IPosition casaCursor( dims, 0);
        for( int y = 0 ; y < height ; y ++ ) {
            casaCursor[1] = y;
            for( int x = 0 ; x < width ; x ++ ) {
                casaCursor[0] = x;
                Pixel val = imgptr-> operator()( casaCursor);
                if( std::isfinite( val)) {
                    allValues.push_back( val);
                }
            }
        }
        qDebug() << "Found: " << allValues.size() << " values";
        qDebug() << " ==> " << width * height - allValues.size() << "nans";
        allValues.resize( 0);
    }

    {
        casa::IPosition cursorShape(2, width, height);
        casa::LatticeStepper stepper( imgptr-> shape(), cursorShape);
        casa::IPosition blc(dims, 0);
        casa::IPosition trc(dims, 0);
        trc(0) = width-1;
        trc(1) = height-1;
        if ( dims > 2 ){
        	trc(2) = channel;
        	blc(2) = channel;
        }
        stepper.subSection(blc, trc);

        casa::RO_LatticeIterator<casa::Float> iterator(* imgptr, stepper);

        for (iterator.reset(); !iterator.atEnd(); iterator++) {
            for( const auto & val : iterator.cursor()) {
                if( std::isfinite( val)) {
                    allValues.push_back( val);
                }
            }
        }
        qDebug() << "Found: " << allValues.size() << " values";
        qDebug() << " ==> " << width * height - allValues.size() << "nans";
    }

    // make sure there is at least 1 non-nan value, if not, return a black image
    if( allValues.size() == 0) {
        // there were no values.... return a black image
        result = QImage( width, height, QImage::Format_ARGB32_Premultiplied);
        result.fill( 0);
        return true;
    }

    if( false) {
        // report min/max of the frame, for debugging purposes only

        // make sure first element is the smallest
        std::nth_element( allValues.begin(), allValues.begin(), allValues.end());
        // last element is the largest
        std::nth_element( allValues.begin(), allValues.end()-1, allValues.end());

        Pixel min = allValues.front();
        Pixel max = allValues.back();
        qDebug() << "minmax=" << min << max;
    }

    double hist = 95; // 95%
    Pixel clip1, clip2;
    {
        hist = (1.0 - hist / 100) / 2.0;
        int x1 = allValues.size() * hist;
        std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end());
        clip1 = allValues[x1];
        x1 = allValues.size() - x1;
        std::nth_element( allValues.begin(), allValues.begin() + x1, allValues.end());
        clip2 = allValues[x1];
        qDebug() << "clips=" << clip1 << clip2;
    }
    Pixel clipd = clip2 - clip1;
    Pixel clipdinv = 1.0 / clipd; // precalculated linear transformation coeff.

    //
    // revisit the pixels again, but this time construct the image using the clips
    //

    casa::IPosition cursorShape(2, width, height);
    casa::LatticeStepper stepper( imgptr-> shape(), cursorShape);
    casa::IPosition blc(dims, 0);
    casa::IPosition trc(dims, 0);
    trc(0) = width-1;
    trc(1) = height-1;
    if ( dims > 2 ){
    	trc(2) = channel;
    	blc(2) = channel;
    }
    stepper.subSection(blc, trc); // limit to first frame only

    casa::RO_LatticeIterator<casa::Float> iterator(* imgptr, stepper);
    iterator.reset();
    const casa::Matrix<Pixel> & srcm = iterator.matrixCursor();
    qDebug() << "matrix cursor" << srcm.shape()[0] << srcm.shape()[1];
    auto srci = srcm.begin();

    // stuff the pixels into an qrgb buffer, because constructing qimage this way is
    // the fastest
    static std::vector < QRgb > outBuffer;
    outBuffer.resize( width * height);

    for( int y = 0 ; y < height ; y ++ ) {
        QRgb * outPtr = & (outBuffer[(height-y-1) * width]);
        for( int x = 0 ; x < width ; x ++ ) {
            Pixel val = * srci;
            if( std::isfinite( val)) {
                val = (val - clip1) * clipdinv;
                int gray = val * 255;
                if( gray > 255) gray = 255;
                if( gray < 0) gray = 0;
                * outPtr = qRgb( gray, gray, gray);
            }
            else {
                * outPtr = qRgb( 255, 0, 0);
            }
            outPtr ++;
            srci ++;
        }
    }

    // construct the qimage from the temporary buffer
    result = QImage( reinterpret_cast<uchar *>( & outBuffer[0]),
            width, height, QImage::Format_ARGB32);

    return true;
}
