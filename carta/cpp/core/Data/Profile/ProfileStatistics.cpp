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


Carta::Lib::ProfileInfo::AggregateType ProfileStatistics::getTypeFor( const QString& statStr ) const {
    Carta::Lib::ProfileInfo::AggregateType agType = Carta::Lib::ProfileInfo::AggregateType::OTHER;
    QString actualStat = getActualStatistic( statStr );
    if ( !actualStat.isEmpty() ){
        if ( statStr == STAT_MEAN ){
            agType = Carta::Lib::ProfileInfo::AggregateType::MEAN;
        }
        if ( statStr == STAT_MEDIAN ){
            agType = Carta::Lib::ProfileInfo::AggregateType::MEDIAN;
        }
        else if ( statStr == STAT_SUM ){
            agType = Carta::Lib::ProfileInfo::AggregateType::SUM;
        }
        else if ( statStr == STAT_VARIANCE ){
            agType = Carta::Lib::ProfileInfo::AggregateType::VARIANCE;
        }
        else if ( statStr == STAT_MIN ){
            agType = Carta::Lib::ProfileInfo::AggregateType::MIN;
        }
        else if ( statStr == STAT_MAX ){
            agType = Carta::Lib::ProfileInfo::AggregateType::MAX;
        }
        else if ( statStr == STAT_RMS ){
            agType = Carta::Lib::ProfileInfo::AggregateType::RMS;
        }
        else if ( statStr == STAT_FLUX_DENSITY ){
            agType = Carta::Lib::ProfileInfo::AggregateType::FLUX_DENSITY;
        }
    }
    return agType;
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

QString ProfileStatistics::typeToString( Carta::Lib::ProfileInfo::AggregateType agType ) const {
    QString statStr;
    if ( agType == Carta::Lib::ProfileInfo::AggregateType::MEAN ){
        statStr = STAT_MEAN;
    }
    else if ( agType == Carta::Lib::ProfileInfo::AggregateType::MEDIAN ){
        statStr = STAT_MEDIAN;
    }
    else if ( agType == Carta::Lib::ProfileInfo::AggregateType::SUM ){
        statStr = STAT_SUM;
    }
    else if ( agType == Carta::Lib::ProfileInfo::AggregateType::VARIANCE ){
        statStr = STAT_VARIANCE;
    }
    else if ( agType == Carta::Lib::ProfileInfo::AggregateType::MIN ){
        statStr = STAT_MIN;
    }
    else if ( agType == Carta::Lib::ProfileInfo::AggregateType::MAX ){
        statStr = STAT_MAX;
    }
    else if ( agType == Carta::Lib::ProfileInfo::AggregateType::RMS ){
        statStr = STAT_RMS;
    }
    else if ( agType == Carta::Lib::ProfileInfo::AggregateType::FLUX_DENSITY ){
        statStr = STAT_FLUX_DENSITY;
    }
    return statStr;
}

ProfileStatistics::~ProfileStatistics(){

}
}
}
