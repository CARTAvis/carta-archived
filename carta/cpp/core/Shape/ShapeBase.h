/**
 * Base class for shapes.
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/InputEvents.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "IShape.h"

namespace Carta {
namespace Shape {

class ShapeBase : public IShape {
	CLASS_BOILERPLATE( ShapeBase );

public:

	/**
	 * Notification that the editable state of the shape has changed.
	 */
	/// reimplement this if your shape needs to know whether edit mode changed
	/// default implementation does nothing
	virtual void editableChanged() override;

	/**
	 * Return any user data associated with the shape.
	 * @return - user data associated with the shape.
	 */
	void * getUserData() const override;

	/**
	 * Handle an unspecified event.
	 * @param ev - an event.
	 */
	virtual void handleEvent( Carta::Lib::InputEvents::JsonEvent & ev ) override;

	/**
	 * Returns whether or not the shape is participating in event delivery.
	 * @return - true if the shape is participating in event delivery; false otherwise.
	 */
	bool isActive() const override;

	/**
	 * Returns whether or not the shape can be deleted.
	 * @return - whether or not the shape can be deleted.
	 */
	bool isDeletable() const override;

	/**
	 * Return information about the cursor position withen the shape.
	 * @retun - information about the cursor position in the shape.
	 */
	QString getCursor() const override;

	/**
	 * Return the vector graphics for the shape.
	 * @return - the shape vector graphics.
	 */
	virtual Carta::Lib::VectorGraphics::VGList getVGList() const override;

	/**
	 * Notification that a drag event has started on the shape.
	 * @param pt - the starting point of the drag.
	 */
	virtual void handleDragStart( const QPointF & pt ) override;

	/**
	 * Notification of a drag as it progresses.
	 * @param pt - the current location of the drag.
	 */
	virtual void handleDrag( const QPointF & pt ) override;

	/**
	 * Notification that a drag event has ended.
	 * @param pt - the ending point of the drag.
	 */
	virtual void handleDragDone( const QPointF & pt ) override;

	/**
	 * Returns whether or not the shapee is editable.
	 * @return - true if the shape is editable; false, otherwise.
	 */
	bool isEditable() const override;

	/**
	 * Returns whether or not the shape is being hovered.
	 * @return - true if the shape is hovered; false otherwise.
	 */
	bool isHovered() const override;

	/**
	 * Returns whether or not the point is inside the shape or not.
	 * @return - true if the point is inside the shape; false otherwise.
	 */
	/// in order to deliver events to shapes, we need to know whether a point
	/// is inside the shape or not
	virtual bool isPointInside( const QPointF & pt ) const override;

	/**
	 * Returns whether or not the shape is selected.
	 * @return - true if the shape is selected; false otherwise.
	 */
	bool isSelected() const override;

	/**
	 * Set whether or not the shape participates in user events.
	 * @param flag - true if the shape participates in user events; false,
	 * 	otherwise.
	 */
	void setActive( bool flag ) override;

	/**
	 * Set information about the current cursor position in the shape.
	 * @param value - information about the cursor position in the shape.
	 */
	void setCursor( const QString & value ) override;

	/**
	 * Set whether or not the shape can be deleted.
	 * @param value - true if the shape can be deleted; false otherwise.
	 */
	void setDeletable( bool value ) override;


	/**
	 * Set whether or not the shape is editable.
	 * @param editable - true if the shape is editable; false otherwise.
	 */
	void setEditable( bool flag ) override;

	/**
	 * Sets whether or not the shape is in a hovered state.
	 * @param hovered - true if the shape is hovered; false otherwise.
	 * @return - true if the underlying VG graphics need to be updated; false,
	 * 	otherwise.
	 */
	/// Default is to return false. Reimplement if the shape needs to be redrawn as
	/// a result of changing hover state.
	virtual bool setHovered( bool value ) override;

	/**
	 * Set whether or not the shape is selected;
	 * @param value - true if the shape is selected; false otherwise.
	 */
	void setSelected( bool value ) override;

	/**
	 * Set user data associated with the shape.
	 * @param value - user data associated with the shape.
	 */
	void setUserData( void * value ) override;

protected:
	static QPen shadowPen;
	static QBrush shadowBrush;
	static QPen outlinePen;
	static QBrush controlPointBrush;



private:
	bool m_active = true;
	bool m_deletable = false;
	bool m_editable = false;
	bool m_hovered = false;
	QString m_cursor;
	bool m_selected = false;
	void * m_userData = nullptr;
};

}
}
