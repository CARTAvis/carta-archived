#include "CasaImageLoader.h"
#include "CCImage.h"
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/LoadAstroImage.h"
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
#include <cstdint>

CasaImageLoader::CasaImageLoader(QObject *parent) :
    QObject(parent)
{
}

bool CasaImageLoader::handleHook(BaseHook & hookData)
{
    qDebug() << "CasaImageLoader plugin is handling hook #" << hookData.hookId();
    if( hookData.is<Carta::Lib::Hooks::Initialize>()) {
        // Register FITS and Miriad image types
        casa::FITSImage::registerOpenFunction();
        casa::MIRIADImage::registerOpenFunction();
        return true;
    }

    else if( hookData.is<Carta::Lib::Hooks::LoadAstroImage>()) {
        Carta::Lib::Hooks::LoadAstroImage & hook
                = static_cast<Carta::Lib::Hooks::LoadAstroImage &>( hookData);
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
        Carta::Lib::Hooks::Initialize::staticId,
        Carta::Lib::Hooks::LoadAstroImage::staticId
    };
}

template <typename T>
static CCImageBase::SharedPtr tryCast( casa::LatticeBase * lat)
{
    typedef casa::ImageInterface<T> CCIT;
    CCIT * cii = dynamic_cast<CCIT *>(lat);
    typename CCImage<T>::SharedPtr res = nullptr;
    if( cii) {
        res = CCImage<T>::create( cii);
    }
    return res;
}

///
/// \brief Attempts to load an image using casacore library, namely the very first
/// frame of it. Then converts the frame to a QImage using 95% histogram clip values.
/// \param fname file name with the image
/// \param result where to store the result
/// \return true if successful, false otherwise
///
Carta::Lib::Image::ImageInterface::SharedPtr CasaImageLoader::loadImage( const QString & fname)
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
    } catch ( casa::AipsError & e) {
        qWarning() << "\t-paged image open failed: " << e.what();
    }
    // if paged didn't work, try as unpaged
    if( ! lat ) {
        try {
            lat = casa::ImageOpener::openImage ( fname.toStdString());
        qDebug() << "\t-opened as unpaged image";
        } catch ( casa::AipsError & e) {
        qWarning() << "\t-unpaged image open failed: " << e.what();
        }
    }
    // if we failed to open the lattice, we are done :(
    if( lat == 0 ) {
        qDebug() << "\t-out of ideas, bailing out";
        return nullptr;
    }
    lat->reopen();
    qDebug() << "lat=" << lat;
    auto shape = lat->shape();
    auto shapes = shape.asStdVector();
    qDebug() << "lat.shape = " << std::string( lat->shape().toString()).c_str();
    qDebug() << "lat.dataType = " << lat->dataType();
    qDebug() << "Float type is " << casa::TpFloat;

    CCImageBase::SharedPtr res;
    res = tryCast<float>(lat);
    if( ! res) res = tryCast<double>(lat);
    if( ! res) res = tryCast<u_int8_t>(lat);
    if( ! res) res = tryCast<int16_t>(lat);
    if( ! res) res = tryCast<int32_t>(lat);
    if( ! res) res = tryCast<casa::Int>(lat);
    //Certain image related functions are defined only for Int, there is no
    //long long in the image class right now.  TempImage<int32_t> fails to
    //compile, for example.
    //if( ! res) res = tryCast<int64_t>(lat);

    // if we were successful, return the result
    if( res) {
        return res;
    }


    // if the initial conversion attempt failed, try a LEL expression
/*
    casa::ImageInterface<casa::Float> * img = 0;
    try {
        qDebug() << "Trying LEL conversion";
        std::string expr = "float('" + fname.toStdString() + "')";
        qDebug() << "Espression is " << expr.c_str();
        casa::LatticeExpr<casa::Float> le ( casa::ImageExprParse::command( expr ));
        img = new casa::ImageExpr<casa::Float> ( le, expr );
        qDebug() << "\t-LEL conversion successful";
        return CCImage<float>::create( img);
    } catch ( ... ) {} 
*/
    

    // indicate failure
    qWarning() << "Unsupported lattice type:" << lat-> dataType();
    delete lat;
    return nullptr;
}

CasaImageLoader::~CasaImageLoader(){
}
