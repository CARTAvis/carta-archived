/**
 * Base class for shapes.
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/InputEvents.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include <QObject>

namespace Carta {
namespace Shape {

class ControlPointEditable;

class ShapeBase : public QObject {
	Q_OBJECT

public:

	ShapeBase( QObject* parent = nullptr );

	/**
	 * Notification that the editable state of the shape has changed.
	 */
	virtual void editModeChanged();

	/**
	 * Return any user data associated with the shape.
	 * @return - user data associated with the shape.
	 */
	void * getUserData() const;

	/**
	 * Handle an unspecified event.
	 * @param ev - an event.
	 */
	virtual void handleEvent( Carta::Lib::InputEvents::JsonEvent & ev );

	/**
	 * Returns whether or not the shape is participating in event delivery.
	 * @return - true if the shape is participating in event delivery; false otherwise.
	 */
	bool isActive() const;

	/**
	 * Returns whether or not the shape can be deleted.
	 * @return - whether or not the shape can be deleted.
	 */
	bool isDeletable() const;

	/**
	 * Return the center of the bounding box containing the shape.
	 * @return - the center of the bounding box containing the shape.
	 */
	virtual QPointF getCenter() const = 0;

	/**
	 * Return information about the cursor position withen the shape.
	 * @retun - information about the cursor position in the shape.
	 */
	QString getCursor() const;

	/**
	 * Return the size of the bounding box containing the shape.
	 * @return - the size of the bounding box containing the shape.
	 */
	virtual QSizeF getSize() const = 0;

	/**
	 * Return the vector graphics for the shape.
	 * @return - the shape vector graphics.
	 */
	virtual Carta::Lib::VectorGraphics::VGList getVGList() const;

	/**
	 * Notification that a drag event has started on the shape.
	 * @param pt - the starting point of the drag.
	 */
	virtual void handleDragStart( const QPointF & pt );

	/**
	 * Notification of a drag as it progresses.
	 * @param pt - the current location of the drag.
	 */
	virtual void handleDrag( const QPointF & pt );

	/**
	 * Notification that a drag event has ended.
	 * @param pt - the ending point of the drag.
	 */
	virtual void handleDragDone( const QPointF & pt );

	/**
	 * Returns whether or not the shape is in edit mode.
	 * @return - true if the shape is in edit mode (being created or adjusted); false, otherwise.
	 */
	bool isEditMode() const;

	/**
	 * Returns whether or not the shape is being hovered.
	 * @return - true if the shape is hovered; false otherwise.
	 */
	bool isHovered() const;

	/**
	 * Returns whether or not the point is inside the shape or not.
	 * @return - true if the point is inside the shape; false otherwise.
	 */
	/// in order to deliver events to shapes, we need to know whether a point
	/// is inside the shape or not
	virtual bool isPointInside( const QPointF & pt ) const;

	/**
	 * Returns whether or not the shape is selected.
	 * @return - true if the shape is selected; false otherwise.
	 */
	bool isSelected() const;

	/**
	 * Set whether or not the shape participates in user events.
	 * @param flag - true if the shape participates in user events; false,
	 * 	otherwise.
	 */
	void setActive( bool flag );

	/**
	 * Set information about the current cursor position in the shape.
	 * @param value - information about the cursor position in the shape.
	 */
	void setCursor( const QString & value );

	/**
	 * Set whether or not the shape can be deleted.
	 * @param value - true if the shape can be deleted; false otherwise.
	 */
	void setDeletable( bool value );


	/**
	 * Set whether or not the shape is in edit mode.
	 * @param editMode - true if the shape is being created (points added); false otherwise.
	 */
	void setEditMode( bool editMode );

	/**
	 * Sets whether or not the shape is in a hovered state.
	 * @param hovered - true if the shape is hovered; false otherwise.
	 */
	/// Default is to return false. Reimplement if the shape needs to be redrawn as
	/// a result of changing hover state.
	virtual void setHovered( bool value );

	/**
	 * Set a model for the shape.
	 * @param json - the shape model (corner points, center, etc).
	 */
	virtual void setModel( const QJsonObject& json ) = 0;

	/**
	 * Set whether or not the shape is selected;
	 * @param value - true if the shape is selected; false otherwise.
	 */
	virtual void setSelected( bool value );

	/**
	 * Set user data associated with the shape.
	 * @param value - user data associated with the shape.
	 */
	void setUserData( void * value );

signals:

	void shapeChanged( const QJsonObject& jsonObj );

protected:
	virtual void _editShadow( const QPointF& pt ) = 0;
	virtual void _moveShadow( const QPointF& pt ) = 0;
	virtual void _syncShadowToCPs() = 0;
	const static QPen shadowPen;
	const static QBrush shadowBrush;
	const static QPen outlinePen;
	bool m_dragMode;
	QPointF m_dragStart;
	int m_dragControlIndex;
	std::vector < std::shared_ptr<ControlPointEditable> > m_controlPoints;

private:
	bool m_active = true;
	bool m_deletable = false;
	bool m_editMode = false;
	bool m_hovered = false;
	QString m_cursor;
	bool m_selected = false;
	void * m_userData = nullptr;

};

}
}
