#include "RegionRectangle.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString RegionRectangle::CLASS_NAME = "RegionRectangle";

bool RegionRectangle::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass (CLASS_NAME,
                                                   new RegionRectangle::Factory());

RegionRectangle::RegionRectangle(const QString& path, const QString& id ):
    Region( CLASS_NAME, path, id ),
    TOP_LEFT_X( "topLeftX"),
    TOP_LEFT_Y( "topLeftY"),
    BOTTOM_RIGHT_X( "bottomRightX"),
    BOTTOM_RIGHT_Y( "bottomRightY"){
    _initializeState();
}

/*QString RegionRectangle::getType() const {
    return CLASS_NAME;
}*/

void RegionRectangle::_initializeState(){
    m_state.insertValue<int>( TOP_LEFT_X, 0 );
    m_state.insertValue<int>( TOP_LEFT_Y, 0 );
    m_state.insertValue<int>( BOTTOM_RIGHT_X, 0 );
    m_state.insertValue<int>( BOTTOM_RIGHT_Y, 0 );
    //m_state.flushState();
}

void RegionRectangle::resetStateData(const QString & params ){
    QStringList coords = params.split( " ");
    int coordCount = coords.size();
    if ( coordCount == 4 ){
        int values[4];
        bool validInts = false;
        for ( int i = 0; i < coordCount; i++ ){
            values[i] = coords[i].toInt( & validInts );
            if ( !validInts ){
                break;
            }
        }
        if ( validInts ){
            if ( values[0] <= values[2] && values[1] <= values[3] ){
                bool changed = false;
                if ( m_state.getValue<int>( TOP_LEFT_X) != values[0]){
                    m_state.setValue<int>( TOP_LEFT_X, values[0] );
                    changed = true;
                }
                if ( m_state.getValue<int>(TOP_LEFT_Y) != values[1] ){
                    m_state.setValue<int>( TOP_LEFT_Y, values[1] );
                    changed = true;
                }
                if ( m_state.getValue<int>(BOTTOM_RIGHT_X) != values[2]){
                    m_state.setValue<int>( BOTTOM_RIGHT_X, values[2]);
                    changed = true;
                }
                if ( m_state.getValue<int>(BOTTOM_RIGHT_Y) != values[3]){
                    m_state.setValue<int>( BOTTOM_RIGHT_Y, values[3] );
                    changed = true;
                }
                if ( changed ){
                    m_state.flushState();
                }
            }
            else {
                qDebug() << "Invalid rectangle region; expecting (topLeft,bottomRight): "<<params;
            }
        }
        else {
            qDebug() << "Invalid rectangular pixel coordinates: "<<params;
        }
    }
    else {
        qDebug() << "Invalid coordinate size for a region rectangle="<<coordCount;
    }
}


RegionRectangle::~RegionRectangle(){

}
}
}
