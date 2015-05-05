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
    // grid density
    m_connector-> addStateCallback(
                m_statePrefix + "gridDensity",
                std::bind( & Me::gridDensityCB, this, _1, _2 ));

    // internal labels
    m_connector-> addStateCallback(
                m_statePrefix + "internalLabels",
                std::bind( & Me::internalLabelsCB, this, _1, _2 ));

    // set up some defaults
    double lineThickness = 0.5;
    m_connector-> setState( m_statePrefix + "lineThickness", QString::number( lineThickness));
    m_wcsGridRenderer-> setLineThickness( lineThickness);
    QColor lineColor( 255, 255, 0, 255);
    m_connector-> setState( m_statePrefix + "lineOpacity", QString::number( lineColor.alphaF()));
    m_wcsGridRenderer-> setLineColor( lineColor);
    double gridDensity = 1.0;
    m_connector-> setState( m_statePrefix + "gridDensity", QString::number( gridDensity));
    m_wcsGridRenderer-> setGridDensity( gridDensity);
    bool internalLabels = true;
    m_connector-> setState( m_statePrefix + "internalLabels", internalLabels ? "1" : "0");
    m_wcsGridRenderer-> setInternalLabels( internalLabels);
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

void WcsGridOptionsController::gridDensityCB(const QString &, const QString & val)
{
    bool ok;
    double t = val.toDouble( & ok );
    if ( ! ok ) {
        return;
    }
    m_wcsGridRenderer-> setGridDensity( t );
    m_wcsGridRenderer-> startRendering();

}

void WcsGridOptionsController::internalLabelsCB(const QString &, const QString & val)
{
    bool t = val.simplified() == "1";
    m_wcsGridRenderer-> setInternalLabels( t);
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
