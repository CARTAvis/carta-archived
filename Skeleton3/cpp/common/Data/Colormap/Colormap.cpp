#include "Data/Colormap/Colormap.h"
#include "Data/Colormap/Colormaps.h"
#include "Data/Controller.h"
#include "Data/Colormap/TransformsData.h"
#include "Data/IColoredView.h"
#include "Data/Util.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"
#include "ImageView.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"

#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

const QString Colormap::CLASS_NAME = "Colormap";
const QString Colormap::COLOR_MAP_NAME = "colorMapName";
const QString Colormap::COLORED_OBJECT = "coloredObject";
const QString Colormap::REVERSE = "reverse";
const QString Colormap::INVERT = "invert";
const QString Colormap::CACHE = "cacheMap";
const QString Colormap::INTERPOLATED = "interpolatedCaching";
const QString Colormap::CACHE_SIZE = "cacheSize";
const QString Colormap::COLOR_MIX = "colorMix";
const QString Colormap::RED_PERCENT = "redPercent";
const QString Colormap::GREEN_PERCENT = "greenPercent";
const QString Colormap::BLUE_PERCENT = "bluePercent";
const QString Colormap::COLOR_MIX_RED = COLOR_MIX + "/" + RED_PERCENT;
const QString Colormap::COLOR_MIX_GREEN = COLOR_MIX + "/" + GREEN_PERCENT;
const QString Colormap::COLOR_MIX_BLUE = COLOR_MIX + "/" + BLUE_PERCENT;
const QString Colormap::SCALE_1 = "scale1";
const QString Colormap::SCALE_2 = "scale2";
const QString Colormap::GAMMA = "gamma";
const QString Colormap::TRANSFORM_IMAGE = "imageTransform";
const QString Colormap::TRANSFORM_DATA = "dataTransform";

Colormaps* Colormap::m_colors = nullptr;
TransformsData* Colormap::m_dataTransforms = nullptr;

class Colormap::Factory : public Carta::State::CartaObjectFactory {

    public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new Colormap (path, id);
        }
    };

bool Colormap::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Colormap::Factory());

Colormap::Colormap( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ),
    m_linkImpl( new LinkableImpl( path ) ),
    m_stateMouse(Carta::State::UtilState::getLookup(path,ImageView::VIEW)){
    m_significantDigits = 6;
    _initializeDefaultState();
    _initializeCallbacks();
    _initializeStatics();
}

QString Colormap::addLink( CartaObject*  cartaObject ){
    Controller* target = dynamic_cast<Controller*>(cartaObject);
    bool objAdded = false;
    QString result;
    if ( target != nullptr ){
        objAdded = m_linkImpl->addLink( target );
        if ( objAdded ){
            _setColorProperties( target );
            connect( target, SIGNAL(dataChanged(Controller*)), this, SLOT(_setColorProperties(Controller*)));
        }
    }
    else {
        result = "Color map only supports linking to images.";
    }
    return result;
}

QString Colormap::getStateString( const QString& /*sessionId*/, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        result = m_state.toString();
    }
    else if ( type == SNAPSHOT_LAYOUT ){
        result = m_linkImpl->getStateString(getIndex(), getType( type ));
    }
    return result;
}

void Colormap::_initializeDefaultState(){
    m_state.insertValue<QString>( COLOR_MAP_NAME, "Gray" );
    m_state.insertValue<bool>(REVERSE, false);
    m_state.insertValue<bool>(INVERT, false );
    m_state.insertValue<bool>(CACHE, true);
    m_state.insertValue<bool>(INTERPOLATED, true );
    m_state.insertValue<int>(CACHE_SIZE, 1000 );

    m_state.insertValue<double>(GAMMA, 1.0 );
    m_state.insertValue<double>(SCALE_1, 0.0 );
    m_state.insertValue<double>(SCALE_2, 0.0 );

    m_state.insertObject( COLOR_MIX );
    m_state.insertValue<double>( COLOR_MIX_RED, 1 );
    m_state.insertValue<double>(COLOR_MIX_GREEN, 1 );
    m_state.insertValue<double>(COLOR_MIX_BLUE, 1 );

    m_state.insertValue<QString>(TRANSFORM_IMAGE, "Gamma");
    m_state.insertValue<QString>(TRANSFORM_DATA, "None");
    m_state.insertValue<bool>(Util::STATE_FLUSH, false );
    m_state.flushState();

    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.flushState();

}

