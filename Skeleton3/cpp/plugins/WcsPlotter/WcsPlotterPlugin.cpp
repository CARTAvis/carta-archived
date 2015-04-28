#include "WcsPlotterPlugin.h"
#include "AstGridPlotter.h"
#include "CartaLib/Hooks/DrawWcsGrid.h"
#include "../CasaImageLoader/CCImage.h"
#include "AstGridRenderer.h"
#include "FitsHeaderExtractor.h"
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



namespace WcsPlotterPluginNS
{


//static QString fixedHdr() {
//    QStringList ss;

//    ss << "SIMPLE  =                    T /  Standard FITS file";
//    ss << "BITPIX  =                  -32 /  IEEE 4-byte float";
//    ss << "NAXIS   =                    3 /  Number of image dimensions";
//    ss << "NAXIS1  =                  500 /  1st dimension in pixels";
//    ss << "NAXIS2  =                  399 /  2nd dimension in pixels";
//    ss << "NAXIS3  =                  350 /  3rd dimension in pixels";
//    ss << "OBJECT  = 'GALFACTS_FIELD1 Stokes I'";
//    ss << "CTYPE1  = 'RA---CAR'           /  1st axis type";
//    ss << "CRVAL1  =            63.000000 /  Reference pixel value";
//    ss << "CRPIX1  =               1549.5 /  Reference pixel location";
//    ss << "CDELT1  =           -0.0166667 /  Pixel size in world coordinate units";
//    ss << "CROTA1  =               0.0000 /  Axis rotation in degrees";
//    ss << "CTYPE2  = 'DEC--CAR'           /  2nd axis type";
//    ss << "CRVAL2  =            28.650000 /  Reference pixel value";
//    ss << "CRPIX2  =                256.5 /  Reference pixel location";
//    ss << "CDELT2  =            0.0166667 /  Pixel size in world coordinate units";
//    ss << "CROTA2  =               0.0000 /  Axis rotation in degrees";
//    ss << "CTYPE3  = 'FREQ'               /  3rd axis type";
//    ss << "CRVAL3  =           1525705000 /  Value at reference pixel";
//    ss << "CRPIX3  =                    0 /  Reference pixel";
//    ss << "CDELT3  =              -420000 /  Pixel size";
//    ss << "CROTA3  =               0.0000 /  Axis rotation in degrees";
//    ss << "EQUINOX =              2000.00 /  Equinox of coordinates (if any)";
//    ss << "BUNIT   = 'Kelvin'";
//    ss << "END";

//    QString res;
//    for( QString s : ss) {
//        res.append( s.leftJustified( 80, ' ', true));
//    }
//    return res;
//}

typedef Carta::Lib::Hooks::DrawWcsGridHook DrawWcsGrid;
typedef Carta::Lib::Hooks::GetWcsGridRendererHook GetWcsGridRendererHook;

WcsPlotterPlugin::WcsPlotterPlugin( QObject * parent ) :
    QObject( parent )
{ }

bool
WcsPlotterPlugin::handleHook( BaseHook & hookData )
{
    qDebug() << "WcsPlotter plugin is handling hook #" << hookData.hookId();
    if ( hookData.is < Initialize > () ) {
        return true;
    }
    else if ( hookData.is < DrawWcsGrid > () ) {
        DrawWcsGrid & hook = static_cast < DrawWcsGrid & > ( hookData );
        auto & params = * hook.paramsPtr;
        auto image = hook.paramsPtr-> m_astroImage;

        hook.result = QImage( params.outputSize, QImage::Format_ARGB32_Premultiplied );
        hook.result.fill( QColor( 0, 0, 0, 0 ) );

        FitsHeaderExtractor fhExtractor;
        fhExtractor.setInput( params.m_astroImage );
        QString hdr = fhExtractor.getHeader().join( "" );
        qDebug() << "hdr:\n" << hdr;
        if ( hdr.isEmpty() ) {
            qWarning() << "Could not get header\n"
                       << fhExtractor.getErrors();
            return false;
        }
        AstGridPlotterQImage sgp;
        sgp.setInputRect( params.frameRect );
        sgp.setOutputRect( params.outputRect );
        sgp.setFitsHeader( hdr );
        sgp.setOutputImage( & hook.result );

//        sgp.setFitsHeader( fixedHdr());

        sgp.setPlotOption( "tol=0.001" );
        sgp.setPlotOption( "Labelling=Exterior" );

//        sgp.setPlotOption( "Labelling=Interior");
        sgp.setPlotOption( "DrawTitle=0" );
        sgp.setPlotOption( "Width(axes)=2" );
        sgp.setPlotOption( "Width(border)=2" );
        sgp.setPlotOption( "Width(Grid)=0.5" );
        sgp.setPlotOption( "LabelUp(2)=0" );
        sgp.setPlotOption( "Size=9" );
        sgp.setPlotOption( "TextLab(1)=1" );
        sgp.setPlotOption( "TextLab(2)=1" );
        sgp.setPlotOption( "Size(TextLab1)=11" );
        sgp.setPlotOption( "Size(TextLab2)=11" );

        bool plotSuccess = sgp.plot();
        qDebug() << "plot error" << plotSuccess;
        qDebug() << "===>" << sgp.getError();

        // return true if result is not null
        return plotSuccess;
    }
    else if ( hookData.is < GetWcsGridRendererHook > () ) {
        auto & hook = static_cast < GetWcsGridRendererHook & > ( hookData );
        hook.result.reset( new WcsGridRendererAst() );
        return true;
    }

    qCritical() << "Sorrry, WcsPlotterPlugin doesnt' know how to handle this hook" << hookData.hookId();
    return false;
} // handleHook

std::vector < HookId >
WcsPlotterPlugin::getInitialHookList()
{
    return {
               Initialize::staticId,
//               DrawWcsGrid::staticId,
               GetWcsGridRendererHook::staticId
    };
}

/*
template < typename T >
static CCImageBase::SharedPtr
tryCast( casa::LatticeBase * lat )
{
    typedef casa::ImageInterface < T > CCIT;
    CCIT * cii = dynamic_cast < CCIT * > ( lat );
    typename CCImage < T >::SharedPtr res = nullptr;
    if ( cii ) {
        res = CCImage < T >::create( cii );
    }
    return res;
}
*/
}
