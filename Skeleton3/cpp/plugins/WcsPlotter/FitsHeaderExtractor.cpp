/**
 *
 **/

#include "FitsHeaderExtractor.h"
#include "../CasaImageLoader/CCImage.h"
#include "SimpleFitsParser.h"

//#include <images/Images/ImageInterface.h>

FitsHeaderExtractor::FitsHeaderExtractor()
{ }

FitsHeaderExtractor::~FitsHeaderExtractor()
{ }

void
FitsHeaderExtractor::setInput( Image::ImageInterface::SharedPtr image )
{
    m_cartaImage = image;
}

QStringList
FitsHeaderExtractor::getHeader()
{
    m_errors.clear();
    if ( ! m_cartaImage ) {
        m_errors << "Input is NULL";
        return QStringList();
    }

    QStringList result;

    // was this created using CasaImageLoader plugin?
    CCImageBase * base = dynamic_cast < CCImageBase * > ( & * m_cartaImage );
    if ( base ) {
        casa::LatticeBase * latticeBase = base-> getCasaImage();
        if ( latticeBase ) {
            // see if we can use a simple fits parser first
            if ( latticeBase-> isPersistent() ) {
                result = tryRawFits( latticeBase->name().c_str() );
                if ( ! result.isEmpty() ) {
                    return result;
                }
                qDebug() << "simple fits parser failed";
            }

            // it's not FITS, or simple FITS parser could not read it, try
            // casacore's fits conversion
            result = tryCasaCoreFitsConverter( latticeBase );
        }
        else {
            m_errors << "Cannot produce FITS header for this image because"
                     << "I have no idea where this image came from.";
        }
    }

    return result;
} // getHeader

QStringList
FitsHeaderExtractor::getErrors()
{
    return m_errors;
}

QStringList
FitsHeaderExtractor::tryRawFits( QString fname )
{
    qDebug() << "Trying simple fits parser on" << fname;

    WcsPlotterPluginNS::SimpleFitsParser fitsParser;
    if( ! fitsParser.loadFile( fname)) {
        return QStringList();
    }

    return fitsParser.getHeaderInfo().headerLines;
}

QStringList
FitsHeaderExtractor::tryCasaCoreFitsConverter( casa::LatticeBase * lbase )
{
    // can we get float image interface ?
    casa::ImageInterface < casa::Float > * fii =
        dynamic_cast < casa::ImageInterface < casa::Float > * > ( lbase );
    if( ! fii) {
        m_errors << "Could not convert base to float image";
        return QStringList();
    }

    // let's go bananas
    return QStringList();

}