void Colormap::_initializeCallbacks(){

    addCommandCallback( "cacheColormap", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                QString result = _commandCacheColorMap( params );
                return result;
            });

    addCommandCallback( "setCacheSize", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
                    QString result = _commandCacheSize( params );
                    return result;
                });

    addCommandCallback( "setDataTransform", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {TRANSFORM_DATA};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString dataTransformStr = dataValues[*keys.begin()];
        QString result = setDataTransform( dataTransformStr );
        return result;
   });

    addCommandCallback( "interpolatedColormap", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
                    QString result = _commandInterpolatedColorMap( params );
                    return result;
                });

    addCommandCallback( "setColormap", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            QString result = _commandSetColorMap( params );
            return result;
        });

    addCommandCallback( "invertColormap", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            QString result = _commandInvertColorMap( params );
            return result;
        });

    addCommandCallback( "reverseColormap", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            QString result = _commandReverseColorMap( params );
            return result;
        });

    addCommandCallback( "setColorMix", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                QString result = _commandSetColorMix( params );
                return result;
            });

    addCommandCallback( "setScales", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {SCALE_1, SCALE_2};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        bool valid = false;
        double scale1 = dataValues[SCALE_1].toDouble( & valid );
        if ( !valid ){
            result = "Invalid color scale: "+params;
        }
        else {
            double scale2 = dataValues[SCALE_2].toDouble( &valid );
            if ( !valid ){
                result = "Invalid color scale: "+params;
            }
            else {
                double oldScale1 = m_state.getValue<double>(SCALE_1);
                double oldScale2 = m_state.getValue<double>(SCALE_2);
                bool changedState = false;
                if ( scale1 != oldScale1 ){
                    m_state.setValue<double>(SCALE_1, scale1 );
                    changedState = true;
                }
                if ( scale2 != oldScale2 ){
                    m_state.setValue<double>(SCALE_2, scale2 );
                    changedState = true;
                }
                if ( changedState ){
                    m_state.flushState();
                    //Calculate the new gamma
                    double maxndig = 10;
                    double ndig = (scale2 + 1) / 2 * maxndig;
                    double expo = std::pow( 2.0, ndig);
                    double xx = std::pow(scale1 * 2, 3) / 8.0;
                    double gamma = fabs(xx) * expo + 1;
                    if( scale1 < 0){
                        gamma = 1 / gamma;
                    }
                    result = setGamma( gamma );
                }
            }
        }
        return result;
    });
}

void Colormap::_initializeStatics(){
    //Load the available color maps.
    if ( m_colors == nullptr ){
        CartaObject* obj = Util::findSingletonObject( Colormaps::CLASS_NAME );
        m_colors = dynamic_cast<Colormaps*>( obj );
    }

    //Data transforms
    if ( m_dataTransforms == nullptr ){
        CartaObject* obj = Util::findSingletonObject( TransformsData::CLASS_NAME );
        m_dataTransforms =dynamic_cast<TransformsData*>( obj );
    }
}

void Colormap::clear(){
    m_linkImpl->clear();
}

QString Colormap::_commandCacheColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {CACHE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString cacheStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool cache = Util::toBool(cacheStr, &validBool);
    bool oldCache = m_state.getValue<bool>(CACHE);
    if ( validBool ){
        if ( cache != oldCache){
            m_state.setValue<bool>(CACHE, cache );
            m_state.flushState();
            int linkCount = m_linkImpl->getLinkCount();
            for( int i = 0; i < linkCount; i++ ){
                Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
                controller -> setPixelCaching ( cache );
            }
        }
    }
    else {
        result = "Invalid color map cache parameters: "+ params;
    }
    Util::commandPostProcess( result );
    return result;
}

QString Colormap::_commandCacheSize( const QString& params ){
    QString result;
    std::set<QString> keys = {CACHE_SIZE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString cacheSizeStr = dataValues[*keys.begin()];
    bool validInt = false;
    int cacheSize = cacheSizeStr.toInt(&validInt);
    int currentCacheSize = m_state.getValue<int>(CACHE_SIZE);
    if ( validInt ){
        if ( cacheSize != currentCacheSize ){
            m_state.setValue<int>(CACHE_SIZE, cacheSize );
            m_state.flushState();
            int linkCount = m_linkImpl->getLinkCount();
            for( int i = 0; i < linkCount; i++ ){
                Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
                controller -> setCacheSize ( cacheSize );
            }
        }
    }
    else {
        result = "Invalid color map cache size parameters: "+ params;
    }
    Util::commandPostProcess( result );
    return result;
}

QString Colormap::_commandInterpolatedColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {INTERPOLATED};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString interpolateStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool interpolated = Util::toBool(interpolateStr, &validBool);
    bool currentInterpolated = m_state.getValue<bool>(INTERPOLATED);
    if ( validBool ){
        if ( interpolated != currentInterpolated ){
            m_state.setValue<bool>(INTERPOLATED, interpolated );
            m_state.flushState();
            int linkCount = m_linkImpl->getLinkCount();
            for( int i = 0; i < linkCount; i++ ){
                Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
                controller -> setCacheInterpolation ( interpolated );
            }
        }
    }
    else {
        result = "Invalid interpolated caching color map parameters: "+ params;
    }
    Util::commandPostProcess( result );
    return result;
}

