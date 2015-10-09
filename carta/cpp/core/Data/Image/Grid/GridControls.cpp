#include "GridControls.h"
#include "Data/Image/Grid/AxisMapper.h"
#include "Data/Image/Grid/Fonts.h"
#include "Data/Image/Grid/DataGrid.h"
#include "Data/Util.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"

#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

using Carta::Lib::AxisInfo;

const QString GridControls::CLASS_NAME = "GridControls";
const QString GridControls::ALL = "applyAll";


class GridControls::Factory : public Carta::State::CartaObjectFactory {

    public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new GridControls (path, id);
        }
    };

bool GridControls::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new GridControls::Factory());

GridControls::GridControls( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){

    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    DataGrid* gridObj = objMan->createObject<DataGrid>();
     m_dataGrid.reset( gridObj );

    _initializeDefaultState();
    _initializeCallbacks();
}


void GridControls::_initializeDefaultState(){
    m_state.insertValue<bool>( ALL, true  );
    m_state.insertObject( DataGrid::GRID, m_dataGrid->_getState().toString());
    m_state.flushState();
}

void GridControls::_initializeCallbacks(){

    addCommandCallback( "setApplyAll", [=] (const QString & /*cmd*/,
                   const QString & params, const QString & /*sessionId*/) -> QString {
               QString result;
               std::set<QString> keys = {ALL};
               std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
               QString applyAllStr = dataValues[ALL];
               bool validBool = false;
               bool applyAll = Util::toBool( applyAllStr, &validBool );
               if ( validBool ){
                   setApplyAll( applyAll  );
               }
               else {
                   result = "Whether or not to apply grid changes to all images must be true/false:"+params;
               }
               Util::commandPostProcess( result );
               return result;
           });


    addCommandCallback( "setAxesColor", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
        int redAmount = 0;
        int greenAmount = 0;
        int blueAmount = 0;
        QStringList result = _parseColorParams( params, "Axes", &redAmount, &greenAmount, &blueAmount);
        if ( result.size() == 0 ){
            result = setAxesColor( redAmount, greenAmount, blueAmount );
        }
        QString errors;
        if ( result.size() > 0 ){
            errors = result.join( ",");
        }
        Util::commandPostProcess( errors );
        return errors;
    });

    addCommandCallback( "setAxesThickness", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
       QString result;
       std::set<QString> keys = {DataGrid::AXES};
       std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
       QString thicknessStr = dataValues[DataGrid::AXES];
       bool validInt = false;
       int thickness = thicknessStr.toInt( &validInt );
       if ( validInt ){
           result = setAxesThickness( thickness  );
       }
       else {
           result = "Axes thickness must be a number:"+params;
       }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setAxesTransparency", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {Util::ALPHA};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString transparencyStr = dataValues[Util::ALPHA];
        bool validInt = false;
        int transparency = transparencyStr.toInt( &validInt );
        if ( validInt ){
            result = setAxesTransparency( transparency );
        }
        else {
            result = "Axes transparency must be an integer:"+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setAxisX", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) ->QString {
        std::set<QString> keys = {AxisMapper::AXIS_X};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString axisName = dataValues[AxisMapper::AXIS_X];
        QString result = setAxis( AxisMapper::AXIS_X, axisName );
        return result;
    });

    addCommandCallback( "setAxisY", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) ->QString {
        std::set<QString> keys = {AxisMapper::AXIS_Y};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString axisName = dataValues[AxisMapper::AXIS_Y];
        QString result = setAxis( AxisMapper::AXIS_Y, axisName );
        return result;
    });

    addCommandCallback( "setCoordinateSystem", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
                std::set<QString> keys = {DataGrid::COORD_SYSTEM};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString showAxisStr = dataValues[DataGrid::SHOW_AXIS];
                QString result = setCoordinateSystem( dataValues[DataGrid::COORD_SYSTEM] );
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setGridLabelFormat", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                    std::set<QString> keys = {DataGrid::FORMAT, DataGrid::LABEL_SIDE};
                    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                    QString format = dataValues[DataGrid::FORMAT];
                    //Because the map expects colons and the label format has colons,
                    //the format colons are replaced with a - before sending from the
                    //client and the server must restore the colons.
                    format = format.replace( "-", ":");
                    QString labelSide = dataValues[DataGrid::LABEL_SIDE];
                    QString result = setLabelFormat( labelSide, format );
                    Util::commandPostProcess( result );
                    return result;
                });

    addCommandCallback( "setGridColor", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
            int redAmount = 0;
            int greenAmount = 0;
            int blueAmount = 0;
            QStringList result = _parseColorParams( params, DataGrid::GRID, &redAmount, &greenAmount, &blueAmount);
            if ( result.size() == 0 ){
                result = setGridColor( redAmount, greenAmount, blueAmount );
            }
            QString errors;
            if ( result.size() > 0 ){
                errors = result.join( ",");
            }
            Util::commandPostProcess( errors );
            return errors;
        });

    addCommandCallback( "setFontFamily", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                    std::set<QString> keys = {Fonts::FONT_FAMILY};
                    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                    QString result = setFontFamily( dataValues[Fonts::FONT_FAMILY] );
                    Util::commandPostProcess( result );
                    return result;
                });

    addCommandCallback( "setFontSize", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
                        QString result;
                        std::set<QString> keys = {Fonts::FONT_SIZE};
                        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                        QString fontSizeStr = dataValues[Fonts::FONT_SIZE];
                        bool validInt = false;
                        int fontSize = fontSizeStr.toInt(&validInt);
                        if ( validInt ){
                            result = setFontSize( fontSize );
                        }
                        else {
                            result = "Font size must be a nonnegative integer: "+fontSizeStr;
                        }
                        Util::commandPostProcess( result );
                        return result;
                    });

    addCommandCallback( "setGridThickness", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                    QString result;
                    std::set<QString> keys = {DataGrid::GRID};
                    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                    QString thicknessStr = dataValues[DataGrid::GRID];
                    bool validInt = false;
                    int thickness = thicknessStr.toInt( &validInt );
                    if ( validInt ){
                        result = setGridThickness( thickness  );
                    }
                    else {
                        result = "Grid thickness must be a number:"+params;
                    }
                    Util::commandPostProcess( result );
                    return result;
                });

    addCommandCallback( "setGridSpacing", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
                        QString result;
                        std::set<QString> keys = {DataGrid::SPACING};
                        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                        QString spacingStr = dataValues[DataGrid::SPACING];
                        bool validDouble = false;
                        double spacing = spacingStr.toDouble( &validDouble );
                        if ( validDouble ){
                            result = setGridSpacing( spacing  );
                        }
                        else {
                            result = "Grid spacing must be a number:"+params;
                        }
                        Util::commandPostProcess( result );
                        return result;
                    });

    addCommandCallback( "setGridTransparency", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {Util::ALPHA};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString transparencyStr = dataValues[Util::ALPHA];
            bool validInt = false;
            int transparency = transparencyStr.toInt( &validInt );
            if ( validInt ){
                result = setGridTransparency( transparency );
            }
            else {
                result = "Grid transparency must be an integer:"+params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setLabelDecimals", [=] (const QString & /*cmd*/,
                                    const QString & params, const QString & /*sessionId*/) -> QString {
        QString result;
        std::set<QString> keys = {DataGrid::LABEL_DECIMAL_PLACES};
        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
        QString decimalsStr = dataValues[DataGrid::LABEL_DECIMAL_PLACES];
        bool validInt = false;
        int decimalPlaces = decimalsStr.toInt( &validInt );
        if ( validInt ){
            result = setLabelDecimals( decimalPlaces );
        }
        else {
            result = "Grid label decimal places must be an integer:"+params;
        }
        Util::commandPostProcess( result );
        return result;
    });

    addCommandCallback( "setLabelColor", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
            int redAmount = 0;
            int greenAmount = 0;
            int blueAmount = 0;
            QStringList result = _parseColorParams( params, "Label", &redAmount, &greenAmount, &blueAmount);
            if ( result.size() == 0 ){
                result = setLabelColor( redAmount, greenAmount, blueAmount );
            }
            QString errors;
            if ( result.size() > 0 ){
                errors = result.join( ",");
            }
            Util::commandPostProcess( errors );
            return errors;
        });

    addCommandCallback( "setShowAxis", [=] (const QString & /*cmd*/,
                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {DataGrid::SHOW_AXIS};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString showAxisStr = dataValues[DataGrid::SHOW_AXIS];
            bool validBool = false;
            bool showAxis = Util::toBool( showAxisStr, &validBool );
            if ( validBool ){
                result = setShowAxis( showAxis  );
            }
            else {
                result = "Making an axis visible/invisible must be true/false:"+params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setShowCoordinateSystem", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
                QString result;
                std::set<QString> keys = {DataGrid::SHOW_COORDS};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString showCoordsStr = dataValues[DataGrid::SHOW_COORDS];
                bool validBool = false;
                bool showCoord = Util::toBool( showCoordsStr, &validBool );
                if ( validBool ){
                    result = setShowCoordinateSystem( showCoord );
                }
                else {
                    result = "Whether or not to show the coordinate system must be true/false:"+params;
                }
                Util::commandPostProcess( result );
                return result;
            });


    addCommandCallback( "setShowGridLines", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
                QString result;
                std::set<QString> keys = {DataGrid::SHOW_GRID_LINES};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString showLinesStr = dataValues[DataGrid::SHOW_GRID_LINES];
                bool validBool = false;
                bool showLines = Util::toBool( showLinesStr, &validBool );
                if ( validBool ){
                    result = setShowGridLines( showLines  );
                }
                else {
                    result = "Setting axis grid lines visible/invisible must be true/false:"+params;
                }
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setShowInternalLabels", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                    QString result;
                    std::set<QString> keys = {DataGrid::SHOW_INTERNAL_LABELS};
                    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                    QString showLabelsStr = dataValues[DataGrid::SHOW_INTERNAL_LABELS];
                    bool validBool = false;
                    bool showLabels = Util::toBool( showLabelsStr, &validBool );
                    if ( validBool ){
                        result = setShowInternalLabels( showLabels  );
                    }
                    else {
                        result = "Showing internal axis labels must be true/false:"+params;
                    }
                    Util::commandPostProcess( result );
                    return result;
                });

    addCommandCallback( "setShowTicks", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
                QString result;
                std::set<QString> keys = {DataGrid::SHOW_TICKS};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString showTicksStr = dataValues[DataGrid::SHOW_TICKS];
                bool validBool = false;
                bool showTicks = Util::toBool( showTicksStr, &validBool );
                if ( validBool ){
                    result = setShowTicks( showTicks  );
                }
                else {
                    result = "Making ticks visible/invisible must be true/false:"+params;
                }
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setShowStatistics", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) -> QString {
                    QString result;
                    std::set<QString> keys = {DataGrid::SHOW_STATISTICS};
                    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                    QString showStatisticsStr = dataValues[DataGrid::SHOW_STATISTICS];
                    bool validBool = false;
                    bool showStatistics = Util::toBool( showStatisticsStr, &validBool );
                    if ( validBool ){
                        result = setShowStatistics( showStatistics  );
                    }
                    else {
                        result = "Making statistics visible/invisible must be true/false:"+params;
                    }
                    Util::commandPostProcess( result );
                    return result;
                });

    addCommandCallback( "setTickColor", [=] (const QString & /*cmd*/,
                                           const QString & params, const QString & /*sessionId*/) -> QString {
               int redAmount = 0;
               int greenAmount = 0;
               int blueAmount = 0;
               QStringList result = _parseColorParams( params, DataGrid::TICK, &redAmount, &greenAmount, &blueAmount);
               if ( result.size() == 0 ){
                   result = setTickColor( redAmount, greenAmount, blueAmount );
               }
               QString errors;
               if ( result.size() > 0 ){
                   errors = result.join( ",");
               }
               Util::commandPostProcess( errors );
               return errors;
           });

    addCommandCallback( "setTickLength", [=] (const QString & /*cmd*/,
                                const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {DataGrid::TICK_LENGTH};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString lengthStr = dataValues[DataGrid::TICK_LENGTH];
            bool validInt = false;
            int length = lengthStr.toInt( &validInt );
            if ( validInt ){
                result = setTickLength( length  );
            }
            else {
                result = "Tick length must be a number:"+params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setTickThickness", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
                        QString result;
                        std::set<QString> keys = {DataGrid::TICK};
                        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                        QString thicknessStr = dataValues[DataGrid::TICK];
                        bool validInt = false;
                        int thickness = thicknessStr.toInt( &validInt );
                        if ( validInt ){
                            result = setTickThickness( thickness  );
                        }
                        else {
                            result = "Tick thickness must be a number:"+params;
                        }
                        Util::commandPostProcess( result );
                        return result;
                    });

    addCommandCallback( "setTickTransparency", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
            QString result;
            std::set<QString> keys = {Util::ALPHA};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString transparencyStr = dataValues[Util::ALPHA];
            bool validInt = false;
            int transparency = transparencyStr.toInt( &validInt );
            if ( validInt ){
                result = setTickTransparency( transparency );
            }
            else {
                result = "Tick transparency must be an integer:"+params;
            }
            Util::commandPostProcess( result );
            return result;
        });

    addCommandCallback( "setTheme", [=] (const QString & /*cmd*/,
                            const QString & params, const QString & /*sessionId*/) -> QString {
                        std::set<QString> keys = {DataGrid::THEME};
                        std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                        QString themeStr = dataValues[DataGrid::THEME];
                        QString result = setTheme( themeStr );
                        Util::commandPostProcess( result );
                        return result;
                    });
}

