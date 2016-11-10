#include "ContourControls.h"
#include "ContourGenerateModes.h"
#include "ContourSpacingModes.h"
#include "GeneratorState.h"
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

const QString ContourControls::LEVEL_LIST = "levels";

const QString ContourControls::LEVEL_SEPARATOR = ";";


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
    m_generatorState( new GeneratorState() ),
    m_stateData( UtilState::getLookup(path, StateInterface::STATE_DATA)){
    _initializeDefaultState();
    _initializeCallbacks();
}



void ContourControls::_addContourSet( const std::vector<double>& levels,
        const QString& contourSetName ){
    int count = levels.size();
    bool levelChanged = false;
    if ( count > 0 ){

        //Make the individual contours in the set.
        std::set<Contour> contours;
        for ( int i = 0; i < count; i++ ){
            Contour contour;
            contour.setLevel( levels[i], &levelChanged );
            contours.insert( contour );
        }

        //See if there is an existing contour with that name.
        DataContours* dataContours = _getContour( contourSetName );

        //Create a new contour set if there is not an existing one.
        if ( !dataContours ){
            ObjectManager* objMan = ObjectManager::objectManager();
            dataContours = objMan->createObject<DataContours>();
            dataContours->setName( contourSetName );
            std::shared_ptr<DataContours> dContours(dataContours );
            m_dataContours.insert( dContours );
            m_percentIntensityMap->addContourSet( dContours );

        }

        //Reset the contours
        dataContours->setContours( contours );
        dataContours->_updateGeneratorState( m_generatorState );

        _updateContourSetState();
    }
}


