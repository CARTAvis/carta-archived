#include "ScriptFacade.h"
#include "Data/Snapshot/Snapshots.h"
#include "Data/ViewManager.h"
#include "Data/Animator/Animator.h"
#include "Data/Animator/AnimatorType.h"
#include "Data/Image/Controller.h"
#include "Data/Colormap/Colormap.h"
#include "Data/Colormap/Colormaps.h"
#include "Data/Util.h"
#include "Data/Histogram/Histogram.h"
#include "Data/Layout/Layout.h"
#include "Data/Statistics.h"
#include "Data/Image/GridControls.h"

#include <QDebug>

using Carta::State::ObjectManager;
//using Carta::State::CartaObject;

QString ScriptFacade::TOGGLE = "toggle";

ScriptFacade * ScriptFacade::getInstance (){
    static ScriptFacade * sc = new ScriptFacade ();
    return sc;
}


ScriptFacade::ScriptFacade(){
    m_viewManager = Carta::Data::Util::findSingletonObject<Carta::Data::ViewManager>();

    int numControllers = m_viewManager->getControllerCount();
    for (int i = 0; i < numControllers; i++) {
        QString imageView = getImageViewId( i );
        ObjectManager* objMan = ObjectManager::objectManager();
        QString id = objMan->parseId( imageView );
        Carta::State::CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
            if ( controller != nullptr ){
                connect( controller, & Carta::Data::Controller::saveImageResult, this, & ScriptFacade::saveImageResultCB );
            }
        }
    }
}

QString ScriptFacade::getColorMapId( int index ) const {
    return m_viewManager->getObjectId( Carta::Data::Colormap::CLASS_NAME, index);
}

QStringList ScriptFacade::getColorMaps() const {
    Carta::Data::Colormaps* maps = Carta::Data::Util::findSingletonObject<Carta::Data::Colormaps>();
    return maps->getColorMaps();
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
    if (numControllers == 0) {
        imageViewList = QStringList("");
    }
    return imageViewList;
}

