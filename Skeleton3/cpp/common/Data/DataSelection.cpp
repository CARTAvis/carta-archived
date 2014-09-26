#include "DataSelection.h"
#include "Globals.h"
#include "IConnector.h"

#include <QDebug>

DataSelection::DataSelection(const QString& identifier, StateKey lowKey,
        StateKey indexKey, StateKey highKey) :
        m_id(identifier), m_highKey(highKey), m_indexKey(indexKey), m_lowKey(
                lowKey) {
    _initializeStates();
}

void DataSelection::_initializeStates(){
    auto & globals = *Globals::instance();
    IConnector * connector = globals.connector();
    QString lowBoundStr = connector->getState( m_lowKey, m_id );
    if ( lowBoundStr.length() == 0 ){
        connector->setState(m_lowKey, m_id, "0");
    }
    QString highBoundStr = connector->getState( m_highKey, m_id );
    if ( highBoundStr.length() == 0 ){
        connector->setState(m_highKey, m_id, "1");
    }
    QString indexStr = connector->getState( m_indexKey, m_id );
    if ( indexStr.length() == 0 ){
        connector->setState(m_indexKey, m_id, "0");
    }
}

int DataSelection::_getValue(StateKey key) const {

    //Look up the value.
    auto & globals = *Globals::instance();
    IConnector * connector = globals.connector();
    QString value = "0";
    if (m_id.length() > 0) {
        value = connector->getState(key, m_id);
    }

    //Try to parse value as an integer.
    bool validValue = false;
    int val = value.toInt(&validValue);
    if (!validValue) {
        val = -1;
    }
    return val;
}

int DataSelection::getIndex() const {
    return _getValue(m_indexKey);
}

int DataSelection::getLowerBound() const {
    return _getValue(m_lowKey);
}

int DataSelection::getUpperBound() const {
    return _getValue(m_highKey);
}

void DataSelection::setIndex(int newIndex) {
    _setIndexCheck(newIndex);
}

void DataSelection::setUpperBound(int newUpperBound) {
    _setFrameBoundsCheck(m_highKey, newUpperBound);
}

void DataSelection::setLowerBound(int newLowerBound) {
    _setFrameBoundsCheck(m_lowKey, newLowerBound);
}

void DataSelection::_setFrameBounds(StateKey key, const QString& val) {
    bool validInt = false;
    int frameBound = val.toInt(&validInt);
    if (validInt) {
        _setFrameBoundsCheck(key, frameBound);

    }
}

void DataSelection::_setFrameBoundsCheck(StateKey key, int bound) {
    if (bound >= 0) {
        auto & globals = *Globals::instance();
        IConnector * connector = globals.connector();
        connector->setState(key, m_id, QString::number(bound));
    }
}

int DataSelection::setIndex(const QString& params) {
    bool validFrame = false;
    int frameValue = params.toInt(&validFrame);
    if (validFrame) {
        _setIndexCheck(frameValue);
    }
    return frameValue;
}

bool DataSelection::_setIndexCheck(int frameValue) {
    bool validFrame = true;
    auto & globals = *Globals::instance();
    IConnector * connector = globals.connector();
    QString frameStartStr = connector->getState(m_lowKey, m_id);
    QString frameEndStr = connector->getState(m_highKey, m_id);
    if (frameValue < frameStartStr.toInt()) {
        validFrame = false;
    } else if (frameValue > frameEndStr.toInt()) {
        validFrame = false;
    }
    if (validFrame) {
        connector->setState(m_indexKey, m_id, QString::number(frameValue));
    }
    return validFrame;
}

DataSelection::~DataSelection() {

}
