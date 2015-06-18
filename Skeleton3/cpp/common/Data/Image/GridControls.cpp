#include "GridControls.h"
#include "Fonts.h"
#include "DataGrid.h"
#include "Data/Util.h"
#include "State/StateInterface.h"
#include "State/UtilState.h"

#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

const QString GridControls::CLASS_NAME = "GridControls";
const QString GridControls::ALL = "allImages";
const QString GridControls::GRID = "grid";

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

    Carta::State::CartaObject* gridObj = Util::createObject( DataGrid::CLASS_NAME );
     m_dataGrid.reset( dynamic_cast<DataGrid*>(gridObj) );

    _initializeDefaultState();
    _initializeCallbacks();
}


void GridControls::_initializeDefaultState(){
    m_state.insertValue<bool>( ALL, true  );
    m_state.insertObject( GRID, m_dataGrid->getStateString());
    m_state.flushState();
}

void GridControls::_initializeCallbacks(){

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

    addCommandCallback( "setCoordinateSystem", [=] (const QString & /*cmd*/,
                    const QString & params, const QString & /*sessionId*/) -> QString {
                std::set<QString> keys = {DataGrid::COORD_SYSTEM};
                std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                QString showAxisStr = dataValues[DataGrid::SHOW_AXIS];
                QString result = setCoordinateSystem( dataValues[DataGrid::COORD_SYSTEM] );
                Util::commandPostProcess( result );
                return result;
            });

    addCommandCallback( "setGridColor", [=] (const QString & /*cmd*/,
                                        const QString & params, const QString & /*sessionId*/) -> QString {
            int redAmount = 0;
            int greenAmount = 0;
            int blueAmount = 0;
            QStringList result = _parseColorParams( params, "Grid", &redAmount, &greenAmount, &blueAmount);
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
                    bool validDouble = false;
                    double thickness = thicknessStr.toDouble( &validDouble );
                    if ( validDouble ){
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
                            std::set<QString> keys = {DataGrid::TRANSPARENCY};
                            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
                            QString transparencyStr = dataValues[DataGrid::TRANSPARENCY];
                            bool validDouble = false;
                            double transparency = transparencyStr.toDouble( &validDouble );
                            if ( validDouble ){
                                result = setGridTransparency( transparency );
                            }
                            else {
                                result = "Grid transparency must be a number:"+params;
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
}

QStringList GridControls::_parseColorParams( const QString& params, const QString& label,
        int* red, int* green, int* blue ) const {
    QStringList result;
    std::set<QString> keys = {DataGrid::RED,DataGrid::GREEN,DataGrid::BLUE};
    std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
    QString redStr = dataValues[DataGrid::RED];
    bool validInt = false;
    *red = redStr.toInt(&validInt);
    if ( !validInt ){
        result.append(label + " red amount must be a nonnegative integer: "+params);
    }
    QString greenStr = dataValues[DataGrid::GREEN];
    *green = greenStr.toInt( &validInt );
    if ( !validInt ){
        result.append( label + " green amount must be a nonnegative integer: "+params);
    }

    QString blueStr = dataValues[DataGrid::BLUE];
    *blue = blueStr.toInt( &validInt );
    if ( !validInt ){
        result.append(label + " blue amount must be a nonnegative integer:"+params);
    }
    return result;
}



QStringList GridControls::setAxesColor( int redAmount, int greenAmount, int blueAmount ){
    bool axesColorChanged = false;
    QStringList result = m_dataGrid->_setAxesColor( redAmount, greenAmount, blueAmount, &axesColorChanged );
    if ( axesColorChanged ){
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

QString GridControls::setGridThickness( double thickness ){
    bool thicknessChanged = false;
    QString result = m_dataGrid->_setGridThickness( thickness, &thicknessChanged );
    if ( thicknessChanged ){
        _updateGrid();
    }
    return result;
}

QString GridControls::setGridTransparency( double transparency ){
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

QString GridControls::setShowAxis( bool showAxis ){
    bool axisChanged = false;
    QString result = m_dataGrid->_setShowAxis( showAxis, & axisChanged );
    if ( axisChanged ){
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

void GridControls::_updateGrid(){
    emit gridChanged( m_dataGrid->_getState() );
    m_state.setObject( GRID, m_dataGrid->getStateString() );
    m_state.flushState();
}

GridControls::~GridControls(){

}
}
}