QString Colormap::_commandInvertColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {INVERT};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString invertStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool invert = Util::toBool(invertStr, &validBool);
    bool oldInvert = m_state.getValue<bool>(INVERT );
    if ( validBool ){
        if ( invert != oldInvert ){
            m_state.setValue<bool>(INVERT, invert );
            m_state.flushState();
            int linkCount = m_linkImpl->getLinkCount();
            for( int i = 0; i < linkCount; i++ ){
                Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
                controller -> setColorInverted ( invert );
            }
            emit colorMapChanged( this );
        }
    }
    else {
        result = "Invalid color map invert parameters: "+ params;
    }
    Util::commandPostProcess( result );
    return result;
}

QString Colormap::_commandSetColorMix( const QString& params ){
    QString result;
    std::set<QString> keys = {RED_PERCENT, GREEN_PERCENT, BLUE_PERCENT};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );

    bool validRed = false;
    bool redChanged = _processColorStr( COLOR_MIX_RED, dataValues[RED_PERCENT], &validRed );

    bool validBlue = false;
    bool blueChanged = _processColorStr( COLOR_MIX_BLUE, dataValues[BLUE_PERCENT], &validBlue );

    bool validGreen = false;
    bool greenChanged = _processColorStr( COLOR_MIX_GREEN, dataValues[GREEN_PERCENT], &validGreen);
    QString currValues;
    if ( redChanged || blueChanged || greenChanged ){
        m_state.flushState();
        double newRed = m_state.getValue<double>(COLOR_MIX_RED );
        double newGreen = m_state.getValue<double>(COLOR_MIX_GREEN );
        double newBlue = m_state.getValue<double>(COLOR_MIX_BLUE );
        int linkCount = m_linkImpl->getLinkCount();
        for( int i = 0; i < linkCount; i++ ){
            Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
            controller->setColorAmounts( newRed, newGreen, newBlue );
        }
        emit colorMapChanged( this );
    }
    else if ( !validRed || !validGreen || !validBlue ){
        result = "Invalid Colormap color percent: "+ params;
    }
    Util::commandPostProcess( result );
    return result;
}


QString Colormap::_commandReverseColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {REVERSE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString reverseStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool reverse = Util::toBool(reverseStr, &validBool);
    bool oldReverse = m_state.getValue<bool>(REVERSE);
    if ( validBool ){
        if ( reverse != oldReverse ){
            m_state.setValue<bool>(REVERSE, reverse );
            m_state.flushState();
            int linkCount = m_linkImpl->getLinkCount();
            for( int i = 0; i < linkCount; i++ ){
                Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
                controller -> setColorReversed( reverse );
            }
            emit colorMapChanged( this );
        }
    }
    else {
        result = "Invalid color map reverse parameters: "+ params;
    }
    Util::commandPostProcess( result );
    return result;
}

QString Colormap::_commandSetColorMap( const QString& params ){
    std::set<QString> keys = {"name"};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString colorMapStr = dataValues[*keys.begin()];
    QString result = setColorMap( colorMapStr );
    return result;
}

/*std::shared_ptr<Carta::Lib::PixelPipeline::IColormapNamed> Colormap::getColorMap( ) const {

    QString colorMapName = m_state.getValue<QString>(COLOR_MAP_NAME);
    std::shared_ptr<Carta::Lib::PixelPipeline::IColormapNamed> cMap = m_colors->getColorMap( colorMapName );

    return cMap;
}*/

Controller* Colormap::getControllerSelected() const {
    //TODO:  Add sophistication.
    Controller* target = nullptr;
    int linkCount = m_linkImpl->getLinkCount();
    for ( int i = 0; i < linkCount; i++ ){
        CartaObject* obj = m_linkImpl->getLink(i);
        Controller* control = dynamic_cast<Controller*>(obj);
        if ( control != nullptr ){
            target = control;
            break;
        }
    }
    return target;
}

