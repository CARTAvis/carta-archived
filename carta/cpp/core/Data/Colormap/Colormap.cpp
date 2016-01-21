#include "Colormap.h"
#include "ColorState.h"
#include "Data/Settings.h"
#include "Data/Image/Controller.h"
#include "Data/Histogram/Histogram.h"
#include "Data/Util.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"
#include "ImageView.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include <QtCore/qmath.h>
#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

const QString Colormap::CLASS_NAME = "Colormap";
const QString Colormap::INTENSITY_MIN = "intensityMin";
const QString Colormap::INTENSITY_MAX = "intensityMax";


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
    m_stateData( Carta::State::UtilState::getLookup(path, Carta::State::StateInterface::STATE_DATA)),
    m_stateMouse(Carta::State::UtilState::getLookup(path,ImageView::VIEW)){

    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    Settings* prefObj = objMan->createObject<Settings>();
    m_settings.reset( prefObj );

    ColorState* colorStateObj = objMan->createObject<ColorState>();
    m_stateColorGlobal.reset( colorStateObj );
    connect( colorStateObj, SIGNAL( colorStateChanged()), this, SLOT( _colorStateChanged()));
    m_stateColorGlobal->_initializeDefaultState( m_state );
    m_stateColors.push_back( m_stateColorGlobal);
    _colorStateChanged();

    _initializeDefaultState();
    _initializeCallbacks();
}

QString Colormap::addLink( CartaObject*  cartaObject ){
    Controller* target = dynamic_cast<Controller*>(cartaObject);
    bool objAdded = false;
    QString result;
    if ( target != nullptr ){
        objAdded = m_linkImpl->addLink( target );
        if ( objAdded ){
            target->_setColorMapGlobal( m_stateColorGlobal );
            connect( target, SIGNAL(colorChanged(Controller*)), this, SLOT(_setColorStates(Controller*)));
            _setColorStates( target );
        }
    }
    else {
        Histogram* hist = dynamic_cast<Histogram*>(cartaObject);
        if ( hist != nullptr ){
            objAdded = m_linkImpl->addLink( hist );
            if ( objAdded ){
                //connect( this, SIGNAL(colorMapChanged( Colormap*)), hist, SLOT( updateColorMap( Colormap*)));
                //hist->updateColorMap( this );

                connect( hist,SIGNAL(colorIntensityBoundsChanged(double,double)), this, SLOT(_updateIntensityBounds( double, double )));
            }
        }
        else {
            result = "Colormap only supports linking to histograms and images.";
        }
    }
    return result;
}


void Colormap::clear(){
    m_linkImpl->clear();
}


void Colormap::_colorStateChanged(){
    if ( m_stateColors.size() > 0 ){
        m_stateColors[0]->_replicateTo( m_state );
        m_state.flushState();
    }
}


QString Colormap::_commandInvertColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {ColorState::INVERT};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString invertStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool invert = Util::toBool( invertStr, &validBool );
    if ( validBool ){
        result = setInvert( invert );
    }
    else {
        result = "Invert color map parameters must be true/false: "+params;
    }
    Util::commandPostProcess( result );
    return result;
}

QString Colormap::_commandReverseColorMap( const QString& params ){
    QString result;
    std::set<QString> keys = {ColorState::REVERSE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString reverseStr = dataValues[*keys.begin()];
    bool validBool = false;
    bool reverse = Util::toBool(reverseStr, &validBool);
    if ( validBool ){
        result = setReverse( reverse );
    }
    else {
        result = "Invalid color map reverse parameters: "+ params;
    }
    Util::commandPostProcess( result );
    return result;
}

QString Colormap::_commandSetColorMix( const QString& params ){
    QString result;
    std::set<QString> keys = {Util::RED, Util::GREEN, Util::BLUE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );

    bool validRed = false;
    double redValue = dataValues[Util::RED].toDouble(&validRed );

    bool validBlue = false;
    double blueValue = dataValues[Util::BLUE].toDouble(&validBlue );

    bool validGreen = false;
    double greenValue = dataValues[Util::GREEN].toDouble(&validGreen);

    if ( validRed && validBlue && validGreen ){
        result = setColorMix( redValue, greenValue, blueValue );
    }
    else {
        result = "Color mix values must be numbers: "+params;
    }
    Util::commandPostProcess( result );
    return result;
}

QString Colormap::_commandSetColorMap( const QString& params ){
    std::set<QString> keys = {"name"};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString colorMapStr = dataValues[*keys.begin()];
    QString result = setColorMap( colorMapStr );
    Util::commandPostProcess( result );
    return result;
}

QList<QString> Colormap::getLinks() const {
    return m_linkImpl->getLinkIds();
}

QString Colormap::_getPreferencesId() const {
    return m_settings->getPath();
}

QString Colormap::getStateString( const QString& sessionId, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        Carta::State::StateInterface prefState( "");
        prefState.setValue<QString>(Carta::State::StateInterface::OBJECT_TYPE, CLASS_NAME );
        prefState.insertValue<QString>( Util::PREFERENCES, m_state.toString());
        prefState.insertValue<QString>( Settings::SETTINGS, m_settings->getStateString(sessionId, type) );
        result = prefState.toString();
    }
    else if ( type == SNAPSHOT_LAYOUT ){
        result = m_linkImpl->getStateString(getIndex(), getSnapType(type));
    }
    return result;
}


