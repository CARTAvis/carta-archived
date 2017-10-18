#include "GenerateModes.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString GenerateModes::GEN_LIST = "genModes";
const QString GenerateModes::CLASS_NAME = "GenerateModes";
const QString GenerateModes::GEN_ALL = "All";
const QString GenerateModes::GEN_CURRENT = "Current";
//const QString GenerateModes::GEN_CUSTOM = "Custom";
// const QString GenerateModes::GEN_NO_SINGLE_PLANES = "Exclude Single Plane";


class GenerateModes::Factory : public Carta::State::CartaObjectFactory {
public:

    Factory():
        CartaObjectFactory(CLASS_NAME){
    };

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new GenerateModes (path, id);
    }
};



bool GenerateModes::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new GenerateModes::Factory());

GenerateModes::GenerateModes( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}

QString GenerateModes::getActualMode( const QString& modeStr ) const {
    QString actualMode;
    int dataCount = m_state.getArraySize( GEN_LIST );
    for ( int i = 0; i < dataCount; i++ ){
        QString key = Carta::State::UtilState::getLookup( GEN_LIST, i );
        QString genName = m_state.getValue<QString>( key );
        int result = QString::compare( genName, modeStr, Qt::CaseInsensitive );
        if ( result == 0 ){
            actualMode = genName;
            break;
        }
    }
    return actualMode;
}


QString GenerateModes::getDefault() const {
    return GEN_CURRENT;
}


void GenerateModes::_initializeDefaultState(){
    m_state.insertArray( GEN_LIST, 2 );
    int i = 0;

    _initMode( &i, GEN_CURRENT );
    _initMode( &i, GEN_ALL );
    // _initMode( &i, GEN_CUSTOM );
    // This one is set as the fundamental condition, so the item is useless
    // The related codes may be removed in the future.
    // _initMode( &i, GEN_NO_SINGLE_PLANES );

    m_state.flushState();
}


void GenerateModes::_initMode( int * index, const QString& name ){
    QString key = Carta::State::UtilState::getLookup( GEN_LIST, *index );
    m_state.setValue<QString>( key, name );
    *index = *index + 1;
}


bool GenerateModes::isAll( const QString& mode) const {
    bool allMode = false;
    QString actualMode = getActualMode( mode );
    if ( actualMode == GEN_ALL ){
        allMode = true;
    }
    return allMode;
}


// bool GenerateModes::isAllExcludeSingle( const QString& mode ) const {
//     bool allExcludeSingle = false;
//     QString actualMode = getActualMode( mode );
//     if ( actualMode == GEN_NO_SINGLE_PLANES ){
//         allExcludeSingle = true;
//     }
//     return allExcludeSingle;
// }


bool GenerateModes::isCurrent( const QString& mode ) const {
    bool currentMode = false;
    QString actualMode = getActualMode( mode );
    if ( actualMode == GEN_CURRENT ){
        currentMode = true;
    }
    return currentMode;
}


GenerateModes::~GenerateModes(){

}
}
}
