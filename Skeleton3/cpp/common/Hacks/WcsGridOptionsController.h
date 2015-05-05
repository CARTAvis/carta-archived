/**
 * Helper class to manage UI related to grid options.
 **/

#pragma once

#include "CartaLib/ICoordinateGridPlotter.h"
#include "CartaLib/CartaLib.h"
#include "common/IConnector.h"
#include <QObject>

namespace Hacks
{
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

    void
    gridDensityCB( const QString &, const QString & val);

    void
    internalLabelsCB( const QString &, const QString & val);

    Carta::Lib::IWcsGridRenderService::SharedPtr m_wcsGridRenderer = nullptr;
    IConnector * m_connector = nullptr;
    QString m_statePrefix = "/";

};
}
