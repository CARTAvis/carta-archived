/**
 *
 **/

//#include "AstGridPlotter.h"

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

QString AstWcsGridRenderService::_getFitsHeaderforAst(QStringList &fitsHeader)
{
    QStringList AstFitsHeader = fitsHeader;

#if CARTA_RUNTIME_CHECKS > 0
    for(int ii = 0; ii < AstFitsHeader.length(); ii = ii + 1)
    {
        std::cout << AstFitsHeader[ii] << "\n";
    }
#endif

    return AstFitsHeader.join( "" );
}

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
            auto len = header.length();
            std::sort(&header[0],&header[len-2]);
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
//    AstGridPlotter sgp;

//    for ( const QPen & pen : m().pens ) {
//        sgp.pens().push_back( pen );
//    }
    sgp.pens() = m().pens;

    sgp.setInputRect( m_imgRect );
    sgp.setOutputRect( m_outRect );
    sgp.setFitsHeader( _getFitsHeaderforAst(m().fitsHeader) );
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
           sgp.setPlotOption( system );
       }
   }

    // labelOPtion for Ast
    QString labelOPtion = _setDisplayLabelOptionforAst();
    sgp.setPlotOption(labelOPtion);

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

    connect(&sgp, SIGNAL(startPlotSignal()), &sgp, SLOT(startPlotSlot()));

//    connect(&sgp,
//            SIGNAL(plotResultSignal(bool ok)),
//            this,
//            SLOT(plotResultsSlot(bool ok))
//            );
    connect(&sgp,
            SIGNAL(plotResultSignal()),
            this,
            SLOT(plotResultsSlot())
            );

    sgp.moveToThread(&AstGridPlotter::astThread);

    if(AstGridPlotter::astThread.isRunning()==false) {
        qDebug() <<"start ast thread";
        AstGridPlotter::astThread.setObjectName("astThread");

        AstGridPlotter::astThread.start();
    }

    // do the actual plot
    emit sgp.startPlotSignal();

} // startRendering

void AstWcsGridRenderService::plotResultsSlot(){

    // Report the result.
    emit done( m_vgc.vgList(), m().lastSubmittedJobId );
}

QString AstWcsGridRenderService::_setDisplayLabelOptionforAst()
{
    // get PermOrder
    int NumAxis = m_axisDisplayInfos.size();
    int fperm[NumAxis];
    int iperm[NumAxis];
    for(int ii = 0; ii < NumAxis;ii=ii+1)
    {
        auto thisAxis = m_axisDisplayInfos[ii].getPermuteIndex();
        fperm[thisAxis] = ii;
        iperm[ii] = thisAxis;

    }

    QStringList setPlotOption;

    if (!m_axes)
    {
        // turn off
        setPlotOption << "TextLab=0";
        setPlotOption << "NumLab=0";
    }
    else
    {
        for ( int ii = 0; ii < 2; ii++ )
        {
            // Format
            Carta::Lib::AxisLabelInfo::Formats labelFormat = m_labelInfos[ii].getFormat();
            if ( labelFormat == Carta::Lib::AxisLabelInfo::Formats::NONE ||
                 labelFormat == Carta::Lib::AxisLabelInfo::Formats::OTHER)
            {
                // turn off
                setPlotOption << QString("TextLab(%1)=0").arg(ii+1);
                setPlotOption << QString("NumLab(%1)=0").arg(ii+1);
            }
            else
            {
                // digit
                int precision = m_labelInfos[ii].getPrecision();
                //setPlotOption << QString("Digits(%1)=%2").arg(ii+1).arg(precision);

                // location
                Carta::Lib::AxisLabelInfo::Locations labelLocation = m_labelInfos[ii].getLocation();
                QString location = _getDisplayLocation( labelLocation );
                if ( location.length() > 0 )
                {
                    setPlotOption << QString("Edge(%1)=%2").arg(ii+1).arg( location );
                }

                // check axisType
                int thisAxis = fperm[ii];
                Carta::Lib::AxisInfo::KnownType axisType = m_axisDisplayInfos[thisAxis].getAxisType();
                if( axisType == Carta::Lib::AxisInfo::KnownType::DIRECTION_LON)
                {
                    // TODO: check system == B1950/J2000/ICRS[default:hms, d] or Galactic/Ecliptic[default:d] or OTHER
                    // set format
                    if (labelFormat == Carta::Lib::AxisLabelInfo::Formats::HR_MIN_SEC)
                    {
                        setPlotOption << QString("Format(%1)=%2.%3").arg(ii+1).arg("hms").arg(precision);
                    }
                    else if (labelFormat == Carta::Lib::AxisLabelInfo::Formats::DECIMAL_DEG)
                    {
                        setPlotOption << QString("Format(%1)=%2.%3").arg(ii+1).arg("d").arg(precision);
                    }
                    else
                    {
                        setPlotOption << QString("Digits(%1)=%2").arg(ii+1).arg(precision);
                    }

                }
                else if( axisType == Carta::Lib::AxisInfo::KnownType::DIRECTION_LAT)
                {
                    // TODO: check system == B1950/J2000/ICRS[default:dms, d] or Galactic/Ecliptic[default:d] or OTHER
                    // set format
                    if (labelFormat == Carta::Lib::AxisLabelInfo::Formats::DEG_MIN_SEC)
                    {
                        setPlotOption << QString("Format(%1)=%2.%3").arg(ii+1).arg("dms").arg(precision);
                    }
                    else if (labelFormat == Carta::Lib::AxisLabelInfo::Formats::DECIMAL_DEG)
                    {
                        setPlotOption << QString("Format(%1)=%2.%3").arg(ii+1).arg("d").arg(precision);
                    }
                    else
                    {
                        setPlotOption << QString("Digits(%1)=%2").arg(ii+1).arg(precision);
                    }

                }
                else if(axisType == Carta::Lib::AxisInfo::KnownType::SPECTRAL)
                {
                    // check system for spectral
                    setPlotOption << QString("system(%1)=%2").arg(ii+1).arg( "VRAD" );
                    //setPlotOption << QString("system(%1)=%2").arg(ii+1).arg( "Freq" );
                    setPlotOption << QString("Digits(%1)=%2").arg(ii+1).arg(precision);

                    // set unit
                    setPlotOption << QString("Unit(%1)=%2").arg(ii+1).arg("km/s");

                }
                else
                {
                    setPlotOption << QString("Digits(%1)=%2").arg(ii+1).arg(precision);
                }
            }

        }
    }

    return setPlotOption.join(",");

}

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
       system = "System=FK5, Equinox=J2000";
       break;
   case KS::B1950 :
       system = "System=FK4, Equinox=B1950";
       break;
   case KS::ICRS :
       system = "System=ICRS";
       break;
   case KS::Galactic :
       system = "System=GALACTIC";
       break;
   case KS::Ecliptic :
       system = "System=ECLIPTIC";
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
}