QString ContourControls::deleteContourSet( const QString& contourSetName ){
    QString result;
    bool foundSet = false;
    std::set<std::shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
    while ( it != m_dataContours.end() ){
       if ( (*it)->getName() == contourSetName ){
           foundSet = true;
           //Reset the draw contour if it is the one we are removing.
           if ( m_drawContours ){
               if ( m_drawContours->getName() == contourSetName ){
                   if ( m_dataContours.size() > 1 ){
                       m_drawContours = (*m_dataContours.begin());
                   }
                   else {
                       m_drawContours.reset();
                   }
               }
           }
           m_percentIntensityMap->removeContourSet( (*it) );
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
        QString generateMethod = m_generatorState->getGenerateMethod();
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
        result = "Please specify the name of the contour set to generate.";
    }
    return result;
}

QString ContourControls::_generateRange( const QString& contourSetName ){
    QString result;
    double maxLevel = m_generatorState->getRangeMax();
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
    double minLevel = m_generatorState->getRangeMin();
    double step = m_generatorState->getSpacingInterval();
    int count = m_generatorState->getLevelCount();
    double maxLevel = minLevel + step * (count - 1);
    std::vector<double> levels = _getLevelsMinMax( maxLevel, result );
    if ( result.isEmpty() && levels.size() > 0 ){
        _addContourSet( levels, contourSetName );
    }
    else if ( levels.size() == 0 ){
        result = "A contour set with the given input parameters could not be generated.";
    }
    return result;
}

QString ContourControls::_generatePercentile( const QString& contourSetName ){
    QString result;
    if ( m_percentIntensityMap != nullptr ){
        double minLevel = m_generatorState->getRangeMin();
        double maxLevel = m_generatorState->getRangeMax();
        if ( minLevel < 0 || maxLevel > 100 ){
            result = "Contour min and max percentiles must be in [0,100].";
        }
        else {
            //First get the levels as percentiles.
            std::vector<double> percentileLevels = _getLevelsMinMax( maxLevel, result );

            //Map the percentiles to intensities
            if ( result.isEmpty() ){
                int percentCount = percentileLevels.size();
                for ( int i = 0; i < percentCount; i++ ){
                    percentileLevels[i] = percentileLevels[i] / 100;
                }

                std::vector<std::pair<int,double> > intensities = m_percentIntensityMap->getIntensity( percentileLevels );
                std::vector<double> levels;
                int intensityCount = intensities.size();

                for ( int i = 0; i < intensityCount; i++ ){
                    if ( intensities[i].first >= 0 ){
                        levels.push_back( intensities[i].second );
                    }
                }
                if ( levels.size() == 0 ){
                   result = "Could not generate contour based on percentiles";
                }
                else {
                    _addContourSet( levels, contourSetName );
                }
            }
        }
    }
    else {
        result = "Generating contours as percentiles is not supported";
    }
    return result;
}

double ContourControls::_getStepSize( double max, double min, int count ) const{
    double step = max - min;
    if ( count > 2 ){
        step = step / (count - 1);
    }
    return step;
}

std::vector<double> ContourControls::_getLevels( double minLevel, double maxLevel ) const {
    int count = m_generatorState->getLevelCount();
    QString spacingMode = m_generatorState->getSpacingMethod();
    std::vector<double> levels;
    if ( spacingMode != ContourSpacingModes::MODE_LOGARITHM ){
        double step = _getStepSize( maxLevel, minLevel, count );
        for ( int i = 0; i < count; i++ ){
            double increment = i * step;
            double level = minLevel + increment;
            levels.push_back( level );
        }
    }
    //Logarithmic levels.
    else {
        //Map the min and max to a power interval.
        double powMin = qExp( minLevel );
        double powMax = qExp( maxLevel );
        if ( powMin > 0 && powMax > 0 ){
            double step = _getStepSize( powMax, powMin, count );
            if ( step > 0 && !std::isinf( step) ){
                for ( int i = 0; i < count; i++ ){
                    double increment = i * step;
                    double level = powMin + increment;
                    level = qLn(level);
                    levels.push_back( level );
                }
            }
        }
    }
    return levels;
}

std::vector<double> ContourControls::_getLevelsMinMax( double max , QString& error) const {
    double minLevel = m_generatorState->getRangeMin();
    double maxLevel = max;
    std::vector<double> levels = _getLevels( minLevel, maxLevel );
    if ( levels.size() == 0 ){
        error = "No contour levels could be generated with the given parameters.";
    }
    return levels;
}


DataContours* ContourControls::_getContour( const QString& setName ) {
    DataContours* target = nullptr;
    for ( std::set<std::shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
                   it != m_dataContours.end(); it++ ){
       if ( (*it)->getName() == setName ){
           target = ( *it ).get();
           break;
       }
   }
   return target;
}


void ContourControls::_initializeDefaultState(){
    QString generateState = m_generatorState->getStateString();
    m_state.setState( generateState );

    int contourSetCount = m_dataContours.size();
    m_stateData.insertArray( CONTOUR_SETS, contourSetCount );
    _updateContourSetState();
}



void ContourControls::_initializeCallbacks(){

    addCommandCallback( "deleteLevels", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                std::set<QString> keys = { Util::NAME };
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString contourSetName = dataValues[Util::NAME];
                QString result = deleteContourSet( contourSetName );
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "generateLevels", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
            std::set<QString> keys = {Util::NAME, GeneratorState::INTERVAL,
                    GeneratorState::LEVEL_MIN, GeneratorState::LEVEL_MAX};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString contourSetName = dataValues[Util::NAME];

            //Make sure any new interval, level min and level max value is valid
            //before adding the contour.
            QString result;
            bool validDouble = false;
            double spaceInterval = dataValues[GeneratorState::INTERVAL].toDouble(&validDouble);
            //The spacing interval can be skipped, if we are doing a range, for example, and have
            //already set the level.
            if ( validDouble ){
                result = setSpacingInterval( spaceInterval );
            }
            if ( result.isEmpty() ){
                //Set the min and the max.
                double levelMin = dataValues[GeneratorState::LEVEL_MIN].toDouble(&validDouble);
                if ( validDouble ){
                    if ( !m_generatorState->isGenerateMethodMinimum() ){
                        double levelMax = dataValues[GeneratorState::LEVEL_MAX].toDouble(&validDouble);
                        if ( validDouble ){
                            result = setLevelMinMax( levelMin, levelMax );
                        }
                    }
                    else {
                        result = setLevelMin( levelMin );
                    }
                    if ( result.isEmpty() ){
                        //If the are all valid, generate the contour set.
                        result = generateContourSet( contourSetName );
                    }
                }
                else {
                    result = "The minimum contour level must be numeric.";
                }
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
                bool validDouble = false;
                std::vector<double> levels = Util::string2VectorDouble( levelStr, &validDouble, LEVEL_SEPARATOR );
                if ( validDouble ){
                    result = setAlpha( setName, levels, alpha );
                }
                else {
                    result = "Could not set contour transparency, levels must be numbers: "+params;
                }
            }
            else {
                result = "Contour transparency must be an integer: "+dataValues[Util::ALPHA];
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
                bool levelError = false;
                std::vector<double> levels = Util::string2VectorDouble( levelStr, &levelError, LEVEL_SEPARATOR );
                if ( !levelError ){
                    QStringList errorList = setColor( setName, levels, red, green, blue );
                    result = errorList.join(",");
                }
                else {
                    result = "Could not set color; contour levels must numbers: "+levelStr;
                }
            }
            else {
                result = "Contour colors must be integer(s): "+dataValues[Util::RED]+","+dataValues[Util::GREEN]+
                        ","+dataValues[Util::BLUE];
            }
            Util::commandPostProcess( result );
            return result;
        });

    //Set the selected contour set.
    addCommandCallback( "selectContourSet", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
        std::set<QString> keys = {Util::NAME};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString contourSetName = dataValues[Util::NAME];
        selectContourSet( contourSetName );
        return "";
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
                std::set<QString> keys = {GeneratorState::INTERVAL};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                bool validDouble = false;
                QString result;
                double spaceInterval = dataValues[GeneratorState::INTERVAL].toDouble(&validDouble);
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
        std::set<QString> keys = { Util::STYLE, CONTOUR_SET_NAME, LEVEL_LIST };
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString style = dataValues[Util::STYLE];
        QString setName = dataValues[CONTOUR_SET_NAME];
        QString levelStr = dataValues[LEVEL_LIST];
        QString result;
        bool validLevels = false;
        std::vector<double> levels = Util::string2VectorDouble( levelStr, &validLevels, LEVEL_SEPARATOR );
        if ( validLevels ){
            result = setLineStyle( setName, levels, style );
        }
        else {
            result = "Style could not be set because contour levels were invalid:"+levelStr;
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
            bool error = false;
            std::vector<double> levels = Util::string2VectorDouble( levelStr, &error, LEVEL_SEPARATOR );
            QString result;
            if ( !error ){
                result = setLevels( setName, levels);
            }
            else {
                result = "Invalid contour levels: "+levelStr;
            }
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
            std::set<QString> keys = {GeneratorState::LEVEL_MIN};
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
            std::set<QString> keys = { Util::WIDTH, CONTOUR_SET_NAME, LEVEL_LIST };
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            bool validDouble = false;
            QString result;
            double thickness = dataValues[Util::WIDTH].toDouble(&validDouble);
            if ( validDouble ){
                QString setName = dataValues[CONTOUR_SET_NAME ];
                QString levelStr = dataValues[LEVEL_LIST];
                bool error = false;
                std::vector<double> levels = Util::string2VectorDouble( levelStr, &error, LEVEL_SEPARATOR );
                if ( !error ){
                    result = setThickness( setName, levels, thickness );
                }
                else {
                    result = "Could not set thickness; contour levels were invalid: "+levelStr;
                }
            }
            else {
                result = "Contour thickness must be an integer: "+dataValues[Util::WIDTH];
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
                bool validLevels = false;
                std::vector<double> levels = Util::string2VectorDouble( levelListStr, &validLevels, LEVEL_SEPARATOR );
                if ( validLevels ){
                    result = setVisibility( setName, levels, visible );
                }
                else {
                    result = "Could not set visibility; invalid contour levels: "+levelListStr;
                }
            }
            else {
                result = "Contour visibility must be true/false: "+visibleStr;
            }
            Util::commandPostProcess( result );
            return result;
        });


}


