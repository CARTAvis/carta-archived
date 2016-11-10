#include "UnitsIntensity.h"
#include "Data/Profile/ProfileStatistics.h"
#include "Data/Util.h"
#include "CartaLib/CartaLib.h"
#include "State/UtilState.h"
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString UnitsIntensity::UNIT_LIST = "units";
const QString UnitsIntensity::CLASS_NAME = "UnitsIntensity";
const QString UnitsIntensity::NAME_PEAK = "Fraction of Peak";
const QString UnitsIntensity::NAME_JYBEAM = "Jy/beam";
const QString UnitsIntensity::NAME_JYSR = "MJy/sr";
const QString UnitsIntensity::NAME_JYARCSEC = "Jy/arcsec^2";
const QString UnitsIntensity::NAME_JY = "Jy";
const QString UnitsIntensity::NAME_KELVIN = "Kelvin";


class UnitsIntensity::Factory : public Carta::State::CartaObjectFactory {
public:

    Factory():
        CartaObjectFactory(CLASS_NAME){
    };

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new UnitsIntensity (path, id);
    }
};



bool UnitsIntensity::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new UnitsIntensity::Factory());

UnitsIntensity::UnitsIntensity( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
	m_spectralAxisAvailable = false;
	ProfileStatistics* stats = Util::findSingletonObject<ProfileStatistics>();
	m_stat = stats->getTypeFor( stats->getDefault() );
    _initializeDefaultState();
}

QString UnitsIntensity::getActualUnits( const QString& unitStr ) const {
    QString actualUnits;
    int dataCount = m_state.getArraySize( UNIT_LIST );
    for ( int i = 0; i < dataCount; i++ ){
        QString key = Carta::State::UtilState::getLookup( UNIT_LIST, i );
        QString unitName = m_state.getValue<QString>( key );
        int result = QString::compare( unitName, unitStr, Qt::CaseInsensitive );
        if ( result == 0 ){
            actualUnits = unitName;
            break;
        }
    }
    return actualUnits;
}


QString UnitsIntensity::getDefault() const {
    return m_defaultUnit;
}

void UnitsIntensity::_initUnit( int * index, const QString& name ){
    QString key = Carta::State::UtilState::getLookup( UNIT_LIST, *index );
    m_state.setValue<QString>( key, name );
    *index = *index + 1;
}

void UnitsIntensity::_initializeDefaultState(){
	int count = 4;
	if ( !m_spectralAxisAvailable ){
		count = 3;
	}
    m_state.insertArray( UNIT_LIST, count );
    int i = 0;
    _initUnit( &i, NAME_JYBEAM );
    _initUnit( &i, NAME_JYSR );
    _initUnit( &i, NAME_JYARCSEC );
    if ( m_spectralAxisAvailable ){
    	 _initUnit( &i, NAME_KELVIN );
    }

    m_defaultUnit = NAME_JYBEAM;
    m_state.flushState();
}

void UnitsIntensity::setSpectralAxisAvailable( bool available ){
	if ( available != m_spectralAxisAvailable ){
		m_spectralAxisAvailable = available;
		resetUnits( m_stat );
	}
}

void UnitsIntensity::resetUnits( Carta::Lib::ProfileInfo::AggregateType stat ){

	QStringList units;
	if ( stat == Carta::Lib::ProfileInfo::AggregateType::SUM ){
		const QString PIXELS = "*pixels";
		units.append( NAME_JYBEAM + PIXELS );
		units.append( NAME_JYSR + PIXELS );
		units.append( NAME_JYARCSEC + PIXELS );
		if ( m_spectralAxisAvailable ){
			units.append( NAME_KELVIN + PIXELS );
			m_defaultUnit = NAME_JYBEAM + PIXELS;
		}
	}
	else if ( stat == Carta::Lib::ProfileInfo::AggregateType::FLUX_DENSITY ){
		units.append( NAME_JY );
		m_defaultUnit = NAME_JY;
	}
	else {
		units.append( NAME_JYBEAM );
		units.append( NAME_JYSR );
		units.append( NAME_JYARCSEC );
		if ( m_spectralAxisAvailable ){
			units.append( NAME_KELVIN );
		}
		m_defaultUnit = NAME_JYBEAM;
	}
	int unitCount = units.size();
	m_state.resizeArray( UNIT_LIST, unitCount );
	for ( int i = 0; i < unitCount; i++ ){
		QString key = Carta::State::UtilState::getLookup( UNIT_LIST, i );
		m_state.setValue<QString>( key, units[i] );
	}

	m_state.flushState();
}




UnitsIntensity::~UnitsIntensity(){

}
}
}
