#include "UnitsIntensity.h"
#include "Data/Profile/ProfileStatistics.h"
#include "Data/Util.h"
#include "CartaLib/CartaLib.h"
#include "State/UtilState.h"
#include <QRegExp>
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString UnitsIntensity::UNIT_LIST = "units";
const QString UnitsIntensity::CLASS_NAME = "UnitsIntensity";
const QString UnitsIntensity::NAME_PEAK = "Fraction of Peak";
const QString UnitsIntensity::NAME_JYPIXEL = "Jy/pixel";
const QString UnitsIntensity::NAME_JYBEAM = "Jy/beam";
const QString UnitsIntensity::NAME_JYSR = "MJy/sr";
const QString UnitsIntensity::NAME_JYARCSEC = "Jy/arcsec^2";
const QString UnitsIntensity::NAME_JY = "Jy";
const QString UnitsIntensity::NAME_KELVIN = "K";
const QString UnitsIntensity::NAME_NA = "N/A";


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

    m_state.insertArray( UNIT_LIST, 1 );
    int i = 0;
    _initUnit( &i, NAME_NA );

    m_defaultUnit = NAME_NA;
    m_state.flushState();
}

QStringList UnitsIntensity::_transTable(QString headerUnit)
{
    headerUnit.replace(" ","");
    m_defaultUnit = headerUnit;
    QStringList units;
    if( headerUnit.contains(QRegExp("[Mmu]?Jy/pixel")) )
    {
        QRegExp rule("[Mmu]?Jy/pixel");
        units.append( headerUnit );
        //if (m_hasbeam)
        //{
            //units.append( QString(headerUnit).replace(rule, NAME_JYBEAM) );
        //}
        //units.append( QString(headerUnit).replace(rule, NAME_JYARCSEC) );
        //units.append( QString(headerUnit).replace(rule, NAME_JYSR) );
		//if (m_spectralAxisAvailable){
            //units.append( QString(headerUnit).replace(rule, NAME_KELVIN) );
        //}
    }
    else if ( headerUnit.contains(QRegExp("[Mmu]?Jy/beam")) )
    {
        QRegExp rule("[Mmu]?Jy/beam");
        units.append( headerUnit );
        if (m_hasbeam)
        {
            //units.append( QString(headerUnit).replace(rule, NAME_JYPIXEL) );
            units.append( QString(headerUnit).replace(rule, NAME_JYARCSEC) );
            units.append( QString(headerUnit).replace(rule, NAME_JYSR) );
            if (m_spectralAxisAvailable){
                units.append( QString(headerUnit).replace(rule, NAME_KELVIN) );
            }
        }
    }
    else if ( headerUnit.contains(QRegExp("[Mmu]?Jy/arcsec^2")) )
    {
        QRegExp rule("[Mmu]?Jy/arcsec^2");
        units.append( headerUnit );
        //units.append( QString(headerUnit).replace(rule, NAME_JYPIXEL) );
        if (m_hasbeam)
        {
            units.append( QString(headerUnit).replace(rule, NAME_JYBEAM) );
        }
        units.append( QString(headerUnit).replace(rule, NAME_JYSR) );
        if (m_spectralAxisAvailable){
            units.append( QString(headerUnit).replace(rule, NAME_KELVIN) );
        }
    }
    else if ( headerUnit.contains(QRegExp("[Mmu]?Jy/sr")) )
    {
        QRegExp rule("[Mmu]?Jy/sr");
        units.append( headerUnit );
        //units.append( QString(headerUnit).replace(rule, NAME_JYPIXEL) );
        if (m_hasbeam)
        {
            units.append( QString(headerUnit).replace(rule, NAME_JYBEAM) );
        }
        units.append( QString(headerUnit).replace(rule, NAME_JYARCSEC) );
        if (m_spectralAxisAvailable){
            units.append( QString(headerUnit).replace(rule, NAME_KELVIN) );
        }
    }
    else if (headerUnit.contains(QRegExp("[Mmu]?Jy")) )
    {
        units.append( headerUnit );
    }
    else if(headerUnit.contains(QRegExp("[Mmu]?K")) )
    {
        QRegExp rule("[Mmu]?K");
        units.append( headerUnit );
        if (m_spectralAxisAvailable){
            //units.append( QString(headerUnit).replace(rule, NAME_JYPIXEL) );
            if (m_hasbeam)
            {
                units.append( QString(headerUnit).replace(rule, NAME_JYBEAM) );
            }
            units.append( QString(headerUnit).replace(rule, NAME_JYARCSEC) );
            units.append( QString(headerUnit).replace(rule, NAME_JYSR) );
        }
    }
    else if (headerUnit.contains(QRegExp("km/s")) )
    {
        units.append( headerUnit );
        //units.append( QString("m/s") );
    }
    else if (headerUnit.contains(QRegExp("m/s")) )
    {
        units.append( headerUnit );
        //units.append( QString("km/s") );
    }
    else if (headerUnit.size() != 0)
    {
        units.append( headerUnit );
    }
    else
    {
        units.append( NAME_NA );
        m_defaultUnit = NAME_NA;
    }

    return units;

}

void UnitsIntensity::setDefaultUnit(QString headerUnit, bool hasbeam, bool spectralAxis)
{
    m_hasbeam = hasbeam;
    m_spectralAxisAvailable = spectralAxis;
    QStringList units = _transTable(headerUnit);

    // add unit in m_state
    int unitCount = units.size();
    m_state.resizeArray( UNIT_LIST, unitCount );
    for ( int i = 0; i < unitCount; i++ ){
            QString key = Carta::State::UtilState::getLookup( UNIT_LIST, i );
            m_state.setValue<QString>( key, units[i] );
    }

    m_state.flushState();
}

/**
 * This function will be deprecated
 */
void UnitsIntensity::resetUnits( Carta::Lib::ProfileInfo::AggregateType stat ){

	QStringList units;
	if ( stat == Carta::Lib::ProfileInfo::AggregateType::SUM ){
		const QString PIXELS = "*pixels";
		units.append( NAME_JYBEAM + PIXELS );
		units.append( NAME_JYSR + PIXELS );
		units.append( NAME_JYARCSEC + PIXELS );
		if ( m_spectralAxisAvailable ){
			units.append( NAME_KELVIN + PIXELS );
            // Is this right? Should this be outside the if?
			m_defaultUnit = NAME_JYBEAM + PIXELS;
		}
	}
	else if ( stat == Carta::Lib::ProfileInfo::AggregateType::FLUX_DENSITY ){
		units.append( NAME_JY );
		m_defaultUnit = NAME_JY;
	}
	else {
            units = _transTable(m_defaultUnit);
	}

        // add unit in m_state
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
} // namespace Data
} // namespace Carta
