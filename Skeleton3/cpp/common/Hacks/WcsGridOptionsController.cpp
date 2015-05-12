/**
 *
 **/

#include "WcsGridOptionsController.h"
#include "common/Globals.h"
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
              "gridDensity", 0.5);
    SetupVar( m_borderLinesPen, SS::TypedVariable<QPen>,
              "borderLines", QPen( QColor( 0, 255, 0), 2));
    SetupVar( m_axisLines1Pen, SS::TypedVariable<QPen>,
              "aixsLines1", QPen( QColor( 0, 0, 180), 2));
    SetupVar( m_axisLines2Pen, SS::TypedVariable<QPen>,
              "aixsLines2", QPen( QColor( 255, 0, 180), 2));
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
              "numText2pen", QPen( QColor( "purple"), 1));
    SetupVar( m_labelText1Pen, SS::TypedVariable<QPen>,
              "labelText1pen", QPen( QColor( "orange"), 1));
    SetupVar( m_labelText2Pen, SS::TypedVariable<QPen>,
              "labelText2pen", QPen( QColor( "gray"), 1));

    SetupVar( m_shadowPen, SS::TypedVariable<QPen>,
              "shadowPen", QPen( QColor( 0,0,0,64), 1));


#undef SetupVar

    //
    // add callbacks for state changes
    // ====================================
    using namespace std::placeholders;
    // line thickness
    m_connector-> addStateCallback(
                m_statePrefix + "lineThickness",
                std::bind( & Me::lineThicknessCB, this, _1, _2 ));
    // line opacity
    m_connector-> addStateCallback(
                m_statePrefix + "lineOpacity",
                std::bind( & Me::lineOpacityCB, this, _1, _2 ));

    // set up some defaults
    double lineThickness = 0.5;
    m_connector-> setState( m_statePrefix + "lineThickness", QString::number( lineThickness));
    m_wcsGridRenderer-> setLineThickness( lineThickness);
    QColor lineColor( 255, 255, 0, 255);
    m_connector-> setState( m_statePrefix + "lineOpacity", QString::number( lineColor.alphaF()));
    m_wcsGridRenderer-> setLineColor( lineColor);

    m_wcsGridRenderer-> setSkyCS( Carta::Lib::KnownSkyCS::Galactic);

    stdVarCB();
}

void WcsGridOptionsController::lineThicknessCB(const QString &, const QString & val)
{
    bool ok;
    double t = val.toDouble( & ok );
    if ( ! ok ) {
        return;
    }
    m_wcsGridRenderer-> setLineThickness( t );
    m_wcsGridRenderer-> startRendering();

}

//void WcsGridOptionsController::gridDensityCB(const QString &, const QString & val)
//{
//    bool ok;
//    double t = val.toDouble( & ok );
//    if ( ! ok ) {
//        return;
//    }
//    m_wcsGridRenderer-> setGridDensity( t );
//    m_wcsGridRenderer-> startRendering();

//}

//void WcsGridOptionsController::internalLabelsCB(const QString &, const QString & val)
//{
//    bool t = val.simplified() == "1";
//    m_wcsGridRenderer-> setInternalLabels( t);
//    m_wcsGridRenderer-> startRendering();

//}

void WcsGridOptionsController::stdVarCB()
{
    qDebug() << "stdVarCB";
    qDebug() << "   inetrnalLabels" << m_internalLabels-> get();
    qDebug() << "   gridDensity" << m_gridDensityModifier-> get();
    qDebug() << "   gridLines1" << m_gridLines1Pen-> get();

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

    m_wcsGridRenderer-> startRendering();


}

void WcsGridOptionsController::lineOpacityCB(const QString &, const QString & val)
{
    bool ok;
    double t = val.toDouble( & ok );
    if ( ! ok ) {
        return;
    }
    QColor color = m_wcsGridRenderer-> lineColor();
    color.setAlphaF( t);
    m_wcsGridRenderer-> setLineColor( color);
    m_wcsGridRenderer-> startRendering();
}

}