bool ContourControls::_isDuplicate( const QString& contourSetName ) const {
    bool duplicateSet = false;
    for ( std::set<std::shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
                it != m_dataContours.end(); it++ ){
        if ( (*it)->getName() == contourSetName ){
            duplicateSet = true;
            break;
        }
    }
    return duplicateSet;
}


void ContourControls::selectContourSet( const QString& name ){
   DataContours* dataContours = _getContour( name );
   if ( dataContours != nullptr){
       //Existing contour set - use it's method of generating contour levels
       m_generatorState = dataContours->_getGenerator();
   }
   else {
       //Use a new blank generator but update it with the state of the current generator.
       std::shared_ptr<GeneratorState> newGen( new GeneratorState() );
       newGen->_updateState( m_generatorState );
       m_generatorState = newGen;
   }
   QString genStateStr = m_generatorState->getStateString();

   m_state.setState( genStateStr );
   CartaObject::refreshState();
   m_state.flushState();
}

QString ContourControls::setAlpha( const QString& contourName,
        std::vector<double>& levels, int transparency ){
    QString result;
    int levelCount = levels.size();
    if ( levelCount > 0 ){
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
    }
    else {
        result = "Please specify one or more levels when setting transparency";
    }
    return result;
}

QStringList ContourControls::setColor( const QString& contourName,
        std::vector<double>& levels, int red, int green, int blue ){
    QStringList result;
    int levelCount = levels.size();
    if ( levelCount > 0 ){
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
    }
    else {
        result.append( "Please specify one or more contour levels when specifying the color");
    }
    return result;
}

