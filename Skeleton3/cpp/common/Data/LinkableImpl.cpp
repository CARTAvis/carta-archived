
#include "Data/LinkableImpl.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString LinkableImpl::LINK = "links";
const QString LinkableImpl::PARENT_ID = "source";

LinkableImpl::LinkableImpl( const QString& parentPath ):
    m_state( parentPath +StateInterface::DELIMITER+LINK, "LinkableImpl" ){
    _initializeState( parentPath );

}

bool LinkableImpl::addLink( CartaObject* cartaObj ){
    bool linkAdded = false;
    if ( cartaObj ){
        int index = _getIndex( cartaObj );
        if ( index < 0  ){
            m_cartaObjs.push_back( cartaObj );
            _adjustState();
        }
        linkAdded = true;
    }
    return linkAdded;
}

void LinkableImpl::_adjustState(){
    int cartaObjCount = m_cartaObjs.size();
    m_state.resizeArray( LINK, cartaObjCount );
    for ( int i = 0; i < cartaObjCount; i++ ){
        QString idStr( LINK + StateInterface::DELIMITER + QString::number(i));
        m_state.setValue<QString>(idStr, m_cartaObjs[i]->getPath());
    }
    m_state.flushState();
}


void LinkableImpl::clear(){
    m_cartaObjs.clear();
    m_state.resizeArray( LINK, 0 );
    m_state.flushState();
}

int LinkableImpl::_getIndex( CartaObject* cartaObj ){
    int index = -1;
    if ( cartaObj != nullptr ){
        int cartaObjCount = m_cartaObjs.size();
        QString targetPath = cartaObj->getPath();
        for ( int i = 0; i < cartaObjCount; i++ ){
            if ( targetPath == m_cartaObjs[i]->getPath()){
               index = i;
               break;
            }
         }
     }
     return index;
}

CartaObject* LinkableImpl::getLink( int index ) const {
    CartaObject* link = nullptr;
    if ( 0 <= index && index < m_cartaObjs.size() ){
        link = m_cartaObjs[index];
    }
    return link;
}

int LinkableImpl::getLinkCount() const {
    int linkCount = m_state.getArraySize( LINK );
    return linkCount;
}

QList<QString> LinkableImpl::getLinkIds() const {
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
        linkId = m_state.getValue<QString>( idStr );
    }
    return linkId;
}

QString LinkableImpl::getStateString() const{
    return m_state.toString();
}

void LinkableImpl::_initializeState( const QString& parentPath ){
    m_state.insertArray(LINK, 0 );
    m_state.insertValue<QString>(PARENT_ID,parentPath);
    m_state.flushState();
}

bool LinkableImpl::removeLink( CartaObject* cartaObj ){
    bool linkRemoved = false;
    if ( cartaObj ){
        int index = _getIndex( cartaObj );
        if ( index >= 0  ){
            m_cartaObjs.removeAt( index );
            _adjustState();
            linkRemoved = true;
        }
    }
    return linkRemoved;
}

CartaObject* LinkableImpl::searchLinks(const QString& link){
    CartaObject* result = nullptr;
    int cartaObjCount = m_cartaObjs.size();
    for( int i = 0; i < cartaObjCount; i++ ){
        if(m_cartaObjs[i]->getPath() == link){
            result = m_cartaObjs[i];
            break;
        }
    }
    return result;
}

LinkableImpl::~LinkableImpl(){

}

}
}
