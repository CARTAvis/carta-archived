#include "ContourControls.h"
#include "ContourGenerateModes.h"
#include "ContourSpacingModes.h"
#include "DataContours.h"
#include "Data/Util.h"
#include "Data/Image/IPercentIntensityMap.h"
#include "Globals.h"
#include "MainConfig.h"
#include "State/UtilState.h"

#include <set>
#include <QtMath>
#include <QDebug>

namespace Carta {

namespace Data {

const QString ContourControls::CLASS_NAME = "ContourControls";
const QString ContourControls::CONTOUR_SETS = "contourSets";
const QString ContourControls::CONTOUR_SET_NAME = "set";
const QString ContourControls::DASHED_NEGATIVE = "dashedNegative";
const QString ContourControls::GENERATE_MODE = "generateMode";
const QString ContourControls::INTERVAL = "interval";
const QString ContourControls::LEVEL_COUNT = "levelCount";
const QString ContourControls::LEVEL_COUNT_MAX = "levelCountMax";
const QString ContourControls::LEVEL_LIST = "levels";
const QString ContourControls::LEVEL_MIN = "min";
const QString ContourControls::LEVEL_MAX = "max";
const QString ContourControls::LEVEL_SEPARATOR = ";";
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

using Carta::State::UtilState;
using Carta::State::StateInterface;
using Carta::State::ObjectManager;

bool ContourControls::m_registered =
        ObjectManager::objectManager()->registerClass ( CLASS_NAME, new ContourControls::Factory());



ContourControls::ContourControls( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ),
    m_percentIntensityMap( nullptr ),
    m_stateData( UtilState::getLookup(path, StateInterface::STATE_DATA)){
     _initializeSingletons();
    _initializeDefaultState();
    _initializeCallbacks();
}

void ContourControls::_addContourSet( const std::vector<double>& levels,
        const QString& contourSetName ){
    int count = levels.size();
    bool levelChanged = false;
    if ( count > 0 ){
        std::set<Contour> contours;
        for ( int i = 0; i < count; i++ ){
            Contour contour;
            contour.setLevel( levels[i], &levelChanged );
            contours.insert( contour );
        }
        ObjectManager* objMan = ObjectManager::objectManager();
        std::shared_ptr<DataContours> dataContours(objMan->createObject<DataContours>());
        dataContours->setContours( contours );
        dataContours->setName( contourSetName );
        m_dataContours.insert( dataContours );
        _updateContourSetState();
    }
}

void ContourControls::_clearContours(){
    ObjectManager* objMan = ObjectManager::objectManager();
    for ( std::set<shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
                it != m_dataContours.end(); it++ ){
        QString contourId = (*it)->getId();
        objMan->removeObject( contourId );
    }
}

QString ContourControls::deleteContourSet( const QString& contourSetName ){
    QString result;
    bool foundSet = false;
    std::set<shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
    while ( it != m_dataContours.end() ){
           if ( (*it)->getName() == contourSetName ){
               foundSet = true;
               ObjectManager* objMan = ObjectManager::objectManager();
               QString id = (*it)->getId();
               m_dataContours.erase(it);
               objMan->removeObject( id );
               _updateContourSetState();
               break;
           }
           it++;
    }
    if ( !foundSet ){
        result = "Unrecognized contour set to delete: "+contourSetName;
    }
    return result;
}

QString ContourControls::generateContourSet( const QString& contourSetName ){
    QString result;
    QString setName = contourSetName.trimmed();
    if ( setName.length() > 0 ){
        bool existingContour = _isDuplicate( contourSetName );
        if ( !existingContour ){
            QString generateMethod = m_state.getValue<QString>(GENERATE_MODE);
            std::set<Contour> contours;
            if ( generateMethod == ContourGenerateModes::MODE_RANGE ){
                 result = _generateRange( setName );
            }
            else if ( generateMethod == ContourGenerateModes::MODE_MINIMUM ){
                result = _generateMinimum( setName );
            }
            else if ( generateMethod == ContourGenerateModes::MODE_PERCENTILE ){
                result = _generatePercentile( setName );
            }
            else {
                result = "Unsupported contour generation mode: "+generateMethod;
            }
        }
        else {
            result = "The contour set: "+contourSetName+" already exists.  Please choose a new name";
        }
    }
    else {
        result = "Please specify the name of the contour set to generate.";
    }
    return result;
}

QString ContourControls::_generateRange( const QString& contourSetName ){
    QString result;
    double maxLevel = m_state.getValue<double>( RANGE_MAX );
    std::vector<double> levels = _getLevelsMinMax(maxLevel, result );
    if ( result.isEmpty() && levels.size() > 0 ){
        _addContourSet( levels, contourSetName );
    }
    else if ( levels.size() == 0 ){
        result = "A countour set could not be generated with the given input parameters.";
    }
    return result;
}


QString ContourControls::_generateMinimum( const QString& contourSetName ){
    QString result;
    double minLevel = m_state.getValue<double>(RANGE_MIN );
    double step = m_state.getValue<double>(SPACING_INTERVAL );
    int count = m_state.getValue<int>(LEVEL_COUNT );
    double maxLevel = minLevel + step * (count - 1);
    QString spacingMode = m_state.getValue<QString>(SPACING_MODE );
    if ( spacingMode == ContourSpacingModes::MODE_LOGARITHM ){
        maxLevel = minLevel + qPow( step, (count-1) );
    }
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


std::vector<double> ContourControls::_getLevels( double minLevel, double maxLevel ) const {
    int count = m_state.getValue<int>( LEVEL_COUNT );
    double step = maxLevel - minLevel;
    if ( count > 2 ){
        step = step / (count - 1 );
    }
    QString spacingMode = m_state.getValue<QString>(SPACING_MODE);
    std::vector<double> levels;
    if ( ( step > 0 && step != 1 ) || count == 1 ){
        for ( int i = 0; i < count; i++ ){
            double increment = i * step;
            double level = minLevel + increment;
            if ( spacingMode == ContourSpacingModes::MODE_LOGARITHM ){
                level = qLn(level) / qLn( step );
            }
            levels.push_back( level );
        }
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
        if ( minLevel <= 0 ){
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


DataContours* ContourControls::_getContour( const QString& setName ) {
    DataContours* target = nullptr;
    for ( std::set<shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
                   it != m_dataContours.end(); it++ ){
       if ( (*it)->getName() == setName ){
           target = ( *it ).get();
           break;
       }
   }
   return target;
}

QString ContourControls::getStateString( const QString& /*sessionId*/, SnapshotType type ) const{
    QString result("");
    if ( type == SNAPSHOT_PREFERENCES ){
        result = m_state.toString();
    }
    else if ( type == SNAPSHOT_DATA ){
        StateInterface dataCopy( m_stateData );
        dataCopy.setValue<QString>( StateInterface::OBJECT_TYPE, CLASS_NAME+StateInterface::STATE_DATA);
        dataCopy.setValue<int>( StateInterface::INDEX, getIndex());
        result = dataCopy.toString();
    }
    return result;
}


void ContourControls::_initializeDefaultState(){
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
    m_state.flushState();
    int contourSetCount = m_dataContours.size();
    m_stateData.insertArray( CONTOUR_SETS, contourSetCount );
    _updateContourSetState();

}



void ContourControls::_initializeCallbacks(){

    addCommandCallback( "deleteLevels", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                std::set<QString> keys = {DataContours::SET_NAME };
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString contourSetName = dataValues[DataContours::SET_NAME];
                QString result = deleteContourSet( contourSetName );
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "generateLevels", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {DataContours::SET_NAME, INTERVAL, LEVEL_MIN, LEVEL_MAX};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString contourSetName = dataValues[DataContours::SET_NAME];

            //Make sure any new interval, level min and level max value is valid
            //before adding the contour.
            QString result;
            bool validDouble = false;
            double spaceInterval = dataValues[INTERVAL].toDouble(&validDouble);
            if ( validDouble ){
                result = setSpacingInterval( spaceInterval );
                if ( result.isEmpty() ){
                    double levelMin = dataValues[LEVEL_MIN].toDouble(&validDouble);
                    if ( validDouble ){
                        result = setLevelMin( levelMin );
                        if ( result.isEmpty() ){
                            double levelMax = dataValues[LEVEL_MAX].toDouble(&validDouble);
                            if ( validDouble ){
                                result = setLevelMax( levelMax );
                                if ( result.isEmpty() ){
                                    //If the are all valid, generate the contour set.
                                    result = generateContourSet( contourSetName );
                                }
                            }
                        }
                    }
                }
            }
            else {
                result = "Please make sure the contour spacing interval is a valid number: "+dataValues[INTERVAL];
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setAlpha", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = { Util::ALPHA, CONTOUR_SET_NAME, LEVEL_LIST };
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            bool validInt = false;
            QString result;
            int alpha = dataValues[Util::ALPHA].toInt(&validInt);
            if ( validInt ){
                QString setName = dataValues[CONTOUR_SET_NAME ];
                QString levelStr = dataValues[LEVEL_LIST];
                std::vector<double> levels = Util::string2VectorDouble( levelStr, LEVEL_SEPARATOR );
                result = setAlpha( setName, levels, alpha );
            }
            else {
                result = "Contour transparency level must be an integer: "+dataValues[Util::ALPHA];
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setColor", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = { Util::RED, Util::GREEN, Util::BLUE, CONTOUR_SET_NAME, LEVEL_LIST };
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString result;
            bool validRed = false;
            int red = dataValues[Util::RED].toInt(&validRed);
            bool validGreen = false;
            int green = dataValues[Util::GREEN].toInt(&validGreen);
            bool validBlue = false;
            int blue = dataValues[Util::BLUE].toInt(&validBlue);
            if ( validRed && validGreen && validBlue ){
                QString setName = dataValues[CONTOUR_SET_NAME ];
                QString levelStr = dataValues[LEVEL_LIST];
                std::vector<double> levels = Util::string2VectorDouble( levelStr, LEVEL_SEPARATOR );
                QStringList errorList = setColor( setName, levels, red, green, blue );
                result = errorList.join(",");
            }
            else {
                result = "Contour colors must be integer(s): "+dataValues[Util::RED]+","+dataValues[Util::GREEN]+
                        ","+dataValues[Util::BLUE];
            }
            Util::commandPostProcess( result );
            return result;
        });

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
                std::set<QString> keys = {INTERVAL};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                bool validDouble = false;
                QString result;
                double spaceInterval = dataValues[INTERVAL].toDouble(&validDouble);
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

    addCommandCallback( "setStyle", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = { Contour::STYLE, CONTOUR_SET_NAME, LEVEL_LIST };
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString style = dataValues[Contour::STYLE];
        QString setName = dataValues[CONTOUR_SET_NAME];
        QString levelStr = dataValues[LEVEL_LIST];
        std::vector<double> levels = Util::string2VectorDouble( levelStr, LEVEL_SEPARATOR );
        QString result;
        if ( levels.size() > 0 ){
            result = setLineStyle( setName, levels, style );
        }
        else {
            result = "Contour level(s) must be numbers:"+levelStr;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLevels", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = { CONTOUR_SET_NAME, LEVEL_LIST };
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString setName = dataValues[CONTOUR_SET_NAME];
            QString levelStr = dataValues[LEVEL_LIST];
            std::vector<double> levels = Util::string2VectorDouble( levelStr, LEVEL_SEPARATOR );
            QString result = setLevels( setName, levels);
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
            std::set<QString> keys = {LEVEL_MIN};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            bool validDouble = false;
            QString result;
            double levelMin = dataValues[*keys.begin()].toDouble(&validDouble);
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
                double levelMax = dataValues[*keys.begin()].toDouble(&validDouble);
                if ( validDouble ){
                    result = setLevelMax( levelMax );
                }
                else {
                    result = "Contour maximum level must be a number: "+dataValues[*keys.begin()];
                }
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setThickness", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = { Util::PEN_WIDTH, CONTOUR_SET_NAME, LEVEL_LIST };
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            bool validDouble = false;
            QString result;
            double thickness = dataValues[Util::PEN_WIDTH].toDouble(&validDouble);
            if ( validDouble ){
                QString setName = dataValues[CONTOUR_SET_NAME ];
                QString levelStr = dataValues[LEVEL_LIST];
                std::vector<double> levels = Util::string2VectorDouble( levelStr, LEVEL_SEPARATOR );
                result = setThickness( setName, levels, thickness );
            }
            else {
                result = "Contour thickness must be an integer: "+dataValues[Util::PEN_WIDTH];
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setVisibility", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = { Util::VISIBLE, CONTOUR_SET_NAME, LEVEL_LIST };
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString visibleStr = dataValues[Util::VISIBLE];
            bool validBool = false;
            bool visible = Util::toBool( visibleStr, &validBool );
            QString result;
            if ( validBool ){
                QString setName = dataValues[CONTOUR_SET_NAME];
                QString levelListStr = dataValues[LEVEL_LIST];
                std::vector<double> levels = Util::string2VectorDouble( levelListStr, LEVEL_SEPARATOR );
                if ( levels.size() > 0 ){
                    result = setVisibility( setName, levels, visible );
                }
                else {
                    result = "Contour level(s) must be numbers: "+levelListStr;
                }
            }
            else {
                result = "Contour visibility must be true/false: "+visibleStr;
            }
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



void ContourControls::resetStateData( const QString& state ){
    StateInterface dataState( "");
    dataState.setState( state );
    int contourCount = dataState.getArraySize( CONTOUR_SETS );
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    for ( int i = 0; i < contourCount; i++ ){
        QString lookup = UtilState::getLookup( CONTOUR_SETS, i );
        QString contourSetState = dataState.toString( lookup );
        std::shared_ptr<DataContours> dataContours(objMan->createObject<DataContours>());
        dataContours->resetState( contourSetState );
        m_dataContours.insert( dataContours );
    }
    _updateContourSetState();
}

QString ContourControls::setAlpha( const QString& contourName,
        std::vector<double>& levels, int transparency ){
    QString result;
    DataContours* target = _getContour( contourName );
    if ( target != nullptr ){
        result = target->setAlpha( levels, transparency );
        if ( result.isEmpty() ){
            _updateContourSetState();
        }
    }
    else {
        result = "Unrecognized contour set: "+ contourName;
    }
    return result;
}

QStringList ContourControls::setColor( const QString& contourName,
        std::vector<double>& levels, int red, int green, int blue ){
    QStringList result;
    DataContours* target = _getContour( contourName );
    if ( target != nullptr ){
        result = target->setColor( levels, red, green, blue );
        if ( result.isEmpty() ){
            _updateContourSetState();
        }
    }
    else {
        result.append( "Unrecognized contour set: "+ contourName);
    }
    return result;
}

void ContourControls::setDashedNegative( bool useDash ){
    int oldDashed = m_state.getValue<bool>( DASHED_NEGATIVE );
    if ( oldDashed != useDash ){
        m_state.setValue<bool>( DASHED_NEGATIVE, useDash );
        m_state.flushState();
    }
}

void ContourControls::_setDrawContours( std::shared_ptr<DataContours> contours ){
    m_drawContours = contours;
    std::set<Contour> drawContours;
    for ( std::set<shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
            it != m_dataContours.end(); it++ ){
        std::set<Contour> setContours = (*it)->_getContours();
        drawContours.insert( setContours.begin(), setContours.end());
    }
    if ( m_drawContours ){
        m_drawContours->setContours( drawContours );
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
    //If the method used to generate levels  is percentiles, the
    //value must be in [0,1].
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

QString ContourControls::setLevels( const QString& contourName, std::vector<double>& levels ){
    QString result;
    DataContours* target = _getContour( contourName );
    if ( target != nullptr ){
        bool levelCount = target->getLevelCount();
        bool levelsChanged = target->setLevels( levels );
        if ( levelsChanged ){
            _updateContourSetState();
            if ( levelCount != levels.size() ){
                //Note:  A state refresh here is needed because the user could add a duplicate contour level in
                    //the UI.  The code will refuse to accept it so the state will not officially change and get flushed
                    //to the UI.  However, the UI needs the old state so it can remove the duplicate the user added.
                    CartaObject::refreshState();
                    m_state.flushState();
            }
        }
    }
    else {
        result = "Unrecognized contour set: "+ contourName;
    }
    return result;
}

QString ContourControls::setLineStyle( const QString& contourName, std::vector<double>& levels, const QString& lineStyle ){
    QString result;
    DataContours* target = _getContour( contourName );
    if ( target != nullptr ){
        result = target->setLineStyle( levels, lineStyle );
        if ( result.isEmpty() ){
            _updateContourSetState();
        }
    }
    else {
        result = "Unrecognized contour set: "+ contourName;
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

QString ContourControls::setThickness( const QString& contourName,
        std::vector<double>& levels, double thickness ){
    QString result;
    DataContours* target = _getContour( contourName );
    if ( target != nullptr ){
        result = target->setThickness( levels, thickness );
        if ( result.isEmpty() ){
            _updateContourSetState();
        }
    }
    else {
        result = "Unrecognized contour set: "+ contourName;
    }
    return result;
}

QString ContourControls::setVisibility( const QString& contourName,
        std::vector<double>& levels, bool visible ){
    QString result;
    DataContours* target = _getContour( contourName );
    if ( target != nullptr ){
        result = target->setVisibility( levels, visible );
        if ( result.isEmpty() ){
            _updateContourSetState();
        }
    }
    else {
        result = "Unrecognized contour set: "+ contourName;
    }
    return result;
}

void ContourControls::_updateContourSetState(){
    int contourSetCount = m_dataContours.size();
    m_stateData.resizeArray( CONTOUR_SETS, contourSetCount );
    std::set<Contour> drawContours;
    int i = 0;
    for ( std::set<shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
            it != m_dataContours.end(); it++ ){
        QString lookup = Carta::State::UtilState::getLookup( CONTOUR_SETS, i );
        Carta::State::StateInterface dataState = (*it)->_getState();
        QString contourState = dataState.toString();
        m_stateData.setObject( lookup, contourState);
        std::set<Contour> setContours = (*it)->_getContours();
        for ( std::set<Contour>::iterator contourIt = setContours.begin();
                contourIt != setContours.end(); contourIt++ ){
            if ( (*contourIt).isVisible() ){
                drawContours.insert( (*contourIt) );
            }
        }
        i++;
    }

    m_stateData.flushState();
    if ( m_drawContours ){
        m_drawContours->setContours( drawContours );
        emit drawContoursChanged();
    }
}


ContourControls::~ContourControls(){
    _clearContours();
}
}
}
