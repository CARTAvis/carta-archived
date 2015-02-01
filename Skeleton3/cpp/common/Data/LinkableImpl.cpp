
#include "Data/Controller.h"
#include "Data/LinkableImpl.h"
#include "State/StateInterface.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString LinkableImpl::LINK = "links";


LinkableImpl::LinkableImpl( StateInterface* state ){
    m_state = state;
    _initializeState();
}

bool LinkableImpl::addLink( const std::shared_ptr<Controller>& controller ){
    bool linkAdded = false;
    if ( controller ){
        int index = _getIndex( controller );
        if ( index < 0  ){

            m_controllers.push_back( controller );
            _adjustStateController();
        }
        linkAdded = true;
    }
    return linkAdded;
}



void LinkableImpl::_adjustStateController(){
    int controllerCount = m_controllers.size();
    m_state->resizeArray( LINK, controllerCount );
    for ( int i = 0; i < controllerCount; i++ ){
        QString idStr( LINK + StateInterface::DELIMITER + QString::number(i));
        m_state->setValue<QString>(idStr, m_controllers[i]->getPath());
    }
    m_state->flushState();
}



void LinkableImpl::clear(){
    m_controllers.clear();
    m_state->resizeArray( LINK, 0 );
}

int LinkableImpl::_getIndex( const std::shared_ptr<Controller>& controller ){
    int index = -1;
    int controllerCount = m_controllers.size();
    QString targetPath = controller->getPath();
    for ( int i = 0; i < controllerCount; i++ ){
        if ( targetPath == m_controllers[i]->getPath()){
           index = i;
           break;
        }
     }
     return index;
}

int LinkableImpl::getLinkCount() const {
    int linkCount = m_state->getArraySize( LINK );
    return linkCount;
}

QList<QString> LinkableImpl::getLinks() const {
    int linkCount = getLinkCount();
    QList<QString> linkList;
    for ( int i = 0; i < linkCount; i++ ){
        linkList.append(getLinkId( i ));
    }
    return linkList;
}


QString LinkableImpl::getLinkId( int linkIndex ) const {
    QString linkId;
    if ( 0 <= linkIndex && linkIndex < getLinkCount()){
        QString idStr( LINK + StateInterface::DELIMITER + QString::number(linkIndex));
        linkId = m_state->getValue<QString>( idStr );
    }
    return linkId;
}


void LinkableImpl::_initializeState(){
    m_state->insertArray(LINK, 0 );
}

bool LinkableImpl::removeLink( const std::shared_ptr<Controller>& controller ){
    bool linkRemoved = false;
    if ( controller ){
        int index = _getIndex( controller );
        if ( index >= 0  ){
            m_controllers.removeAt( index );
            _adjustStateController();
            linkRemoved = true;
        }
    }
    return linkRemoved;
}

LinkableImpl::~LinkableImpl(){

}

}
}

