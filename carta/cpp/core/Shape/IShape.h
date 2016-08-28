/**
 * Interface for shapes which can be edited.
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/InputEvents.h"
#include "CartaLib/VectorGraphics/VGList.h"

namespace Carta {
namespace Shape {

class IShape {


public:

	/**
	 * Notification that the editable state of the shape has changed.
	 */
	virtual void editableChanged() = 0;

	/**
	 * Return information about the cursor position withen the shape.
	 * @retun - information about the cursor position in the shape.
	 */
	virtual QString getCursor() const = 0;

	/**
	 * Return any user data associated with the shape.
	 * @return - user data associated with the shape.
	 */
	virtual void * getUserData() const = 0;

	/**
	 * Return the vector graphics for the shape.
	 * @return - the shape vector graphics.
	 */
	virtual Carta::Lib::VectorGraphics::VGList getVGList() const = 0;


	/**
	 * Notification of a drag as it progresses.
	 * @param pt - the current location of the drag.
	 */
	virtual void handleDrag( const QPointF & pt ) = 0;

	/**
	 * Notification that a drag event has ended.
	 * @param pt - the ending point of the drag.
	 */
	virtual void handleDragDone( const QPointF & pt ) = 0;

	/**
	 * Notification that a drag event has started on the shape.
	 * @param pt - the starting point of the drag.
	 */
	virtual void handleDragStart( const QPointF & pt ) = 0;

	/**
	 * Handle an unspecified event.
	 * @param ev - an unspecified event.
	 */
	virtual void handleEvent( Carta::Lib::InputEvents::JsonEvent & ev ) = 0;

	/**
	 * Returns whether or not the shape is participating in event delivery.
	 * @return - true if the shape is participating in event delivery; false otherwise.
	 */
	virtual bool isActive() const = 0;


	/**
	 * Returns whether or not the shape can be deleted.
	 * @return - whether or not the shape can be deleted.
	 */
	virtual bool isDeletable() const = 0;


	/**
	 * Returns whether or not the shapee is editable.
	 * @return - true if the shape is editable; false, otherwise.
	 */
	virtual bool isEditable() const = 0;

	/**
	 * Returns whether or not the shape is being hovered.
	 * @return - true if the shape is hovered; false otherwise.
	 */
	virtual bool isHovered() const = 0;

	/**
	 * Returns whether or not the point is inside the shape or not.
	 * @return - true if the point is inside the shape; false otherwise.
	 */
	virtual bool isPointInside( const QPointF & pt ) const = 0;

	/**
	 * Returns whether or not the shape is selected.
	 * @return - true if the shape is selected; false otherwise.
	 */
	virtual bool isSelected() const = 0;

	/**
	 * Set whether or not the shape participates in user events.
	 * @param flag - true if the shape participates in user events; false,
	 * 	otherwise.
	 */
	virtual void setActive( bool flag ) = 0;

	/**
	 * Set information about the current cursor position in the shape.
	 * @param value - information about the cursor position in the shape.
	 */
	virtual void setCursor( const QString & value ) = 0;

	/**
	 * Set whether or not the shape can be deleted.
	 * @param value - true if the shape can be deleted; false otherwise.
	 */
	virtual void setDeletable( bool value ) = 0;

	/**
	 * Set whether or not the shape is editable.
	 * @param editable - true if the shape is editable; false otherwise.
	 */
	virtual void setEditable( bool editable ) = 0;

	/**
	 * Sets whether or not the shape is in a hovered state.
	 * @param hovered - true if the shape is hovered; false otherwise.
	 * @return - true if the underlying VG graphics need to be updated; false,
	 * 	otherwise.
	 */
	virtual bool setHovered( bool hovered ) = 0;

	/**
	 * Set whether or not the shape is selected;
	 * @param value - true if the shape is selected; false otherwise.
	 */
	virtual void setSelected( bool value ) = 0;

	/**
	 * Set user data associated with the shape.
	 * @param value - user data associated with the shape.
	 */
	virtual void setUserData( void * value ) = 0;

};

}
}
