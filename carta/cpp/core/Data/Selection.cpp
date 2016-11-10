#include "Selection.h"

#include <QDebug>

namespace Carta {

namespace Data {

const QString Selection::HIGH_KEY = "frameEnd";
const QString Selection::HIGH_KEY_USER = "frameEndUser";
const QString Selection::INDEX_KEY = "frame";
const QString Selection::LOW_KEY = "frameStart";
const QString Selection::LOW_KEY_USER = "frameStartUser";
const QString Selection::CLASS_NAME = "Selection";
const QString Selection::SELECTIONS = "selections";
const QString Selection::IMAGE = "Image";
const QString Selection::REGION = "Region";
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
    m_state.insertValue<int>(LOW_KEY_USER, 0 );
    m_state.insertValue<int>( HIGH_KEY, 1);
    m_state.insertValue<int>( HIGH_KEY_USER, 0 );
    m_state.insertValue<int>( INDEX_KEY, 0);
    m_state.flushState();
}



int Selection::_getValue(const QString& key) const {
    int val = m_state.getValue<int>( key );
    return val;
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

int Selection::getLowerBoundUser() const {
    return _getValue( LOW_KEY_USER );
}

int Selection::getUpperBound() const {
    return _getValue( HIGH_KEY);
}

int Selection::getUpperBoundUser() const {
    return _getValue( HIGH_KEY_USER );
}

QString Selection::setLowerBoundUser( int userLowerBound ){
    QString result;
    if ( userLowerBound >= 0 ){
        int oldUserLowerBound = getLowerBoundUser();
        if ( userLowerBound != oldUserLowerBound ){
            //The user lower bound must be greater than or equal to the lower bound limit
            int lowerLimit = getLowerBound();
            if ( userLowerBound >= lowerLimit ){
                //The user lower bound must be less than or equal to the user upper bound.
                int upperBound = getUpperBoundUser();
                if ( userLowerBound <= upperBound ){
                    m_state.setValue<int>(LOW_KEY_USER, userLowerBound );
                    m_state.flushState();
                }
                else {
                    result = "User lower bound must be less than or equal to "+QString::number(upperBound);
                }
            }
            else {
                result = "User upper bound must be at least "+QString::number(lowerLimit);
            }
        }
    }
    else {
        result="User lower bound must be nonnegative: "+QString::number(userLowerBound);
    }
    return result;
}

void Selection::setUpperBound(int newUpperBound) {
    if ( newUpperBound >= 1 ){
        int oldUpperBound = m_state.getValue<int>(HIGH_KEY );
        if ( newUpperBound != oldUpperBound ){
            m_state.setValue<int>(HIGH_KEY, newUpperBound);
            //Also set the user upper bound if the user bound was either tracking
            //the upper bound or the user upper bound is too large.
            int userUpper = m_state.getValue<int>(HIGH_KEY_USER );
            if ( userUpper == (oldUpperBound-1) || userUpper > ( newUpperBound-1) ){
                int newUpper = newUpperBound - 1;
                m_state.setValue<int>(HIGH_KEY_USER, newUpper );
                int index = m_state.getValue<int>(INDEX_KEY);
                if ( index > newUpper ){
                    setIndex( newUpper );
                }
            }
            m_state.flushState();
        }
    }
}

QString Selection::setUpperBoundUser( int userUpperBound ){
    QString result;
    if ( userUpperBound >= 0 ){
        int oldUserUpperBound = getUpperBoundUser();
        if ( userUpperBound != oldUserUpperBound ){
            //The user upper bound must be less than or equal to the upper bound limit
            int upperLimit = getUpperBound();
            if ( userUpperBound <= upperLimit -1 ){
                //The user upper bound must be at least as large as the user lower bound.
                int lowerBound = getLowerBoundUser();
                if ( userUpperBound >= lowerBound ){
                    m_state.setValue<int>(HIGH_KEY_USER, userUpperBound );
                    int index = m_state.getValue<int>(INDEX_KEY);
                    if ( index > userUpperBound ){
                        setIndex( userUpperBound );
                    }
                    m_state.flushState();
                }
                else {
                    result = "User upper bound must be less than or equal to "+QString::number(lowerBound);
                }
            }
            else {
                result = "User upper bound must be less than "+QString::number(upperLimit);
            }
        }
    }
    else {
        result="User upper bound must be nonnegative: "+QString::number(userUpperBound);
    }
    return result;
}

void Selection::setLowerBound(int newLowerBound) {
    if ( newLowerBound >= 0 ){
        int oldLowerBound = m_state.getValue<int>(LOW_KEY );
        if ( newLowerBound != oldLowerBound ){
            m_state.setValue<int>(LOW_KEY, newLowerBound);
            //Also set the user lower bound if the user bound was either tracking
            //the lower bound or the user lower bound is too small.
            int userLower = m_state.getValue<int>(LOW_KEY_USER );
            if ( userLower == oldLowerBound || userLower > newLowerBound ){
                m_state.setValue<int>(LOW_KEY_USER, newLowerBound );
                int index = m_state.getValue<int>(INDEX_KEY);
                if ( index < newLowerBound ){
                    setIndex( newLowerBound );
                }
            }
            m_state.flushState();
        }
    }
}




QString Selection::setIndex(int frameValue) {
    QString result;
    if ( frameValue >= 0 ){
        int upperBound = getUpperBoundUser();
        int lowerBound = getLowerBoundUser();
        if ( lowerBound <= frameValue && frameValue <= upperBound ){
            int oldValue = m_state.getValue<int>(INDEX_KEY );
            if ( oldValue != frameValue ){
                m_state.setValue<int>(INDEX_KEY, frameValue);
                m_state.flushState();
                emit indexChanged( );
            }
        }
        else {
            result = "Selection index "+ QString::number(frameValue)+" must be between "+
                    QString::number(lowerBound) + " and " + QString::number(upperBound);
        }
    }
    else {
        result = "Selection index must be nonnegative: "+ QString::number( frameValue );
    }
    return result;
}

Selection::~Selection() {

}
}
}