QStringList ScriptFacade::getColorMapViews() {
    QStringList colorMapViewList;
    int numColorMaps = m_viewManager->getColormapCount();
    for (int i = 0; i < numColorMaps; i++) {
        QString colorMapView = getColorMapId( i );
        colorMapViewList << colorMapView;
    }
    if (numColorMaps == 0) {
        colorMapViewList = QStringList("");
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
    if (numAnimators == 0) {
        animatorViewList = QStringList("");
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
    if (numHistograms == 0) {
        histogramViewList = QStringList("");
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
    if (numStatistics == 0) {
        statisticsViewList = QStringList("");
    }
    return statisticsViewList;
}

QStringList ScriptFacade::addLink( const QString& sourceId, const QString& destId ){
    QStringList resultList("");
    QString result = m_viewManager->linkAdd( sourceId, destId );
    if ( result != "" ) {
        resultList = QStringList( result );
    }
    return resultList;
}

QStringList ScriptFacade::removeLink( const QString& sourceId, const QString& destId ){
    QStringList resultList("");
    QString result = m_viewManager->linkRemove( sourceId, destId );
    if ( result != "" ) {
        resultList = QStringList( result );
    }
    return resultList;
}

QStringList ScriptFacade::setImageLayout(){
    m_viewManager->setImageView();
    QStringList result("");
    return result;
}

QStringList ScriptFacade::setPlugins( const QStringList& names ) {
    QStringList resultList("");
    bool result = m_viewManager->setPlugins( names );
    if ( result == false ) {
        resultList = QStringList( "error" );
        QString errorStr = "There was an error setting the plugins: " + names.join( ',' );
        resultList.append( errorStr );
    }
    return resultList;
}

QStringList ScriptFacade::loadFile( const QString& objectId, const QString& fileName ){
    QStringList resultList("");
    bool result = m_viewManager->loadFile( objectId, fileName );
    if ( result == false ) {
        resultList = QStringList( "error" );
        QString errorStr = "Could not load file " + fileName;
        resultList.append( errorStr );
    }
    return resultList;
}

QStringList ScriptFacade::loadLocalFile( const QString& objectId, const QString& fileName ){
    QStringList resultList("");
    bool result = m_viewManager->loadLocalFile( objectId, fileName );
    if ( result == false ) {
        resultList = QStringList( "error" );
        QString errorStr = "Could not load file " + fileName;
        resultList.append( errorStr );
    }
    return resultList;
}

QStringList ScriptFacade::setAnalysisLayout(){
    m_viewManager->setAnalysisView();
    QStringList result("");
    return result;
}

QStringList ScriptFacade::setCustomLayout( int rows, int cols ){
    QStringList resultList;
    Carta::Data::Layout* layout = Carta::Data::Util::findSingletonObject<Carta::Data::Layout>();
    QString resultStr = layout->setLayoutSize( rows, cols );
    resultList = QStringList( resultStr );

    //QString resultStr = m_viewManager->setCustomView( rows, cols );
    //QStringList result( resultStr );
    //return result;
    return resultList;
}

QStringList ScriptFacade::setColorMap( const QString& colormapId, const QString& colormapName ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setColorMap( colormapName );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified colormap view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::reverseColorMap( const QString& colormapId, const QString& reverseStr ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            bool reverse = false;
            bool validBool = true;
            if ( reverseStr == TOGGLE ){
                reverse = ! colormap->isReversed();
            }
            else {
                reverse = Carta::Data::Util::toBool( reverseStr, &validBool );
            }
            if ( validBool ){
                QString result = colormap->reverseColorMap( reverse );
                resultList = QStringList( result );
            }
            else {
                resultList = QStringList( "error");
                resultList.append( "An invalid value was passed to reverse color map");
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified colormap view could not be found." );
    }
    return resultList;
}

/*QStringList ScriptFacade::setCacheColormap( const QString& colormapId, const QString& cacheStr ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setCacheColormap( cacheStr );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified colormap view could not be found." );
    }
    return resultList;
}*/

/*QStringList ScriptFacade::setCacheSize( const QString& colormapId, const QString& cacheSize ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setCacheSize( cacheSize );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified colormap view could not be found." );
    }
    return resultList;
}*/

/*QStringList ScriptFacade::setInterpolatedColorMap( const QString& colormapId, const QString& interpolateStr ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setInterpolatedColorMap( interpolateStr );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified colormap view could not be found." );
    }
    return resultList;
}*/

QStringList ScriptFacade::invertColorMap( const QString& colormapId, const QString& invertStr ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
           bool invert = false;
           bool validBool = true;
           if ( invertStr == TOGGLE ){
               invert = ! colormap->isInverted();
           }
           else {
               invert = Carta::Data::Util::toBool( invertStr, &validBool );
           }
           if ( validBool ){
               QString result = colormap->invertColorMap( invert );
               resultList = QStringList( result );
           }
           else {
               resultList = QStringList( "error");
               resultList.append( "An unrecognized parameter was passed to invert color map");
           }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified colormap view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setColorMix( const QString& colormapId, double red, double green, double blue ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setColorMix( red, green, blue );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified colormap view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setGamma( const QString& colormapId, double gamma ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setGamma( gamma );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified colormap view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setDataTransform( const QString& colormapId, const QString& transformString ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( colormapId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setDataTransform( transformString );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified colormap view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::showImageAnimator( const QString& animatorId ){
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( animatorId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
        if ( animator != nullptr){
            QString animId; 
            animator->addAnimator( "Image", animId );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified animator could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setChannel( const QString& animatorId, int index ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( animatorId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
        if ( animator != nullptr){
            Carta::Data::AnimatorType* animType = animator->getAnimator( "Channel");
            if ( animType != nullptr ){
                animType->setFrame( index );
            }
            else {
                qDebug()<<"Could not get channel animator";
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified animator could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setImage( const QString& animatorId, int index ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( animatorId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
        if ( animator != nullptr){
            animator->changeImageIndex( index );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified animator could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setClipValue( const QString& controlId, double clipValue ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->setClipValue( clipValue );
            resultList = QStringList( result );

        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified colormap view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::saveImage( const QString& controlId, const QString& filename ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            bool result = controller->saveImage( filename );
            if ( !result ) {
                resultList = QStringList( "Could not save image to " + filename );
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

void ScriptFacade::saveFullImage( const QString& controlId, const QString& filename, int width, int height, double scale, Qt::AspectRatioMode aspectRatioMode ){
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->saveFullImage( filename, width, height, scale, aspectRatioMode );
        }
    }
}

void ScriptFacade::saveImageResultCB( bool result ){
    emit saveImageResult( result );
}


QStringList ScriptFacade::saveSnapshot( const QString& sessionId, const QString& saveName, bool saveLayout, bool savePreferences, bool saveData, const QString& description ){
    Carta::State::ObjectManager* objMan = ObjectManager::objectManager();
    Carta::Data::Snapshots* m_snapshots = objMan->createObject<Carta::Data::Snapshots>();
    QString result = m_snapshots->saveSnapshot( sessionId, saveName, saveLayout, savePreferences, saveData, description );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::getSnapshots( const QString& sessionId ){
    Carta::State::ObjectManager* objMan = ObjectManager::objectManager();
    Carta::Data::Snapshots* snapshots = objMan->createObject<Carta::Data::Snapshots>();
    QStringList resultList;
    QList<Carta::Data::Snapshot> snapshotList = snapshots->getSnapshots( sessionId );
    int count = snapshotList.size();
    if ( count == 0 ) {
        resultList = QStringList("");
    }
    for ( int i = 0; i < count; i++ ){
        resultList.append( snapshotList[i].getName() );
    }
    return resultList;
}

QStringList ScriptFacade::getSnapshotObjects( const QString& sessionId ){
    Carta::State::ObjectManager* objMan = ObjectManager::objectManager();
    Carta::Data::Snapshots* snapshots = objMan->createObject<Carta::Data::Snapshots>();
    QStringList resultList;
    QList<Carta::Data::Snapshot> snapshotList = snapshots->getSnapshots( sessionId );
    int count = snapshotList.size();
    if ( count == 0 ) {
        resultList = QStringList("");
    }
    for ( int i = 0; i < count; i++ ){
        resultList.append( snapshotList[i].toString() );
    }
    return resultList;
}

QStringList ScriptFacade::deleteSnapshot( const QString& sessionId, const QString& saveName ){
    Carta::State::ObjectManager* objMan = ObjectManager::objectManager();
    Carta::Data::Snapshots* snapshots = objMan->createObject<Carta::Data::Snapshots>();
    QString result = snapshots->deleteSnapshot( sessionId, saveName );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::restoreSnapshot( const QString& sessionId, const QString& saveName ){
    Carta::State::ObjectManager* objMan = ObjectManager::objectManager();
    Carta::Data::Snapshots* snapshots = objMan->createObject<Carta::Data::Snapshots>();
    QString result = snapshots->restoreSnapshot( sessionId, saveName );
    QStringList resultList(result);
    return resultList;
}

QStringList ScriptFacade::getLinkedColorMaps( const QString& controlId ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = 0; i < m_viewManager->getColormapCount(); i++ ){
        QString colormapId = getColorMapId( i );
        QString id = objMan->parseId( colormapId );
        Carta::State::CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
            QList<QString> oldLinks = colormap->getLinks();
            if (oldLinks.contains( controlId )) {
                resultList.append( colormapId );
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "Could not find colormap." );
        }
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::getLinkedAnimators( const QString& controlId ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = 0; i < m_viewManager->getAnimatorCount(); i++ ){
        QString animatorId = getAnimatorViewId( i );
        QString id = objMan->parseId( animatorId );
        Carta::State::CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
            QList<QString> oldLinks = animator->getLinks();
            if (oldLinks.contains( controlId )) {
                resultList.append( animatorId );
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "Could not find animator." );
        }
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::getLinkedHistograms( const QString& controlId ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = 0; i < m_viewManager->getHistogramCount(); i++ ){
        QString histogramId = getHistogramViewId( i );
        QString id = objMan->parseId( histogramId );
        Carta::State::CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
            QList<QString> oldLinks = histogram->getLinks();
            if (oldLinks.contains( controlId )) {
                resultList.append( histogramId );
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "Could not find histogram." );
        }
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::getLinkedStatistics( const QString& controlId ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( int i = 0; i < m_viewManager->getStatisticsCount(); i++ ){
        QString statisticsId = getStatisticsViewId( i );
        QString id = objMan->parseId( statisticsId );
        Carta::State::CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Statistics* statistics = dynamic_cast<Carta::Data::Statistics*>(obj);
            QList<QString> oldLinks = statistics->getLinks();
            if (oldLinks.contains( controlId )) {
                resultList.append( statisticsId );
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "Could not find statistics view." );
        }
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::centerOnPixel( const QString& controlId, double x, double y ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->centerOnPixel( x, y );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setZoomLevel( const QString& controlId, double zoomLevel ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->setZoomLevel( zoomLevel );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::getZoomLevel( const QString& controlId ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            double zoomLevel = controller->getZoomLevel( );
            resultList = QStringList( QString::number( zoomLevel ) );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::getImageDimensions( const QString& controlId ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getImageDimensions( );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::getOutputSize( const QString& controlId ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getOutputSize( );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::getPixelCoordinates( const QString& controlId, double ra, double dec ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getPixelCoordinates( ra, dec );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::getPixelValue( const QString& controlId, double x, double y ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString resultStr = controller->getPixelValue( x, y );
            resultList = QStringList( resultStr );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::getPixelUnits( const QString& controlId ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString resultStr = controller->getPixelUnits();
            resultList = QStringList( resultStr );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::getCoordinates( const QString& controlId, double x, double y, const Carta::Lib::KnownSkyCS system ){
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getCoordinates( x, y, system );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::getImageNames( const QString& controlId ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            int imageCount = controller->getStackedImageCount();
            if ( imageCount == 0 ) {
                resultList = QStringList("");
            }
            for ( int i = 0; i < imageCount; i++ ) {
                resultList.append( controller->getImageName( i ) );
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::closeImage( const QString& controlId, const QString& imageName ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->closeImage( imageName );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setClipBuffer( const QString& histogramId, int bufferAmount ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setClipBuffer( bufferAmount );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified histogram view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setUseClipBuffer( const QString& histogramId, const QString& useBufferStr ) {
    QStringList resultList;
    bool validBool = false;
    bool useBuffer = Carta::Data::Util::toBool( useBufferStr, &validBool );
    if ( validBool || useBufferStr.toLower() == "toggle" ) {
        ObjectManager* objMan = ObjectManager::objectManager();
        QString id = objMan->parseId( histogramId );
        Carta::State::CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
            if ( histogram != nullptr ){
                if ( useBufferStr.toLower() == "toggle" ) {
                    bool currentUseBuffer = histogram->getUseClipBuffer();
                    useBuffer = !currentUseBuffer;
                }
                QString result = histogram->setUseClipBuffer( useBuffer );
                resultList = QStringList( result );
            }
            else {
                resultList = QStringList( "error" );
                resultList.append( "An unknown error has occurred." );
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "The specified histogram view could not be found." );
        }
    }
    else {
        resultList = QStringList( "Set clip buffer parameter must be true/false or 'toggle': " + useBufferStr );
    }
    return resultList;
}

QStringList ScriptFacade::setClipRange( const QString& histogramId, double minRange, double maxRange ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setClipRange( minRange, maxRange );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified histogram view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::applyClips( const QString& histogramId, double clipMinValue, double clipMaxValue, QString mode ) {
    QStringList resultList;
    QString result = "";
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            if ( mode == "percent" ) {
                result += histogram->setClipMinPercent( clipMinValue );
                result += histogram->setClipMaxPercent( clipMaxValue );
            }
            else if ( mode == "intensity" ) {
                result += histogram->setClipMin( clipMinValue );
                result += histogram->setClipMax( clipMaxValue );
            }
            else {
                result = "invalid mode: " + mode;
            }
            histogram->applyClips();
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified histogram view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::getIntensity( const QString& controlId, int frameLow, int frameHigh, double percentile ) {
    QStringList resultList;
    double intensity;
    bool valid;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            valid = controller->getIntensity( frameLow, frameHigh, percentile, &intensity );
            if ( valid ) {
                resultList = QStringList( QString::number( intensity ) );
            }
            else {
                resultList = QStringList( "error" );
                resultList.append( "Could not get intensity for the specified parameters." );
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    qDebug() << "resultList =" << resultList;
    return resultList;
}

QStringList ScriptFacade::setBinCount( const QString& histogramId, int binCount ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setBinCount( binCount );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified histogram could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setBinWidth( const QString& histogramId, double binWidth ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setBinWidth( binWidth );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified histogram could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setPlaneMode( const QString& histogramId, const QString& planeMode ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setPlaneMode( planeMode );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified histogram could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setPlaneRange( const QString& histogramId, double minPlane, double maxPlane) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setPlaneRange( minPlane, maxPlane );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified histogram could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setChannelUnit( const QString& histogramId, const QString& units ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setChannelUnit( units );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified histogram could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setGraphStyle( const QString& histogramId, const QString& style ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setGraphStyle( style );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified histogram could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setLogCount( const QString& histogramId, const QString& logCountStr ) {
    QStringList resultList;
    bool validBool = false;
    bool logCount = Carta::Data::Util::toBool( logCountStr, &validBool );
    if ( validBool || logCountStr.toLower() == "toggle" ) {
        ObjectManager* objMan = ObjectManager::objectManager();
        QString id = objMan->parseId( histogramId );
        Carta::State::CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
            if ( histogram != nullptr ){
                if ( logCountStr.toLower() == "toggle" ) {
                    bool currentLogCount = histogram->getLogCount();
                    logCount = !currentLogCount;
                }
                QString result = histogram->setLogCount( logCount );
                resultList = QStringList( result );
            }
            else {
                resultList = QStringList( "error" );
                resultList.append( "An unknown error has occurred." );
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "The specified histogram could not be found." );
        }
    }
    else {
        resultList = QStringList( "Set log count parameter must be true/false or 'toggle': " + logCountStr );
    }
    return resultList;
}

QStringList ScriptFacade::setColored( const QString& histogramId, const QString& coloredStr ) {
    QStringList resultList;
    bool validBool = false;
    bool colored = Carta::Data::Util::toBool( coloredStr, &validBool );
    if ( validBool || coloredStr.toLower() == "toggle" ) {
        ObjectManager* objMan = ObjectManager::objectManager();
        QString id = objMan->parseId( histogramId );
        Carta::State::CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
            if ( histogram != nullptr ){
                if ( coloredStr.toLower() == "toggle" ) {
                    bool currentColored = histogram->getColored();
                    colored = !currentColored;
                }
                QString result = histogram->setColored( colored );
                resultList = QStringList( result );
            }
            else {
                resultList = QStringList( "error" );
                resultList.append( "An unknown error has occurred." );
            }
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "The specified histogram could not be found." );
        }
    }
    else {
        resultList = QStringList( "Set colored parameter must be true/false or 'toggle': " + coloredStr );
    }
    return resultList;
}

QStringList ScriptFacade::saveHistogram( const QString& histogramId, const QString& filename, int width, int height ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( histogramId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->saveHistogram( filename, width, height );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified histogram could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setGridAxesColor( const QString& controlId, int red, int green, int blue ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->setGridAxesColor( red, green, blue );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridAxesThickness( const QString& controlId, int thickness ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->setGridAxesThickness( thickness );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridAxesTransparency( const QString& controlId, int transparency ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->setGridAxesTransparency( transparency );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridApplyAll( const QString& controlId, bool applyAll ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->setGridApplyAll( applyAll );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setGridCoordinateSystem( const QString& controlId, const QString& coordSystem ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->setGridCoordinateSystem( coordSystem );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridFontFamily( const QString& controlId, const QString& fontFamily ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->setGridFontFamily( fontFamily );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridFontSize( const QString& controlId, int fontSize ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->setGridFontSize( fontSize );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridColor( const QString& controlId, int redAmount, int greenAmount, int blueAmount ) {
    QStringList resultList;
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->setGridColor( redAmount, greenAmount, blueAmount );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridLines( const QString& controlId, bool showGridLines ) {
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( controlId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->setShowGridLines( showGridLines );
            resultList = QStringList( result );
        }
        else {
            resultList = QStringList( "error" );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( "error" );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}
