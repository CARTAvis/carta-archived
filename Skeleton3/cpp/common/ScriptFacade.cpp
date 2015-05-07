#include "ScriptFacade.h"
#include "Data/ViewManager.h"
#include "Data/Animator/Animator.h"
#include "Data/Animator/AnimatorType.h"
#include "Data/Controller.h"
#include "Data/Colormap/Colormap.h"
#include "Data/Colormap/Colormaps.h"
#include "Data/Util.h"

#include <QDebug>

ScriptFacade * ScriptFacade::getInstance (){
    static ScriptFacade * sc = new ScriptFacade ();
    return sc;
}


ScriptFacade::ScriptFacade(){
    QString vmLookUp = Carta::Data::ViewManager::CLASS_NAME;
    Carta::State::CartaObject* obj = Carta::Data::Util::findSingletonObject( vmLookUp );
    m_viewManager = dynamic_cast<Carta::Data::ViewManager*>(obj);
}

QString ScriptFacade::getColorMapId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Colormap::CLASS_NAME, index);
}

QStringList ScriptFacade::getColorMaps() const {
    QString cmLookUp = Carta::Data::Colormaps::CLASS_NAME;
    Carta::State::CartaObject* obj = Carta::Data::Util::findSingletonObject( cmLookUp );
    Carta::Data::Colormaps* maps = dynamic_cast<Carta::Data::Colormaps*>(obj);
    return maps->getColorMaps();
}

QString ScriptFacade::getImageViewId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Controller::PLUGIN_NAME, index );
}

void ScriptFacade::linkAdd( const QString& sourceId, const QString& destId ){
    m_viewManager->linkAdd( sourceId, destId );
}

void ScriptFacade::loadFile( const QString& objectId, const QString& fileName ){
    m_viewManager->loadFile( objectId, fileName);
}

void ScriptFacade::setAnalysisLayout(){
    m_viewManager->setAnalysisView();
}

Carta::Data::Animator* ScriptFacade::getAnimator(){
    QString animId = m_viewManager->getObjectId( Carta::Data::Animator::CLASS_NAME, 0 );
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    QString id = objMan->parseId( animId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
    return animator;
}

Carta::Data::Colormap* ScriptFacade::getColormap(){
    QString animId = m_viewManager->getObjectId( Carta::Data::Colormap::CLASS_NAME, 0 );
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    QString id = objMan->parseId( animId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
    return colormap;
}

void ScriptFacade::showImageAnimator(){
    Carta::Data::Animator* animator = getAnimator();
    if ( animator ){
        QString animId;
        animator->addAnimator( "Image", animId);
    }
}

void ScriptFacade::setChannel( int channel ){
    Carta::Data::Animator* animator = getAnimator();
    if ( animator ){
        Carta::Data::AnimatorType* animType = animator->getAnimator( "Channel");
        if ( animType != nullptr ){
            animType->setFrame( channel );
        }
        else {
            qDebug()<<"Could not get channel animator";
        }
    }
    else {
        qDebug() << "Could not find animator";
    }
}

void ScriptFacade::setColorMap( const QString& /*colormapId*/, const QString& colormapName ){
    Carta::Data::Colormap* colormap = getColormap();
    colormap->setColorMap( colormapName );
}

void ScriptFacade::setImageLayout(){
    m_viewManager->setImageView();
}
