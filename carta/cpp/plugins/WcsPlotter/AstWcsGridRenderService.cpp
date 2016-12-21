/**
 *
 **/

#include "AstGridPlotter.h"
#include "AstWcsGridRenderService.h"
#include "FitsHeaderExtractor.h"
#include "CartaLib/LinearMap.h"
#include <QPainter>
#include <QTime>
#include <set>


typedef Carta::Lib::LinearMap1D LinMap;
namespace VG = Carta::Lib::VectorGraphics; // love c++11
namespace VGE = VG::Entries;

namespace WcsPlotterPluginNS
{
inline AstWcsGridRenderService::Element operator++( AstWcsGridRenderService::Element& x ) { return x = (AstWcsGridRenderService::Element)(((int)(x) + 1)); }


struct AstWcsGridRenderService::Pimpl
{
    // we want to remember index and size about fonts
    typedef std::pair < int, double > FontInfo;

    // fits header from the input image
    QStringList fitsHeader;

    // current sky CS
    Carta::Lib::KnownSkyCS knownSkyCS = Carta::Lib::KnownSkyCS::J2000;

    // list of pens
    std::vector < QPen > pens;

    // where in the VG list did we set the pen indices
    std::vector < int64_t > penEntries;

    // where in the VG list did we set the margin dim color
    int64_t dimBrushIndex = - 1;

    // font info
    std::vector < FontInfo > fonts;

