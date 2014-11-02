

#include "StateReader.h"
#include "StateInterface.h"
#include "ObjectManager.h"
#include <QDebug>
#include <QFile>

StateReader::StateReader( const QString& fileName ) :
        m_fileName( fileName ),
        m_state("",""){
}

QString StateReader::getState( const QString& key ) const {
    QString baseVal = ObjectManager::objectManager()->getRoot();
    QString layoutLookup = baseVal + StateInterface::DELIMITER + key;
    return m_state.toString( layoutLookup );
}

QList<std::pair<QString,QString> > StateReader::getViews( const QString& type ) const {
    QList<QString> keys = _getViewObjectIds();
    QList<std::pair<QString,QString> > views;
    for ( QString key : keys ){
        QString typeLookup( key + StateInterface::DELIMITER + "type");
        try {
            QString objectType = m_state.getValue<QString>( typeLookup);
            if ( objectType == type ){
                QString objectState = m_state.toString( key );
                views.append( std::pair<QString,QString>(key, objectState ));
            }

        }
        catch( std::invalid_argument & e ){
            qDebug() << "Got exception no state for key="<<typeLookup;
        }
    }
    return views;
}

QList<QString> StateReader::_getViewObjectIds() const {
    //Keys returned begin at the base path for the object.
    QString baseVal = ObjectManager::objectManager()->getRoot();
    QList<QString> keys = m_state.getMemberNames( baseVal );
    int keyCount = keys.size();
    for ( int i = 0; i < keyCount; i++  ){
        keys[i] = baseVal + StateInterface::DELIMITER + keys[i];
    }
    return keys;
}


bool StateReader::restoreState(){
    //Read in the file and store it in StateInterface
    QFile file( m_fileName );
    bool successfullyRead = true;
    if ( !file.open(QFile::ReadOnly | QFile::Text)){
        successfullyRead = false;
        qDebug() << "Could not open file="<<m_fileName<<" for reading";
    }
    else {
        QTextStream in(&file);
        QString line = in.readLine();
        if (!line.isNull()){
            m_state.setState( line );
        }
    }
    return successfullyRead;
}


