#include "ColorState.h"
#include "Colormaps.h"
#include "Data/Image/Controller.h"
#include "TransformsData.h"
#include "Data/Util.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"
#include <set>
#include <QtCore/qmath.h>
#include <QDebug>

namespace Carta {

namespace Data {

const QString ColorState::CLASS_NAME = "ColorState";
const QString ColorState::BORDER_COLOR = "borderColor";
const QString ColorState::BORDER_DEFAULT = "borderDefault";
const QString ColorState::COLOR_MAP_NAME = "colorMapName";
const QString ColorState::COLORED_OBJECT = "coloredObject";
const QString ColorState::REVERSE = "reverse";
const QString ColorState::INVERT = "invert";
const QString ColorState::GLOBAL = "global";
const QString ColorState::COLOR_MIX = "colorMix";
const QString ColorState::INTENSITY_MIN = "intensityMin";
const QString ColorState::INTENSITY_MAX = "intensityMax";
const QString ColorState::NAN_COLOR = "nanColor";
const QString ColorState::NAN_DEFAULT = "nanDefault";
const QString ColorState::SCALE_1 = "scale1";
const QString ColorState::SCALE_2 = "scale2";
const QString ColorState::GAMMA = "gamma";
const QString ColorState::SIGNIFICANT_DIGITS = "significantDigits";
const QString ColorState::TRANSFORM_IMAGE = "imageTransform";
const QString ColorState::TRANSFORM_DATA = "dataTransform";

Colormaps* ColorState::m_colors = nullptr;
TransformsData* ColorState::m_dataTransforms = nullptr;

class ColorState::Factory : public Carta::State::CartaObjectFactory {

    public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new ColorState (path, id);
        }
    };

bool ColorState::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new ColorState::Factory());

ColorState::ColorState( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState( m_state );
    _initializeStatics();
    _setErrorMargin();
}

int ColorState::_getBorderGreen() const {
    QString greenLookup = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::GREEN );
    return m_state.getValue<int>( greenLookup );
}

int ColorState::_getBorderRed() const {
    QString redLookup = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::RED );
    return m_state.getValue<int>( redLookup );
}

int ColorState::_getBorderBlue() const {
    QString blueLookup = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::BLUE );
    return m_state.getValue<int>( blueLookup );
}

int ColorState::_getBorderTransparency() const {
    QString alphaLookup = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::ALPHA );
    return m_state.getValue<int>( alphaLookup );
}

QString ColorState::_getColorMap() const {
    return m_state.getValue<QString>( COLOR_MAP_NAME );
}

QString ColorState::_getDataTransform() const {
    return m_state.getValue<QString>( TRANSFORM_DATA );
}

double ColorState::_getGamma() const {
    return m_state.getValue<double>( GAMMA );
}

double ColorState::_getMixGreen() const {
    QString greenLookup = Carta::State::UtilState::getLookup( COLOR_MIX, Util::GREEN );
    return m_state.getValue<double>( greenLookup );
}

double ColorState::_getMixRed() const {
    QString redLookup = Carta::State::UtilState::getLookup( COLOR_MIX, Util::RED );
    return m_state.getValue<double>( redLookup );
}

double ColorState::_getMixBlue() const {
    QString blueLookup = Carta::State::UtilState::getLookup( COLOR_MIX, Util::BLUE );
    return m_state.getValue<double>( blueLookup );
}

int ColorState::_getNanGreen() const {
    QString greenLookup = Carta::State::UtilState::getLookup( NAN_COLOR, Util::GREEN );
    return m_state.getValue<int>( greenLookup );
}

int ColorState::_getNanRed() const {
    QString redLookup = Carta::State::UtilState::getLookup( NAN_COLOR, Util::RED );
    return m_state.getValue<int>( redLookup );
}

int ColorState::_getNanBlue() const {
    QString blueLookup = Carta::State::UtilState::getLookup( NAN_COLOR, Util::BLUE );
    return m_state.getValue<int>( blueLookup );
}

QString ColorState::getStateString( const QString& /*sessionId*/, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        result = m_state.toString();
    }
    return result;
}


