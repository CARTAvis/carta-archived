#include "Snapshot.h"

#include <QDebug>


namespace Carta {

namespace Data {

const QString Snapshot::DATE_CREATED = "dateCreated";
const QString Snapshot::DESCRIPTION = "description";
const QString Snapshot::DIR_LAYOUT = "layout";
const QString Snapshot::DIR_PREFERENCES = "preferences";
const QString Snapshot::DIR_DATA = "data";
const QString Snapshot::CLASS_NAME = "Snapshot";

Snapshot::Snapshot( const QString name):
    m_state( ""){
    _initializeState(name );
}

QString Snapshot::getName() const {
    return m_state.getValue<QString>( CLASS_NAME );
}

void Snapshot::setCreatedDate( const QString& dateCreated ){
    QString oldDateCreated = m_state.getValue<QString>( DATE_CREATED );
    if ( oldDateCreated != dateCreated ){
        m_state.setValue<QString>( DATE_CREATED, dateCreated );
    }
}

void Snapshot::setDescription( const QString& descript ){
    QString oldDescript = m_state.getValue<QString>( DESCRIPTION );
    if ( oldDescript != descript ){
        m_state.setValue<QString>( DESCRIPTION, descript );
    }
}

void Snapshot::setState( const QString& stateName, bool saved ){
    bool oldSaved = m_state.getValue<bool>( stateName );
    if ( oldSaved != saved ){
        m_state.setValue<bool>( stateName, saved );
    }
}


void Snapshot::_initializeState( const QString& name ){
    m_state.insertValue<QString>( CLASS_NAME, name );
    m_state.insertValue<bool>( DIR_LAYOUT, false );
    m_state.insertValue<bool>( DIR_PREFERENCES, false );
    m_state.insertValue<bool>( DIR_DATA, false );
    m_state.insertValue<QString>( DESCRIPTION, "");
    m_state.insertValue<QString>( DATE_CREATED, "");
}

QString Snapshot::toString() const {
    return m_state.toString();
}


Snapshot::~Snapshot(){

}
}
}
