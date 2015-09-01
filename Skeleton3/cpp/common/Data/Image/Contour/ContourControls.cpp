#include "ContourControls.h"
#include "ContourGenerateModes.h"
#include "ContourSpacingModes.h"
#include "DataContours.h"
#include "Data/Util.h"
#include "Data/Image/IPercentIntensityMap.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"

#include <set>
#include <QtMath>
#include <QDebug>

namespace Carta {

namespace Data {

const QString ContourControls::CLASS_NAME = "ContourControls";
const QString ContourControls::CONTOUR_SETS = "contourSets";
const QString ContourControls::DASHED_NEGATIVE = "dashedNegative";
const QString ContourControls::GENERATE_MODE = "generateMode";
const QString ContourControls::LEVEL_COUNT = "levelCount";
const QString ContourControls::LEVEL_COUNT_MAX = "levelCountMax";
const QString ContourControls::RANGE_MIN = "rangeMin";
const QString ContourControls::RANGE_MAX = "rangeMax";
const QString ContourControls::SPACING_MODE = "spacingMode";
const QString ContourControls::SPACING_INTERVAL = "spacingInterval";

const int ContourControls::LEVEL_COUNT_MAX_VALUE = 30;
const double ContourControls::ERROR_MARGIN = 0.000001;

ContourGenerateModes* ContourControls::m_generateModes = nullptr;
ContourSpacingModes* ContourControls::m_spacingModes = nullptr;

class ContourControls::Factory : public Carta::State::CartaObjectFactory {

    public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new ContourControls (path, id);
        }
    };

bool ContourControls::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new ContourControls::Factory());

ContourControls::ContourControls( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ),
    m_percentIntensityMap( nullptr ){
     _initializeSingletons();
    _initializeDefaultState();
    _initializeCallbacks();
}

void ContourControls::_addContourSet( const std::vector<double>& levels,
        const QString& contourSetName ){
    int count = levels.size();
    if ( count > 0 ){
        std::set<Contour> contours;
        for ( int i = 0; i < count; i++ ){
            Contour contour;
            contour.setLevel( levels[i] );
            contours.insert( contour );
        }
        Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
        std::shared_ptr<DataContours> dataContours(objMan->createObject<DataContours>());
        dataContours->setContours( contours );
        dataContours->setName( contourSetName );
        m_dataContours.insert( dataContours );
        _updateContourSetState();
        m_state.flushState();
    }
}


QString ContourControls::generateContourSet( const QString& contourSetName ){
    QString result;
    bool existingContour = _isDuplicate( contourSetName );
    if ( !existingContour ){
        QString generateMethod = m_state.getValue<QString>(GENERATE_MODE);
        std::set<Contour> contours;
        if ( generateMethod == ContourGenerateModes::MODE_RANGE ){
             result = _generateRange( contourSetName );
        }
        else if ( generateMethod == ContourGenerateModes::MODE_MINIMUM ){
            result = _generateMinimum( contourSetName );
        }
        else if ( generateMethod == ContourGenerateModes::MODE_PERCENTILE ){
            result = _generatePercentile( contourSetName );
        }
        else {
            result = "Unsupported contour generation mode: "+generateMethod;
        }
        //emit contourAdded( contours );
    }
    else {
        result = "The contour set: "+contourSetName+" already exists.  Please choose a new name";
    }
    return result;
}



std::vector<double> ContourControls::_getLevels( double minLevel, double maxLevel ) const {
    int count = m_state.getValue<int>( LEVEL_COUNT );
    double step = maxLevel - minLevel;
    if ( count > 2 ){
        step = step / (count - 1 );
    }

    std::vector<double> levels;
    for ( int i = 0; i < count; i++ ){
        double increment = i * step;
        double level = minLevel + increment;
        levels.push_back( level );
    }
    return levels;
}

std::vector<double> ContourControls::_getLevelsMinMax( double max, QString& error ) const {
    double minLevel = m_state.getValue<double>(RANGE_MIN );
    double maxLevel = max;
    QString spacingMode = m_state.getValue<QString>( SPACING_MODE );
    std::vector<double> levels;
    bool validRange = true;
    if ( spacingMode == ContourSpacingModes::MODE_LOGARITHM ){
        if ( minLevel > 0 && maxLevel > 0 ){
            double interval = m_state.getValue<double>( SPACING_INTERVAL );
            if ( interval > 0 && interval != 1 ){
                minLevel = qLn( minLevel ) / qLn( interval );
                maxLevel = qLn( maxLevel ) / qLn( interval );
            }
            else {
                validRange = false;
                error = "Cannot calculate a logarithm with base="+QString::number( interval );
            }
        }
        else {
            validRange = false;
            error = "Logarithm is not defined on the interval ["+QString::number( minLevel)+
                    ","+QString::number( maxLevel )+"].";
        }
    }
    if ( validRange ){
        levels = _getLevels( minLevel, maxLevel );
    }
    return levels;
}

