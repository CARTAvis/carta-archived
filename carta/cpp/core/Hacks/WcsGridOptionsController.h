/**
 * Helper class to manage UI related to grid options.
 *
 * It monitors the shared state, and automatically tells the grid render service if
 * any changes are detected.
 **/

#pragma once

#include "CartaLib/IWcsGridRenderService.h"
#include "CartaLib/CartaLib.h"
#include "core/IConnector.h"
#include "core/Hacks/SharedState.h"
#include <QObject>

namespace Hacks
{
namespace SS = Carta::Lib::SharedState;

class WcsGridOptionsController : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( WcsGridOptionsController );

public:

    explicit
    WcsGridOptionsController( QObject * parent,
                              QString pathPrefix,
                              Carta::Lib::IWcsGridRenderService::SharedPtr wcsGridRenderService
                              );

signals:

    void updated();

public slots:

private:

    /// shared callback for all state variables
    void
    stdVarCB();

    Carta::Lib::IWcsGridRenderService::SharedPtr m_wcsGridRenderer = nullptr;
    IConnector * m_connector = nullptr;
    QString m_statePrefix = "/";

    SS::BoolVar::UniquePtr m_internalLabels;
    SS::DoubleVar::UniquePtr m_gridDensityModifier;
    SS::TypedVariable < QPen >::SharedPtr m_borderLinesPen;
    SS::TypedVariable < QPen >::SharedPtr m_axisLines1Pen;
    SS::TypedVariable < QPen >::SharedPtr m_axisLines2Pen;
    SS::TypedVariable < QPen >::SharedPtr m_gridLines1Pen;
    SS::TypedVariable < QPen >::SharedPtr m_gridLines2Pen;
    SS::TypedVariable < QPen >::SharedPtr m_tickLines1Pen;
    SS::TypedVariable < QPen >::SharedPtr m_tickLines2Pen;
    SS::TypedVariable < QPen >::SharedPtr m_numText1Pen;
    SS::TypedVariable < QPen >::SharedPtr m_numText2Pen;
    SS::TypedVariable < QPen >::SharedPtr m_labelText1Pen;
    SS::TypedVariable < QPen >::SharedPtr m_labelText2Pen;
    SS::TypedVariable < QPen >::SharedPtr m_shadowPen;
    SS::TypedVariable < QPen >::SharedPtr m_marginDimPen;

    SS::DoubleVar::UniquePtr m_numText1FontSize;
    SS::IntVar::UniquePtr m_numText1FontIndex;
    SS::IntVar::UniquePtr m_currentSkyCS;

};
}
