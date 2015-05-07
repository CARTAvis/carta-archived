#include "AnimationTypes.h"
#include "Data/Selection.h"
#include "State/UtilState.h"
#include "CartaLib/CartaLib.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString AnimationTypes::ANIMATION_LIST = "animators";
const QString AnimationTypes::CLASS_NAME = "AnimationTypes";

typedef Carta::State::UtilState UtilState;

class AnimationTypes::Factory : public Carta::State::CartaObjectFactory {
    public:

        Factory():
            Carta::State::CartaObjectFactory(CLASS_NAME){
        };

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new AnimationTypes (path, id);
        }
    };



bool AnimationTypes::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new AnimationTypes::Factory());

AnimationTypes::AnimationTypes( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeState();
}

QStringList AnimationTypes::getAnimations() const {
    int animCount = m_state.getArraySize( ANIMATION_LIST );
    QStringList animList;
    for ( int i = 0; i < animCount; i++ ){
        QString indexStr = UtilState::getLookup(ANIMATION_LIST,QString::number(i));
        animList.append( m_state.getValue<QString>( indexStr ));
    }
    return animList;
}

void AnimationTypes::_initializeState(){
    const int ANIM_COUNT = 2;
    m_state.insertArray( ANIMATION_LIST, ANIM_COUNT );
    QString channelIndexStr = UtilState::getLookup(ANIMATION_LIST, QString::number(0));
    m_state.setValue<QString>(channelIndexStr, Selection::CHANNEL );
    QString imageIndexStr = UtilState::getLookup(ANIMATION_LIST, QString::number(1));
    m_state.setValue<QString>( imageIndexStr, Selection::IMAGE );
    m_state.flushState();
}


AnimationTypes::~AnimationTypes(){

}
}
}
