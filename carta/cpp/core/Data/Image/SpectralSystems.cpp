#include "Data/Image/SpectralSystems.h"
#include "Data/Util.h"
#include "State/UtilState.h"

#include <QDebug>
#include <set>

namespace Carta {

namespace Data {

const QString SpectralSystems::CLASS_NAME = "SpectralSystems";
const QString SpectralSystems::SYSTEM_NAME = "specCS";
const QString SpectralSystems::COUNT = "specCSCount";
const QString SpectralSystems::NATIVE = "Native";
const QString SpectralSystems::CHANNEL = "Channel";
const QString SpectralSystems::FREQUENCY = "Frequency";
const QString SpectralSystems::RADIOVELOCITY = "Radio Velocity";
const QString SpectralSystems::OPTICALVELOCITY = "Optical Velocity";

class SpectralSystems::Factory : public Carta::State::CartaObjectFactory {

    public:

        Factory():
            CartaObjectFactory( CLASS_NAME ){};

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new SpectralSystems (path, id);
        }
    };


bool SpectralSystems::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new SpectralSystems::Factory());


SpectralSystems::SpectralSystems( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    m_specSystems.insert(Carta::Lib::KnownSpecCS::Default, NATIVE );
    m_specSystems.insert(Carta::Lib::KnownSpecCS::FREQ, FREQUENCY);
    m_specSystems.insert(Carta::Lib::KnownSpecCS::VRAD, RADIOVELOCITY);
    m_specSystems.insert(Carta::Lib::KnownSpecCS::VOPT, OPTICALVELOCITY);
    //m_specSystems.insert(Carta::Lib::KnownSpecCS::BETA, BETA);
    //m_specSystems.insert(Carta::Lib::KnownSpecCS::WAVE, WAVELENGTH);
    //m_specSystems.insert(Carta::Lib::KnownSpecCS::AWAV, AIRWAVELENGTH);

    _initializeDefaultState();
    _initializeCallbacks();
}

QString SpectralSystems::getDefault() const {
    return m_specSystems[ getDefaultType() ];
}

Carta::Lib::KnownSpecCS  SpectralSystems::getDefaultType() const {
    return Carta::Lib::KnownSpecCS::VRAD;
}

QString SpectralSystems::getSpectralSystem( const QString& system ) const {
    QString actualSystem;
    int coordCount = m_specSystems.size();
    QList<Carta::Lib::KnownSpecCS> keys = m_specSystems.keys();
    for ( int i = 0; i < coordCount; i++ ){
        int compareResult = QString::compare( system, m_specSystems[keys[i]], Qt::CaseInsensitive);
        if ( compareResult == 0 ){
            actualSystem = m_specSystems[keys[i]];
            break;
        }
    }
    return actualSystem;
}

QStringList SpectralSystems::getSpectralSystems() const {
    QStringList buff;
    int coordCount = m_specSystems.size();
    QList<Carta::Lib::KnownSpecCS> keys = m_specSystems.keys();
    for ( int i = 0; i < coordCount; i++ ){
        buff.append( m_specSystems[keys[i]] );
    }
    return buff;
}

Carta::Lib::KnownSpecCS SpectralSystems::getIndex( const QString& name ) const {
    Carta::Lib::KnownSpecCS index = Carta::Lib::KnownSpecCS::Unknown;
    QMapIterator<Carta::Lib::KnownSpecCS, QString> iter(m_specSystems);
    while (iter.hasNext()) {
        iter.next();
        if ( iter.value() == name ){
            index = iter.key();
            break;
        }
    }
    return index;
}

QList<Carta::Lib::KnownSpecCS> SpectralSystems::getIndices() const{
    return m_specSystems.keys();
}

QString SpectralSystems::getName(Carta::Lib::KnownSpecCS specCS ) const {
    QString name;
    if ( m_specSystems.contains( specCS )){
        name = m_specSystems[specCS];
    }
    return name;
}


void SpectralSystems::_initializeDefaultState(){
    int coordCount = m_specSystems.size();
    m_state.insertValue<int>( COUNT, coordCount );
    m_state.insertArray( SYSTEM_NAME, coordCount );
    QList<Carta::Lib::KnownSpecCS> keys = m_specSystems.keys();
    for ( int i = 0; i < coordCount; i++ ){
        QString lookup = Carta::State::UtilState::getLookup( SYSTEM_NAME, i );
        m_state.setValue<QString>( lookup,m_specSystems[keys[i]]);
    }
    m_state.flushState();
}

void SpectralSystems::_initializeCallbacks(){
    addCommandCallback( "getSpecSystems", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
            QStringList coordList = getSpectralSystems();
            QString result = coordList.join(",");
            return result;
        });
}


SpectralSystems::~SpectralSystems(){

}
}
}
