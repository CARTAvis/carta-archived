#include "GeneratorState.h"
#include "ContourGenerateModes.h"
#include "ContourSpacingModes.h"
#include "Data/Util.h"
#include "Globals.h"
#include "MainConfig.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString GeneratorState::DASHED_NEGATIVE = "dashedNegative";
const QString GeneratorState::GENERATE_MODE = "generateMode";
const QString GeneratorState::INTERVAL = "interval";
const QString GeneratorState::LEVEL_COUNT = "levelCount";
const QString GeneratorState::LEVEL_COUNT_MAX = "levelCountMax";
const QString GeneratorState::LEVEL_MIN = "min";
const QString GeneratorState::LEVEL_MAX = "max";
const QString GeneratorState::RANGE_MIN = "rangeMin";
const QString GeneratorState::RANGE_MAX = "rangeMax";
const QString GeneratorState::SPACING_MODE = "spacingMode";
const QString GeneratorState::SPACING_INTERVAL = "spacingInterval";

const int GeneratorState::LEVEL_COUNT_MAX_VALUE = 30;
const double GeneratorState::ERROR_MARGIN = 0.000001;

ContourGenerateModes* GeneratorState::m_generateModes = nullptr;
ContourSpacingModes* GeneratorState::m_spacingModes = nullptr;

using Carta::State::StateInterface;

GeneratorState::GeneratorState():
    m_state( "", ""){
     _initializeSingletons();
    _initializeDefaultState();
}

QString GeneratorState::getGenerateMethod() const {
    QString method = m_state.getValue<QString>( GENERATE_MODE );
    return method;
}

int GeneratorState::getLevelCount() const {
    int levelCount = m_state.getValue<int>(LEVEL_COUNT );
    return levelCount;
}

double GeneratorState::getRangeMax() const {
    double rangeMax = m_state.getValue<double>( RANGE_MAX );
    return rangeMax;
}

double GeneratorState::getRangeMin() const {
    double rangeMax = m_state.getValue<double>( RANGE_MIN );
    return rangeMax;
}

double GeneratorState::getSpacingInterval() const {
    double spacing = m_state.getValue<double>(SPACING_INTERVAL );
    return spacing;
}

QString GeneratorState::getSpacingMethod() const {
    QString method = m_state.getValue<QString>(SPACING_MODE );
    return method;
}

QString GeneratorState::getStateString() const{
    return m_state.toString();
}


void GeneratorState::_initializeDefaultState(){
    m_state.insertValue<QString>(GENERATE_MODE, m_generateModes->getModeDefault());
    m_state.insertValue<double>(RANGE_MIN, 0 );
    m_state.insertValue<double>(RANGE_MAX, 1 );
    m_state.insertValue<bool>(DASHED_NEGATIVE, false );
    m_state.insertValue<int>(LEVEL_COUNT, 3 );
    int contourLevelCountMax = Globals::instance()->mainConfig()->getContourLevelCountMax();
    if ( contourLevelCountMax > 0 ){
       m_state.insertValue<int>(LEVEL_COUNT_MAX, contourLevelCountMax );
    }
    else {
       m_state.insertValue<int>(LEVEL_COUNT_MAX, LEVEL_COUNT_MAX_VALUE );
    }
    m_state.insertValue<QString>(SPACING_MODE, m_spacingModes->getModeDefault());
    m_state.insertValue<double>(SPACING_INTERVAL, 0.25);
}


void GeneratorState::_initializeSingletons( ){
    //Load the available contour generate modes.
    if ( m_generateModes  == nullptr){
        m_generateModes = Util::findSingletonObject<ContourGenerateModes>();
    }
    if ( m_spacingModes == nullptr ){
        m_spacingModes = Util::findSingletonObject<ContourSpacingModes>();
    }
}

bool GeneratorState::isDashedNegative() const {
    bool negsDashed = m_state.getValue<bool>( DASHED_NEGATIVE );
    return negsDashed;
}

void GeneratorState::setDashedNegative( bool useDash ){
    int oldDashed = m_state.getValue<bool>( DASHED_NEGATIVE );
    if ( oldDashed != useDash ){
        m_state.setValue<bool>( DASHED_NEGATIVE, useDash );
    }
}


QString GeneratorState::setGenerateMethod( const QString& method ){
    QString recognizedMethod = m_generateModes->getGenerateMethod( method );
    QString result;
    if ( !recognizedMethod.isEmpty() ){
        QString oldMethod = m_state.getValue<QString>( GENERATE_MODE );
        if ( oldMethod != recognizedMethod ){
            m_state.setValue<QString>( GENERATE_MODE, recognizedMethod );
        }
    }
    else {
        result = "The contour generate method, "+method+" is not supported.";
    }
    return result;
}



QString GeneratorState::setSpacing( const QString& method ){
    QString recognizedMethod = m_spacingModes->getSpacingMethod( method );
    QString result;
    if ( !recognizedMethod.isEmpty() ){
        QString oldMethod = m_state.getValue<QString>( SPACING_MODE );
        if ( oldMethod != recognizedMethod ){
            m_state.setValue<QString>( SPACING_MODE, recognizedMethod );
        }
    }
    else {
        result = "The contour spacing method, "+method+" is not supported.";
    }
    return result;
}

QString GeneratorState::setLevelCount( int count ){
    QString result;
    if ( count >= 1 ){
        int oldLevelCount = m_state.getValue<int>( LEVEL_COUNT );
        if ( oldLevelCount != count ){
            m_state.setValue<int>( LEVEL_COUNT, count );
        }
    }
    else {
        result = "A contour set must have at least one level:"+count;
    }
    return result;
}

QString GeneratorState::setLevelMax( double value ){
    QString result;
    double minValue = m_state.getValue<double>( RANGE_MIN );
    if ( minValue <= value ){
        double oldMax = m_state.getValue<double>( RANGE_MAX );
        if ( qAbs( oldMax - value ) > ERROR_MARGIN ){
            m_state.setValue<double>( RANGE_MAX, value );
        }
    }
    else {
        result = "The maximum contour level must be at least as large as the minimum.";
    }
    return result;
}

QString GeneratorState::setLevelMin( double value ){
    QString result;
    //If the method used to generate levels  is percentiles, the
    //value must be in [0,1].
    double maxValue = m_state.getValue<double>( RANGE_MAX );
    if ( value <= maxValue ){
        double oldMin = m_state.getValue<double>( RANGE_MIN );
        if ( qAbs( oldMin - value ) > ERROR_MARGIN ){
            m_state.setValue<double>( RANGE_MIN, value );
        }
    }
    else {
        result = "The minimum contour level must be less than or equal to the maximum.";
    }
    return result;
}



QString GeneratorState::setSpacingInterval( double interval ){
    QString result;
    if ( interval > 0 ){
        double oldInterval = m_state.getValue<double>( SPACING_INTERVAL );
        if ( qAbs( oldInterval - interval ) > ERROR_MARGIN ){
            m_state.setValue<double>( SPACING_INTERVAL, interval );
        }
    }
    else {
        result = "The contour spacing interval must be positive.";
    }
    return result;
}

void GeneratorState::_updateState( const std::shared_ptr<GeneratorState>& other ){
    QString stateStr = other->getStateString( );
    m_state.setState( stateStr );
}

GeneratorState::~GeneratorState(){
}
}
}
