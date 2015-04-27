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

QStringList ScriptFacade::getFileList() const {
    QString fileList = m_viewManager->getFileList();
    QStringList result(fileList);
    return result;
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

QStringList ScriptFacade::addLink( const QString& sourceId, const QString& destId ){
    QString la = m_viewManager->linkAdd( sourceId, destId );
    QStringList result(la);
    result.append(sourceId);
    result.append(destId);
    return result;
}

QStringList ScriptFacade::removeLink( const QString& sourceId, const QString& destId ){
    QString lr = m_viewManager->linkRemove( sourceId, destId );
    QStringList result(lr);
    result.append(sourceId);
    result.append(destId);
    return result;
}

QStringList ScriptFacade::loadFile( const QString& objectId, const QString& fileName ){
    m_viewManager->loadFile( objectId, fileName );
    QStringList result("loadFile");
    result.append(fileName);
    return result;
}

QStringList ScriptFacade::loadLocalFile( const QString& objectId, const QString& fileName ){
    m_viewManager->loadLocalFile( objectId, fileName );
    QStringList result("loadLocalFile");
    result.append(fileName);
    return result;
}

QStringList ScriptFacade::setAnalysisLayout(){
    m_viewManager->setAnalysisView();
    QStringList result("setAnalysisLayout");
    return result;
}

QStringList ScriptFacade::setCustomLayout( int rows, int cols ){
    m_viewManager->setCustomView( rows, cols );
    QStringList result("setAnalysisLayout");
    result.append("rows=" + QString::number(rows));
    result.append("cols=" + QString::number(cols));
    return result;
}

Carta::Data::Animator* ScriptFacade::getAnimator( const QString& index ){
    Carta::Data::Animator* animator;
    QString animId;
    int numAnimators = m_viewManager->getAnimatorCount();
    for ( int i = 0; i < numAnimators; i++ ) {
        QString animatorView = getAnimatorViewId( i );
        if ( animatorView == index ) {
            animId = m_viewManager->getObjectId( Carta::Data::Animator::CLASS_NAME, i );
            break;
        }
    }
    if ( animId != NULL ) {
        ObjectManager* objMan = ObjectManager::objectManager();
        QString id = objMan->parseId( animId );
        CartaObject* obj = objMan->getObject( id );
        animator = dynamic_cast<Carta::Data::Animator*>(obj);
    }
    return animator;
}

QStringList ScriptFacade::showImageAnimator( const QString& index ){
    QString result = "failure";
    Carta::Data::Animator* animator = getAnimator( index );
    if ( animator ){
        result = "success";
        QString animId;
        animator->addAnimator( "Image", animId );
    }
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setColorMap( const QString& colormapId, const QString& colormapName ){
    m_viewManager->setColorMap( colormapId, colormapName );
    QStringList result("setColorMap");
    result.append(colormapName);
    return result;
}

QStringList ScriptFacade::reverseColorMap( const QString& colormapId, const QString& reverseStr ){
    m_viewManager->reverseColorMap( colormapId, reverseStr );
    QStringList result("reverseColorMap");
    result.append(reverseStr);
    return result;
}

QStringList ScriptFacade::setCacheColormap( const QString& colormapId, const QString& cacheStr ){
    QString output = m_viewManager->setCacheColormap( colormapId, cacheStr );
    QStringList result(output);
    return result;
}

QStringList ScriptFacade::setCacheSize( const QString& colormapId, const QString& cacheSize ){
    QString output = m_viewManager->setCacheSize( colormapId, cacheSize );
    QStringList result(output);
    return result;
}

QStringList ScriptFacade::setInterpolatedColorMap( const QString& colormapId, const QString& interpolateStr ){
    QString output = m_viewManager->setInterpolatedColorMap( colormapId, interpolateStr );
    QStringList result(output);
    return result;
}

QStringList ScriptFacade::invertColorMap( const QString& colormapId, const QString& invertStr ){
    m_viewManager->invertColorMap( colormapId, invertStr );
    QStringList result("invertColorMap");
    result.append(invertStr);
    return result;
}

QStringList ScriptFacade::setColorMix( const QString& colormapId, const QString& percentString ){
    m_viewManager->setColorMix( colormapId, percentString );
    QStringList result("setColorMix");
    result.append(percentString);
    return result;
}

QStringList ScriptFacade::setGamma( const QString& colormapId, double gamma ){
    QString output = m_viewManager->setGamma( colormapId, gamma );
    QStringList result(output);
    return result;
}

QStringList ScriptFacade::setDataTransform( const QString& colormapId, const QString& transformString ){
    QString output = m_viewManager->setDataTransform( colormapId, transformString );
    QStringList result(output);
    return result;
}

QStringList ScriptFacade::setImageLayout(){
    m_viewManager->setImageView();
    QStringList result("setImageLayout");
    return result;
}

QStringList ScriptFacade::setPlugins( const QStringList& names ) {
    m_viewManager->setPlugins( names );
    QStringList result("setPlugins");
    result.append(names);
    return result;
}

QStringList ScriptFacade::setChannel( const QString& animatorId, int index ) {
    Carta::Data::Animator* animator = getAnimator( animatorId );
    if ( animator ){
        Carta::Data::AnimatorType* animType = animator->getAnimator( "Channel");
        if ( animType != nullptr ){
            animType->setFrame( index );
        }
        else {
            qDebug()<<"Could not get channel animator";
        }
    }
    else {
        qDebug() << "Could not find animator";
    }
    QStringList result("setChannel");
    return result;
}

QStringList ScriptFacade::setImage( const QString& animatorId, int index ) {
    m_viewManager->setImage( animatorId, index );
    QStringList result("setImage");
    result.append(QString::number(index));
    return result;
}

QStringList ScriptFacade::setClipValue( const QString& controlId, const QString& clipValue ) {
    const QString& param = "clipValue:" + clipValue;
    m_viewManager->setClipValue( controlId, param );
    QStringList result("setClipValue");
    result.append(clipValue);
    return result;
}

QStringList ScriptFacade::saveImage( const QString& controlId, const QString& filename ) {
    bool result = m_viewManager->saveImage( controlId, filename );
    QStringList resultList;
    if ( !result ) {
        resultList.append( "Could not save image to " + filename );
    }
    else {
        resultList.append("");
    }
    return resultList;
}

QStringList ScriptFacade::saveFullImage( const QString& controlId, const QString& filename, double scale ) {
    bool result = m_viewManager->saveFullImage( controlId, filename, scale );
    QStringList resultList;
    if ( !result ) {
        resultList.append( "Could not save full image to " + filename );
    }
    else {
        resultList.append("");
    }
    return resultList;
}

/*
QStringList ScriptFacade::saveState( const QString& saveName ) {
    QString result = m_viewManager->saveState( "fakeSessionId", saveName, true, true, true );
    QStringList resultList(result);
    return resultList;
}
*/

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

QStringList ScriptFacade::centerOnPixel( const QString& controlId, double x, double y ) {
    QString result = m_viewManager->centerOnPixel( controlId, x, y );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setZoomLevel( const QString& controlId, double zoomLevel ) {
    QString result = m_viewManager->setZoomLevel( controlId, zoomLevel );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::getZoomLevel( const QString& controlId ) {
    double zoom = m_viewManager->getZoomLevel( controlId );
    QStringList resultList( QString::number( zoom ) );
    return resultList;
}

QStringList ScriptFacade::getImageDimensions( const QString& controlId ) {
    QStringList result = m_viewManager->getImageDimensions( controlId );
    return result;
}

QStringList ScriptFacade::getOutputSize( const QString& controlId ) {
    QStringList result = m_viewManager->getOutputSize( controlId );
    return result;
}

QStringList ScriptFacade::setClipBuffer( const QString& histogramId, int bufferAmount ) {
    QString result = m_viewManager->setClipBuffer( histogramId, bufferAmount );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setUseClipBuffer( const QString& histogramId, const QString& useBuffer ) {
    QString result = m_viewManager->setUseClipBuffer( histogramId, useBuffer );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setClipRange( const QString& histogramId, double minRange, double maxRange ) {
    QString result = m_viewManager->setClipRange( histogramId, minRange, maxRange );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::applyClips( const QString& histogramId, double clipMinValue, double clipMaxValue, QString mode ) {
    QString result = m_viewManager->applyClips( histogramId, clipMinValue, clipMaxValue, mode );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::getIntensity( const QString& controlId, int frameLow, int frameHigh, double percentile ) {
    QString result = m_viewManager->getIntensity( controlId, frameLow, frameHigh, percentile );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setBinCount( const QString& histogramId, int binCount ) {
    QString result = m_viewManager->setBinCount( histogramId, binCount );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setBinWidth( const QString& histogramId, double binWidth ) {
    QString result = m_viewManager->setBinWidth( histogramId, binWidth );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setPlaneMode( const QString& histogramId, const QString& planeMode ) {
    QString result = m_viewManager->setPlaneMode( histogramId, planeMode );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setPlaneRange( const QString& histogramId, double minPlane, double maxPlane) {
    QString result = m_viewManager->setPlaneRange( histogramId, minPlane, maxPlane );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setChannelUnit( const QString& histogramId, const QString& units ) {
    QString result = m_viewManager->setChannelUnit( histogramId, units );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setGraphStyle( const QString& histogramId, const QString& style ) {
    QString result = m_viewManager->setGraphStyle( histogramId, style );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setLogCount( const QString& histogramId, const QString& logCount ) {
    QString result = m_viewManager->setLogCount( histogramId, logCount );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::setColored( const QString& histogramId, const QString& colored ) {
    QString result = m_viewManager->setColored( histogramId, colored );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::saveHistogram( const QString& histogramId, const QString& filename, int width, int height ) {
    QString result = m_viewManager->saveHistogram( histogramId, filename, width, height );
    QStringList resultList(result);
    return resultList;
}
