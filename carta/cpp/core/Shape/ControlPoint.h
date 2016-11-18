/**
 * A point that is capable of changing a shape through user interaction.
 **/

#pragma once
#include "CartaLib/CartaLib.h"
#include <QPointF>

namespace Carta {

namespace Shape {

class ControlPoint
{
	CLASS_BOILERPLATE( ControlPoint );

public:

	/**
	 * Return the location of the control point.
	 * @return - the location of the control point.
	 */
	const QPointF & getPosition() const;

	/**
	 * Return additional data associated with the control point.
	 * @return - a pointer to additional data associated with the control point.
	 */
	void * getUserData() const;

	/**
	 * Returns whether or not the control point can be moved.
	 * @return - true if the control point can be moved; false otherwise.
	 */
	bool isMovable() const;

	/**
	 * Set whether or not the control point can be moved.
	 * @param movable - true if the control point can be moved; false, otherwise.
	 */
	void setMovable( bool movable );

	/**
	 * Set the position of the control point.
	 * @param pt - the location of the control point.
	 */
	void setPosition( const QPointF & pt );

	/**
	 * Set additional data associated with the control point.
	 * @param data - additional data associated with the control point.
	 */
	void setUserData( void * data );

private:

	QPointF m_position = QPointF( 0.0, 0.0 );
	bool m_movable = true;
	void * m_userData = nullptr;
};
}
}
