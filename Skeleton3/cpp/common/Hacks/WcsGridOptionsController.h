/**
 * Helper class to manage UI related to grid options.
 **/

#pragma once

#include "CartaLib/ICoordinateGridPlotter.h"
#include "CartaLib/CartaLib.h"
#include "common/IConnector.h"
#include "common/Hacks/SharedState.h"
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

public slots:

private:

    void
    lineOpacityCB(const QString &, const QString & val);

    void
    lineThicknessCB( const QString &, const QString & val );

//    void
//    gridDensityCB( const QString &, const QString & val);

    Carta::Lib::IWcsGridRenderService::SharedPtr m_wcsGridRenderer = nullptr;
    IConnector * m_connector = nullptr;
    QString m_statePrefix = "/";

    SS::BoolVar::UniquePtr m_internalLabels;
    SS::DoubleVar::UniquePtr m_gridDensityModifier;
//    SS::TypedVariable< double, SS::ClampedDouble >::UniquePtr m_gd;
    SS::TypedVariable<QPen>::SharedPtr m_borderLinesPen;
    SS::TypedVariable<QPen>::SharedPtr m_axisLines1Pen;
    SS::TypedVariable<QPen>::SharedPtr m_axisLines2Pen;
    SS::TypedVariable<QPen>::SharedPtr m_gridLines1Pen;
    SS::TypedVariable<QPen>::SharedPtr m_gridLines2Pen;
    SS::TypedVariable<QPen>::SharedPtr m_tickLines1Pen;
    SS::TypedVariable<QPen>::SharedPtr m_tickLines2Pen;
    SS::TypedVariable<QPen>::SharedPtr m_numText1Pen;
    SS::TypedVariable<QPen>::SharedPtr m_numText2Pen;
    SS::TypedVariable<QPen>::SharedPtr m_labelText1Pen;
    SS::TypedVariable<QPen>::SharedPtr m_labelText2Pen;
    SS::TypedVariable<QPen>::SharedPtr m_shadowPen;

    void stdVarCB();

};
}
