#include "core/Data/Clips.h"
#include "CartaLib/CartaLib.h"
#include "core/State/UtilState.h"
#include <QDebug>
#include <cmath>

namespace Carta {

namespace Data {

const QString Clips::CLIP_LIST = "clipList";
const QString Clips::CLASS_NAME = "Clips";
const QString Clips::CLIP_COUNT = "clipCount";

using Carta::State::UtilState;

class Clips::Factory : public Carta::State::CartaObjectFactory {
    public:

        Factory():
            CartaObjectFactory(CLASS_NAME){
        };

        Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new Clips (path, id);
        }
    };



bool Clips::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Clips::Factory());

Clips::Clips( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
}

void Clips::_initializeDefaultState(){

    // get all colormaps provided by core
    std::vector <double> clips;
    clips.push_back( 0.95 );
    clips.push_back( 0.98 );
    clips.push_back( 0.99 );
    clips.push_back( 0.995 );
    clips.push_back( 0.999 );
    clips.push_back( 1 );


    int clipCount = clips.size();
    m_state.insertValue<int>( CLIP_COUNT, clipCount );
    m_state.insertArray( CLIP_LIST, clipCount );
    for ( int i = 0; i < clipCount; i++ ){
        QString arrayIndexStr = UtilState::getLookup(CLIP_LIST,i);
        m_state.setValue<double>(arrayIndexStr, clips[i] );
    }
    m_state.flushState();
}

double Clips::getClip( int index ) const {
    int clipCount = m_state.getValue<int>(CLIP_COUNT);
    double clipValue = -1;
    if ( index < 0 || index  >= clipCount ) {
        CARTA_ASSERT( "clip index out of range!" );
    }
    else {
        QString clipLookup = UtilState::getLookup( CLIP_LIST, index );
        clipValue = m_state.getValue<double>(clipLookup);
    }
    return clipValue;
}


int Clips::getIndex( const QString& clipValue ) const {
    int clipIndex = -1;
    bool validDouble = false;
    double target = clipValue.toDouble(&validDouble);
    if ( validDouble ){
        const double ERROR_MARGIN = 0.000001;
        int clipCount = m_state.getValue<int>(CLIP_COUNT);
        for ( int i = 0; i < clipCount; i++ ){
            QString clipLookup = UtilState::getLookup( CLIP_LIST, i );
            double clip = m_state.getValue<double>(clipLookup);
            if ( fabs( target - clip )<ERROR_MARGIN ){
                clipIndex = i;
                break;
            }
        }
    }
    else {
        qWarning()<< "Invalid clip value: "<<clipValue;
    }
    return clipIndex;
}

Clips::~Clips(){

}
}
}
