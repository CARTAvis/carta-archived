/**
 *
 **/

#include "AstGridPlotter.h"
#include "AstGridRenderer.h"
#include "FitsHeaderExtractor.h"
#include "CartaLib/LinearMap.h"
#include <QPainter>

typedef Carta::Lib::LinearMap1D LinMap;

namespace WcsPlotterPluginNS
{
struct WcsGridRendererAst::Pimpl
{
    QStringList fitsHeader;
};

WcsGridRendererAst::WcsGridRendererAst() : IWcsGridRenderService()
{
    qDebug() << "creating ast grid renderer";
    m_pimpl.reset( new Pimpl );
}

WcsGridRendererAst::~WcsGridRendererAst()
{ }

void
WcsGridRendererAst::setInputImage( Image::ImageInterface::SharedPtr image )
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
WcsGridRendererAst::setOutputSize( const QSize & size )
{
    m_outSize = size;
}

void
WcsGridRendererAst::setImageRect( const QRectF & rect )
{
    m_imgRect = rect;
}

void
WcsGridRendererAst::setOutputRect( const QRectF & rect )
{
    m_outRect = rect;
}

void
WcsGridRendererAst::setLineColor( QColor color )
{
    m_lineColor = color;
}

void
WcsGridRendererAst::setLineThickness( double thickness )
{
    m_lineThickness = thickness;
}

void
WcsGridRendererAst::startRendering()
{
    Pimpl & m = * m_pimpl;

    // clear the current vector graphics in case something goes wrong later
    m_vgList = VGList();

    namespace VG = Carta::Lib::VectorGraphics; // love c++11
    namespace VGE = VG::Entries;
    { } // uncrustify hack to circumvent a bug...

    // make a new VG composer
    VG::VGComposer vgc;

    // get the fits header if we don't have one yet

    LinMap tx( m_imgRect.left(), m_imgRect.right(), m_outRect.left(), m_outRect.right() );
    LinMap ty( m_imgRect.top(), m_imgRect.bottom(), m_outRect.top(), m_outRect.bottom() );

    if ( false && m_iimage && m_iimage-> dims().size() >= 2 ) {
        QPointF c( m_iimage-> dims()[0] / 2, m_iimage-> dims()[1] / 2 );
        c = c - QPointF( 0.5, 0.5 );
        int size = 10;
        QRectF r1( c - QPointF( size / 2.0, size / 2.0 ), c + QPointF( size / 2.0, size / 2.0 ) );
        QRectF r2( QPointF( tx( r1.left() ), ty( r1.top() ) ),
                   QPointF( tx( r1.right() ), ty( r1.bottom() ) ) );

        vgc.append < VGE::DrawLine > ( r2.topLeft(), r2.topRight() );
        vgc.append < VGE::DrawLine > ( r2.topRight(), r2.bottomRight() );
        vgc.append < VGE::DrawLine > ( r2.bottomRight(), r2.bottomLeft() );
        vgc.append < VGE::DrawLine > ( r2.bottomLeft(), r2.topLeft() );

        vgc.append < VGE::DrawLine > ( QPointF( 0, 0 ), r2.topLeft() );
    }

    // draw the grid
    // =============================
    AstGridPlotterQImage sgp;
    sgp.colors().push_back( m_lineColor);
    sgp.colors().push_back( m_lineColor);
    sgp.colors().push_back( m_lineColor);
    sgp.colors().push_back( QColor( "magenta"));
    sgp.colors().push_back( QColor( "orange"));

    sgp.setInputRect( m_imgRect );
    sgp.setOutputRect( m_outRect );
    sgp.setFitsHeader( m.fitsHeader.join( "" ) );
    sgp.setOutputVGComposer( & vgc );
    sgp.setPlotOption( "tol=0.001" );
//    sgp.setPlotOption( "Labelling=Exterior" );
    sgp.setPlotOption( QString("Labelling=%1").arg(
                           m_internalLabels ? "Interior" : "Exterior") );
    sgp.setPlotOption( "DrawTitle=0" );
    sgp.setPlotOption( QString( "Width(axes)=%1" ).arg( m_lineThickness * 2 ) );
    sgp.setPlotOption( QString( "Width(border)=%1" ).arg( m_lineThickness * 2 ) );
    sgp.setPlotOption( QString( "Width(grid)=%1" ).arg( m_lineThickness ) );

    sgp.setPlotOption( "LabelUp(2)=0" );
    sgp.setPlotOption( "Size=9" );
    sgp.setPlotOption( "TextLab(1)=1" );
    sgp.setPlotOption( "TextLab(2)=1" );
    sgp.setPlotOption( "Size(TextLab1)=11" );
    sgp.setPlotOption( "Size(TextLab2)=11" );
    sgp.setPlotOption( "Colour(grid1)=0");
    sgp.setPlotOption( "Colour(grid2)=1");
    sgp.setPlotOption( "Colour(border)=2");
    sgp.setPlotOption( "Colour(TextLab)=3");
    sgp.setPlotOption( "Colour(NumLab)=4");
//    sgp.setPlotOption( QString("Gap=%1").arg( m_gridDensity));
    sgp.setDensityModifier( m_gridDensity);
    bool plotSuccess = sgp.plot();
    qDebug() << "plotSuccess" << plotSuccess;
    qDebug() << sgp.getError();

//    QTransform tr;
//    tr.translate( m_outRect.width() / 2.0, m_outRect.height() / 2.0);
//    tr.rotate( m_dbgAngle);
//    tr.translate( - m_outRect.width() / 2.0, - m_outRect.height() / 2.0);
//    vgc.append<VGE::Save>();
//    vgc.append<VGE::SetTransform>(tr);
//    vgc.append<VGE::DrawText> ("Hello", QPointF( 100, 100));
//    vgc.append<VGE::Restore>();

    // save a copy of the VG list from the composer
    m_vgList = vgc.vgList();

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

double
WcsGridRendererAst::lineThickness()
{
    return m_lineThickness;
}

QColor
WcsGridRendererAst::lineColor()
{
    return m_lineColor;
}

void WcsGridRendererAst::setGridDensity(double density)
{
    m_gridDensity = density;
}

void WcsGridRendererAst::setInternalLabels(bool on)
{
    m_internalLabels = on;
}

void
WcsGridRendererAst::reportResult()
{
    emit done( m_vgList );
}

void
WcsGridRendererAst::dbgSlot()
{
    m_dbgAngle += 0.1;
    startRendering();
}
}
