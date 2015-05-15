/**
 *
 **/

#include "AstGridPlotter.h"
#include "AstWcsGridRenderService.h"
#include "FitsHeaderExtractor.h"
#include "CartaLib/LinearMap.h"
#include <QPainter>
#include <QTime>

typedef Carta::Lib::LinearMap1D LinMap;

namespace WcsPlotterPluginNS
{
struct AstWcsGridRenderService::Pimpl
{
    QStringList fitsHeader;
//    double lineThickness = 1.0;
    Carta::Lib::KnownSkyCS knownSkyCS = Carta::Lib::KnownSkyCS::J2000;
    std::vector < QPen > pens;

    typedef std::pair<int,double> FontInfo;
    std::vector < FontInfo > fonts;
    QColor shadowColor = QColor( 0, 0, 0, 64 );
};

AstWcsGridRenderService::AstWcsGridRenderService() : IWcsGridRenderService()
{
    qDebug() << "creating ast grid renderer";

    // initialize private members, starting with pimpl
    m_pimpl.reset( new Pimpl );
    m().pens.resize( static_cast < int > ( Element::__count ), QPen( QColor( "white" ) ) );
    Pimpl::FontInfo tuple( 0, 10.0);
    m().fonts.resize( static_cast < int > ( Element::__count ), tuple );

    // setup the render timer
    m_renderTimer.setSingleShot( true);
    connect( & m_renderTimer, & QTimer::timeout, this, & Me::renderNow );
}

AstWcsGridRenderService::~AstWcsGridRenderService()
{ }

void
AstWcsGridRenderService::setInputImage( Image::ImageInterface::SharedPtr image )
{
    m_iimage = image;
    CARTA_ASSERT( m_iimage );

    Pimpl & m = * m_pimpl;

    // get the fits header from this image
    FitsHeaderExtractor fhExtractor;
    fhExtractor.setInput( m_iimage );
    m.fitsHeader = fhExtractor.getHeader();

    // sanity check
    if ( m.fitsHeader.size() < 1 ) {
        qWarning() << "Could not extract fits header..."
                   << fhExtractor.getErrors();
    }
} // setInputImage

void
AstWcsGridRenderService::setOutputSize( const QSize & size )
{
    m_outSize = size;
}

void
AstWcsGridRenderService::setImageRect( const QRectF & rect )
{
    m_imgRect = rect;
}

void
AstWcsGridRenderService::setOutputRect( const QRectF & rect )
{
    m_outRect = rect;
}

//void
//AstWcsGridRenderService::setLineColor( QColor color )
//{
//    m_lineColor = color;
//}

//void
//AstWcsGridRenderService::setLineThickness( double thickness )
//{
//    m().lineThickness = thickness;
//}

void
AstWcsGridRenderService::startRendering()
{
    if( ! m_renderTimer.isActive()) {
        m_renderTimer.start( 1);
    }
}

void AstWcsGridRenderService::renderNow()
{
    QTime t; t.start();

    // clear the current vector graphics in case something goes wrong later
    m_vgList = VGList();

    namespace VG = Carta::Lib::VectorGraphics; // love c++11
    namespace VGE = VG::Entries;
    { } // uncrustify hack to circumvent a bug...

    // make a new VG composer
    VG::VGComposer vgc;

    // add a dim
    {
        double x0 = 0;
        double x1 = m_outRect.left();
        double x2 = m_outRect.right();
        double x3 = m_outSize.width();
        double y0 = 0;
        double y1 = m_outRect.top();
        double y2 = m_outRect.bottom();
        double y3 = m_outSize.height();
        QColor dimColor( 0, 0, 0, 64);
        vgc.append<VGE::FillRect>( QRectF( QPointF(x0,y0),QPointF(x1,y3)), dimColor);
        vgc.append<VGE::FillRect>( QRectF( QPointF(x2,y0),QPointF(x3,y3)), dimColor);
        vgc.append<VGE::FillRect>( QRectF( QPointF(x1,y0),QPointF(x2,y1)), dimColor);
        vgc.append<VGE::FillRect>( QRectF( QPointF(x1,y2),QPointF(x2,y3)), dimColor);
    }


    // get the fits header if we don't have one yet

    LinMap tx( m_imgRect.left(), m_imgRect.right(), m_outRect.left(), m_outRect.right() );
    LinMap ty( m_imgRect.top(), m_imgRect.bottom(), m_outRect.top(), m_outRect.bottom() );

    // draw the grid
    // =============================
    AstGridPlotter sgp;
    for( const QPen & pen : m().pens) {
        sgp.colors().push_back( pen.color());
    }

//    sgp.colors().push_back( m_lineColor );
//    sgp.colors().push_back( m_lineColor );
//    sgp.colors().push_back( m_lineColor );
//    sgp.colors().push_back( QColor( "magenta" ) );
//    sgp.colors().push_back( QColor( "orange" ) );

    sgp.setInputRect( m_imgRect );
    sgp.setOutputRect( m_outRect );
    sgp.setFitsHeader( m().fitsHeader.join( "" ) );
    sgp.setOutputVGComposer( & vgc );
//    sgp.setPlotOption( "tol=0.001" ); // this can slow down the grid rendering!!!
    sgp.setPlotOption( "DrawTitle=0" );

    if( m_internalLabels) {
        sgp.setPlotOption( QString( "Labelling=Interior" ));
    } else {
        sgp.setPlotOption( QString( "Labelling=Exterior" ));
        sgp.setPlotOption( QString( "ForceExterior=1" )); // undocumented AST option
    }

    sgp.setPlotOption( "LabelUp(2)=0" ); // align labels to axes
    sgp.setPlotOption( "Size=9" ); // default font
    sgp.setPlotOption( "TextLab(1)=1" );
    sgp.setPlotOption( "TextLab(2)=1" );

    // local helpers
    auto si = [&]( Element e) { return static_cast<int>(e); };
    auto pi = [&]( Element e) -> QPen & { return m().pens[si(e)]; };
    auto fi = [&]( Element e) -> Pimpl::FontInfo & { return m().fonts[si(e)]; };

    // fonts
    sgp.setPlotOption( QString("Font(TextLab1)=%1").arg( fi( Element::LabelText1).first));
    sgp.setPlotOption( QString("Font(TextLab2)=%1").arg( fi( Element::LabelText2).first));
    sgp.setPlotOption( QString("Font(NumLab1)=%1").arg( fi( Element::NumText1).first));
    sgp.setPlotOption( QString("Font(NumLab2)=%1").arg( fi( Element::NumText2).first));

    // font sizes
    sgp.setPlotOption( QString("Size(TextLab1)=%1").arg( fi( Element::LabelText1).second));
    sgp.setPlotOption( QString("Size(TextLab2)=%1").arg( fi( Element::LabelText2).second));
    sgp.setPlotOption( QString("Size(NumLab1)=%1").arg( fi( Element::NumText1).second));
    sgp.setPlotOption( QString("Size(NumLab2)=%1").arg( fi( Element::NumText2).second));

    // line widths
    sgp.setPlotOption( QString("Width(grid1)=%1").arg( pi( Element::GridLines1).widthF()));
    sgp.setPlotOption( QString("Width(grid2)=%1").arg( pi( Element::GridLines2).widthF()));
    sgp.setPlotOption( QString("Width(border)=%1").arg( pi( Element::BorderLines).widthF()));
    sgp.setPlotOption( QString("Width(axis1)=%1").arg( pi( Element::AxisLines1).widthF()));
    sgp.setPlotOption( QString("Width(axis2)=%1").arg( pi( Element::AxisLines2).widthF()));
    sgp.setPlotOption( QString("Width(ticks1)=%1").arg( pi( Element::TickLines1).widthF()));
    sgp.setPlotOption( QString("Width(ticks2)=%1").arg( pi( Element::TickLines2).widthF()));

    // colors
    sgp.setPlotOption( QString("Colour(grid1)=%1").arg( si( Element::GridLines1)));
    sgp.setPlotOption( QString("Colour(grid2)=%1").arg( si( Element::GridLines2)));
    sgp.setPlotOption( QString("Colour(border)=%1").arg( si( Element::BorderLines)));
    sgp.setPlotOption( QString("Colour(axis1)=%1").arg( si( Element::AxisLines1)));
    sgp.setPlotOption( QString("Colour(axis2)=%1").arg( si( Element::AxisLines2)));
    sgp.setPlotOption( QString("Colour(ticks1)=%1").arg( si( Element::TickLines1)));
    sgp.setPlotOption( QString("Colour(ticks2)=%1").arg( si( Element::TickLines2)));
    sgp.setPlotOption( QString("Colour(NumLab1)=%1").arg( si( Element::NumText1)));
    sgp.setPlotOption( QString("Colour(NumLab2)=%1").arg( si( Element::NumText2)));
    sgp.setPlotOption( QString("Colour(TextLab1)=%1").arg( si( Element::LabelText1)));
    sgp.setPlotOption( QString("Colour(TextLab2)=%1").arg( si( Element::LabelText2)));
    sgp.setShadowPen( pi( Element::Shadow));

    // grid density
    sgp.setDensityModifier( m_gridDensity );

    QString system;
    {
        typedef Carta::Lib::KnownSkyCS KS;
        switch ( m().knownSkyCS )
        {
        case KS::J2000 :
            system = "J2000";
            break;
        case KS::B1950 :
            system = "FK4";
            break;
        case KS::ICRS :
            system = "ICRS";
            break;
        case KS::Galactic :
            system = "GALACTIC";
            break;
        case KS::Ecliptic :
            system = "ECLIPTIC";
            break;
        default :
            system = "";
        } // switch
    }

    if ( ! system.isEmpty() ) {
        sgp.setPlotOption( "System=" + system );
    }

    // do the actual plot
    bool plotSuccess = sgp.plot();
    qDebug() << "plotSuccess" << plotSuccess;
    qDebug() << sgp.getError();

    // save a copy of the VG list from the composer
    m_vgList = vgc.vgList();

    static int counter = 0;
    qDebug() << "Grid rendered in " << t.elapsed() / 1000.0 << "s" << ++ counter << "xyz";

    // Report the result.
    // For debugging purposes we fire off the signal asynchronously.
    if ( ! m_dbgTimer ) {
        m_dbgTimer.reset( new QTimer );
        m_dbgTimer-> setSingleShot( true );
        connect( m_dbgTimer.get(), & QTimer::timeout, this, & Me::reportResult );

//        connect( & m_dbgTimer2, & QTimer::timeout, this, & Me::dbgSlot);
//        m_dbgTimer2.start( 1000 / 60.0);
    }
    m_dbgTimer-> start( qrand() % 1 );
} // startRendering

//double
//AstWcsGridRenderService::lineThickness()
//{
//    return m().lineThickness;
//}

//QColor
//AstWcsGridRenderService::lineColor()
//{
//    return m_lineColor;
//}

void
AstWcsGridRenderService::setGridDensityModifier( double density )
{
    m_gridDensity = density;
}

void
AstWcsGridRenderService::setInternalLabels( bool on )
{
    m_internalLabels = on;
}

void
AstWcsGridRenderService::setSkyCS( Carta::Lib::KnownSkyCS cs )
{
    m().knownSkyCS = cs;
}

void
AstWcsGridRenderService::setPen( Carta::Lib::IWcsGridRenderService::Element e, const QPen & pen )
{
    int ind = static_cast < int > ( e );
    CARTA_ASSERT( ind >= 0 && ind < int ( m().pens.size() ) );
    m().pens[ind] = pen;
}

const QPen &
AstWcsGridRenderService::pen( Carta::Lib::IWcsGridRenderService::Element e )
{
    int ind = static_cast < int > ( e );
    CARTA_ASSERT( ind >= 0 && ind < int ( m().pens.size() ) );
    return m().pens[ind];
}

void
AstWcsGridRenderService::setFont( Carta::Lib::IWcsGridRenderService::Element e, int fontIndex, double pointSize)
{
    int ind = static_cast < int > ( e );
    CARTA_ASSERT( ind >= 0 && ind < int ( m().fonts.size() ) );
    m().fonts[ind] = { fontIndex, pointSize };
}

//const QFont &
//AstWcsGridRenderService::font( Carta::Lib::IWcsGridRenderService::Element e )
//{
//    int ind = static_cast < int > ( e );
//    CARTA_ASSERT( ind >= 0 && ind < int ( m().fonts.size() ) );
//    return m().fonts[ind];
//}

void
AstWcsGridRenderService::setShadowColor( const QColor & color )
{
    m().shadowColor = color;
}

const QColor &
AstWcsGridRenderService::shadowColor()
{
    return m().shadowColor;
}

void
AstWcsGridRenderService::reportResult()
{
    emit done( m_vgList );
}

void
AstWcsGridRenderService::dbgSlot()
{
    m_dbgAngle += 0.1;
    startRendering();
}

inline AstWcsGridRenderService::Pimpl &
AstWcsGridRenderService::m()
{
    return * m_pimpl;
}
}
