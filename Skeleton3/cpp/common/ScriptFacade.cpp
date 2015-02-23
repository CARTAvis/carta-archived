#include "ScriptFacade.h"
#include "Data/ViewManager.h"
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

QString ScriptFacade::linkAdd( const QString& sourceId, const QString& destId ){
    QString result = m_viewManager->linkAdd( sourceId, destId );
    return result;
}

void ScriptFacade::loadFile( const QString& objectId, const QString& fileName ){
    m_viewManager->loadFile( objectId, fileName );
}

void ScriptFacade::loadLocalFile( const QString& objectId, const QString& fileName ){
    m_viewManager->loadLocalFile( objectId, fileName );
}

void ScriptFacade::setAnalysisLayout(){
    m_viewManager->setAnalysisView();
}

void ScriptFacade::setCustomLayout( int rows, int cols ){
    m_viewManager->setCustomView( rows, cols );
}

void ScriptFacade::setColorMap( const QString& colormapId, const QString& colormapName ){
    m_viewManager->setColorMap( colormapId, colormapName );
}

void ScriptFacade::reverseColorMap( const QString& colormapId, const QString& trueOrFalse ){
    m_viewManager->reverseColorMap( colormapId, trueOrFalse );
}

QString ScriptFacade::interpolateColorMap( const QString& colormapId, const QString& trueOrFalse ){
    QString output = m_viewManager->interpolateColorMap( colormapId, trueOrFalse );
    return output;
}

void ScriptFacade::invertColorMap( const QString& colormapId, const QString& trueOrFalse ){
    m_viewManager->invertColorMap( colormapId, trueOrFalse );
}

void ScriptFacade::setColorMix( const QString& colormapId, const QString& percentString ){
    m_viewManager->setColorMix( colormapId, percentString );
}

QString ScriptFacade::setGamma( const QString& colormapId, double gamma ){
    QString output = m_viewManager->setGamma( colormapId, gamma );
    return output;
}

void ScriptFacade::setImageLayout(){
    m_viewManager->setImageView();
}

void ScriptFacade::setPlugins( const QStringList& names ) {
    m_viewManager->setPlugins( names );
}

void ScriptFacade::setFrame( const QString& animatorId, const QString& index ) {
    m_viewManager->setFrame( animatorId, index );
}

void ScriptFacade::setClipValue( const QString& controlId, const QString& clipValue ) {
    const QString& param = "clipValue:" + clipValue;
    m_viewManager->setClipValue( controlId, param );
}

bool ScriptFacade::saveState( const QString& saveName ) {
    bool result = m_viewManager->saveState( saveName );
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