void ColorState::_initializeDefaultState( Carta::State::StateInterface& state ){
    state.insertValue<QString>( COLOR_MAP_NAME, "Gray" );
    state.insertValue<bool>(REVERSE, false);
    state.insertValue<bool>(INVERT, false );
    state.insertValue<bool>(GLOBAL, true );
    state.insertValue<bool>(NAN_DEFAULT, true );
    state.insertValue<bool>(BORDER_DEFAULT, true );

    state.insertValue<double>(GAMMA, 1.0 );
    state.insertValue<double>(SCALE_1, 0.0 );
    state.insertValue<double>(SCALE_2, 0.0 );

    //Color mix
    state.insertObject( COLOR_MIX );
    QString redKey = Carta::State::UtilState::getLookup( COLOR_MIX, Util::RED );
    state.insertValue<double>( redKey, 1 );
    QString greenKey = Carta::State::UtilState::getLookup( COLOR_MIX, Util::GREEN );
    state.insertValue<double>( greenKey, 1 );
    QString blueKey = Carta::State::UtilState::getLookup( COLOR_MIX, Util::BLUE );
    state.insertValue<double>( blueKey, 1 );

    state.insertValue<int>(SIGNIFICANT_DIGITS, 6 );
    state.insertValue<QString>(TRANSFORM_IMAGE, "Gamma");
    state.insertValue<QString>(TRANSFORM_DATA, "None");

    //Nan color
    state.insertObject( NAN_COLOR );
    QString redLookup = Carta::State::UtilState::getLookup( NAN_COLOR, Util::RED );
    state.insertValue<int>( redLookup, 255 );
    QString blueLookup = Carta::State::UtilState::getLookup( NAN_COLOR, Util::BLUE );
    state.insertValue<int>( blueLookup, 0 );
    QString greenLookup = Carta::State::UtilState::getLookup( NAN_COLOR, Util::GREEN );
    state.insertValue<int>( greenLookup, 0 );
    QString alphaLookup = Carta::State::UtilState::getLookup( NAN_COLOR, Util::ALPHA );
    state.insertValue<int>( alphaLookup, 255 );

    //Border color
    state.insertObject( BORDER_COLOR );
    redLookup = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::RED );
    state.insertValue<int>( redLookup, 0 );
    blueLookup = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::BLUE );
    state.insertValue<int>( blueLookup, 0 );
    greenLookup = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::GREEN );
    state.insertValue<int>( greenLookup, 0 );
    alphaLookup = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::ALPHA );
    state.insertValue<int>( alphaLookup, 255 );

    //Default Tab
    state.insertValue<int>( Util::TAB_INDEX, 0 );
}



void ColorState::_initializeStatics(){
    //Load the available color maps.
    if ( m_colors == nullptr ){
        m_colors = Util::findSingletonObject<Colormaps>();
    }

    //Data transforms
    if ( m_dataTransforms == nullptr ){
        m_dataTransforms = Util::findSingletonObject<TransformsData>();
    }
}

bool ColorState::_isGlobal() const {
    return m_state.getValue<bool>( GLOBAL );
}

bool ColorState::_isBorderDefault() const {
    return m_state.getValue<bool>( BORDER_DEFAULT );
}

bool ColorState::_isNanDefault() const {
    return m_state.getValue<bool>( NAN_DEFAULT );
}

bool ColorState::_isReversed() const {
    return m_state.getValue<bool>( REVERSE );
}

bool ColorState::_isInverted() const {
    return m_state.getValue<bool>( INVERT );
}

void ColorState::_replicateTo( ColorState* otherState ){
    if ( otherState != nullptr ){
        _replicateTo( otherState->m_state );
    }
}


