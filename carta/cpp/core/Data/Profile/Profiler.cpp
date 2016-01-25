#include "Profiler.h"
#include "Data/Clips.h"
#include "Data/Settings.h"
#include "Data/LinkableImpl.h"
#include "Data/Image/Controller.h"
#include "Data/Error/ErrorManager.h"
#include "Data/Util.h"
#include "Data/Plotter/Plot2DManager.h"
#include "Data/Plotter/PlotStyles.h"
#include "Plot2D/Plot2DGenerator.h"

#include "CartaLib/Hooks/Plot2DResult.h"
#include "State/UtilState.h"
#include <QDebug>

namespace Carta {

namespace Data {

const QString Profiler::CLASS_NAME = "Profiler";


class Profiler::Factory : public Carta::State::CartaObjectFactory {
public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
    {
        return new Profiler (path, id);
    }
};

bool Profiler::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Profiler::Factory());

using Carta::State::UtilState;
using Carta::State::StateInterface;
using Carta::Plot2D::Plot2DGenerator;

Profiler::Profiler( const QString& path, const QString& id):
            CartaObject( CLASS_NAME, path, id ),
            m_linkImpl( new LinkableImpl( path )),
            m_preferences( nullptr),
            m_plotManager( new Plot2DManager( path, id ) ){

    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    Settings* prefObj = objMan->createObject<Settings>();
    m_preferences.reset( prefObj );

    _initializeDefaultState();
    _initializeCallbacks();

    m_controllerLinked = false;
    m_plotManager->setPlotGenerator( new Plot2DGenerator( Plot2DGenerator::PlotType::PROFILE) );
    m_plotManager->setTitleAxisY( "Intensity" );
    m_plotManager->setTitleAxisX( "Radio Velocity" );
}



QString Profiler::addLink( CartaObject*  target){
    Controller* controller = dynamic_cast<Controller*>(target);
    bool linkAdded = false;
    QString result;
    if ( controller != nullptr ){
        if ( !m_controllerLinked ){
            linkAdded = m_linkImpl->addLink( controller );
            if ( linkAdded ){
                connect(controller, SIGNAL(dataChanged(Controller*)), this , SLOT(_createProfiler(Controller*)));
                m_controllerLinked = true;
                _createProfiler( controller );
            }
        }
        else {
            CartaObject* obj = m_linkImpl->searchLinks( target->getPath());
            if ( obj != nullptr ){
                linkAdded = true;
            }
            else {
                result = "Profiler only supports linking to a single image source.";
            }
        }
    }
    else {
        result = "Profiler only supports linking to images";
    }
    return result;
}


void Profiler::_createProfiler( Controller* controller){
    std::shared_ptr<Carta::Lib::PixelPipeline::CustomizablePixelPipeline> pipeline = controller->getPipeline();
    m_plotManager->setPipeline( pipeline );

    //TODO: Update the data state.
    _generateProfile( true, controller );
}


std::vector<std::shared_ptr<Carta::Lib::Image::ImageInterface>> Profiler::_generateData(Controller* controller){
    std::vector<std::shared_ptr<Carta::Lib::Image::ImageInterface>> result;
    if ( controller != nullptr ){
        result = controller->getDataSources();
    }
    return result;
}


void Profiler::_generateProfile( bool newDataNeeded, Controller* controller ){
    Controller* activeController = controller;
    if ( activeController == nullptr ){
        activeController = _getControllerSelected();
    }
    if ( newDataNeeded ){
        _loadProfile( activeController );
    }
}


Controller* Profiler::_getControllerSelected() const {
    //We are only supporting one linked controller.
    Controller* controller = nullptr;
    int linkCount = m_linkImpl->getLinkCount();
    for ( int i = 0; i < linkCount; i++ ){
        CartaObject* obj = m_linkImpl->getLink(i );
        Controller* control = dynamic_cast<Controller*>( obj);
        if ( control != nullptr){
            controller = control;
            break;
        }
    }
    return controller;
}


QList<QString> Profiler::getLinks() const {
    return m_linkImpl->getLinkIds();
}


bool Profiler::getLogCount(){
    return false;
}


QString Profiler::_getPreferencesId() const {
    return m_preferences->getPath();
}


void Profiler::_initializeDefaultState(){
}


void Profiler::_initializeCallbacks(){
    addCommandCallback( "registerPreferences", [=] (const QString & /*cmd*/,
                    const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                QString result = _getPreferencesId();
                return result;
            });
}


void Profiler::_initializeStatics(){
}


bool Profiler::isLinked( const QString& linkId ) const {
    bool linked = false;
    CartaObject* obj = m_linkImpl->searchLinks( linkId );
    if ( obj != nullptr ){
        linked = true;
    }
    return linked;
}


void Profiler::_loadProfile( Controller* controller ){
    if( ! controller) {
        return;
    }
    std::vector<std::shared_ptr<Carta::Lib::Image::ImageInterface> > dataSources = controller-> getDataSources();
    if ( dataSources.size() > 0 ) {
    	std::vector < int > pos( dataSources[0]-> dims().size(), 0 );
        int axis = Util::getAxisIndex( dataSources[0], Carta::Lib::AxisInfo::KnownType::SPECTRAL );
        Profiles::PrincipalAxisProfilePath path( axis, pos );
        Carta::Lib::NdArray::RawViewInterface * rawView = dataSources[0]-> getDataSlice( SliceND() );
        Profiles::ProfileExtractor * extractor = new Profiles::ProfileExtractor( rawView );
        auto profilecb = [ = ] () {
            auto data = extractor->getDataD();
            int dataCount = data.size();
            std::vector<std::pair<double,double> > plotData( dataCount );
            for( int i = 0 ; i < dataCount; i ++ ){
                plotData[i].first = i;
                plotData[i].second = data[i];
            }
            Carta::Lib::Hooks::Plot2DResult plotResult( "Profile", "km/s", "Jy/beam", plotData );

            m_plotManager->setData( plotResult );
            m_plotManager->setLogScale( false );
            m_plotManager->setStyle( PlotStyles::PLOT_STYLE_OUTLINE );
            m_plotManager->setColored( false );
            m_plotManager->updatePlot();
        };
        connect( extractor, & Profiles::ProfileExtractor::progress, profilecb );
        extractor-> start( path );
    }
}



QString Profiler::removeLink( CartaObject* cartaObject){
    bool removed = false;
    QString result;
    Controller* controller = dynamic_cast<Controller*>( cartaObject );
    if ( controller != nullptr ){
        removed = m_linkImpl->removeLink( controller );
        if ( removed ){
            controller->disconnect(this);
            m_controllerLinked = false;
            //_resetDefaultStateData();
        }
    }
    else {
       result = "Profiler was unable to remove link only image links are supported";
    }
    return result;
}


QString Profiler::setGraphStyle( const QString& /*styleStr*/ ){
    QString result;
    /*QString oldStyle = m_state.getValue<QString>(GRAPH_STYLE);
    QString actualStyle = _getActualGraphStyle( styleStr );
    if ( actualStyle != "" ){
        if ( actualStyle != oldStyle ){
            m_state.setValue<QString>(GRAPH_STYLE, actualStyle );
            m_state.flushState();
            _generateProfile( false );
        }
    }
    else {
        result = "Unrecognized Profiler graph style: "+ styleStr;
    }*/
    return result;
}


QString Profiler::setLogCount( bool /*logCount*/ ){
    QString result;
    /*bool oldLogCount = m_state.getValue<bool>(GRAPH_LOG_COUNT);
    if ( logCount != oldLogCount ){
        m_state.setValue<bool>(GRAPH_LOG_COUNT, logCount );
        m_state.flushState();
        _generateProfiler( false );
    }*/
    return result;
}


Profiler::~Profiler(){
}
}
}