QString ContourControls::_generateRange( const QString& contourSetName ){
    QString result;
    double maxLevel = m_state.getValue<double>( RANGE_MAX );
    std::vector<double> levels = _getLevelsMinMax(maxLevel, result );
    if ( result.isEmpty() ){
        _addContourSet( levels, contourSetName );
    }
    return result;
}


QString ContourControls::_generateMinimum( const QString& contourSetName ){
    QString result;
    double minLevel = m_state.getValue<double>(RANGE_MIN );
    double step = m_state.getValue<double>(SPACING_INTERVAL );
    int count = m_state.getValue<int>(LEVEL_COUNT );
    double maxLevel = minLevel + step * (count - 1);
    std::vector<double> levels = _getLevelsMinMax( maxLevel, result );
    if ( result.isEmpty() ){
        _addContourSet( levels, contourSetName );
    }
    return result;
}

QString ContourControls::_generatePercentile( const QString& contourSetName ){
    QString result;
    if ( m_percentIntensityMap != nullptr ){
        double maxLevel = m_state.getValue<double>( RANGE_MAX );
        //First get the levels as percentiles.
        std::vector<double> percentileLevels = _getLevelsMinMax( maxLevel, result );

        //Map the percentiles to intensities
        if ( result.isEmpty() ){
            int percentCount = percentileLevels.size();
            std::vector<double> levels( percentCount );
            bool validIntensities = false;
            for ( int i = 0; i < percentCount; i++ ){
                validIntensities = m_percentIntensityMap->getIntensity( percentileLevels[i], &levels[i] );
                if ( !validIntensities ){
                    break;
                }
            }
            if ( !validIntensities ){
               result = "Could not generate contour based on percentiles";
            }
            else {
                _addContourSet( levels, contourSetName );
            }
        }
    }
    else {
        result = "Generating contours as percentiles is not supported";
    }
    return result;
}

void ContourControls::_initializeDefaultState(){
    m_state.insertValue<QString>(GENERATE_MODE, m_generateModes->getModeDefault());
    m_state.insertValue<double>(RANGE_MIN, 0 );
    m_state.insertValue<double>(RANGE_MAX, 1 );
    m_state.insertValue<bool>(DASHED_NEGATIVE, false );
    m_state.insertValue<int>(LEVEL_COUNT, 3 );
    m_state.insertValue<int>(LEVEL_COUNT_MAX, LEVEL_COUNT_MAX_VALUE );
    m_state.insertValue<QString>(SPACING_MODE, m_spacingModes->getModeDefault());
    m_state.insertValue<double>(SPACING_INTERVAL, 0.25);
    int contourSetCount = m_dataContours.size();
    m_state.insertArray( CONTOUR_SETS, contourSetCount );
    _updateContourSetState();
    m_state.flushState();
}



void ContourControls::_initializeCallbacks(){

    addCommandCallback( "setDashedNegative", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {"dashed"};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString dashedStr = dataValues[*keys.begin()];
            bool validBool = false;
            bool dashedNegatives = Util::toBool( dashedStr, &validBool );
            QString result;
            if ( validBool ){
                setDashedNegative( dashedNegatives );
            }
            else {
                result = "Whether or not to draw negative contours dashed must be true/false.";
            }
            Util::commandPostProcess( result );
            return result;
        });

    //Set the method for generating contour levels.
    addCommandCallback( "setGenerateMethod", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {"method"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString method = dataValues[*keys.begin()];
        QString result = setGenerateMethod( method );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setInterval", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
                std::set<QString> keys = {"interval"};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                bool validDouble = false;
                QString result;
                double spaceInterval = dataValues[*keys.begin()].toDouble(&validDouble);
                if ( validDouble ){
                    result = setSpacingInterval( spaceInterval );
                }
                else {
                    result = "Contour spacing interval must be a number: "+dataValues[*keys.begin()];
                }
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setSpacing", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {"method"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString method = dataValues[*keys.begin()];
        QString result = setSpacing( method );
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLevelCount", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {"count"};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        bool validInt = false;
        QString result;
        int levelCount = dataValues[*keys.begin()].toInt(&validInt);
        if ( validInt ){
            result = setLevelCount( levelCount );
        }
        else {
            result = "Contour level count must be an integer: "+dataValues[*keys.begin()];
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLevelMin", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {"min"};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            bool validDouble = false;
            QString result;
            int levelMin = dataValues[*keys.begin()].toDouble(&validDouble);
            if ( validDouble ){
                result = setLevelMin( levelMin );
            }
            else {
                result = "Contour minimum level must be a number: "+dataValues[*keys.begin()];
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setLevelMax", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
                std::set<QString> keys = {"max"};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                bool validDouble = false;
                QString result;
                int levelMax = dataValues[*keys.begin()].toDouble(&validDouble);
                if ( validDouble ){
                    result = setLevelMax( levelMax );
                }
                else {
                    result = "Contour maximum level must be a number: "+dataValues[*keys.begin()];
                }
                Util::commandPostProcess( result );
                return result;
            });

    //Set the method for generating contour levels.
    addCommandCallback( "generateLevels", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {DataContours::SET_NAME };
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString contourSetName = dataValues[DataContours::SET_NAME];
        QString result = generateContourSet( contourSetName );
        Util::commandPostProcess( result );
        return result;
    });

}
void ContourControls::_initializeSingletons( ){
    //Load the available contour generate modes.
    if ( m_generateModes  == nullptr){
        m_generateModes = Util::findSingletonObject<ContourGenerateModes>();
    }
    if ( m_spacingModes == nullptr ){
        m_spacingModes = Util::findSingletonObject<ContourSpacingModes>();
    }
}