void ContourControls::setDashedNegative( bool useDash ){
    m_generatorState->setDashedNegative( useDash );
}

void ContourControls::_setDrawContours( std::shared_ptr<DataContours> contours ){
    m_drawContours = contours;
    DataContours* existingContour = _getContour( contours->getName());
    if ( !existingContour ){
        m_dataContours.insert( contours );
        _updateContourSetState();
    }
}



QString ContourControls::setGenerateMethod( const QString& method ){
    QString result = m_generatorState->setGenerateMethod( method );
    return result;
}

void ContourControls::setPercentIntensityMap( IPercentIntensityMap* mapper ){
    m_percentIntensityMap = mapper;
}

QString ContourControls::setSpacing( const QString& method ){
    QString result = m_generatorState->setSpacing( method );
    return result;
}

QString ContourControls::setLevelCount( int count ){
    QString result = m_generatorState->setLevelCount( count );
    return result;
}

QString ContourControls::setLevelMax( double value ){
    QString result = m_generatorState->setLevelMax( value );
    return result;
}

QString ContourControls::setLevelMin( double value ){
    QString result = m_generatorState->setLevelMin( value );
    return result;
}

QString ContourControls::setLevelMinMax( double minValue, double maxValue ){
    QString result = m_generatorState->setLevelMinMax( minValue, maxValue );
    return result;
}

QString ContourControls::setLevels( const QString& contourName, std::vector<double>& levels ){
    QString result;
    DataContours* target = _getContour( contourName );
    int newLevelCount = levels.size();
    if ( newLevelCount > 0 ){
        if ( target != nullptr ){
            bool levelCount = target->getLevelCount();
            bool levelsChanged = target->setLevels( levels );
            if ( levelsChanged ){
                _updateContourSetState();
                if ( levelCount != newLevelCount ){
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
    }
    else {
        result = "A contour set must have at least one level.";
    }
    return result;
}

QString ContourControls::setLineStyle( const QString& contourName, std::vector<double>& levels, const QString& lineStyle ){
    QString result;
    DataContours* target = _getContour( contourName );
    int levelCount = levels.size();
    if ( levelCount > 0 ){
        if ( target != nullptr ){
            result = target->setLineStyle( levels, lineStyle );
            if ( result.isEmpty() ){
                _updateContourSetState();
            }
        }
        else {
            result = "Unrecognized contour set: "+ contourName;
        }
    }
    else {
        result = "Please specify one or more levels when setting the line style.";
    }
    return result;
}

QString ContourControls::setSpacingInterval( double interval ){
    QString result = m_generatorState->setSpacingInterval( interval );
    return result;
}

QString ContourControls::setThickness( const QString& contourName,
        std::vector<double>& levels, double thickness ){
    QString result;
    int levelCount = levels.size();
    if ( levelCount > 0 ){
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
    }
    else {
        result = "Please specify one or more contour levels when setting contour level thickness";
    }
    return result;
}

QString ContourControls::setVisibility( const QString& contourName,
        std::vector<double>& levels, bool visible ){
    QString result;
    int levelCount = levels.size();
    if ( levelCount > 0 ){
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
    }
    else {
        result = "Please specify one or more contour levels when setting the visibility.";
    }
    return result;
}

void ContourControls::_updateContourSetState(){
    int contourSetCount = m_dataContours.size();
    m_stateData.resizeArray( CONTOUR_SETS, contourSetCount );
    std::set<Contour> drawContours;
    int i = 0;
    for ( std::set<std::shared_ptr<DataContours> >::iterator it = m_dataContours.begin();
            it != m_dataContours.end(); it++ ){
        QString lookup = Carta::State::UtilState::getLookup( CONTOUR_SETS, i );
        Carta::State::StateInterface dataState = (*it)->_getState();
        QString contourState = dataState.toString();
        m_stateData.setObject( lookup, contourState);
        i++;
    }
    m_stateData.flushState();
    emit drawContoursChanged();
}


ContourControls::~ContourControls(){
}
}
}
