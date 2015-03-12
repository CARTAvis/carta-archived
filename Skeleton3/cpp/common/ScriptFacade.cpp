#include "ScriptFacade.h"
#include "Data/ViewManager.h"
#include "Data/Animator.h"
#include "Data/AnimatorType.h"
#include "Data/Controller.h"
#include "Data/Colormap.h"
#include "Data/Colormaps.h"
#include "Data/Util.h"
#include "Data/Animator.h"
#include "Data/Histogram.h"
#include "Data/Statistics.h"

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
    QString fileList = m_viewManager->getFileList();
    return fileList;
}

QString ScriptFacade::getImageViewId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Controller::PLUGIN_NAME, index );
}

QString ScriptFacade::getAnimatorViewId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Animator::CLASS_NAME, index );
}

QString ScriptFacade::getHistogramViewId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Histogram::CLASS_NAME, index );
}

QString ScriptFacade::getStatisticsViewId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Statistics::CLASS_NAME, index );
}

QStringList ScriptFacade::getImageViews() {
    QStringList imageViewList;
    int numControllers = m_viewManager->getControllerCount();
    for (int i = 0; i < numControllers; i++) {
        QString imageView = getImageViewId( i );
        imageViewList << imageView;
    }
    return imageViewList;
}

QStringList ScriptFacade::getColorMapViews() {
    QStringList colorMapViewList;
    int numColorMaps = m_viewManager->getColorMapCount();
    for (int i = 0; i < numColorMaps; i++) {
        QString colorMapView = getColorMapId( i );
        colorMapViewList << colorMapView;
    }
    return colorMapViewList;
}

QStringList ScriptFacade::getAnimatorViews() {
    QStringList animatorViewList;
    int numAnimators = m_viewManager->getAnimatorCount();
    for (int i = 0; i < numAnimators; i++) {
        QString animatorView = getAnimatorViewId( i );
        animatorViewList << animatorView;
    }
    return animatorViewList;
}

QStringList ScriptFacade::getHistogramViews() {
    QStringList histogramViewList;
    int numHistograms = m_viewManager->getHistogramCount();
    for (int i = 0; i < numHistograms; i++) {
        QString histogramView = getHistogramViewId( i );
        histogramViewList << histogramView;
    }
    return histogramViewList;
}

QStringList ScriptFacade::getStatisticsViews() {
    QStringList statisticsViewList;
    int numStatistics = m_viewManager->getStatisticsCount();
    for (int i = 0; i < numStatistics; i++) {
        QString statisticsView = getStatisticsViewId( i );
        statisticsViewList << statisticsView;
    }
    return statisticsViewList;
}

QString ScriptFacade::addLink( const QString& sourceId, const QString& destId ){
    QString result = m_viewManager->linkAdd( sourceId, destId );
    return result;
}

QString ScriptFacade::removeLink( const QString& sourceId, const QString& destId ){
    QString result = m_viewManager->linkRemove( sourceId, destId );
    return result;
}

QString ScriptFacade::loadFile( const QString& objectId, const QString& fileName ){
    m_viewManager->loadFile( objectId, fileName );
    return "loadFile";
}

QString ScriptFacade::loadLocalFile( const QString& objectId, const QString& fileName ){
    m_viewManager->loadLocalFile( objectId, fileName );
    return "loadLocalFile";
}

QString ScriptFacade::setAnalysisLayout(){
    m_viewManager->setAnalysisView();
    return "setAnalysisLayout";
}

QString ScriptFacade::setCustomLayout( int rows, int cols ){
    m_viewManager->setCustomView( rows, cols );
    return "setCusomLayout";
}

Carta::Data::Animator* ScriptFacade::getAnimator(){
    QString animId = m_viewManager->getObjectId( Carta::Data::Animator::CLASS_NAME, 0 );
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( animId );
    CartaObject* obj = objMan->getObject( id );
    Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
    return animator;
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

QString ScriptFacade::setColorMap( const QString& colormapId, const QString& colormapName ){
    m_viewManager->setColorMap( colormapId, colormapName );
    return "setColorMap";
}

QString ScriptFacade::reverseColorMap( const QString& colormapId, const QString& reverseStr ){
    m_viewManager->reverseColorMap( colormapId, reverseStr );
    return "reverseColorMap";
}

QString ScriptFacade::setCacheColormap( const QString& colormapId, const QString& cacheStr ){
    QString output = m_viewManager->setCacheColormap( colormapId, cacheStr );
    return output;
}

QString ScriptFacade::setCacheSize( const QString& colormapId, const QString& cacheSize ){
    QString output = m_viewManager->setCacheSize( colormapId, cacheSize );
    return output;
}

QString ScriptFacade::setInterpolatedColorMap( const QString& colormapId, const QString& interpolateStr ){
    QString output = m_viewManager->setInterpolatedColorMap( colormapId, interpolateStr );
    return output;
}

QString ScriptFacade::invertColorMap( const QString& colormapId, const QString& invertStr ){
    m_viewManager->invertColorMap( colormapId, invertStr );
    return "invertColorMap";
}

QString ScriptFacade::setColorMix( const QString& colormapId, const QString& percentString ){
    m_viewManager->setColorMix( colormapId, percentString );
    return "setColorMix";
}

QString ScriptFacade::setGamma( const QString& colormapId, double gamma ){
    QString output = m_viewManager->setGamma( colormapId, gamma );
    return output;
}

QString ScriptFacade::setDataTransform( const QString& colormapId, const QString& transformString ){
    QString output = m_viewManager->setDataTransform( colormapId, transformString );
    return output;
}

QString ScriptFacade::setImageLayout(){
    m_viewManager->setImageView();
    return "setImageLayout";
}

QString ScriptFacade::setPlugins( const QStringList& names ) {
    m_viewManager->setPlugins( names );
    return "setPlugins";
}

QString ScriptFacade::setChannel( const QString& animatorId, int index ) {
    m_viewManager->setChannel( animatorId, index );
    return "setChannel";
}

QString ScriptFacade::setImage( const QString& animatorId, int index ) {
    m_viewManager->setImage( animatorId, index );
    return "setImage";
}

QString ScriptFacade::setClipValue( const QString& controlId, const QString& clipValue ) {
    const QString& param = "clipValue:" + clipValue;
    m_viewManager->setClipValue( controlId, param );
    return "setClipValue";
}

QString ScriptFacade::saveState( const QString& saveName ) {
    QString result = m_viewManager->saveState( saveName );
    return result;
}

QStringList ScriptFacade::getLinkedColorMaps( const QString& controlId ) {
    QStringList linkedColorMaps = m_viewManager->getLinkedColorMaps( controlId );
    return linkedColorMaps;
}

QStringList ScriptFacade::getLinkedAnimators( const QString& controlId ) {
    QStringList linkedAnimators = m_viewManager->getLinkedAnimators( controlId );
    return linkedAnimators;
}

QStringList ScriptFacade::getLinkedHistograms( const QString& controlId ) {
    QStringList linkedHistograms = m_viewManager->getLinkedHistograms( controlId );
    return linkedHistograms;
}

QStringList ScriptFacade::getLinkedStatistics( const QString& controlId ) {
    QStringList linkedStatistics = m_viewManager->getLinkedStatistics( controlId );
    return linkedStatistics;
}

QString ScriptFacade::updatePan( const QString& controlId, double x, double y ) {
    QString result = m_viewManager->updatePan( controlId, x, y );
    return result;
}

QString ScriptFacade::updateZoom( const QString& controlId, double x, double y, double z ) {
    QString result = m_viewManager->updateZoom( controlId, x, y, z );
    return result;
}
