/**
 *
 **/

#pragma once
#include "CartaLib/IContourGeneratorService.h"

#include <QObject>
#include <QTimer>

namespace Carta
{
namespace Core
{
/// Default implementation of IC
class DefaultContourGeneratorService : public Lib::IContourGeneratorService
{
    Q_OBJECT
    CLASS_BOILERPLATE( DefaultContourGeneratorService );

public:

    explicit
    DefaultContourGeneratorService( QObject * parent = 0 );

    virtual void
    setLevels( const std::vector < double > & levels ) override;

    virtual void
    setInput( NdArray::RawViewInterface::SharedPtr rawView ) override;

    virtual JobId
    start( JobId jobId ) override;

signals:

private slots:

    void timerCB();

private:

    std::vector < double > m_levels;
    JobId m_lastJobId = - 1;
    NdArray::RawViewInterface::SharedPtr m_rawView = nullptr;
    QTimer m_timer;

};
}
}