    // last submitted job id
    IWcsGridRenderService::JobId lastSubmittedJobId = 0;
};

AstWcsGridRenderService::AstWcsGridRenderService()
    : IWcsGridRenderService(),
      m_labelInfos(2){
    // initialize private members, starting with pimpl
    m_pimpl.reset( new Pimpl );
    // make default pens
    m().pens.resize( static_cast < int > ( Element::__count ), QPen( QColor( "white" ) ) );
    // make default fonts
    Pimpl::FontInfo tuple( 0, 10.0 );
    m().fonts.resize( static_cast < int > ( Element::__count ), tuple );
    // make default pen entries indicating we have not set them yet
    m().penEntries.resize( static_cast < int > ( Element::__count ), - 1 );

    // setup render timer & hook it up
    m_renderTimer.setSingleShot( true );
    connect( & m_renderTimer, & QTimer::timeout, this, & Me::renderNow );
}

AstWcsGridRenderService::~AstWcsGridRenderService()
{ }

void
AstWcsGridRenderService::setInputImage( Carta::Lib::Image::ImageInterface::SharedPtr image )
{
    CARTA_ASSERT( image );
    if ( !m_iimage || m_iimage.get() != image.get() ){
        m_iimage = image;

        // get the fits header from this image
        FitsHeaderExtractor fhExtractor;
        fhExtractor.setInput( m_iimage );
        QStringList header = fhExtractor.getHeader();

        // sanity check
        if ( header.size() < 1 ) {
            qWarning() << "Could not extract fits header..."
                       << fhExtractor.getErrors();
        }

        if ( header != m().fitsHeader ) {
            m_vgValid = false;
            m().fitsHeader = header;
        }
    }
} // setInputImage

void
AstWcsGridRenderService::setOutputSize( const QSize & size )
{
    if ( m_outSize != size ) {
        m_vgValid = false;
        m_outSize = size;
    }
}

void
AstWcsGridRenderService::setImageRect( const QRectF & rect )
{
    if ( m_imgRect != rect ) {
        m_vgValid = false;
        m_imgRect = rect;
    }
}

void
AstWcsGridRenderService::setOutputRect( const QRectF & rect )
{
    if ( m_outRect != rect ) {
        m_vgValid = false;
        m_outRect = rect;
    }
}

Carta::Lib::IWcsGridRenderService::JobId AstWcsGridRenderService::startRendering(JobId jobId)
{
    if( jobId < 0) {
        m().lastSubmittedJobId ++;
    } else {
        m().lastSubmittedJobId = jobId;
    }
    // call renderNow asap... asynchronously
    if ( ! m_renderTimer.isActive() ) {
        m_renderTimer.start( 1 );
    }
    return m().lastSubmittedJobId;
}

void
AstWcsGridRenderService::renderNow()
{
    // if the VGList is still valid, we are done
    if ( m_vgValid ) {
        //qDebug() << "vgValid saved us a grid redraw xyz";
        emit done( m_vgc.vgList(), m().lastSubmittedJobId );
        return;
    }

    QTime t;
    t.start();

    // clear the current vector graphics in case something goes wrong later
//    m_vgList = VGList();
    m_vgc.clear();

    // if the empty grid reporting is activated, report an empty grid
    if ( m_emptyGridFlag ) {
        emit done( m_vgc.vgList(), m().lastSubmittedJobId );
        return;
    }

    m_vgValid = true;

    // local helper - element to integer
    auto si = [&] ( Element e ) {
        return static_cast < int > ( e );
    };

    // element to pen reference
    auto pi = [&] ( Element e ) -> QPen & {
        return m().pens[si( e )];
    };

    // element to font info reference
    auto fi = [&] ( Element e ) -> Pimpl::FontInfo & {
        return m().fonts[si( e )];
    };

    // make a new VG composer
//    VG::VGComposer m_vgc;
//    m_vgc.clear();

    // dim the border
    {
        double x0 = 0;
        double x1 = m_outRect.left();
        double x2 = m_outRect.right();
        double x3 = m_outSize.width();
        double y0 = 0;
        double y1 = m_outRect.top();
        double y2 = m_outRect.bottom();
        double y3 = m_outSize.height();
        m_vgc.append < VGE::Save > ();
        m_vgc.append < VGE::SetPen > ( Qt::NoPen );
        m().dimBrushIndex =
            m_vgc.append < VGE::StoreIndexedBrush > ( 0, QBrush( pi( Element::MarginDim ).brush() ) );
        m_vgc.append < VGE::SetIndexedBrush > ( 0 );
        m_vgc.append < VGE::DrawRect > ( QRectF( QPointF( x0, y0 ), QPointF( x1, y3 ) ) );
        m_vgc.append < VGE::DrawRect > ( QRectF( QPointF( x2, y0 ), QPointF( x3, y3 ) ) );
        m_vgc.append < VGE::DrawRect > ( QRectF( QPointF( x1, y0 ), QPointF( x2, y1 ) ) );
        m_vgc.append < VGE::DrawRect > ( QRectF( QPointF( x1, y2 ), QPointF( x2, y3 ) ) );
        m_vgc.append < VGE::Restore > ();
    }

    // setup indexed pens
    for ( Element e=Element::BorderLines; e!=Element::__count; ++e ) {
        m().penEntries[si( e )] =
            m_vgc.append < VGE::StoreIndexedPen > ( si( e ), pi( e ) );
    }

//    LinMap tx( m_imgRect.left(), m_imgRect.right(), m_outRect.left(), m_outRect.right() );
//    LinMap ty( m_imgRect.top(), m_imgRect.bottom(), m_outRect.top(), m_outRect.bottom() );

    // draw the grid
    // =============================
    AstGridPlotter sgp;

//    for ( const QPen & pen : m().pens ) {
//        sgp.pens().push_back( pen );
//    }
    sgp.pens() = m().pens;

    sgp.setInputRect( m_imgRect );
    sgp.setOutputRect( m_outRect );
    sgp.setFitsHeader( m().fitsHeader.join( "" ) );
    sgp.setAxisDisplayInfo( m_axisDisplayInfos );
    sgp.setOutputVGComposer( & m_vgc );

//    sgp.setPlotOption( "tol=0.001" ); // this can slow down the grid rendering!!!
    sgp.setPlotOption( "DrawTitle=0" );

    if ( !m_gridLines ){
        sgp.setPlotOption( "Grid=0");
    }

    if ( !m_axes ) {
        sgp.setPlotOption("Border=0");
        sgp.setPlotOption("DrawAxes(2)=0");
        sgp.setPlotOption("DrawAxes(1)=0");
        _turnOffTicks( &sgp );
    }
    else {
        if ( !m_ticks ){
            _turnOffTicks(&sgp);
        }
        else {
            sgp.setPlotOption(QString("MinTickLen(1)=%1").arg( m_tickLength ));
            sgp.setPlotOption(QString("MinTickLen(2)=%2").arg( m_tickLength ));
        }
    }

    if ( m_internalLabels ) {
        sgp.setPlotOption( QString( "Labelling=Interior" ) );
    }
    else {
        sgp.setPlotOption( QString( "Labelling=Exterior" ) );
        sgp.setPlotOption( QString( "ForceExterior=1" ) ); // undocumented AST option
    }

    sgp.setPlotOption( "LabelUp(2)=0" ); // align labels to axes
    sgp.setPlotOption( "Size=9" ); // default font

    QString system = _getSystem();
    if ( ! system.isEmpty() ){
       //System only makes sense if the display axes are RA and DEC.
       if ( Carta::Lib::AxisDisplayInfo::isCelestialPlane( m_axisDisplayInfos) ){
           sgp.setPlotOption( "System=" + system );
       }
   }

    //Turn axis  labelling off if we are not drawing the axes.
    if (m_axes){
        int labelCount = m_labels.size();
        for ( int i = 0; i < labelCount; i++ ){
            int axisIndex = i+ 1;
            sgp.setPlotOption( QString("TextLab(%1)=1").arg(axisIndex) );
            if ( m_labels[i].length() > 0 ){
                QString baseLabel = m_labels[i];

                //Format
                Carta::Lib::AxisLabelInfo::Formats labelFormat = m_labelInfos[i].getFormat();
                int precision = m_labelInfos[i].getPrecision();
                QString completeFormat = _getDisplayFormat( labelFormat, precision );
                if ( labelFormat != Carta::Lib::AxisLabelInfo::Formats::NONE ){
                    if ( completeFormat.length() > 0 ){
                        QString format = QString( "Format(%1)=%2").arg(axisIndex).arg( completeFormat );
                        sgp.setPlotOption( format );

                        //Label with format added - seems to be added automatically for J2000.
                        if ( system != "J2000" ){
                            baseLabel = baseLabel +"(" + completeFormat+")";
                        }
                    }
                    else {
                        QString digits = QString( "Digits(%1)=%2").arg(axisIndex).arg(precision);
                        sgp.setPlotOption( digits );
                    }
                    QString label = QString( "Label(%1)=%2").arg(axisIndex).arg( baseLabel);
                    sgp.setPlotOption( label );

                    //Label location
                    Carta::Lib::AxisLabelInfo::Locations labelLocation = m_labelInfos[i].getLocation();
                    QString location = _getDisplayLocation( labelLocation );
                    if ( location.length() > 0 ){
                        QString edgeStr =QString("Edge(%1)=%2").arg(axisIndex).arg( location );
                        sgp.setPlotOption( edgeStr );
                    }
                }
                //If there is no format, turn axis labelling off
                else {
                    _turnOffLabels( &sgp, axisIndex );
                }
            }
        }
    }
    else {
        _turnOffLabels( &sgp, 1 );
        _turnOffLabels( &sgp, 2 );
    }

    // fonts
    sgp.setPlotOption( QString( "Font(TextLab1)=%1" ).arg( fi( Element::LabelText1 ).first ) );
    sgp.setPlotOption( QString( "Font(TextLab2)=%1" ).arg( fi( Element::LabelText2 ).first ) );
    sgp.setPlotOption( QString( "Font(NumLab1)=%1" ).arg( fi( Element::NumText1 ).first ) );
    sgp.setPlotOption( QString( "Font(NumLab2)=%1" ).arg( fi( Element::NumText2 ).first ) );

    // font sizes
    sgp.setPlotOption( QString( "Size(TextLab1)=%1" ).arg( fi( Element::LabelText1 ).second ) );
    sgp.setPlotOption( QString( "Size(TextLab2)=%1" ).arg( fi( Element::LabelText2 ).second ) );
    sgp.setPlotOption( QString( "Size(NumLab1)=%1" ).arg( fi( Element::NumText1 ).second ) );
    sgp.setPlotOption( QString( "Size(NumLab2)=%1" ).arg( fi( Element::NumText2 ).second ) );

    // line widths
//    sgp.setPlotOption( QString( "Width(grid1)=%1" ).arg( pi( Element::GridLines1 ).widthF() ) );
//    sgp.setPlotOption( QString( "Width(grid2)=%1" ).arg( pi( Element::GridLines2 ).widthF() ) );
//    sgp.setPlotOption( QString( "Width(border)=%1" ).arg( pi( Element::BorderLines ).widthF() ) );
//    sgp.setPlotOption( QString( "Width(axis1)=%1" ).arg( pi( Element::AxisLines1 ).widthF() ) );
//    sgp.setPlotOption( QString( "Width(axis2)=%1" ).arg( pi( Element::AxisLines2 ).widthF() ) );
//    sgp.setPlotOption( QString( "Width(ticks1)=%1" ).arg( pi( Element::TickLines1 ).widthF() ) );
//    sgp.setPlotOption( QString( "Width(ticks2)=%1" ).arg( pi( Element::TickLines2 ).widthF() ) );

    // colors
    sgp.setPlotOption( QString( "Colour(grid1)=%1" ).arg( si( Element::GridLines1 ) ) );
    sgp.setPlotOption( QString( "Colour(grid2)=%1" ).arg( si( Element::GridLines2 ) ) );
    sgp.setPlotOption( QString( "Colour(border)=%1" ).arg( si( Element::BorderLines ) ) );
    sgp.setPlotOption( QString( "Colour(axis1)=%1" ).arg( si( Element::AxisLines1 ) ) );
    sgp.setPlotOption( QString( "Colour(axis2)=%1" ).arg( si( Element::AxisLines2 ) ) );
    sgp.setPlotOption( QString( "Colour(ticks1)=%1" ).arg( si( Element::TickLines1 ) ) );
    sgp.setPlotOption( QString( "Colour(ticks2)=%1" ).arg( si( Element::TickLines2 ) ) );
    sgp.setPlotOption( QString( "Colour(NumLab1)=%1" ).arg( si( Element::NumText1 ) ) );
    sgp.setPlotOption( QString( "Colour(NumLab2)=%1" ).arg( si( Element::NumText2 ) ) );
    sgp.setPlotOption( QString( "Colour(TextLab1)=%1" ).arg( si( Element::LabelText1 ) ) );
    sgp.setPlotOption( QString( "Colour(TextLab2)=%1" ).arg( si( Element::LabelText2 ) ) );

    sgp.setShadowPenIndex( si( Element::Shadow ) );



//    sgp.setPlotOption( "Format(1)=\"+tms.10\"");
//            sgp.setPlotOption( "Format(1)=\"gtms\"");
    // grid density
    sgp.setDensityModifier( m_gridDensity );



    // do the actual plot
    bool plotSuccess = sgp.plot();
//    qDebug() << "plotSuccess=" << plotSuccess;
//    qDebug() << "plotError=" << sgp.getError();
    if( ! plotSuccess) {
        qWarning() << "Grid rendering error:" << sgp.getError();
    }

    //qDebug() << "Grid rendered in " << t.elapsed() / 1000.0 << "s";

    // Report the result.
    emit done( m_vgc.vgList(), m().lastSubmittedJobId );
} // startRendering

void AstWcsGridRenderService::setAxisDisplayInfo( std::vector<Carta::Lib::AxisDisplayInfo> displayInfos ){
    if ( displayInfos.size() != m_axisDisplayInfos.size()){
        m_axisDisplayInfos = displayInfos;
        m_vgValid = false;
    }
    else {
        int infoCount = displayInfos.size();
        for ( int i = 0; i < infoCount; i++ ){
            if ( displayInfos[i] != m_axisDisplayInfos[i] ){
                m_axisDisplayInfos[i] = displayInfos[i];
                //m_vgValid = false;
            }
        }
    }
}


void AstWcsGridRenderService::setAxesVisible( bool flag ){
    if ( m_axes != flag ){
        m_vgValid = false;
        m_axes = flag;
    }
}

void
AstWcsGridRenderService::setGridDensityModifier( double density )
{
    if ( m_gridDensity != density ) {
        m_gridDensity = density;
        m_vgValid = false;
    }
}

void
AstWcsGridRenderService::setInternalLabels( bool flag )
{
    if ( m_internalLabels != flag ) {
        m_vgValid = false;
        m_internalLabels = flag;
    }
}

void
AstWcsGridRenderService::setGridLinesVisible( bool flag ){
    if ( m_gridLines != flag ){
        m_vgValid = false;
        m_gridLines = flag;
    }
}

void
AstWcsGridRenderService::setSkyCS( Carta::Lib::KnownSkyCS cs )
{
    // invalidate vglist if the requested coordinate system is different
    // from the last one
    if ( m().knownSkyCS != cs ) {
        m_vgValid = false;
        m().knownSkyCS = cs;
    }
}



void
AstWcsGridRenderService::setPen( Carta::Lib::IWcsGridRenderService::Element e, const QPen & pen )
{
    int ind = static_cast < int > ( e );
    CARTA_ASSERT( ind >= 0 && ind < int ( m().pens.size() ) );
    m().pens[ind] = pen;

    auto si = [&] ( Element e ) {
        return static_cast < int > ( e );
    };
    auto pi = [&] ( Element e ) -> QPen & {
        return m().pens[si( e )];
    };

    // if the list is valid, just change the entry directly
    if ( m_vgValid ) {
    	int penIndex=m().penEntries[si(e)];
        m_vgc.set < VGE::StoreIndexedPen > ( penIndex, si( e ), pi( e ) );
        int brushIndex = m().dimBrushIndex;
        if ( e == Element::MarginDim && brushIndex >= 0 ) {
            m_vgc.set < VGE::StoreIndexedBrush > ( brushIndex, 0, pi( e ).brush() );
        }
    }
} // setPen


void AstWcsGridRenderService::setAxisLabelInfo( int axisIndex, const Carta::Lib::AxisLabelInfo& labelInfo ){
    CARTA_ASSERT( axisIndex == 0 || axisIndex == 1 );

    if ( m_labelInfos[axisIndex] != labelInfo ){
        m_vgValid = false;
        m_labelInfos[axisIndex] = labelInfo;
    }
}

void
AstWcsGridRenderService::setAxisLabel( int axisIndex, const QString& label ){
    CARTA_ASSERT( axisIndex == 0 || axisIndex ==1 );
    CARTA_ASSERT( !label.isEmpty() );
    if ( m_labels[axisIndex] != label ){
        m_vgValid = false;
        m_labels[axisIndex] = label;
    }
}

QString AstWcsGridRenderService::_getDisplayFormat( const Carta::Lib::AxisLabelInfo::Formats& baseFormat,
        int decimals ) const {
    QString displayFormat = "";
    //Standard behaviour for an HMS axis is to have one extra decimal
    //place compared to a DMS axis so they have roughly the same precision
    //(an hour of arc is bigger than a degree of arc).
    //Implemented by subtracting one from decimals (when it is positive) in the case of dms
    int actualDecimals = decimals;
    if ( baseFormat == Carta::Lib::AxisLabelInfo::Formats::DEG_MIN_SEC ){
        displayFormat = "dms";
        if ( decimals > 0 ){
            actualDecimals = decimals - 1;
        }
    }
    else if ( baseFormat == Carta::Lib::AxisLabelInfo::Formats::DECIMAL_DEG ){
        displayFormat = "d";
    }
    else if ( baseFormat == Carta::Lib::AxisLabelInfo::Formats::HR_MIN_SEC ){
        displayFormat = "hms";
    }

    if ( displayFormat.length() > 0 ){
        if ( actualDecimals > 0 ){
            displayFormat = displayFormat + "."+QString::number(actualDecimals);
        }
    }
    return displayFormat;
}

QString AstWcsGridRenderService::_getDisplayLocation( const Carta::Lib::AxisLabelInfo::Locations& labelLocation ) const {
    QString location = "";
    if ( labelLocation == Carta::Lib::AxisLabelInfo::Locations::EAST ){
        location = "left";
    }
    else if ( labelLocation == Carta::Lib::AxisLabelInfo::Locations::WEST ){
        location = "right";
    }
    else if ( labelLocation == Carta::Lib::AxisLabelInfo::Locations::NORTH ){
        location = "top";
    }
    else if ( labelLocation == Carta::Lib::AxisLabelInfo::Locations::SOUTH ){
        location = "bottom";
    }
    return location;
}



//const QPen &
//AstWcsGridRenderService::pen( Carta::Lib::IWcsGridRenderService::Element e )
//{
//    int ind = static_cast < int > ( e );
//    CARTA_ASSERT( ind >= 0 && ind < int ( m().pens.size() ) );
//    return m().pens[ind];
//}

void
AstWcsGridRenderService::setFont( Carta::Lib::IWcsGridRenderService::Element e,
                                  int fontIndex,
                                  double pointSize )
{
    int ind = static_cast < int > ( e );
    CARTA_ASSERT( ind >= 0 && ind < int ( m().fonts.size() ) );
    Pimpl::FontInfo fontInfo {
        fontIndex, pointSize
    };

    if ( m().fonts[ind] != fontInfo ) {
        m_vgValid = false;
        m().fonts[ind] = fontInfo;
    }
}

void
AstWcsGridRenderService::setEmptyGrid( bool flag )
{
    if ( m_emptyGridFlag != flag ) {
        m_vgValid = false;
        m_emptyGridFlag = flag;
    }
}

void
AstWcsGridRenderService::setTickLength( double length )
{
    CARTA_ASSERT( length >= 0 );
    if ( m_tickLength != length ){
        m_vgValid = false;
        m_tickLength = length;
    }
}

void
AstWcsGridRenderService::setTicksVisible( bool flag )
{
    if ( m_ticks != flag ){
        m_vgValid = false;
        m_ticks = flag;
    }
}

inline AstWcsGridRenderService::Pimpl &
AstWcsGridRenderService::m()
{
    return * m_pimpl;
}

QString
AstWcsGridRenderService::_getSystem( ){
   QString system;
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


   return system;
}

void
AstWcsGridRenderService::_turnOffTicks(WcsPlotterPluginNS::AstGridPlotter* sgp){
    sgp->setPlotOption("MajTickLen(1)=0");
    sgp->setPlotOption("MajTickLen(2)=0");
    sgp->setPlotOption("MinTickLen(1)=0");
    sgp->setPlotOption("MinTickLen(2)=0");
}

void AstWcsGridRenderService::_turnOffLabels( WcsPlotterPluginNS::AstGridPlotter* sgp, int index ){
    sgp->setPlotOption( QString("TextLab(%1)=0").arg(index));
    sgp->setPlotOption( QString("NumLab(%1)=0").arg(index));
}
}
