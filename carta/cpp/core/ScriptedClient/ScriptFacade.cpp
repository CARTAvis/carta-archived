#include "ScriptFacade.h"
#include "Data/Snapshot/Snapshots.h"
#include "Data/ViewManager.h"
#include "Data/Animator/Animator.h"
#include "Data/Animator/AnimatorType.h"
#include "Data/Image/Controller.h"
#include "Data/Selection.h"
#include "Data/Colormap/Colormap.h"
#include "Data/Colormap/Colormaps.h"
#include "Data/Util.h"
#include "Data/Histogram/Histogram.h"
#include "Data/Layout/Layout.h"
#include "Data/Preferences/PreferencesSave.h"
#include "Data/Statistics.h"
#include "Data/Image/Grid/GridControls.h"

#include <QDebug>

using Carta::State::ObjectManager;
//using Carta::State::CartaObject;

const QString ScriptFacade::TOGGLE = "toggle";
const QString ScriptFacade::ERROR = "error";
const QString ScriptFacade::UNKNOWN_ERROR = "An unknown error has occurred";

ScriptFacade * ScriptFacade::getInstance (){
    static ScriptFacade * sc = new ScriptFacade ();
    return sc;
}


ScriptFacade::ScriptFacade(){
    m_viewManager = Carta::Data::Util::findSingletonObject<Carta::Data::ViewManager>();

    int numControllers = m_viewManager->getControllerCount();
    for (int i = 0; i < numControllers; i++) {
        QString imageView = getImageViewId( i );
        Carta::State::CartaObject* obj = _getObject( imageView );
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
        resultList = _logErrorMessage( ERROR, "There was an error setting the plugins: " + names.join( ',' ) );
    }
    return resultList;
}

QStringList ScriptFacade::getPluginList() const {
    QStringList resultList;
    Carta::Data::Layout* layout = Carta::Data::Util::findSingletonObject<Carta::Data::Layout>();
    resultList = layout->getPluginList();
    return resultList;
}

QStringList ScriptFacade::loadFile( const QString& objectId, const QString& fileName ){
    QStringList resultList("");
    bool result = m_viewManager->loadFile( objectId, fileName );
    if ( result == false ) {
        resultList = _logErrorMessage( ERROR, "Could not load file " + fileName );
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
    return resultList;
}

QStringList ScriptFacade::setColorMap( const QString& colormapId, const QString& colormapName ){
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( colormapId );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setColorMap( colormapName );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified colormap view could not be found: " + colormapId );
    }
    return resultList;
}

QStringList ScriptFacade::reverseColorMap( const QString& colormapId, const QString& reverseStr ){
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( colormapId );
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
                resultList = _logErrorMessage( ERROR, "An invalid value was passed to reverse color map: " + reverseStr );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified colormap view could not be found: " + colormapId );
    }
    return resultList;
}

QStringList ScriptFacade::invertColorMap( const QString& colormapId, const QString& invertStr ){
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( colormapId );
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
                resultList = _logErrorMessage( ERROR, "An unrecognized parameter was passed to invert color map: " + invertStr );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified colormap view could not be found: " + colormapId );
    }
    return resultList;
}

QStringList ScriptFacade::setColorMix( const QString& colormapId, double red, double green, double blue ){
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( colormapId );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setColorMix( red, green, blue );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified colormap view could not be found: " + colormapId );
    }
    return resultList;
}

QStringList ScriptFacade::setGamma( const QString& colormapId, double gamma ){
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( colormapId );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setGamma( gamma );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified colormap view could not be found: " + colormapId );
    }
    return resultList;
}

QStringList ScriptFacade::setDataTransform( const QString& colormapId, const QString& transformString ){
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( colormapId );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            QString result = colormap->setDataTransform( transformString );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified colormap view could not be found: " + colormapId );
    }
    return resultList;
}

