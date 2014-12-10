#include "Data/Colormap.h"
#include "Data/Colormaps.h"
#include "Data/Util.h"
#include "State/StateInterface.h"
#include "ImageView.h"

#include <set>

#include <QDebug>


const QString Colormap::CLASS_NAME = "Colormap";
const QString Colormap::COLOR_MAP_INDEX = "colorMapIndex";
const QString Colormap::REVERSE = "reverse";
const QString Colormap::INVERT = "invert";
const QString Colormap::COLOR_MIX = "colorMix";
const QString Colormap::COLOR_MIX_RED = COLOR_MIX + StateInterface::DELIMITER + "redPercent";
const QString Colormap::COLOR_MIX_GREEN = COLOR_MIX + StateInterface::DELIMITER + "greenPercent";
const QString Colormap::COLOR_MIX_BLUE = COLOR_MIX + StateInterface::DELIMITER + "bluePercent";

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
    m_view(nullptr),
    m_stateMouse(path + StateInterface::DELIMITER+ImageView::VIEW){
    _initializeDefaultState();
    _initializeCallbacks();
    m_view.reset( new ImageView( path, QColor("cyan"), QImage(), &m_stateMouse));
    registerView(m_view.get());

    //Load the available color maps.
    if ( m_colors == nullptr ){
        CartaObject* obj = Util::createObject( Colormaps::CLASS_NAME );
        m_colors.reset( dynamic_cast<Colormaps*>( obj ));
    }
}

void Colormap::_initializeDefaultState(){
    m_state.insertValue<int>( COLOR_MAP_INDEX, 0 );
    m_state.insertValue<bool>(REVERSE, false);
    m_state.insertValue<bool>(INVERT, false );
    m_state.insertObject( COLOR_MIX );
    m_state.insertValue<double>( COLOR_MIX_RED, 0.5 );
    m_state.insertValue<double>(COLOR_MIX_GREEN, 0.75 );
    m_state.insertValue<double>(COLOR_MIX_BLUE, 0.75 );
    m_state.flushState();

    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.flushState();

}

void Colormap::_initializeCallbacks(){
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
}

QString Colormap::_commandInvertColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {INVERT};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString invertStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool invert = Util::toBool(invertStr, &validBool);
    if ( validBool ){
        if ( invert != m_state.getValue<bool>(INVERT)){
            m_state.setValue<bool>(INVERT, invert );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid color map invert parameters: "+ params;
        qWarning() << result;
    }
    return result;
}

QString Colormap::_commandSetColorMix( const QString& params ){
    QString result;
    std::set<QString> keys = {COLOR_MIX_RED, COLOR_MIX_GREEN, COLOR_MIX_BLUE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );

    bool validRed = false;
    bool redChanged = _processColorStr( COLOR_MIX_RED, dataValues[COLOR_MIX_RED], &validRed );

    bool validBlue = false;
    bool blueChanged = _processColorStr( COLOR_MIX_BLUE, dataValues[COLOR_MIX_BLUE], &validBlue );

    bool validGreen = false;
    bool greenChanged = _processColorStr( COLOR_MIX_GREEN, dataValues[COLOR_MIX_GREEN], &validGreen);

    if ( redChanged || blueChanged || greenChanged ){
        m_state.flushState();
    }
    else if ( !validRed || !validGreen || !validBlue ){
        result = "Invalid Colormap color percent: "+ params;
        qWarning() << result;
    }
    return result;
}

bool Colormap::_processColorStr( const QString key, const QString colorStr, bool* valid ){
    double colorPercent = colorStr.toDouble( valid );
    bool colorChanged = false;
    if ( *valid ){
        if ( abs( colorPercent - m_state.getValue<double>( key) ) < 0.0001f ){
            m_state.setValue<double>(key, colorPercent );
            colorChanged = true;
        }
    }
    return colorChanged;
}



QString Colormap::_commandReverseColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {REVERSE};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString reverseStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool reverse = Util::toBool(reverseStr, &validBool);
    if ( validBool ){
        if ( reverse != m_state.getValue<bool>(REVERSE)){
            m_state.setValue<bool>(REVERSE, reverse );
            m_state.flushState();
        }
    }
    else {
        result = "Invalid color map reverse parameters: "+ params;
        qWarning() << result;
    }
    return result;
}

QString Colormap::_commandSetColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {"name"};
    std::map<QString,QString> dataValues = Util::parseParamMap( params, keys );
    QString colorMapStr = dataValues[*keys.begin()];
    if ( m_colors != nullptr ){
       int index = m_colors->getIndex( colorMapStr );
       if ( index >= 0 ){
           if ( index != m_state.getValue<int>(COLOR_MAP_INDEX) ){
              m_state.setValue<int>(COLOR_MAP_INDEX, index );
              m_state.flushState();
              emit colorMapChanged( index );
           }
        }
       else {
           result = "Invalid colormap: " + colorMapStr;
           qWarning() << result;
       }
    }
    return result;
}

Colormap::~Colormap(){

}
