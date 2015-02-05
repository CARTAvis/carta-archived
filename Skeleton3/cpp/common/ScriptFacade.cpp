#include "ScriptFacade.h"
#include "Data/ViewManager.h"
#include "Data/Controller.h"
#include "Data/Colormap.h"
#include "Data/Colormaps.h"
#include "Data/Util.h"

#include <QDebug>

ScriptFacade * ScriptFacade::getInstance (){
    static ScriptFacade * sc = new ScriptFacade ();
    return sc;
}


ScriptFacade::ScriptFacade(){
    QString vmLookUp = Carta::Data::ViewManager::CLASS_NAME;
    CartaObject* obj = Carta::Data::Util::findSingletonObject( vmLookUp );
    m_viewManager = dynamic_cast<Carta::Data::ViewManager*>(obj);
}

QString ScriptFacade::getColorMapId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Colormap::CLASS_NAME, index);
}

QStringList ScriptFacade::getColorMaps() const {
    QString cmLookUp = Carta::Data::Colormaps::CLASS_NAME;
    CartaObject* obj = Carta::Data::Util::findSingletonObject( cmLookUp );
    Carta::Data::Colormaps* maps = dynamic_cast<Carta::Data::Colormaps*>(obj);
    return maps->getColorMaps();
}

QString ScriptFacade::getFileList() const {
    qDebug() << "(JT) ScriptFacade::getFileList()";
    QString fileList = m_viewManager->getFileList();
    return fileList;
}

QString ScriptFacade::getImageViewId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Controller::PLUGIN_NAME, index );
}

QStringList ScriptFacade::getImageViews() {
    qDebug() << "(JT) ScriptFacade::getImageViews()";
    QStringList imageViewList;
    int numControllers = m_viewManager->getControllerCount();
    for (int i = 0; i < numControllers; i++) {
        QString imageView = getImageViewId( i );
        imageViewList << imageView;
        qDebug() << "(JT) imageView =  " << imageView;
    }
//    int i = 0;
//    QString imageView = getImageViewId( i );
//    qDebug() << "(JT) imageView =  " << imageView;
//    while (imageView != "") {
//        imageViewList << imageView;
//        i++;
//        imageView = getImageViewId( i );
//        qDebug() << "(JT) imageView =  " << imageView;
//    }
    return imageViewList;
}

QString ScriptFacade::linkAdd( const QString& sourceId, const QString& destId ){
    QString result = m_viewManager->linkAdd( sourceId, destId );
    return result;
}

void ScriptFacade::loadFile( const QString& objectId, const QString& fileName ){
    qDebug() << "(JT) ScriptFacade::loadFile(" << objectId << ", " << fileName << ")";
    m_viewManager->loadFile( objectId, fileName);
}

void ScriptFacade::setAnalysisLayout(){
    m_viewManager->setAnalysisView();
}

void ScriptFacade::setCustomLayout( int rows, int cols ){
    m_viewManager->setCustomView( rows, cols );
}

void ScriptFacade::setColorMap( const QString& colormapId, const QString& colormapName ){
    qDebug() << "(JT) ScriptFacade::setColorMap(" << colormapId << ", " << colormapName << ")";
    m_viewManager->setColorMap( colormapId, colormapName );
}

void ScriptFacade::setImageLayout(){
    m_viewManager->setImageView();
}

void ScriptFacade::setPlugins( const QStringList& names ) {
    m_viewManager->setPlugins( names );
}
