/**
 *
 **/

#include "WcsGridOptionsController.h"
#include "core/Globals.h"
#include <functional>

namespace Hacks
{
WcsGridOptionsController::WcsGridOptionsController(
    QObject * parent,
    QString statePrefix,
    Carta::Lib::IWcsGridRenderService::SharedPtr
    wcsGridRenderService
    ) : QObject( parent )
{
    auto & globals = * Globals::instance();
    m_connector = globals.connector();
    m_statePrefix = statePrefix;
    if( ! m_statePrefix.endsWith( '/')) {
        m_statePrefix.append( '/');
    }

    m_wcsGridRenderer = wcsGridRenderService;

    const SS::FullPath prefix = SS::FullPath::fromQString( m_statePrefix);

#define SetupVar( vname, type, path, initValue) \
    vname.reset( new type( prefix.with(path))); \
    vname-> set( initValue); \
    connect( vname.get(), & type::valueChanged, this, & Me::stdVarCB);

    SetupVar( m_internalLabels, SS::BoolVar,
              "internalLabels", false);
    SetupVar( m_gridDensityModifier, SS::DoubleVar,
              "gridDensity", 1.0);
    SetupVar( m_borderLinesPen, SS::TypedVariable<QPen>,
              "borderLines", QPen( QColor( 0, 255, 0), 2));
    SetupVar( m_axisLines1Pen, SS::TypedVariable<QPen>,
              "axisLines1", QPen( QColor( 0, 0, 180), 2));
    SetupVar( m_axisLines2Pen, SS::TypedVariable<QPen>,
              "axisLines2", QPen( QColor( 255, 0, 180), 2));
    SetupVar( m_gridLines1Pen, SS::TypedVariable<QPen>,
              "gridLines1", QPen( QColor( 255, 255, 255), 1));
    SetupVar( m_gridLines2Pen, SS::TypedVariable<QPen>,
              "gridLines2", QPen( QColor( 255, 255, 255, 255), 1));
    SetupVar( m_tickLines1Pen, SS::TypedVariable<QPen>,
              "tickLines1", QPen( QColor( "white"), 1));
    SetupVar( m_tickLines2Pen, SS::TypedVariable<QPen>,
              "tickLines2", QPen( QColor( "yellow"), 1));

    SetupVar( m_numText1Pen, SS::TypedVariable<QPen>,
              "numText1pen", QPen( QColor( "white"), 1));
    SetupVar( m_numText2Pen, SS::TypedVariable<QPen>,
              "numText2pen", QPen( QColor( "white"), 1));
    SetupVar( m_labelText1Pen, SS::TypedVariable<QPen>,
              "labelText1pen", QPen( QColor( "orange"), 1));
    SetupVar( m_labelText2Pen, SS::TypedVariable<QPen>,
              "labelText2pen", QPen( QColor( "yellow"), 1));

    SetupVar( m_shadowPen, SS::TypedVariable<QPen>,
              "shadowPen", QPen( QColor( 0,0,0,64), 3));

    SetupVar( m_marginDimPen, SS::TypedVariable<QPen>,
              "marginDimPen", QPen( QColor( 0,0,0,64), 1));

    SetupVar( m_numText1FontIndex, SS::IntVar,
              "numText1FontIndex", 0);
    SetupVar( m_numText1FontSize, SS::DoubleVar,
              "numText1FontSize", 10);

    SetupVar( m_currentSkyCS, SS::IntVar,
              "skyCS", static_cast<int>(Carta::Lib::KnownSkyCS::Galactic));


#undef SetupVar

    stdVarCB();
}

//void WcsGridOptionsController::lineThicknessCB(const QString &, const QString & val)
//{
//    bool ok;
//    double t = val.toDouble( & ok );
//    if ( ! ok ) {
//        return;
//    }
//    m_wcsGridRenderer-> setLineThickness( t );
//    m_wcsGridRenderer-> startRendering();

//}

void WcsGridOptionsController::stdVarCB()
{
    m_wcsGridRenderer-> setInternalLabels( m_internalLabels-> get());
    m_wcsGridRenderer-> setGridDensityModifier( m_gridDensityModifier-> get());
    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::GridLines1,
                                m_gridLines1Pen-> get());
    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::GridLines2,
                                m_gridLines2Pen-> get());
    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::TickLines1,
                                m_tickLines1Pen-> get());
    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::TickLines2,
                                m_tickLines2Pen-> get());
    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::AxisLines1,
                                m_axisLines1Pen-> get());
    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::AxisLines2,
                                m_axisLines2Pen-> get());
    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::BorderLines,
                                m_borderLinesPen-> get());

    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::NumText1,
                                m_numText1Pen-> get());
    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::NumText2,
                                m_numText2Pen-> get());
    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::LabelText1,
                                m_labelText1Pen-> get());
    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::LabelText2,
                                m_labelText2Pen-> get());

    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::Shadow,
                                m_shadowPen-> get());
    m_wcsGridRenderer-> setPen( Carta::Lib::IWcsGridRenderService::Element::MarginDim,
                                m_marginDimPen-> get());

    m_wcsGridRenderer-> setFont( Carta::Lib::IWcsGridRenderService::Element::NumText1,
                                 m_numText1FontIndex-> get(),
                                 m_numText1FontSize-> get());

    m_wcsGridRenderer-> setFont( Carta::Lib::IWcsGridRenderService::Element::NumText2,
                                 m_numText1FontIndex-> get(),
                                 m_numText1FontSize-> get());
    m_wcsGridRenderer-> setFont( Carta::Lib::IWcsGridRenderService::Element::LabelText1,
                                 m_numText1FontIndex-> get(),
                                 m_numText1FontSize-> get());
    m_wcsGridRenderer-> setFont( Carta::Lib::IWcsGridRenderService::Element::LabelText2,
                                 m_numText1FontIndex-> get(),
                                 m_numText1FontSize-> get());


    // coordinate system
    typedef Carta::Lib::KnownSkyCS CS;
    CS skyCS = Carta::Lib::int2knownSkyCS( m_currentSkyCS-> get());
    if( skyCS != CS::Error) {
        m_wcsGridRenderer-> setSkyCS( skyCS);
    } else {
        qWarning() << "Unknown sky cs requested from client:" << m_currentSkyCS-> get();
    }

    // let everyone know something was updated
    emit updated();
}

}
