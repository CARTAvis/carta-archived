#include "Selection.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString Selection::HIGH_KEY = "frameEnd";
const QString Selection::INDEX_KEY = "frame";
const QString Selection::LOW_KEY = "frameStart";
const QString Selection::CLASS_NAME = "Selection";
const QString Selection::SELECTIONS = "selections";
const QString Selection::IMAGE = "Image";
const QString Selection::CHANNEL = "Channel";

bool Selection::m_registered =
        Carta::State::ObjectManager::objectManager()->registerClass ( CLASS_NAME,
                                                   new Selection::Factory());

Selection::Selection( const QString& prefix, const QString& identifier ):
    CartaObject( CLASS_NAME, prefix, identifier ){
    _initializeStates( );
}



void Selection::_initializeStates(){
    m_state.insertValue<int>(LOW_KEY, 0);
    m_state.insertValue<int>( HIGH_KEY, 1);
    m_state.insertValue<int>( INDEX_KEY, 0);
    m_state.flushState();
}



int Selection::_getValue(const QString& key) const {
    int val = m_state.getValue<int>( key );
    return val;
}

int Selection::getState(const QString& key ) const{
    int value = -1;
    if ( key == HIGH_KEY ){
        value = getUpperBound();
    }
    else if ( key == LOW_KEY ){
        value = getLowerBound();
    }
    else if ( key == INDEX_KEY ){
        value = getIndex();
    }
    else {
        qDebug() << "DataSelection::getState unrecognized key="<<key;
    }
    return value;
}

QString Selection::getStateString() const{
    QString result = m_state.toString();
    return result;
}

int Selection::getIndex() const {
    return _getValue( INDEX_KEY);
}

int Selection::getLowerBound() const {
    return _getValue( LOW_KEY);
}

int Selection::getUpperBound() const {
    return _getValue( HIGH_KEY);
}


void Selection::setUpperBound(int newUpperBound) {
    _setFrameBoundsCheck(/*m_highKey*/HIGH_KEY, newUpperBound);
}

void Selection::setLowerBound(int newLowerBound) {
    _setFrameBoundsCheck(/*m_lowKey*/LOW_KEY, newLowerBound);
}

void Selection::_setFrameBounds(const QString& key, const QString& val) {
    bool validInt = false;
    int frameBound = val.toInt(&validInt);
    if (validInt) {
        _setFrameBoundsCheck(key, frameBound);

    }
}

bool Selection::_setFrameBoundsCheck(const QString& key, int bound) {
    bool valueChanged = false;
    if (bound >= 0) {
        int prevValue = m_state.getValue<int>( key );
        if ( prevValue != bound ){
            m_state.setValue<int>(key, bound);
            m_state.flushState();
            valueChanged = true;
        }
    }
    return valueChanged;
}

QString Selection::setIndex(int frameValue) {
    QString result;
    int upperBound = getUpperBound();
    int lowerBound = getLowerBound();
    if ( lowerBound <= frameValue && frameValue < upperBound ){
        bool valChanged = _setFrameBoundsCheck( INDEX_KEY, frameValue );
        if ( valChanged ){
            emit indexChanged( false );
        }
    }
    else {
        result = "Selection index "+ QString::number(frameValue)+" must be between "+
                QString::number(lowerBound) + " and " + QString::number(upperBound);
    }
    return result;
}

Selection::~Selection() {

}
}
}