bool Colormap::_processColorStr( const QString key, const QString colorStr, bool* valid ){
    double colorPercent = colorStr.toDouble( valid );
    bool colorChanged = false;
    if ( *valid ){
        double oldColorPercent = m_state.getValue<double>( key );
        if ( abs( colorPercent - oldColorPercent ) >= 0.001f ){
            m_state.setValue<double>(key, colorPercent );
            colorChanged = true;
        }
    }
    else {
        qWarning() << "colorStr="<<colorStr<<" is not a valid color percent for key="<<key;
    }
    return colorChanged;
}

void Colormap::refreshState(){
    m_state.setValue<bool>(Util::STATE_FLUSH, true );
    m_state.flushState();
    m_state.setValue<bool>(Util::STATE_FLUSH, false );
}

QString Colormap::removeLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool objRemoved = false;
    QString result;
    if ( controller != nullptr ){
        objRemoved = m_linkImpl->removeLink( controller );
        if ( objRemoved ){
            disconnect( controller );
        }
    }
    else {
        result = "Color map could not remove link; only links to images supported.";
    }
    return result;
}

QString Colormap::setColorMap( const QString& colorMapStr ){
    QString mapName = m_state.getValue<QString>(COLOR_MAP_NAME);
    QString result;
    if ( m_colors != nullptr ){
       if( m_colors->isMap( colorMapStr ) ){
           if ( colorMapStr != mapName ){
              m_state.setValue<QString>(COLOR_MAP_NAME, colorMapStr );
              m_state.flushState();
              int linkCount = m_linkImpl->getLinkCount();
              for( int i = 0; i < linkCount; i++ ){
                  Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
                  controller->setColorMap( colorMapStr );
              }
              emit colorMapChanged( this );
           }
        }
       else {
           result = "Invalid colormap: " + colorMapStr;
       }
    }

    Util::commandPostProcess( result );
    return result;
}

QString Colormap::setGamma( double gamma ){
    QString result;
    double oldGamma = m_state.getValue<double>( GAMMA );
    const double ERROR_MARGIN = 1 / m_significantDigits;
    if ( qAbs( gamma - oldGamma) > ERROR_MARGIN ){
        m_state.setValue<double>(GAMMA, Util::roundToDigits(gamma, m_significantDigits ));
        m_state.flushState();
        //Let the controllers know gamma has changed.
        int linkCount = m_linkImpl->getLinkCount();
        for( int i = 0; i < linkCount; i++ ){
            Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
            controller->setGamma( gamma );
        }
        emit colorMapChanged( this );
    }
    return result;
}

QString Colormap::setDataTransform( const QString& transformString ){
    QString transformName = m_state.getValue<QString>(TRANSFORM_DATA);
    QString result;
    if ( m_dataTransforms != nullptr ){
        QString actualTransform;
        bool recognizedTransform = m_dataTransforms->isTransform( transformString, actualTransform );
        if( recognizedTransform ){
            if ( actualTransform != transformName ){
                m_state.setValue<QString>(TRANSFORM_DATA, actualTransform );
                m_state.flushState();
                int linkCount = m_linkImpl->getLinkCount();
                for( int i = 0; i < linkCount; i++ ){
                    Controller* controller = dynamic_cast<Controller*>( m_linkImpl->getLink(i));
                    controller->setTransformData( transformString );
                }
                emit colorMapChanged( this );
            }
            else {
               result = "Invalid data transform: " + transformString;
            }
        }
    }
    Util::commandPostProcess( result );
    return result;
}

void Colormap::_setColorProperties( Controller* target ){
    target->setColorMap( m_state.getValue<QString>(COLOR_MAP_NAME));
    target->setColorInverted( m_state.getValue<bool>(INVERT) );
    target->setColorReversed( m_state.getValue<bool>(REVERSE) );
    double redPercent = m_state.getValue<double>(COLOR_MIX_RED);
    double greenPercent = m_state.getValue<double>(COLOR_MIX_GREEN );
    double bluePercent = m_state.getValue<double>(COLOR_MIX_BLUE);
    target->setColorAmounts( redPercent, greenPercent, bluePercent );
    target->setGamma( m_state.getValue<double>(GAMMA) );
    target->setTransformData( m_state.getValue<QString>(TRANSFORM_DATA ));
    target->setCacheSize( m_state.getValue<int>(CACHE_SIZE) );
    target->setCacheInterpolation( m_state.getValue<bool>(INTERPOLATED));
    target->setPixelCaching( m_state.getValue<bool>(CACHE) );
}

Colormap::~Colormap(){

}
}
}
