#include "Data/Colormap.h"
#include "Data/Colormaps.h"
#include "Data/IColoredView.h"
#include "Data/Util.h"
#include "State/StateInterface.h"
#include "CartaLib/IColormapScalar.h"
#include "ImageView.h"

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

std::shared_ptr<Colormaps> Colormap::m_colors = nullptr;

class Colormap::Factory : public CartaObjectFactory {

    public:

        CartaObject * create (const QString & path, const QString & id)
        {
            return new Colormap (path, id);
        }
    };

bool Colormap::m_registered =
    ObjectManager::objectManager()->registerClass ( CLASS_NAME, new Colormap::Factory());

Colormap::Colormap( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ),
    m_stateMouse(path + StateInterface::DELIMITER+ImageView::VIEW){
    _initializeDefaultState();
    _initializeCallbacks();

    //Load the available color maps.
    if ( m_colors == nullptr ){
        CartaObject* obj = Util::findSingletonObject( Colormaps::CLASS_NAME );
        m_colors.reset( dynamic_cast<Colormaps*>( obj ));
    }
}



void Colormap::_initializeDefaultState(){
    m_state.insertValue<QString>( COLOR_MAP_NAME, "Gray" );
    m_state.insertValue<bool>(REVERSE, false);
    m_state.insertValue<bool>(INVERT, false );
    m_state.insertValue<bool>(CACHE, true);
    m_state.insertValue<bool>(INTERPOLATED, true );
    m_state.insertValue<int>(CACHE_SIZE, 2 );

    m_state.insertObject( COLOR_MIX );
    m_state.insertValue<double>( COLOR_MIX_RED, 1 );
    m_state.insertValue<double>(COLOR_MIX_GREEN, 1 );
    m_state.insertValue<double>(COLOR_MIX_BLUE, 1 );
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

    addCommandCallback( "addColoredObject", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                QString result = _commandAddColoredObject( params );
                return result;
            });
}



bool Colormap::addViewObject( std::shared_ptr<IColoredView> target ){
    bool objAdded = false;
    if ( target.get() ){
        m_coloredViews.append( target );
        target->colorMapChanged( m_state.getValue<QString>(COLOR_MAP_NAME));
        objAdded = true;
    }
    return objAdded;
}

