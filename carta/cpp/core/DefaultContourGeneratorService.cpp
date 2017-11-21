/**
 *
 **/

#include "DefaultContourGeneratorService.h"
#include "CartaLib/Algorithms/ContourConrec.h"
#include <utility>
#include <QElapsedTimer>

namespace Carta
{
namespace Core
{
DefaultContourGeneratorService::DefaultContourGeneratorService( QObject * parent )
    : Lib::IContourGeneratorService( parent )
{
    m_timer.setInterval( 1 );
    m_timer.setSingleShot( true );
    connect( & m_timer, & QTimer::timeout, this, & Me::timerCB );
}

void
DefaultContourGeneratorService::setContourType( const QString & contourType )
{
    m_contourType = contourType;
}

void
DefaultContourGeneratorService::setContourTypesVector( const QStringList & contourTypesVector )
{
    m_contourTypesVector = contourTypesVector;
}

void
DefaultContourGeneratorService::setLevels( const std::vector < double > & levels )
{
    m_levels = levels;
}

void
DefaultContourGeneratorService::setLevelsVector( const std::vector < std::vector < double > > & levelsVector )
{
    m_levelsVector = levelsVector;
}

void
DefaultContourGeneratorService::setHasDifferentContourTypes(const bool & hasDifferentContourTypes )
{
    m_hasDifferentContourTypes = hasDifferentContourTypes;
}

void
DefaultContourGeneratorService::setInput( Carta::Lib::NdArray::RawViewInterface::SharedPtr rawView )
{
    m_rawView = rawView;
}

Lib::IContourGeneratorService::JobId
DefaultContourGeneratorService::start( Lib::IContourGeneratorService::JobId jobId )
{
    if ( jobId < 0 ) {
        m_lastJobId = m_lastJobId + 1;
    }
    else {
        m_lastJobId = jobId;
    }

    m_timer.start();

    return m_lastJobId;
}

void
DefaultContourGeneratorService::timerCB()
{
    // add timer
    QElapsedTimer timer;
    timer.start();

    // set the output result
    Result result;

    // run the contour algorithm
    if (m_hasDifferentContourTypes) {
        // build the result for different contour types
        qDebug() << "++++++++ [contour] build the result for different contour types";
        for (int i = 0; i < m_contourTypesVector.size(); i++) {
            Carta::Lib::Algorithms::ContourConrec cc;
            cc.setLevels(m_levelsVector[i]);
            auto rawContours = cc.compute(m_rawView.get(), m_contourTypesVector[i]);
            for (size_t j = 0; j < m_levelsVector[i].size(); ++ j) {
                Carta::Lib::Contour contour( m_levelsVector[i][j], rawContours[j]);
                result.add(contour);
            }
        }
    } else {
        // build the result for a single contour type
        qDebug() << "++++++++ [contour] build the result for a single contour type";
        Carta::Lib::Algorithms::ContourConrec cc;
        cc.setLevels(m_levels);
        auto rawContours = cc.compute(m_rawView.get(), m_contourType);
        for(size_t i = 0; i < m_levels.size(); ++ i) {
            Carta::Lib::Contour contour( m_levels[i], rawContours[i]);
            result.add(contour);
        }

    }

    int elapsedTime = timer.elapsed();
    if (CARTA_RUNTIME_CHECKS) {
        // stop the timer and print out the elapsed time
        qDebug() << "++++++++ [contour] Spending time for calculating contours:" << elapsedTime << "ms";
    }

    emit done( result, m_lastJobId );
}
}
}