void Colormap::_initializeDefaultState(){

    //Image dependent intensity bounds
    m_stateData.insertValue<double>( INTENSITY_MIN, 0 );
    m_stateData.insertValue<double>( INTENSITY_MAX, 1 );
    m_stateData.flushState();

    //Mouse
    m_stateMouse.insertObject( ImageView::MOUSE );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_X, 0 );
    m_stateMouse.insertValue<QString>(ImageView::MOUSE_Y, 0 );
    m_stateMouse.flushState();

}

void Colormap::_initializeCallbacks(){

    addCommandCallback( "registerPreferences", [=] (const QString & /*cmd*/,
                        const QString & /*params*/, const QString & /*sessionId*/) -> QString {
                    QString result = _getPreferencesId();
                    return result;
    });


    addCommandCallback( "setDataTransform", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {ColorState::TRANSFORM_DATA};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString dataTransformStr = dataValues[*keys.begin()];
        QString result = setDataTransform( dataTransformStr );
        Util::commandPostProcess( result );
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

    addCommandCallback( "setNanColor", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {Util::RED, Util::GREEN, Util::BLUE};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );

        bool validRed = false;
        double redValue = dataValues[Util::RED].toInt(&validRed );

        bool validBlue = false;
        double blueValue = dataValues[Util::BLUE].toInt(&validBlue );

        bool validGreen = false;
        double greenValue = dataValues[Util::GREEN].toInt(&validGreen);

        if ( validRed && validBlue && validGreen ){
            result = setNanColor( redValue, greenValue, blueValue );
        }
        else {
            result = "Nan color values must be in [0,255]: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setScales", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {ColorState::SCALE_1, ColorState::SCALE_2};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        bool valid = false;
        double scale1 = dataValues[ColorState::SCALE_1].toDouble( & valid );
        if ( !valid ){
            result = "Invalid color scale: "+params;
        }
        else {
            double scale2 = dataValues[ColorState::SCALE_2].toDouble( &valid );
            if ( !valid ){
                result = "Invalid color scale: "+params;
            }
            else {
                double oldScale1 = m_state.getValue<double>(ColorState::SCALE_1);
                double oldScale2 = m_state.getValue<double>(ColorState::SCALE_2);
                bool changedState = false;
                if ( scale1 != oldScale1 ){
                    m_state.setValue<double>(ColorState::SCALE_1, scale1 );
                    changedState = true;
                }
                if ( scale2 != oldScale2 ){
                    m_state.setValue<double>(ColorState::SCALE_2, scale2 );
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

    addCommandCallback( "setSignificantDigits", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
                QString result;
                std::set<QString> keys = {ColorState::SIGNIFICANT_DIGITS};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString digitsStr = dataValues[ColorState::SIGNIFICANT_DIGITS];
                bool validDigits = false;
                int digits = digitsStr.toInt( &validDigits );
                if ( validDigits ){
                    result = setSignificantDigits( digits );
                }
                else {
                    result = "Colormap significant digits must be an integer: "+params;
                }
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setDefaultNan", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {ColorState::NAN_DEFAULT};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString defaultNanStr = dataValues[*keys.begin()];
        bool validBool = false;
        bool useDefaultNan = Util::toBool( defaultNanStr, &validBool );
        QString result;
        if ( validBool ){
            setNanDefault( useDefaultNan );
        }
        else {
            result = "Please specify true/false for use default Nan: "+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setGlobal", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {ColorState::GLOBAL};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString globalStr = dataValues[*keys.begin()];
            bool validBool = false;
            bool global = Util::toBool( globalStr, &validBool );
            QString result;
            if ( validBool ){
                setGlobal( global );
            }
            else {
                result = "Please specify true/false when setting whether or not the color map is global: "+params;
            }
            Util::commandPostProcess( result );
            return result;
        });
}


bool Colormap::_isGlobal() const {
    bool global = true;
    if ( m_stateColors.size() > 0 ){
        global = m_stateColors[0]->_isGlobal();
    }
    return global;
}

bool Colormap::isInverted() const {
    bool inverted = false;
    if ( m_stateColors.size() > 0 ){
        inverted = m_stateColors[0]->_isInverted();
    }
    return inverted;
}

bool Colormap::isNanDefault() const {
    bool nanDefault = false;
    if ( m_stateColors.size() > 0 ){
        nanDefault = m_stateColors[0]->_isNanDefault();
    }
    return nanDefault;
}

bool Colormap::isLinked( const QString& linkId ) const {
    bool linked = false;
    CartaObject* obj = m_linkImpl->searchLinks( linkId );
    if ( obj != nullptr ){
        linked = true;
    }
    return linked;
}


bool Colormap::isReversed() const {
    bool reversed = false;
    if ( m_stateColors.size() > 0 ){
        reversed = m_stateColors[0]->_isReversed();
    }
    return reversed;
}


void Colormap::refreshState(){
    CartaObject::refreshState();
    m_settings->refreshState();
    m_linkImpl->refreshState();
}

void Colormap::resetState( const QString& state ){
    Carta::State::StateInterface restoredState( "");
    restoredState.setState( state );

    QString settingStr = restoredState.getValue<QString>(Settings::SETTINGS);
    m_settings->resetStateString( settingStr );

    QString prefStr = restoredState.getValue<QString>(Util::PREFERENCES);
    m_state.setState( prefStr );
    m_state.flushState();
}



QString Colormap::removeLink( CartaObject* cartaObject ){
    Controller* controller = dynamic_cast<Controller*>(cartaObject);
    bool objRemoved = false;
    QString result;
    if ( controller != nullptr ){
        objRemoved = m_linkImpl->removeLink( controller );
        if ( objRemoved ){
            controller->disconnect( this );
        }
    }
    else {
       Histogram* hist = dynamic_cast<Histogram*>(cartaObject);
       if ( hist != nullptr ){
           objRemoved = m_linkImpl->removeLink( hist );
           if ( objRemoved ){
               this->disconnect( hist );
           }
       }
       else {
           result= "Color was unable to remove link, only histogram and image links supported.";
       }
    }
    return result;
}


QString Colormap::setColorMap( const QString& colorMapStr ){
    int stateColorCount = m_stateColors.size();
    QString result;
    for ( int i = 0; i < stateColorCount; i++ ){
        result = m_stateColors[i]->_setColorMap( colorMapStr );
        if ( !result.isEmpty() ){
            break;
        }
    }
    if ( result.isEmpty() ){
        _colorStateChanged();
    }
    return result;
}

QString Colormap::setColorMix( double redValue, double greenValue, double blueValue){
    int stateColorCount = m_stateColors.size();
    QString result;
    for ( int i = 0; i < stateColorCount; i++ ){
        result = m_stateColors[i]->_setColorMix( redValue, greenValue, blueValue );
        if ( !result.isEmpty()){
            break;
        }
    }
    if ( result.isEmpty() ){
        _colorStateChanged();
    }
    return result;
}


QString Colormap::setInvert( bool invert ){
    int stateColorCount = m_stateColors.size();
    QString result;
    for ( int i = 0; i < stateColorCount; i++ ){
        m_stateColors[i]->_setInvert( invert );
    }
    if ( stateColorCount == 0 ){
        result = "There were no color maps to invert.";
    }
    else {
        _colorStateChanged();
    }
    return result;
}

QString Colormap::setGamma( double gamma ){
    int stateColorCount = m_stateColors.size();
    QString result;
    for ( int i = 0; i < stateColorCount; i++ ){
        result = m_stateColors[i]->_setGamma( gamma );
        if ( !result.isEmpty()){
            break;
        }
    }
    if ( result.isEmpty() ){
        _colorStateChanged();
    }
    return result;
}

QString Colormap::setDataTransform( const QString& transformString){
    int stateColorCount = m_stateColors.size();
    QString result;
    for ( int i = 0; i < stateColorCount; i++ ){
        result = m_stateColors[i]->_setDataTransform( transformString );
        if ( !result.isEmpty() ){
            break;
        }
    }
    if ( result.isEmpty() ){
        _colorStateChanged();
    }
    return result;
}


void Colormap::_setColorStates( Controller* controller ){
    std::vector< std::shared_ptr<ColorState> > selectedColorStates = controller->getSelectedColorStates();
    m_stateColors.clear();
    int stateColorCount = selectedColorStates.size();
    for ( int i = 0; i < stateColorCount; i++ ){
        m_stateColors.push_back( selectedColorStates[i] );
    }

    //Update the state the client is listening to.
    _colorStateChanged();
}

void Colormap::setGlobal( bool global ){
    //Notify all the controllers to replace their global color maps with
    //individual ones or vice versa.
    int linkCount = m_linkImpl->getLinkCount();
    for ( int i = 0; i < linkCount; i++ ){
        CartaObject* obj = m_linkImpl->getLink( i );
        Controller* controller = dynamic_cast<Controller*>(obj);
        if ( controller != nullptr ){
           controller->_setColorMapUseGlobal( global );
        }
    }
}

QString Colormap::setNanColor( int redValue, int greenValue, int blueValue){
    int stateColorCount = m_stateColors.size();
    QString result;
    if ( isNanDefault() ){
        setNanDefault( false );
    }
    for ( int i = 0; i < stateColorCount; i++ ){
        result = m_stateColors[i]->_setNanColor( redValue, greenValue, blueValue );
        if ( !result.isEmpty()){
            break;
        }
    }
    if ( result.isEmpty() ){
        _colorStateChanged();
    }
    return result;
}

QString Colormap::setNanDefault( bool nanDefault ) {
    int stateColorCount = m_stateColors.size();
    QString result;
    for ( int i = 0; i < stateColorCount; i++ ){
        m_stateColors[i]->_setNanDefault( nanDefault );
    }
    if ( stateColorCount == 0 ){
        result = "There were no color maps to for nan default.";
    }
    else {
        _colorStateChanged();
    }
    return result;
}

QString Colormap::setReverse( bool reverse ){
    int stateColorCount = m_stateColors.size();
    QString result;
    for ( int i = 0; i < stateColorCount; i++ ){
        m_stateColors[i]->_setReverse( reverse );
    }
    if ( stateColorCount == 0 ){
        result = "There were no color maps to reverse.";
    }
    else {
        _colorStateChanged();
    }
    return result;
}

QString Colormap::setSignificantDigits( int digits ){
    int stateColorCount = m_stateColors.size();
    QString result;
    for ( int i = 0; i < stateColorCount; i++ ){
       result = m_stateColors[i]->_setSignificantDigits( digits );
       if ( !result.isEmpty() ){
           break;
       }
    }
    if ( result.isEmpty() ){
        _colorStateChanged();
    }
    return result;
}



void Colormap::_updateIntensityBounds( double minIntensity, double maxIntensity ){
    double oldMinIntensity = m_stateData.getValue<double>( INTENSITY_MIN );
    bool intensityChanged = false;
    if ( oldMinIntensity != minIntensity ){
        intensityChanged = true;
        m_stateData.setValue<double>( INTENSITY_MIN, minIntensity );
    }

    double oldMaxIntensity = m_stateData.getValue<double>( INTENSITY_MAX );
    if ( oldMaxIntensity != maxIntensity ){
        intensityChanged = true;
        m_stateData.setValue<double>( INTENSITY_MAX, maxIntensity );
    }
    if ( intensityChanged ){
        m_stateData.flushState();
    }
}

Colormap::~Colormap(){
    if ( m_settings != nullptr ){
        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        QString id = m_settings->getId();
        objMan->removeObject( id );
    }
}
}
}