QString Colormap::_commandAddColoredObject( const QString& params ){
    QString result;
    std::set<QString> keys = {COLORED_OBJECT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString objectId = dataValues[*keys.begin()];
    ObjectManager* man = ObjectManager::objectManager();
    CartaObject* coloredObj = man->getObject( objectId );
    if ( coloredObj != nullptr ){
        std::shared_ptr<IColoredView> target( dynamic_cast<IColoredView*>( coloredObj) );
        bool added = addViewObject( target );
        if ( !added ){
            result = "Please check that the object supports a color map: "+params;
        }
    }
    else {
        result = "Invalid colored object: "+ params;
    }
    result = Util::commandPostProcess( result, "");
    return result;
}

QString Colormap::_commandCacheColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {CACHE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString cacheStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool cache = Util::toBool(cacheStr, &validBool);
    bool oldCache = m_state.getValue<bool>(CACHE);
    if ( validBool ){
        if ( cache != oldCache){
            m_state.setValue<bool>(CACHE, cache );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid color map cache parameters: "+ params;
    }
    result = Util::commandPostProcess( result , Util::toString(oldCache));
    return result;
}

QString Colormap::_commandCacheSize( const QString& params ){
    QString result;
    std::set<QString> keys = {CACHE_SIZE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString cacheSizeStr = dataValues[*keys.begin()];
    bool validInt = false;
    int cacheSize = cacheSizeStr.toInt(&validInt);
    int currentCacheSize = m_state.getValue<int>(CACHE_SIZE);
    if ( validInt ){
        if ( cacheSize != currentCacheSize ){
            m_state.setValue<int>(CACHE_SIZE, cacheSize );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid color map cache size parameters: "+ params;
    }
    result = Util::commandPostProcess( result, QString::number(currentCacheSize) );
    return result;
}

QString Colormap::_commandInterpolatedColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {INTERPOLATED};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString interpolateStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool interpolated = Util::toBool(interpolateStr, &validBool);
    bool currentInterpolated = m_state.getValue<bool>(INTERPOLATED);
    if ( validBool ){
        if ( interpolated != currentInterpolated ){
            m_state.setValue<bool>(INTERPOLATED, interpolated );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid interpolated caching color map parameters: "+ params;
    }
    result = Util::commandPostProcess( result, Util::toString(currentInterpolated) );
    return result;
}

QString Colormap::_commandInvertColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {INVERT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString invertStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool invert = Util::toBool(invertStr, &validBool);
    bool oldInvert = m_state.getValue<bool>(INVERT );
    if ( validBool ){
        if ( invert != oldInvert ){
            m_state.setValue<bool>(INVERT, invert );
            m_state.flushState();
            QString mapName = m_state.getValue<QString>(COLOR_MAP_NAME);
            std::shared_ptr<Carta::Lib::IColormapScalar> coloredMap = m_colors->getColorMap( mapName );
            //coloredMap->setInverted( invert );
            for ( int i = 0; i < m_coloredViews.size(); i++ ){
                m_coloredViews[i] -> colorMapChanged( mapName );
            }
        }
    }
    else {
        result = "Invalid color map invert parameters: "+ params;
    }
    result = Util::commandPostProcess( result, Util::toString(oldInvert) );
    return result;
}

QString Colormap::_commandSetColorMix( const QString& params ){
    QString result;
    std::set<QString> keys = {RED_PERCENT, GREEN_PERCENT, BLUE_PERCENT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );

    bool validRed = false;
    bool redChanged = _processColorStr( COLOR_MIX_RED, dataValues[RED_PERCENT], &validRed );

    bool validBlue = false;
    bool blueChanged = _processColorStr( COLOR_MIX_BLUE, dataValues[BLUE_PERCENT], &validBlue );

    bool validGreen = false;
    bool greenChanged = _processColorStr( COLOR_MIX_GREEN, dataValues[GREEN_PERCENT], &validGreen);
    QString currValues;
    if ( redChanged || blueChanged || greenChanged ){
        m_state.flushState();
        QString mapName = m_state.getValue<QString>(COLOR_MAP_NAME);
        /*std::shared_ptr<Carta::Lib::IColormapScalar> coloredMap = m_colors->getColorMap( mapIndex );
        float redPercent = static_cast<float>(m_state.getValue<double>( RED_PERCENT ));
        float greenPercent = static_cast<float>(m_state.getValue<double>( GREEN_PERCENT ));
        float bluePercent = static_cast<float>(m_state.getValue<double>( BLUE_PERCENT ));
        coloredMap->setColorScales( redPercent, greenPercent, bluePercent );*/
        for ( int i = 0; i < m_coloredViews.size(); i++ ){
            m_coloredViews[i]->colorMapChanged( mapName);
        }
    }
    else if ( !validRed || !validGreen || !validBlue ){
        result = "Invalid Colormap color percent: "+ params;
        double redPercent = m_state.getValue<double>( COLOR_MIX_RED );
        double greenPercent = m_state.getValue<double>( COLOR_MIX_GREEN );
        double bluePercent = m_state.getValue<double>( COLOR_MIX_BLUE );
        currValues = QString::number(redPercent )+","+QString::number(greenPercent)+","+QString::number(bluePercent );
    }
    result = Util::commandPostProcess( result, currValues );
    return result;
}


QString Colormap::_commandReverseColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {REVERSE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString reverseStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool reverse = Util::toBool(reverseStr, &validBool);
    bool oldReverse = m_state.getValue<bool>(REVERSE);
    if ( validBool ){
        if ( reverse != oldReverse ){
            m_state.setValue<bool>(REVERSE, reverse );
            m_state.flushState();
            QString mapName = m_state.getValue<QString>(COLOR_MAP_NAME);
            std::shared_ptr<Carta::Lib::IColormapScalar> coloredMap = m_colors->getColorMap( mapName );
            //coloredMap->setReversed( mapName );
            for ( int i = 0; i < m_coloredViews.size(); i++ ){
                m_coloredViews[i] -> colorMapChanged( mapName );
            }
        }
    }
    else {
        result = "Invalid color map reverse parameters: "+ params;
    }
    result = Util::commandPostProcess( result, Util::toString(oldReverse) );
    return result;
}

QString Colormap::_commandSetColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {"name"};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString colorMapStr = dataValues[*keys.begin()];
    QString mapName = m_state.getValue<QString>(COLOR_MAP_NAME);
    if ( m_colors != nullptr ){
       if( m_colors->isMap( colorMapStr ) ){
           if ( colorMapStr != mapName ){
              m_state.setValue<QString>(COLOR_MAP_NAME, colorMapStr );
              m_state.flushState();
              for ( int i = 0; i < m_coloredViews.size(); i++ ){
                  m_coloredViews[i]->colorMapChanged( colorMapStr );
              }
           }
        }
       else {
           result = "Invalid colormap: " + colorMapStr;
       }
    }
    result = Util::commandPostProcess( result, mapName );
    return result;
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

Colormap::~Colormap(){

}
}
}
