#include "Data/Region/Region.h"
#include "CartaLib/CartaLib.h"
#include "State/UtilState.h"
#include <QDebug>
#include <cmath>
#include "ProfileStatistics.h"

namespace Carta {

namespace Data {

const QString ProfileStatistics::STAT_LIST = "profileStats";
const QString ProfileStatistics::CLASS_NAME = "ProfileStatistics";
const QString ProfileStatistics::STAT_MEAN = "Mean";
const QString ProfileStatistics::STAT_MEDIAN = "Median";
const QString ProfileStatistics::STAT_SUM = "Sum";
const QString ProfileStatistics::STAT_VARIANCE = "Variance";
const QString ProfileStatistics::STAT_MIN = "Minimum";
const QString ProfileStatistics::STAT_MAX = "Maximum";
const QString ProfileStatistics::STAT_RMS = "RMS";
const QString ProfileStatistics::STAT_FLUX_DENSITY = "Flux Density";


class ProfileStatistics::Factory : public Carta::State::CartaObjectFactory {
public:

    Factory():
        CartaObjectFactory(CLASS_NAME){
    };

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new ProfileStatistics (path, id);
    }
};



bool ProfileStatistics::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new ProfileStatistics::Factory());

ProfileStatistics::ProfileStatistics( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}

QString ProfileStatistics::getActualStatistic( const QString& unitStr ) const {
    QString actualUnits;
    int dataCount = m_state.getArraySize( STAT_LIST );
    for ( int i = 0; i < dataCount; i++ ){
        QString key = Carta::State::UtilState::getLookup( STAT_LIST, i );
        QString unitName = m_state.getValue<QString>( key );
        int result = QString::compare( unitName, unitStr, Qt::CaseInsensitive );
        if ( result == 0 ){
            actualUnits = unitName;
            break;
        }
    }
    return actualUnits;
}


QString ProfileStatistics::getDefault() const {
    return STAT_MEAN;
}

void ProfileStatistics::_initStat( int * index, const QString& name ){
    QString key = Carta::State::UtilState::getLookup( STAT_LIST, *index );
    m_state.setValue<QString>( key, name );
    *index = *index + 1;
}

void ProfileStatistics::_initializeDefaultState(){
    m_state.insertArray( STAT_LIST, 8 );
    int i = 0;

    _initStat( &i, STAT_MEAN );
    _initStat( &i, STAT_MEDIAN );
    _initStat( &i, STAT_SUM );
    _initStat( &i, STAT_VARIANCE );
    _initStat( &i, STAT_MIN );
    _initStat( &i, STAT_MAX );
    _initStat( &i, STAT_RMS );
    _initStat( &i, STAT_FLUX_DENSITY );

    m_state.flushState();
}


ProfileStatistics::~ProfileStatistics(){

}
}
}