QStringList ScriptFacade::showImageAnimator( const QString& animatorId ){
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( animatorId );
    if ( obj != nullptr ){
        Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
        if ( animator != nullptr){
            QString animId; 
            animator->addAnimator( "Image", animId );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified animator could not be found: " + animatorId );
    }
    return resultList;
}

QStringList ScriptFacade::getMaxImageCount( const QString& animatorId ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( animatorId );
    if ( obj != nullptr ){
        Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
        if ( animator != nullptr){
            int result = animator->getMaxImageCount();
            resultList = QStringList( QString::number( result ) );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified animator could not be found: " + animatorId );
    }
    return resultList;
}

QStringList ScriptFacade::getChannelIndex( const QString& animatorId ){
    QStringList resultList("");
    ObjectManager* objMan = ObjectManager::objectManager();
    QString id = objMan->parseId( animatorId );
    Carta::State::CartaObject* obj = objMan->getObject( id );
    if ( obj != nullptr ){
        Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
        if ( animator != nullptr){
            Carta::Data::AnimatorType* animType = animator->getAnimator( "Channel");
            if ( animType != nullptr ){
                int frame = animType->getFrame();
                resultList = QStringList( QString::number( frame ) );
            }
            else {
                resultList = QStringList( ERROR );
                resultList.append( "Could not get channel animator." );
            }
        }
        else {
            resultList = QStringList( ERROR );
            resultList.append( "An unknown error has occurred." );
        }
    }
    else {
        resultList = QStringList( ERROR );
        resultList.append( "The specified animator could not be found." );
    }
    return resultList;
}

QStringList ScriptFacade::setChannel( const QString& animatorId, int index ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( animatorId );
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
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified animator could not be found: " + animatorId );
    }
    return resultList;
}

QStringList ScriptFacade::setImage( const QString& animatorId, int index ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( animatorId );
    if ( obj != nullptr ){
        Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
        if ( animator != nullptr){
            animator->changeFrame( index, Carta::Data::Selection::IMAGE );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified animator could not be found: " + animatorId );
    }
    return resultList;
}

QStringList ScriptFacade::setClipValue( const QString& controlId, double clipValue ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->setClipValue( clipValue );
            resultList = QStringList( result );

        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified controller could not be found: " + controlId );
    }
    return resultList;
}

/*QStringList ScriptFacade::saveImage( const QString& controlId, const QString& filename ) {
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
            resultList = QStringList( ERROR );
            resultList.append( UNKNOWN_ERROR );
        }
    }
    else {
        resultList = QStringList( ERROR );
        resultList.append( "The specified image view could not be found." );
    }
    return resultList;
}*/

QStringList ScriptFacade::saveFullImage( const QString& controlId, const QString& filename, int width, int height,
        double scale, /*Qt::AspectRatioMode aspectRatioMode*/ const QString& aspectModeStr ){
    ObjectManager* objMan = ObjectManager::objectManager();
    //Save the state so the view will update and parse parameters to make
    //sure they are valid before calling save.
    Carta::Data::PreferencesSave* prefSave = Carta::Data::Util::findSingletonObject<Carta::Data::PreferencesSave>();
    QStringList errorList("");
    QString widthError = prefSave->setWidth( width );
    QString heightError = prefSave->setHeight( height );
    QString aspectModeError = prefSave->setAspectRatioMode( aspectModeStr );
    if ( widthError.isEmpty() && heightError.isEmpty() && aspectModeError.isEmpty() ){
        QString id = objMan->parseId( controlId );
        Carta::State::CartaObject* obj = objMan->getObject( id );
        if ( obj != nullptr ){
            Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
            if ( controller != nullptr ){
                controller->saveImage( filename, scale );
            }
        }
    }
    else {
        if ( !widthError.isEmpty()){
            errorList.append( widthError );
        }
        if ( !heightError.isEmpty()){
            errorList.append( heightError );
        }
        if ( !aspectModeError.isEmpty() ){
            errorList.append( aspectModeError );
        }
    }
    return errorList;
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
            resultList = _logErrorMessage( ERROR, "Could not find colormap." );
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
            resultList = _logErrorMessage( ERROR, "Could not find animator." );
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
            resultList = _logErrorMessage( ERROR, "Could not find histogram." );
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
            resultList = _logErrorMessage( ERROR, "Could not find statistics view." );
        }
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::centerOnPixel( const QString& controlId, double x, double y ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->centerOnPixel( x, y );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::setZoomLevel( const QString& controlId, double zoomLevel ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->setZoomLevel( zoomLevel );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getZoomLevel( const QString& controlId ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            double zoomLevel = controller->getZoomLevel( );
            resultList = QStringList( QString::number( zoomLevel ) );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::resetZoom( const QString& controlId ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->resetZoom();
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::centerImage( const QString& controlId ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->resetPan();
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getImageDimensions( const QString& controlId ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getImageDimensions( );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getOutputSize( const QString& controlId ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getOutputSize( );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getPixelCoordinates( const QString& controlId, double ra, double dec ){
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getPixelCoordinates( ra, dec );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getPixelValue( const QString& controlId, double x, double y ){
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString resultStr = controller->getPixelValue( x, y );
            resultList = QStringList( resultStr );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getPixelUnits( const QString& controlId ){
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString resultStr = controller->getPixelUnits();
            resultList = QStringList( resultStr );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getCoordinates( const QString& controlId, double x, double y, const Carta::Lib::KnownSkyCS system ){
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getCoordinates( x, y, system );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getImageNames( const QString& controlId ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
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
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::closeImage( const QString& controlId, const QString& imageName ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->closeImage( imageName );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::setClipBuffer( const QString& histogramId, int bufferAmount ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setClipBuffer( bufferAmount );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::setUseClipBuffer( const QString& histogramId, const QString& useBufferStr ) {
    QStringList resultList;
    bool validBool = false;
    bool useBuffer = Carta::Data::Util::toBool( useBufferStr, &validBool );
    if ( validBool || useBufferStr.toLower() == TOGGLE ) {
        Carta::State::CartaObject* obj = _getObject( histogramId );
        if ( obj != nullptr ){
            Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
            if ( histogram != nullptr ){
                if ( useBufferStr.toLower() == TOGGLE ) {
                    bool currentUseBuffer = histogram->getUseClipBuffer();
                    useBuffer = !currentUseBuffer;
                }
                QString result = histogram->setUseClipBuffer( useBuffer );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "Set clip buffer parameter must be true/false or 'toggle': " + useBufferStr );
    }
    return resultList;
}

QStringList ScriptFacade::setClipRange( const QString& histogramId, double minRange, double maxRange ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setClipRange( minRange, maxRange );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::setClipRangePercent( const QString& histogramId, double minPercent, double maxPercent ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setClipRangePercent( minPercent, maxPercent );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::getClipRange( const QString& histogramId ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            std::pair<double, double> clipRange = histogram->getClipRange();
            resultList << QString::number( clipRange.first) << QString::number( clipRange.second );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::applyClips( const QString& histogramId ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            histogram->applyClips();
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::getIntensity( const QString& controlId, int frameLow, int frameHigh, double percentile ) {
    QStringList resultList;
    double intensity;
    bool valid;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            valid = controller->getIntensity( frameLow, frameHigh, percentile, &intensity );
            if ( valid ) {
                resultList = QStringList( QString::number( intensity ) );
            }
            else {
                resultList = _logErrorMessage( ERROR, "Could not get intensity for the specified parameters." );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::setBinCount( const QString& histogramId, int binCount ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setBinCount( binCount );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::setBinWidth( const QString& histogramId, double binWidth ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setBinWidth( binWidth );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::setPlaneMode( const QString& histogramId, const QString& planeMode ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setPlaneMode( planeMode );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::setPlaneRange( const QString& histogramId, double minPlane, double maxPlane) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setPlaneRange( minPlane, maxPlane );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::setChannelUnit( const QString& histogramId, const QString& units ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setChannelUnit( units );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::setGraphStyle( const QString& histogramId, const QString& style ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString result = histogram->setGraphStyle( style );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::setLogCount( const QString& histogramId, const QString& logCountStr ) {
    QStringList resultList;
    bool validBool = false;
    bool logCount = Carta::Data::Util::toBool( logCountStr, &validBool );
    if ( validBool || logCountStr.toLower() == TOGGLE ) {
        Carta::State::CartaObject* obj = _getObject( histogramId );
        if ( obj != nullptr ){
            Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
            if ( histogram != nullptr ){
                if ( logCountStr.toLower() == TOGGLE ) {
                    bool currentLogCount = histogram->getLogCount();
                    logCount = !currentLogCount;
                }
                QString result = histogram->setLogCount( logCount );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "Set log count parameter must be true/false or 'toggle': " + logCountStr );
    }
    return resultList;
}

QStringList ScriptFacade::setColored( const QString& histogramId, const QString& coloredStr ) {
    QStringList resultList;
    bool validBool = false;
    bool colored = Carta::Data::Util::toBool( coloredStr, &validBool );
    if ( validBool || coloredStr.toLower() == TOGGLE ) {
        Carta::State::CartaObject* obj = _getObject( histogramId );
        if ( obj != nullptr ){
            Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
            if ( histogram != nullptr ){
                if ( coloredStr.toLower() == TOGGLE ) {
                    bool currentColored = histogram->getColored();
                    colored = !currentColored;
                }
                QString result = histogram->setColored( colored );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "Set colored parameter must be true/false or 'toggle': " + coloredStr );
    }
    return resultList;
}

QStringList ScriptFacade::saveHistogram( const QString& histogramId, const QString& filename, int width, int height ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString widthError;
            QString heightError;
            //Only set the width and height if the user intends to use
            //the non-default save sizes.
            if ( width > 0 || height > 0 ){
                Carta::Data::PreferencesSave* prefSave = Carta::Data::Util::findSingletonObject<Carta::Data::PreferencesSave>();
                if ( width > 0 ){
                    widthError = prefSave->setWidth( width );
                }
                if ( height > 0 ){
                    heightError = prefSave->setHeight( height );
                }
            }
            if ( widthError.isEmpty() && heightError.isEmpty()){
                QString result = histogram->saveHistogram( filename );
                if ( !result.isEmpty() ){
                    resultList = _logErrorMessage( ERROR, result );
                }
            }
            else {
                if ( !widthError.isEmpty() ){
                    resultList = _logErrorMessage( ERROR, widthError );
                }
                if ( !heightError.isEmpty() ){
                    resultList = _logErrorMessage( ERROR, heightError );
                }
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified histogram view could not be found: " + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::setGridAxesColor( const QString& controlId, int red, int green, int blue ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getGridControls()->setAxesColor( red, green, blue );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridAxesThickness( const QString& controlId, int thickness ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setAxesThickness( thickness );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridAxesTransparency( const QString& controlId, int transparency ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setAxesTransparency( transparency );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridApplyAll( const QString& controlId, bool applyAll ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            controller->getGridControls()->setApplyAll( applyAll );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::setGridCoordinateSystem( const QString& controlId, const QString& coordSystem ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setCoordinateSystem( coordSystem );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridFontFamily( const QString& controlId, const QString& fontFamily ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setFontFamily( fontFamily );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridFontSize( const QString& controlId, int fontSize ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setFontSize( fontSize );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridColor( const QString& controlId, int redAmount, int greenAmount, int blueAmount ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getGridControls()->setGridColor( redAmount, greenAmount, blueAmount );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridSpacing( const QString& controlId, double spacing ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setGridSpacing( spacing );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridThickness( const QString& controlId, int thickness ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setGridThickness( thickness );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;

}

QStringList ScriptFacade::setGridTransparency( const QString& controlId, int transparency ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setGridTransparency( transparency );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridLabelColor( const QString& controlId, int redAmount, int greenAmount, int blueAmount ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getGridControls()->setLabelColor( redAmount, greenAmount, blueAmount );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridAxis( const QString& controlId, bool showAxis ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setShowAxis( showAxis );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridCoordinateSystem( const QString& controlId, bool showCoordinateSystem ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setShowCoordinateSystem( showCoordinateSystem );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridLines( const QString& controlId, bool showGridLines ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setShowGridLines( showGridLines );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridInternalLabels( const QString& controlId, bool showInternalLabels ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setShowInternalLabels( showInternalLabels );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridStatistics( const QString& controlId, bool showStatistics ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setShowStatistics( showStatistics );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridTicks( const QString& controlId, bool showTicks ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setShowTicks( showTicks );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setGridTickColor( const QString& controlId, int redAmount, int greenAmount, int blueAmount ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = controller->getGridControls()->setTickColor( redAmount, greenAmount, blueAmount );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridTickThickness( const QString& controlId, int tickThickness ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setTickThickness( tickThickness );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridTickTransparency( const QString& controlId, int transparency ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setTickTransparency( transparency );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setGridTheme( const QString& controlId, const QString& theme ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->getGridControls()->setTheme( theme );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "The specified image view could not be found: " + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

Carta::State::CartaObject* ScriptFacade::_getObject( const QString& id ) {
    ObjectManager* objMan = ObjectManager::objectManager();
    QString objId = objMan->parseId( id );
    Carta::State::CartaObject* obj = objMan->getObject( objId );
    return obj;
}

QStringList ScriptFacade::_logErrorMessage( const QString& key, const QString& value ) {
    QStringList result( key );
    result.append( value );
    return result;
}
