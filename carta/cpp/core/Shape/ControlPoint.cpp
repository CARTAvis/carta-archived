#include "ControlPoint.h"

namespace Carta {
namespace Shape {

const QPointF & ControlPoint::getPosition() const {
	return m_position;
}

void * ControlPoint::getUserData() const {
	return m_userData;
}

bool ControlPoint::isMovable() const {
	return m_movable;
}

void ControlPoint::setMovable( bool flag ) {
	m_movable = flag;
}

void ControlPoint::setPosition( const QPointF & pt ) {
	m_position = pt;
}

void ControlPoint::setUserData( void * data ) {
	m_userData = data;
}

}
}
