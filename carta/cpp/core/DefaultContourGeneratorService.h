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
    setContourType(const QString & contourType ) override;

    virtual void
    setContourTypesVector(const QStringList & contourTypesVector ) override;

    virtual void
    setLevels( const std::vector < double > & levels ) override;

    virtual void
    setLevelsVector( const std::vector < std::vector < double > > & levelsVector ) override;

    virtual void
    setInput( Carta::Lib::NdArray::RawViewInterface::SharedPtr rawView ) override;

    virtual JobId
    start( JobId jobId ) override;

signals:

private slots:

    void timerCB();

private:

    std::vector < double > m_levels;
    std::vector < std::vector < double > > m_levelsVector;
    QString m_contourType;
    QStringList m_contourTypesVector;
    JobId m_lastJobId = - 1;
    Carta::Lib::NdArray::RawViewInterface::SharedPtr m_rawView = nullptr;
    QTimer m_timer;

};
}
}
