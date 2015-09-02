#include "DisplayControls.h"
#include "Data/Util.h"
#include "Data/Image/DataDisplay/AxisMapper.h"
#include "State/UtilState.h"

#include <set>

#include <QDebug>

namespace Carta {

namespace Data {

const QString DisplayControls::CLASS_NAME = "DisplayControls";
const QString DisplayControls::ALL = "applyAll";
const QString DisplayControls::SUPPORTED_AXES = "axes";


class DisplayControls::Factory : public Carta::State::CartaObjectFactory {

    public:

    Carta::State::CartaObject * create (const QString & path, const QString & id)
        {
            return new DisplayControls (path, id);
        }
    };

bool DisplayControls::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME, new DisplayControls::Factory());

DisplayControls::DisplayControls( const QString& path, const QString& id):
    CartaObject( CLASS_NAME, path, id ){
    _initializeDefaultState();
    _initializeCallbacks();
}

void DisplayControls::_addUsedPurpose( const QString& key, const QString& targetPurpose,
    QList<QString>& usedPurposes, QString& usedPurposeKey ){
    QString otherPurpose = m_state.getValue<QString>( key );
    usedPurposes.append( otherPurpose );
    if ( otherPurpose == targetPurpose ){
        usedPurposeKey = key;
    }
}

void DisplayControls::_initializeCallbacks(){

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

    addCommandCallback( "setAxisZ", [=] (const QString & /*cmd*/,
                        const QString & params, const QString & /*sessionId*/) ->QString {
            std::set<QString> keys = {AxisMapper::AXIS_Z};
            std::map<QString,QString> dataValues = Carta::State::UtilState::parseParamMap( params, keys );
            QString axisName = dataValues[AxisMapper::AXIS_Z];
            QString result = setAxis( AxisMapper::AXIS_Z, axisName );
            return result;
        });
}


void DisplayControls::_initializeDefaultState(){
    m_state.insertValue<bool>( ALL, false  );
    m_state.insertArray( SUPPORTED_AXES, 0);
    QString xName = AxisMapper::getDefaultPurpose( AxisMapper::AXIS_X );
    m_state.insertValue<QString>( AxisMapper::AXIS_X, xName );
    QString yName = AxisMapper::getDefaultPurpose( AxisMapper::AXIS_Y );
    m_state.insertValue<QString>( AxisMapper::AXIS_Y, yName );
    QString zName = AxisMapper::getDefaultPurpose( AxisMapper::AXIS_Z );
    m_state.insertValue<QString>( AxisMapper::AXIS_Z, zName );
    m_state.flushState();
}

void DisplayControls::_notifyAxesChanged(){
    std::vector<Carta::Lib::AxisInfo::KnownType> displayTypes( 3 );
    QString xPurposeName = m_state.getValue<QString>( AxisMapper::AXIS_X );
    displayTypes[0] = AxisMapper::getType( xPurposeName );
    QString yPurposeName = m_state.getValue<QString>( AxisMapper::AXIS_Y );
    displayTypes[1] = AxisMapper::getType( yPurposeName );
    QString zPurposeName = m_state.getValue<QString>( AxisMapper::AXIS_Z );
    displayTypes[2] = AxisMapper::getType( zPurposeName );
    bool applyAll = m_state.getValue<bool>( ALL );
    qDebug() << "Notify axes changed displayCount="<<displayTypes.size();
    for ( int i = 0; i < displayTypes.size(); i++ ){
        qDebug() << "i="<<i<<" displayType="<<static_cast<int>(displayTypes[i]);
    }
    emit displayAxesChanged( displayTypes, applyAll );
}

void DisplayControls::setApplyAll( bool applyAll ){
    bool oldApplyAll = m_state.getValue<bool>(ALL );
    if ( oldApplyAll != applyAll ){
        m_state.setValue<bool>(ALL, applyAll );
        if ( applyAll ){
            _updateDisplay();
        }
    }
}

QString DisplayControls::setAxis( const QString& axisId, const QString& purpose ){
    QString result;
    QString actualAxis = AxisMapper::getDisplayName( axisId );
    if ( !actualAxis.isEmpty() ){
        QString actualPurpose = AxisMapper::getPurpose( purpose );
        if ( !actualPurpose.isEmpty() ){
            QString oldPurpose = m_state.getValue<QString>( axisId );
            if ( oldPurpose != actualPurpose ){

                //Get a running total of display axes that are being used; at the same time store
                //the key of the axis containing the duplicate name, if there is one.
                QList<QString> usedPurposes;
                QString duplicateAxisPurposeKey;
                QStringList axisNames = AxisMapper::getDisplayNames();
                int displayAxisCount = axisNames.size();
                for ( int i = 0; i < displayAxisCount; i++ ){
                    if ( actualAxis != axisNames[i] ){
                        _addUsedPurpose( axisNames[i], purpose, usedPurposes, duplicateAxisPurposeKey );
                    }
                }

                //There is an axis that is already displaying the one that we want to display.  Thus,
                //we need to find a new purpose for the duplicate.
                if ( !duplicateAxisPurposeKey.isEmpty() ){
                    int purposeCount = m_state.getArraySize( SUPPORTED_AXES );
                    for ( int i = 0; i < purposeCount; i++ ){
                        QString lookup = Carta::State::UtilState::getLookup( SUPPORTED_AXES, i );
                        QString axisPurpose = m_state.getValue<QString>( lookup );
                        if ( !usedPurposes.contains( axisPurpose ) ){
                            m_state.setValue<QString>(duplicateAxisPurposeKey, axisPurpose );
                            break;
                        }
                    }
                }
                m_state.setValue<QString>( actualAxis, purpose );
                _notifyAxesChanged();
                m_state.flushState();
            }
        }
        else {
            result = "Unrecognized axis type: "+purpose;
        }
    }
    else {
        result = "Unrecognized axis: "+axisId;
    }
    return result;
}

void DisplayControls::setAxisTypes( std::vector<Carta::Lib::AxisInfo::KnownType> supportedAxes ){
    int axisCount = supportedAxes.size();
    m_state.resizeArray( SUPPORTED_AXES, axisCount );
    for ( int i = 0; i < axisCount; i++ ){
        QString name = AxisMapper::getPurpose( supportedAxes[i] );
        QString lookup = Carta::State::UtilState::getLookup( SUPPORTED_AXES, i );
        m_state.setValue( lookup, name );
    }
    m_state.flushState();
}

void DisplayControls::_updateDisplay(){
    /*bool applyAll = m_state.getValue<bool>( ALL );
    Carta::State::StateInterface gridState = m_dataGrid->_getState();
    emit gridChanged( gridState, applyAll );
    QString gridStateStr = gridState.toString();
    m_state.setObject( DataGrid::GRID, gridStateStr );
    m_state.flushState();*/
}

DisplayControls::~DisplayControls(){

}
}
}
