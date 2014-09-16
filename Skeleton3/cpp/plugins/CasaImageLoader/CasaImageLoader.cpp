#include "CasaImageLoader.h"
#include "CCImage.h"
#include <QDebug>
#include <QPainter>
#include <QTime>
#include <casa/Exceptions/Error.h>
#include <images/Images/FITSImage.h>
#include <images/Images/MIRIADImage.h>
#include <images/Images/HDF5Image.h>
#include <images/Images/ImageExpr.h>
#include <images/Images/ImageExprParse.h>
#include <images/Images/ImageOpener.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Quanta.h>
#include <memory>
#include <algorithm>
#include <cstdint>

CasaImageLoader::CasaImageLoader(QObject *parent) :
    QObject(parent)
{
}

bool CasaImageLoader::handleHook(BaseHook & hookData)
{
    qDebug() << "CasaImageLoader plugin is handling hook #" << hookData.hookId();
    if( isHook<Initialize>( hookData)) {
        // Register FITS and Miriad image types
        casa::FITSImage::registerOpenFunction();
        casa::MIRIADImage::registerOpenFunction();
        return true;
    }

    else if( isHook<LoadAstroImage>( hookData)) {
        LoadAstroImage & hook = static_cast<LoadAstroImage &>( hookData);
        auto fname = hook.paramsPtr->fileName;
        hook.result = loadImage( fname);
        // return true if result is not null
        return hook.result != nullptr;
    }

    qWarning() << "Sorrry, dont' know how to handle this hook";
    return false;
}

std::vector<HookId> CasaImageLoader::getInitialHookList()
{
    return {
        Initialize::StaticHookId,
        LoadAstroImage::StaticHookId
    };
}


///
/// \brief Attempts to load an image using casacore library, namely the very first
/// frame of it. Then converts the frame to a QImage using 95% histogram clip values.
/// \param fname file name with the image
/// \param result where to store the result
/// \return true if successful, false otherwise
///
Image::ImageInterface * CasaImageLoader::loadImage( const QString & fname)
{
    qDebug() << "CasaImageLoader plugin trying to load image: " << fname;

    //
    // first we open the image as a lattice
    //
    casa::LatticeBase * lat = nullptr;
    // first try as paged array
    try {
        lat = casa::ImageOpener::openPagedImage ( fname.toStdString());
        qDebug() << "\t-opened as paged image";
    } catch ( ... ) {
        qDebug() << "\t-paged image open failed";
    }
    // if paged didn't work, try as unpaged
    if( ! lat ) {
        try {
            lat = casa::ImageOpener::openImage ( fname.toStdString());
            qDebug() << "\t-opened as unpaged image";
        } catch ( ... ) {
            qDebug() << "\t-unpaged image open failed";
        }
    }
    // if we failed to open the lattice, we are done :(
    if( lat == 0 ) {
        qDebug() << "\t-out of ideas, bailing out";
        return nullptr;
    }

    qDebug() << "lat=" << lat;
    auto shape = lat->shape();
    auto shapes = shape.asStdVector();
    qDebug() << "lat.shape = " << std::string( lat->shape().toString()).c_str();
    // based on the type data in the lattice we construct the CCimage with the
    // appropriate type
    if( lat-> dataType() == casa::TpFloat) {
        return CCImage::create( dynamic_cast<casa::ImageInterface<casa::Float> *>(lat));
    }
    else if( lat-> dataType() == casa::TpDouble) {
        return CCImage::create( dynamic_cast<casa::ImageInterface<casa::Double> *>(lat));
    }
    else if( lat-> dataType() == casa::TpUChar) {
        return CCImage::create( dynamic_cast<casa::ImageInterface<casa::uChar> *>(lat));
    }
    else if( lat-> dataType() == casa::TpInt64) {
        return CCImage::create( dynamic_cast<casa::ImageInterface<std::int64_t> *>(lat));
    }
    else if( lat-> dataType() == casa::TpInt) {
        return CCImage::create( dynamic_cast<casa::ImageInterface<casa::Int> *>(lat));
    }

    // we have an unknown type for lattice, let's try to convert it to double
    qWarning() << "Unsupported lattice type:" << lat-> dataType();

    /// \todo should we try LEL to convert to double or float???
//    << " trying LEL to float";
//    try {
//        std::string expr = "float('" + fname.toStdString() + "')";
//        casa::LatticeExpr<casa::Float> le ( casa::ImageExprParse::command( expr ));
//        img = new casa::ImageExpr<casa::Float> ( le, expr );
//        qDebug() << "\t-LEL conversion successful";
//    } catch ( ... ) {}

    // indicate failure
    delete lat;
    return nullptr;
}