void ColorState::_replicateTo( Carta::State::StateInterface& otherState ){
    QString colorMapName = m_state.getValue<QString>(COLOR_MAP_NAME );
    otherState.setValue<QString>(COLOR_MAP_NAME, colorMapName );
    bool global = _isGlobal();
    otherState.setValue<bool>( GLOBAL, global );
    bool inverted = _isInverted();
    otherState.setValue<bool>( INVERT, inverted );
    bool reversed = _isReversed();
    otherState.setValue<bool>( REVERSE, reversed );
    bool nanDefault = _isNanDefault();
    otherState.setValue<bool>( NAN_DEFAULT, nanDefault );
    bool borderDefault = _isBorderDefault();
    otherState.setValue<bool>( BORDER_DEFAULT, borderDefault );

    //Color Mix
    QString redKey = Carta::State::UtilState::getLookup( COLOR_MIX, Util::RED );
    double redPercent = m_state.getValue<double>(redKey);
    QString greenKey = Carta::State::UtilState::getLookup( COLOR_MIX, Util::GREEN );
    double greenPercent = m_state.getValue<double>( greenKey );
    QString blueKey = Carta::State::UtilState::getLookup( COLOR_MIX, Util::BLUE );
    double bluePercent = m_state.getValue<double>( blueKey );
    otherState.setValue<double>( redKey, redPercent );
    otherState.setValue<double>( greenKey, greenPercent );
    otherState.setValue<double>( blueKey, bluePercent );

    //Nan color
    redKey = Carta::State::UtilState::getLookup( NAN_COLOR, Util::RED );
    int red = m_state.getValue<int>(redKey);
    greenKey = Carta::State::UtilState::getLookup( NAN_COLOR, Util::GREEN );
    int green = m_state.getValue<int>( greenKey );
    blueKey = Carta::State::UtilState::getLookup( NAN_COLOR, Util::BLUE );
    int blue = m_state.getValue<int>( blueKey );
    otherState.setValue<int>( redKey, red );
    otherState.setValue<int>( greenKey, green );
    otherState.setValue<int>( blueKey, blue );

    double gamma = m_state.getValue<double>( GAMMA );
    otherState.setValue<double>(GAMMA, gamma );
    QString dataTransform = m_state.getValue<QString>( TRANSFORM_DATA );
    otherState.setValue<QString>(TRANSFORM_DATA, dataTransform);

    //Border color
    redKey = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::RED );
    red = m_state.getValue<int>(redKey);
    greenKey = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::GREEN );
    green = m_state.getValue<int>( greenKey );
    blueKey = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::BLUE );
    blue = m_state.getValue<int>( blueKey );
    QString alphaKey = Carta::State::UtilState::getLookup( BORDER_COLOR, Util::ALPHA );
    int alpha = m_state.getValue<int>( alphaKey );
    otherState.setValue<int>( redKey, red );
    otherState.setValue<int>( greenKey, green );
    otherState.setValue<int>( blueKey, blue );
    otherState.setValue<int>( alphaKey, alpha );

    int tabIndex = m_state.getValue<int>( Util::TAB_INDEX );
    otherState.setValue<int>( Util::TAB_INDEX, tabIndex );
}

void ColorState::_resetState( const QString& stateStr ){
    m_state.setState( stateStr );
}

QString ColorState::_setBorderAlpha( int alphaValue ){
    QString result;
    const QString USER_ID = "Border background";
    bool alphaChanged = _setColor( Util::ALPHA, BORDER_COLOR, USER_ID, alphaValue, result );
    if ( alphaChanged ){
        m_state.flushState();
        emit colorStateChanged();
    }
    return result;
}


QString ColorState::_setBorderColor( int redValue, int greenValue, int blueValue){
    QString result;
    const QString USER_ID = "Border background";
    bool greenChanged = _setColor( Util::GREEN, BORDER_COLOR, USER_ID, greenValue, result );
    bool redChanged = _setColor( Util::RED, BORDER_COLOR, USER_ID, redValue, result );
    bool blueChanged = _setColor( Util::BLUE, BORDER_COLOR, USER_ID, blueValue, result );
    if ( redChanged || blueChanged || greenChanged ){
        emit colorStateChanged();
    }
    return result;
}

void ColorState::_setBorderDefault( bool useDefault ){
    bool oldBorderDefault = m_state.getValue<bool>( BORDER_DEFAULT );
    if ( useDefault != oldBorderDefault ){
        m_state.setValue<bool>( BORDER_DEFAULT, useDefault );
        _setBorderColor( 0, 0, 0);
        _setBorderAlpha( 255 );
        m_state.flushState();
        emit colorStateChanged();
    }
}

bool ColorState::_setColor( const QString& key, const QString& majorKey, const QString& userId,
        int colorAmount, QString& errorMsg ){
    bool colorChanged = false;
    if ( colorAmount<0 || colorAmount > 255 ){
        errorMsg = errorMsg + userId + " "+key + " must be in [0,255]. ";
    }
    else {
        QString valueKey = Carta::State::UtilState::getLookup( majorKey, key );
        double oldColorAmount = m_state.getValue<int>( valueKey );
        if ( colorAmount != oldColorAmount ){
            m_state.setValue<int>(valueKey, colorAmount );
            colorChanged = true;
        }
    }
    return colorChanged;
}


QString ColorState::_setColorMix( double redValue, double greenValue, double blueValue){
    QString result;
    bool greenChanged = _setColorMix( Util::GREEN, greenValue, result );
    bool redChanged = _setColorMix( Util::RED, redValue, result );
    bool blueChanged = _setColorMix( Util::BLUE, blueValue, result );
    if ( redChanged || blueChanged || greenChanged ){
        emit colorStateChanged();
    }
    return result;
}

bool ColorState::_setColorMix( const QString& key, double colorPercent, QString& errorMsg ){
    bool colorChanged = false;
    if ( colorPercent<0 || colorPercent > 1 ){
        errorMsg = errorMsg + "Mix color "+key + " must be in [0,1]. ";
    }
    else {
        QString mixKey = Carta::State::UtilState::getLookup( COLOR_MIX, key );
        double oldColorPercent = m_state.getValue<double>( mixKey );
        if ( abs( colorPercent - oldColorPercent ) >= 0.001f ){
            m_state.setValue<double>( mixKey, colorPercent );
            colorChanged = true;
        }
    }
    return colorChanged;
}