void GridControls::_notifyAxesChanged(){
    std::vector<AxisInfo::KnownType> displayTypes = m_dataGrid->_getDisplayAxes();
    bool applyAll = m_state.getValue<bool>( ALL );
    emit displayAxesChanged( displayTypes, applyAll );
}

QStringList GridControls::_parseColorParams( const QString& params, const QString& label,
        int* red, int* green, int* blue ) const {
    QStringList result;
    std::set<QString> keys = {Util::RED, Util::GREEN, Util::BLUE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString redStr = dataValues[Util::RED];
    bool validInt = false;
    *red = redStr.toInt(&validInt);
    if ( !validInt ){
        result.append(label + " red amount must be a nonnegative integer: "+params);
    }
    QString greenStr = dataValues[Util::GREEN];
    *green = greenStr.toInt( &validInt );
    if ( !validInt ){
        result.append( label + " green amount must be a nonnegative integer: "+params);
    }

    QString blueStr = dataValues[Util::BLUE];
    *blue = blueStr.toInt( &validInt );
    if ( !validInt ){
        result.append(label + " blue amount must be a nonnegative integer:"+params);
    }
    return result;
}

void GridControls::_resetState( const Carta::State::StateInterface& otherState ){
    m_dataGrid->_resetState( otherState );
    QString gridStateStr = m_dataGrid->_getState().toString();
    m_state.setObject( DataGrid::GRID, gridStateStr );
    m_state.flushState();
}

void GridControls::setApplyAll( bool applyAll ){
    bool oldApplyAll = m_state.getValue<bool>(ALL );
    if ( oldApplyAll != applyAll ){
        m_state.setValue<bool>(ALL, applyAll );
        if ( applyAll ){
            _updateGrid();
        }
    }
}

void GridControls::_setAxisTypes( std::vector<AxisInfo::KnownType> supportedAxes ){
    bool axisTypesChanged = m_dataGrid->_setAxisTypes( supportedAxes );
    if ( axisTypesChanged ){
        _updateGrid();
    }
}

QStringList GridControls::setAxesColor( int redAmount, int greenAmount, int blueAmount ){
    bool axesColorChanged = false;
    QStringList result = m_dataGrid->_setAxesColor( redAmount, greenAmount, blueAmount, &axesColorChanged );
    if ( axesColorChanged ){
        _updateGrid();
    }
    return result;
}



QString GridControls::setAxis( const QString& axisId, const QString& purpose ){
    bool axisChanged = false;
    QString result = m_dataGrid->_setAxis( axisId, purpose, &axisChanged );
    if ( axisChanged ){
        _updateGrid();
        _notifyAxesChanged();
    }
    return result;
}

QString GridControls::setAxesThickness( int thickness ){
    bool thicknessChanged = false;
    QString result = m_dataGrid->_setAxesThickness( thickness, &thicknessChanged );
    if ( thicknessChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setAxesTransparency( int transparency ){
    bool transparencyChanged = false;
    QString result = m_dataGrid->_setAxesTransparency( transparency, &transparencyChanged );
    if ( transparencyChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setCoordinateSystem( const QString& coordSystem ){
    bool coordChanged = false;
    QString result = m_dataGrid->_setCoordinateSystem( coordSystem, &coordChanged );
    if ( coordChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setFontFamily( const QString& fontFamily ){
    bool familyChanged = false;
    QString result = m_dataGrid->_setFontFamily( fontFamily, &familyChanged );
    if ( familyChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setFontSize( int fontSize ){
    bool sizeChanged = false;
    QString result = m_dataGrid->_setFontSize( fontSize, &sizeChanged );
    if ( sizeChanged ){
        _updateGrid();
    }
    return result;
}

QStringList GridControls::setGridColor( int redAmount, int greenAmount, int blueAmount ){
    bool gridColorChanged = false;
    QStringList result = m_dataGrid->_setGridColor( redAmount, greenAmount, blueAmount, &gridColorChanged );
    if ( gridColorChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setGridSpacing( double spacing ){
    bool spacingChanged = false;
    QString result = m_dataGrid->_setGridSpacing( spacing, &spacingChanged );
    if ( spacingChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setGridThickness( int thickness ){
    bool thicknessChanged = false;
    QString result = m_dataGrid->_setGridThickness( thickness, &thicknessChanged );
    if ( thicknessChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setGridTransparency( int transparency ){
    bool transparencyChanged = false;
    QString result = m_dataGrid->_setGridTransparency( transparency, &transparencyChanged );
    if ( transparencyChanged ){
        _updateGrid();
    }
    return result;
}

QStringList GridControls::setLabelColor( int redAmount, int greenAmount, int blueAmount ){
    bool labelColorChanged = false;
    QStringList result = m_dataGrid->_setLabelColor( redAmount, greenAmount, blueAmount, &labelColorChanged );
    if ( labelColorChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setLabelFormat( const QString& labelSide, const QString& labelFormat ){
    bool labelFormatChanged = false;
    QString result = m_dataGrid->_setLabelFormat( labelSide, labelFormat, &labelFormatChanged );
    if ( labelFormatChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setLabelDecimals( int decimalPlaces ){
    bool decimalsChanged = false;
    QString result = m_dataGrid->_setLabelDecimalPlaces( decimalPlaces, &decimalsChanged );
    if ( decimalsChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setShowAxis( bool showAxis ){
    bool axisChanged = false;
    QString result = m_dataGrid->_setShowAxis( showAxis, & axisChanged );
    if ( axisChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setShowCoordinateSystem( bool showCoordinateSystem ){
    bool visibilityChanged = false;
    QString result = m_dataGrid->_setShowCoordinateSystem( showCoordinateSystem, & visibilityChanged );
    if ( visibilityChanged ){
        _updateGrid();
    }
    return result;
}


QString GridControls::setShowGridLines( bool showLines ){
    bool linesChanged = false;
    QString result = m_dataGrid->_setShowGridLines( showLines, &linesChanged );
    if ( linesChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setShowInternalLabels( bool showInternalLabels ){
    bool labelsChanged = false;
    QString result = m_dataGrid->_setShowInternalLabels( showInternalLabels, &labelsChanged );
    if ( labelsChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setShowStatistics( bool showStatistics ){
    bool statisticsChanged = false;
    QString result = m_dataGrid->_setShowStatistics( showStatistics, &statisticsChanged );
    if ( statisticsChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setShowTicks( bool showTicks ){
    bool ticksChanged = false;
    QString result = m_dataGrid->_setShowTicks( showTicks, & ticksChanged );
    if ( ticksChanged ){
        _updateGrid();
    }
    return result;
}



QStringList GridControls::setTickColor( int redAmount, int greenAmount, int blueAmount ){
    bool tickColorChanged = false;
    QStringList result = m_dataGrid->_setTickColor( redAmount, greenAmount, blueAmount, &tickColorChanged );
    if ( tickColorChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setTickLength( int tickLength ){
    bool lengthChanged = false;
    QString result = m_dataGrid->_setTickLength( tickLength, &lengthChanged );
    if ( lengthChanged ){
       _updateGrid();
    }
    return result;
}

QString GridControls::setTickThickness( int tickThickness ){
    bool thicknessChanged = false;
    QString result = m_dataGrid->_setTickThickness( tickThickness, &thicknessChanged );
    if ( thicknessChanged ){
       _updateGrid();
    }
    return result;
}

QString GridControls::setTickTransparency( int transparency ){
    bool transparencyChanged = false;
    QString result = m_dataGrid->_setTickTransparency( transparency, &transparencyChanged );
    if ( transparencyChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setTheme( const QString& theme ){
    bool themeChanged = false;
    QString result = m_dataGrid->_setTheme( theme, &themeChanged );
    if ( themeChanged ){
        _updateGrid();
    }
    return result;
}

void GridControls::_updateGrid(){
    bool applyAll = m_state.getValue<bool>( ALL );
    Carta::State::StateInterface gridState = m_dataGrid->_getState();
    QString gridStateStr = gridState.toString();

    m_state.setObject( DataGrid::GRID, gridStateStr );
    m_state.flushState();
    emit gridChanged( gridState, applyAll );

}

GridControls::~GridControls(){
    Carta::State::ObjectManager* objMan = Carta::State::ObjectManager::objectManager();
    if ( m_dataGrid != nullptr){
        objMan->removeObject(m_dataGrid->getId());
    }
}
}
}
