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
#include "Data/Image/Grid/GridControls.h"
#include "Data/Image/Contour/ContourControls.h"

#include <QDebug>
#include <cmath>

using Carta::State::ObjectManager;

const QString ScriptFacade::TOGGLE = "toggle";
const QString ScriptFacade::ERROR = "error";
const QString ScriptFacade::UNKNOWN_ERROR = "An unknown error has occurred";
const QString ScriptFacade::NO_IMAGE = "No image loaded.";
const QString ScriptFacade::IMAGE_VIEW_NOT_FOUND = "The specified image view could not be found: ";
const QString ScriptFacade::COLORMAP_VIEW_NOT_FOUND = "The specified colormap view could not be found: ";
const QString ScriptFacade::HISTOGRAM_NOT_FOUND = "The specified histogram view could not be found: ";
const QString ScriptFacade::ANIMATOR_NOT_FOUND = "The specified animator could not be found: ";

ScriptFacade * ScriptFacade::getInstance (){
    static ScriptFacade * sc = new ScriptFacade ();
    return sc;
}


ScriptFacade::ScriptFacade(){
    m_viewManager = Carta::Data::Util::findSingletonObject<Carta::Data::ViewManager>();
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

QStringList ScriptFacade::getImageViews() {
    QStringList imageViewList;
    int numControllers = m_viewManager->getControllerCount();
    for (int i = 0; i < numControllers; i++) {
        QString imageView = getImageViewId( i );
        Carta::State::CartaObject* obj = _getObject( imageView );
        if ( obj != nullptr ){
            Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
            if ( controller != nullptr ){
                connect( controller, & Carta::Data::Controller::saveImageResult, this, & ScriptFacade::saveImageResultCB, Qt::UniqueConnection );
            }
        }
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

QStringList ScriptFacade::loadFile( const QString& objectId, const QString& fileName){
    QStringList resultList;
    bool loadSuccess = false;
    QString result = m_viewManager->loadFile( objectId, fileName, &loadSuccess );
    if ( !loadSuccess ) {
        resultList = _logErrorMessage( ERROR, "Could not load file " + fileName + ": "+result);
    }
    else {
        resultList.append( result);
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
        resultList = _logErrorMessage( ERROR, COLORMAP_VIEW_NOT_FOUND + colormapId );
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
                QString result = colormap->setReverse( reverse );
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
        resultList = _logErrorMessage( ERROR, COLORMAP_VIEW_NOT_FOUND + colormapId );
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
                QString result = colormap->setInvert( invert );
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
        resultList = _logErrorMessage( ERROR, COLORMAP_VIEW_NOT_FOUND + colormapId );
    }
    return resultList;
}

QStringList ScriptFacade::setNanDefault( const QString& colormapId, const QString& nanDefaultStr ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( colormapId );
    if ( obj != nullptr ){
        Carta::Data::Colormap* colormap = dynamic_cast<Carta::Data::Colormap*>(obj);
        if ( colormap != nullptr ){
            bool nanDefault = false;
            bool validBool = true;
            nanDefault = Carta::Data::Util::toBool( nanDefaultStr, &validBool );
            if ( validBool ){
                QString result = colormap->setNanDefault(nanDefault);
                resultList = QStringList( result );
            }
             else {
                resultList = _logErrorMessage( ERROR, "An unrecognized parameter was passed to set nan default: " + nanDefaultStr );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, COLORMAP_VIEW_NOT_FOUND + colormapId );
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
        resultList = _logErrorMessage( ERROR, COLORMAP_VIEW_NOT_FOUND + colormapId );
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
        resultList = _logErrorMessage( ERROR, COLORMAP_VIEW_NOT_FOUND + colormapId );
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
        resultList = _logErrorMessage( ERROR, COLORMAP_VIEW_NOT_FOUND + colormapId );
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
        resultList = _logErrorMessage( ERROR, ANIMATOR_NOT_FOUND + animatorId );
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
        resultList = _logErrorMessage( ERROR, ANIMATOR_NOT_FOUND + animatorId );
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
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( animatorId );
    if ( obj != nullptr ){
        Carta::Data::Animator* animator = dynamic_cast<Carta::Data::Animator*>(obj);
        if ( animator != nullptr){
            Carta::Data::AnimatorType* animType = animator->getAnimator( "Channel");
            if ( animType != nullptr ){
                QString result = animType->setFrame( index );
                resultList = QStringList( result );
            }
            else {
                resultList = QStringList( ERROR );
                resultList.append( "Could not get channel animator" );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, ANIMATOR_NOT_FOUND + animatorId );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
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
        resultList = _logErrorMessage( ERROR, ANIMATOR_NOT_FOUND + animatorId );
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
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    return resultList;
}


QStringList ScriptFacade::saveImage( const QString& controlId, const QString& filename,
        int width, int height,
        const QString& aspectModeStr ){
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
                errorList = QStringList( controller->saveImage( filename) );
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
        emit saveImageResult( false );
    }
    if ( errorList.length() == 0 ) {
        errorList = QStringList("");
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
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getCenterPixel( const QString& controlId ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QPointF center = controller->getCenterPixel();
            if ( std::isnan( center.x() ) && std::isnan( center.y() ) ) {
                resultList = _logErrorMessage( ERROR, "The center pixel could not be obtained." );
            }
            else {
                resultList.append( QString::number( center.x() ) );
                resultList.append( QString::number( center.y() ) );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getImageDimensions( const QString& controlId ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            std::vector<int> dimensions = controller->getImageDimensions();
            if ( dimensions.size() == 0 ){
                resultList = _logErrorMessage( ERROR, "Could not obtain image dimensions." );
            }
            else {
                for ( auto &i: dimensions ) {
                    resultList.append( QString::number( i ) );
                }
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getChannelCount( const QString& controlId ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            resultList = QStringList( QString::number( controller->getFrameUpperBound( Carta::Lib::AxisInfo::KnownType::SPECTRAL ) ) );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getOutputSize( const QString& controlId ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QSize size = controller->getOutputSize( );
            if ( size.isEmpty() ) {
                resultList = _logErrorMessage( ERROR, "Could not obtain output size." );
            }
            else {
                resultList.append( QString::number( size.width() ));
                resultList.append( QString::number( size.height() ));
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getPixelCoordinates( const QString& controlId, double ra, double dec ){
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            bool valid = false;
            QPointF coords=controller->getPixelCoordinates( ra, dec, &valid );
            if ( valid ){
                resultList.append( QString::number(coords.x()));
                resultList.append( QString::number(coords.y()));
            }
            else {
                resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::getImageNames( const QString& controlId ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QStringList layerIds = controller->getLayerIds();
            int count = layerIds.size();
            if ( count == 0 ){
                resultList = QStringList("");
            }
            else {
                resultList = layerIds;
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::showImage( const QString& controlId, const QString& imageName ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->setImageVisibility( imageName, true );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::hideImage( const QString& controlId, const QString& imageName ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->setImageVisibility( imageName, false );
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}


QStringList ScriptFacade::setPanZoomAll(const QString& controlId, const QString& setPanZoomAllFlagStr) {
    QStringList resultList;

    bool validBool = false;
    bool setPanZoomAllFlag= Carta::Data::Util::toBool( setPanZoomAllFlagStr, &validBool );
    if ( validBool ) {
        Carta::State::CartaObject* obj = _getObject( controlId );
        if ( obj != nullptr ){
            Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
            if ( controller != nullptr ){
                controller->setPanZoomAll( setPanZoomAllFlag );
                resultList = QStringList( "" );
            }
            else {
                resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
        }
        if ( resultList.length() == 0 ) {
            resultList = QStringList("");
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "Set Pan Zoom All Flag must be true/false " + setPanZoomAllFlagStr);
    }
    return resultList;
}


QStringList ScriptFacade::setMaskAlpha(const QString& controlId, const QString& imageName, const QString& alphaAmountStr) {
    QStringList resultList;

    bool validInt = false;
    int alphaAmount = alphaAmountStr.toInt( &validInt );
    if ( validInt && alphaAmount >= 0 && alphaAmount <= 255 ) {
        Carta::State::CartaObject* obj = _getObject( controlId );
        if ( obj != nullptr ){
            Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
            if ( controller != nullptr ){
                resultList = QStringList(controller->setMaskAlpha( imageName, alphaAmount ));
            }
            else {
                resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
        }
        if ( resultList.length() == 0 ) {
            resultList = QStringList("");
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "alphaAmount  must be valid int between 0 and 255 " + alphaAmountStr );
    }
    return resultList;
}

QStringList ScriptFacade::setMaskColor(const QString& controlId, const QString& imageName,
                                       const QString& redAmountStr, const QString& greenAmountStr,
                                       const QString& blueAmountStr) {
    QStringList resultList;

    bool validIntRed = false;
    int redAmount = redAmountStr.toInt( &validIntRed );

    bool validIntGreen = false;
    int greenAmount = greenAmountStr.toInt(&validIntRed );

    bool validIntBlue = false;
    int blueAmount = blueAmountStr.toInt( &validIntRed );

    if ( validIntRed &&  validIntGreen && validIntBlue &&
         redAmount >= 0 && redAmount <= 255 &&
         greenAmount >= 0 && greenAmount <= 255 &&
         blueAmount >= 0 && blueAmount <= 255 ) {
        Carta::State::CartaObject* obj = _getObject( controlId );
        if ( obj != nullptr ){
            Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
            if ( controller != nullptr ){
                resultList = controller->setMaskColor( imageName, redAmount, greenAmount, blueAmount );
            }
            else {
                resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
        }
        if ( resultList.length() == 0 ) {
            resultList = QStringList("");
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "redAmount/greenAmount/blueAmount  must be valid int between 0 and 255 " + redAmountStr
                 + "/" + greenAmountStr + "/" + blueAmountStr);
    }
    return resultList;
}

QStringList ScriptFacade::setStackSelectAuto( const QString& controlId, const QString& stackSelectFlagStr) {
    QStringList resultList;

    bool validBool = false;
    bool stackSelectFlag = Carta::Data::Util::toBool( stackSelectFlagStr, &validBool );
    if ( validBool ) {
        Carta::State::CartaObject* obj = _getObject( controlId );
        if ( obj != nullptr ){
            Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
            if ( controller != nullptr ){
                controller->setStackSelectAuto( stackSelectFlag );
                resultList = QStringList( "" );
            }
            else {
                resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
        }
        if ( resultList.length() == 0 ) {
            resultList = QStringList("");
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "Set Stack Select Auto parameter must be true/false " + stackSelectFlagStr);
    }
    return resultList;
}

QStringList ScriptFacade::setCompositionMode( const QString& controlId, const QString& imageName) {
    QStringList resultList;

    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            QString result = controller->setCompositionMode(imageName, "mode");
            resultList = QStringList( result );
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
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
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
            resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::getIntensity( const QString& controlId, int frameLow,
        int frameHigh, double percentile ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            std::vector<double> percentiles(1);
            percentiles[0] = percentile;
            std::vector<std::pair<int,double> > intensities = controller->getIntensity(
                    frameLow, frameHigh, percentiles );
            if ( intensities.size() == 1 && intensities[0].first >= 0 ){
                resultList = QStringList( QString::number( intensities[0].second ) );
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
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
            resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
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
            resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, "Set colored parameter must be true/false or 'toggle': " + coloredStr );
    }
    return resultList;
}

QStringList ScriptFacade::saveHistogram( const QString& histogramId, const QString& filename, int width, int height, const QString& aspectModeStr ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( histogramId );
    if ( obj != nullptr ){
        Carta::Data::Histogram* histogram = dynamic_cast<Carta::Data::Histogram*>(obj);
        if ( histogram != nullptr ){
            QString widthError;
            QString heightError;
            QString aspectModeError;
            //Only set the width and height if the user intends to use
            //the non-default save sizes.
            if ( width > 0 || height > 0 ){
                Carta::Data::PreferencesSave* prefSave = Carta::Data::Util::findSingletonObject<Carta::Data::PreferencesSave>();
                aspectModeError = prefSave->setAspectRatioMode( aspectModeStr );
                if ( width > 0 ){
                    widthError = prefSave->setWidth( width );
                }
                if ( height > 0 ){
                    heightError = prefSave->setHeight( height );
                }
            }
            if ( widthError.isEmpty() && heightError.isEmpty() && aspectModeError.isEmpty() ){
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
                if ( !aspectModeError.isEmpty() ){
                    resultList = _logErrorMessage( ERROR, aspectModeError );
                }
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, HISTOGRAM_NOT_FOUND + histogramId );
    }
    return resultList;
}

QStringList ScriptFacade::setGridAxesColor( const QString& controlId, int red, int green, int blue ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                resultList = controller->getGridControls()->setAxesColor( red, green, blue );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setAxesThickness( thickness );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setAxesTransparency( transparency );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
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
            if ( controller->getStackedImageCount() > 0 ) {
                controller->getGridControls()->setApplyAll( applyAll );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId );
    }
    return resultList;
}

QStringList ScriptFacade::setGridCoordinateSystem( const QString& controlId, const QString& coordSystem ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setCoordinateSystem( coordSystem );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setFontFamily( fontFamily );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setFontSize( fontSize );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
            if ( controller->getStackedImageCount() > 0 ) {
                resultList = controller->getGridControls()->setGridColor( redAmount, greenAmount, blueAmount );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setGridSpacing( spacing );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setGridThickness( thickness );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setGridTransparency( transparency );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
            if ( controller->getStackedImageCount() > 0 ) {
                resultList = controller->getGridControls()->setLabelColor( redAmount, greenAmount, blueAmount );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setShowAxis( showAxis );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridCoordinateSystem( const QString& controlId, bool showCoordinateSystem ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setShowCoordinateSystem( showCoordinateSystem );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridLines( const QString& controlId, bool showGridLines ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setShowGridLines( showGridLines );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridInternalLabels( const QString& controlId, bool showInternalLabels ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setShowInternalLabels( showInternalLabels );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridStatistics( const QString& controlId, bool showStatistics ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setShowStatistics( showStatistics );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setShowGridTicks( const QString& controlId, bool showTicks ) {
    QStringList resultList("");
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setShowTicks( showTicks );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    return resultList;
}

QStringList ScriptFacade::setGridTickColor( const QString& controlId, int redAmount, int greenAmount, int blueAmount ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                resultList = controller->getGridControls()->setTickColor( redAmount, greenAmount, blueAmount );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setTickThickness( tickThickness );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setTickTransparency( transparency );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getGridControls()->setTheme( theme );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::deleteContourSet( const QString& controlId, const QString& name ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getContourControls()->deleteContourSet( name );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::generateContourSet( const QString& controlId, const QString& name ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getContourControls()->generateContourSet( name );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::selectContourSet( const QString& controlId, const QString& name ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                controller->getContourControls()->selectContourSet( name );
                resultList = QStringList("");
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setContourAlpha( const QString& controlId, const QString& contourName, std::vector<double>& levels, int transparency ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getContourControls()->setAlpha( contourName, levels, transparency );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setContourColor( const QString& controlId, const QString& contourName, std::vector<double>& levels, int red, int green, int blue ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                resultList = controller->getContourControls()->setColor( contourName, levels, red, green, blue );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setContourDashedNegative( const QString& controlId, bool useDash ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                controller->getContourControls()->setDashedNegative( useDash );
                resultList = QStringList("");
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setContourGenerateMethod( const QString& controlId, const QString& method ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getContourControls()->setGenerateMethod( method );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setContourSpacing( const QString& controlId, const QString& method ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getContourControls()->setSpacing( method );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setContourLevelCount( const QString& controlId, int count ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getContourControls()->setLevelCount( count );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setContourLevelMax( const QString& controlId, double value ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getContourControls()->setLevelMax( value );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setContourLevelMin( const QString& controlId, double value ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getContourControls()->setLevelMin( value );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
    }
    if ( resultList.length() == 0 ) {
        resultList = QStringList("");
    }
    return resultList;
}

QStringList ScriptFacade::setContourLevels( const QString& controlId, const QString& contourName, std::vector<double>& levels ) {
    QStringList resultList;
    Carta::State::CartaObject* obj = _getObject( controlId );
    if ( obj != nullptr ){
        Carta::Data::Controller* controller = dynamic_cast<Carta::Data::Controller*>(obj);
        if ( controller != nullptr ){
            if ( controller->getStackedImageCount() > 0 ) {
                QString result = controller->getContourControls()->setLevels( contourName, levels );
                resultList = QStringList( result );
            }
            else {
                resultList = _logErrorMessage( ERROR, NO_IMAGE );
            }
        }
        else {
            resultList = _logErrorMessage( ERROR, UNKNOWN_ERROR );
        }
    }
    else {
        resultList = _logErrorMessage( ERROR, IMAGE_VIEW_NOT_FOUND + controlId);
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