bool ContourControls::_isDuplicate( const QString& contourSetName ) const {
    bool duplicateSet = false;
    for ( std::set<shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
                it != m_dataContours.end(); it++ ){
        if ( (*it)->getName() == contourSetName ){
            duplicateSet = true;
            break;
        }
    }
    return duplicateSet;
}

void ContourControls::setDashedNegative( bool useDash ){
    int oldDashed = m_state.getValue<bool>( DASHED_NEGATIVE );
    if ( oldDashed != useDash ){
        m_state.setValue<bool>( DASHED_NEGATIVE, useDash );
        m_state.flushState();
    }
}


QString ContourControls::setGenerateMethod( const QString& method ){
    QString recognizedMethod = m_generateModes->getGenerateMethod( method );
    QString result;
    if ( !recognizedMethod.isEmpty() ){
        QString oldMethod = m_state.getValue<QString>( GENERATE_MODE );
        if ( oldMethod != recognizedMethod ){
            m_state.setValue<QString>( GENERATE_MODE, recognizedMethod );
            m_state.flushState();
        }
    }
    else {
        result = "The contour generate method, "+method+" is not supported.";
    }
    return result;
}

void ContourControls::setPercentIntensityMap( IPercentIntensityMap* mapper ){
    m_percentIntensityMap = mapper;
}

QString ContourControls::setSpacing( const QString& method ){
    QString recognizedMethod = m_spacingModes->getSpacingMethod( method );
    QString result;
    if ( !recognizedMethod.isEmpty() ){
        QString oldMethod = m_state.getValue<QString>( SPACING_MODE );
        if ( oldMethod != recognizedMethod ){
            m_state.setValue<QString>( SPACING_MODE, recognizedMethod );
            m_state.flushState();
        }
    }
    else {
        result = "The contour spacing method, "+method+" is not supported.";
    }
    return result;
}

QString ContourControls::setLevelCount( int count ){
    QString result;
    if ( count >= 1 ){
        int oldLevelCount = m_state.getValue<int>( LEVEL_COUNT );
        if ( oldLevelCount != count ){
            m_state.setValue<int>( LEVEL_COUNT, count );
            m_state.flushState();
        }
    }
    else {
        result = "A contour set must have at least one level:"+count;
    }
    return result;
}

QString ContourControls::setLevelMax( double value ){
    QString result;
    double minValue = m_state.getValue<double>( RANGE_MIN );
    if ( minValue <= value ){
        double oldMax = m_state.getValue<double>( RANGE_MAX );
        if ( qAbs( oldMax - value ) > ERROR_MARGIN ){
            m_state.setValue<double>( RANGE_MAX, value );
            m_state.flushState();
        }
    }
    else {
        result = "The maximum contour level must be at least as large as the minimum.";
    }
    return result;
}

QString ContourControls::setLevelMin( double value ){
    QString result;
    double maxValue = m_state.getValue<double>( RANGE_MAX );
    if ( value <= maxValue ){
        double oldMin = m_state.getValue<double>( RANGE_MIN );
        if ( qAbs( oldMin - value ) > ERROR_MARGIN ){
            m_state.setValue<double>( RANGE_MIN, value );
            m_state.flushState();
        }
    }
    else {
        result = "The minimum contour level must be less than or equal to the maximum.";
    }
    return result;
}

QString ContourControls::setSpacingInterval( double interval ){
    QString result;
    if ( interval > 0 ){
        double oldInterval = m_state.getValue<double>( SPACING_INTERVAL );
        if ( qAbs( oldInterval - interval ) > ERROR_MARGIN ){
            m_state.setValue<double>( SPACING_INTERVAL, interval );
            m_state.flushState();
        }
    }
    else {
        result = "The contour spacing interval must be positive.";
    }
    return result;
}

void ContourControls::_updateContourSetState(){
    int contourSetCount = m_dataContours.size();
    m_state.resizeArray( CONTOUR_SETS, contourSetCount );
    int i = 0;
    for ( std::set<shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
            it != m_dataContours.end(); it++ ){
        QString lookup = Carta::State::UtilState::getLookup( CONTOUR_SETS, i );
        Carta::State::StateInterface dataState = (*it)->_getState();
        QString contourState = dataState.toString( /*DataContours::CONTOURS*/);
        m_state.setObject( lookup, contourState);
        i++;
    }
}

ContourControls::~ContourControls(){

}
}
}