QString ColorState::_setColorMap( const QString& colorMapStr ){
    QString mapName = m_state.getValue<QString>(COLOR_MAP_NAME);
    QString result;
    if ( m_colors != nullptr ){
       if( m_colors->isMap( colorMapStr ) ){
           if ( colorMapStr != mapName ){
              m_state.setValue<QString>(COLOR_MAP_NAME, colorMapStr );
              m_state.flushState();
              emit colorStateChanged();
           }
        }
       else {
           result = "Invalid ColorState: " + colorMapStr;
       }
    }
    return result;
}

QString ColorState::_setDataTransform( const QString& transformString ){
    QString result("");
    QString transformName = m_state.getValue<QString>(TRANSFORM_DATA);
    if ( m_dataTransforms != nullptr ){
        QString actualTransform;
        bool recognizedTransform = m_dataTransforms->isTransform( transformString, actualTransform );
        if( recognizedTransform ){
            if ( actualTransform != transformName ){
                m_state.setValue<QString>(TRANSFORM_DATA, actualTransform );
                m_state.flushState();
                emit colorStateChanged();
            }
        }
        else {
           result = "Invalid data transform: " + transformString;
        }
    }
    return result;
}

void ColorState::_setErrorMargin(){
    int significantDigits = m_state.getValue<int>(SIGNIFICANT_DIGITS );
    m_errorMargin = 1.0/qPow(10,significantDigits);
}


QString ColorState::_setGamma( double gamma ){
    QString result;
    double oldGamma = m_state.getValue<double>( GAMMA );

    if ( qAbs( gamma - oldGamma) > m_errorMargin ){
        int digits = m_state.getValue<int>(SIGNIFICANT_DIGITS);
        m_state.setValue<double>(GAMMA, Util::roundToDigits(gamma, digits ));
        emit colorStateChanged();
    }
    return result;
}

void ColorState::_setGlobal( bool global ){
    bool oldGlobal = m_state.getValue<bool>(GLOBAL);
    if ( global != oldGlobal ){
        m_state.setValue<bool>(GLOBAL, global);
    }
}

void ColorState::_setInvert( bool invert ){
    bool oldInvert = m_state.getValue<bool>(INVERT );
    if ( invert != oldInvert ){
        m_state.setValue<bool>(INVERT, invert );
        emit colorStateChanged();
    }
}

QString ColorState::_setNanColor( int redValue, int greenValue, int blueValue){
    QString result;
    const QString USER_ID = "Nan color";
    bool greenChanged = _setColor( Util::GREEN, NAN_COLOR, USER_ID, greenValue, result );
    bool redChanged = _setColor( Util::RED, NAN_COLOR, USER_ID, redValue, result );
    bool blueChanged = _setColor( Util::BLUE, NAN_COLOR, USER_ID, blueValue, result );
    if ( redChanged || blueChanged || greenChanged ){
        emit colorStateChanged();
    }
    return result;
}



void ColorState::_setNanDefault( bool useDefault ){
    bool oldNanDefault = m_state.getValue<bool>( NAN_DEFAULT );
    if ( useDefault != oldNanDefault ){
        m_state.setValue<bool>( NAN_DEFAULT, useDefault );
        m_state.flushState();
        emit colorStateChanged();
    }
}

void ColorState::_setReverse( bool reverse ){
    bool oldReverse = m_state.getValue<bool>(REVERSE);
    if ( reverse != oldReverse ){
        m_state.setValue<bool>(REVERSE, reverse );
        m_state.flushState();
        emit colorStateChanged();
    }
}


QString ColorState::_setSignificantDigits( int digits ){
    QString result;
    if ( digits <= 0 ){
        result = "Invalid significant digits; must be positive:  "+QString::number( digits );
    }
    else {
        if ( m_state.getValue<int>(SIGNIFICANT_DIGITS) != digits ){
            m_state.setValue<int>(SIGNIFICANT_DIGITS, digits );
            _setErrorMargin();
            emit colorStateChanged();
        }
    }
    return result;
}

QString ColorState::_setTabIndex( int index ){
    QString result;
    if ( index >= 0 ){
        int oldIndex = m_state.getValue<int>( Util::TAB_INDEX );
        if ( index != oldIndex ){
            m_state.setValue<int>( Util::TAB_INDEX, index );
            emit colorStateChanged();
        }
    }
    else {
        result = "Colormap settings tab index must be nonnegative: "+ QString::number(index);
    }
    return result;
}


ColorState::~ColorState(){

}
}
}
