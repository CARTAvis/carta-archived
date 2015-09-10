#include "Data/Image/CoordinateSystems.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>
#include <set>

namespace Carta {

namespace Data {

const QString CoordinateSystems::CLASS_NAME = "CoordinateSystems";
const QString CoordinateSystems::SYSTEM_NAME = "skyCS";
const QString CoordinateSystems::COUNT = "skyCSCount";
const QString CoordinateSystems::UNKNOWN = "Unknown";
const QString CoordinateSystems::J2000 =  "J2000";
const QString CoordinateSystems::B1950 = "B1950";
const QString CoordinateSystems::ICRS = "ICRS";
const QString CoordinateSystems::GALACTIC = "Galactic";
const QString CoordinateSystems::ECLIPTIC = "Ecliptic";
const QString CoordinateSystems::NATIVE = "Native";
const QString CoordinateSystems::SUPERGALACTIC = "Supergalactic";

class CoordinateSystems::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new CoordinateSystems (path, id);
        }
    };


bool CoordinateSystems::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new CoordinateSystems::Factory());


CoordinateSystems::CoordinateSystems( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    //m_coordSystems.insert(Carta::Lib::KnownSkyCS::Default, NATIVE );
    m_coordSystems.insert(Carta::Lib::KnownSkyCS::J2000, J2000);
    m_coordSystems.insert(Carta::Lib::KnownSkyCS::Galactic, GALACTIC);
    m_coordSystems.insert(Carta::Lib::KnownSkyCS::ICRS, ICRS);
    m_coordSystems.insert(Carta::Lib::KnownSkyCS::Ecliptic, ECLIPTIC);
    m_coordSystems.insert(Carta::Lib::KnownSkyCS::B1950, B1950 );
    //m_coordSystems.insert(Carta::Lib::KnownSkyCS::Supergalactic, SUPERGALACTIC);

    _initializeDefaultState();
    _initializeCallbacks();
}

QString CoordinateSystems::getDefault() const {
    return m_coordSystems[ Carta::Lib::KnownSkyCS::J2000 ];
}

QString CoordinateSystems::getCoordinateSystem( const QString& system ) const {
    QString actualSystem;
    int coordCount = m_coordSystems.size();
    QList<Carta::Lib::KnownSkyCS> keys = m_coordSystems.keys();
    for ( int i = 0; i < coordCount; i++ ){
        int compareResult = QString::compare( system, m_coordSystems[keys[i]], Qt::CaseInsensitive);
        if ( compareResult == 0 ){
            actualSystem = m_coordSystems[keys[i]];
            break;
        }
    }
    return actualSystem;
}

QStringList CoordinateSystems::getCoordinateSystems() const {
    QStringList buff;
    int coordCount = m_coordSystems.size();
    QList<Carta::Lib::KnownSkyCS> keys = m_coordSystems.keys();
    for ( int i = 0; i < coordCount; i++ ){
        buff.append( m_coordSystems[keys[i]] );
    }
    return buff;
}

Carta::Lib::KnownSkyCS CoordinateSystems::getIndex( const QString& name ) const {
    Carta::Lib::KnownSkyCS index = Carta::Lib::KnownSkyCS::Unknown;
    QMapIterator<Carta::Lib::KnownSkyCS, QString> iter(m_coordSystems);
    while (iter.hasNext()) {
        iter.next();
        if ( iter.value() == name ){
            index = iter.key();
            break;
        }
    }
    return index;
}

QList<Carta::Lib::KnownSkyCS> CoordinateSystems::getIndices() const{
    return m_coordSystems.keys();
}

QString CoordinateSystems::getName(Carta::Lib::KnownSkyCS skyCS ) const {
    QString name;
    if ( m_coordSystems.contains( skyCS )){
        name = m_coordSystems[skyCS];
    }
    return name;
}


void CoordinateSystems::_initializeDefaultState(){
    int coordCount = m_coordSystems.size();
    m_state.insertValue<int>( COUNT, coordCount );
    m_state.insertArray( SYSTEM_NAME, coordCount );
    QList<Carta::Lib::KnownSkyCS> keys = m_coordSystems.keys();
    for ( int i = 0; i < coordCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( SYSTEM_NAME, i );
        m_state.setValue<QString>( lookup,m_coordSystems[keys[i]]);
    }
    m_state.flushState();
}

void CoordinateSystems::_initializeCallbacks(){
    addCommandCallback( "getCoordSystems", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
            QStringList coordList = getCoordinateSystems();
            QString result = coordList.join(",");
            return result;
        });
}


CoordinateSystems::~CoordinateSystems(){

}
}
}
