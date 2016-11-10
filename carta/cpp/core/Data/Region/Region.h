/***
 * A region of an image.
 */

#pragma once

#include "State/StateInterface.h"
#include "State/ObjectManager.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "CartaLib/InputEvents.h"
#include <QObject>

namespace Carta {

namespace Lib {
namespace Regions {
	class RegionBase;
}
}

namespace Shape {
	class ShapeBase;
}


namespace Data {

class Region : public QObject, public Carta::State::CartaObject {

Q_OBJECT

	friend class RegionFactory;
	friend class RegionControls;

public:

	/**
	 * Returns the center point of the bounding box enclosing the region.
	 * @return - the center point of the bounding box enclosing the region.
	 */
	QPointF getCenter() const;

	/**
	 * Return information about the cursor position withen the shape.
	 * @retun - information about the cursor position in the shape.
	 */
	virtual QString getCursor() const;

	/**
	 * Returns the size of the bounding box enclosing the regions.
	 * @return - the size of the bounding box enclosing the region.
	 */
	QSizeF getSize() const;

	/**
	 * Return the information associated with this region.
	 * @return - information about the region.
	 */
	virtual std::shared_ptr<Carta::Lib::Regions::RegionBase> getModel() const;


	/**
	 * Return the name of the region.
	 * @return - the name of the region.
	 */
	QString getRegionName() const;

	/**
	 * Return the case-sensitive region type corresponding to the given string representation.
	 * @param regionTypeStr - a string representation of a region shape such as "ellipse".
	 * @return - the corresponding case-sensitive region type or an empty string if there is no
	 *      such type.
	 */
	static QString getRegionType( const QString& regionTypeStr );

	/**
	 * Return the type of region, which corresponds to its shape.
	 * @return - the type of region.
	 */
	QString getRegionType() const;


	/**
	 * Return any user data associated with the shape.
	 * @return - user data associated with the shape.
	 */
	virtual void * getUserData() const;

	/**
	 * Return the vector graphics for the shape.
	 * @return - the shape vector graphics.
	 */
	Carta::Lib::VectorGraphics::VGList getVGList() const;


	/**
	 * Notification on an overall drag event from mouse down to mouse up.
	 * @param ev - the drag event.
	 * @param location - the position of the cursor translated to the image coordinate system.
	 */
	void handleDrag( const Carta::Lib::InputEvents::Drag2Event& ev, const QPointF& location );

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
	 * Notification that a drag event has started on the shape.
	 * @param pt - the starting point of the drag.
	 */
	virtual void handleDragStart( const QPointF & pt );

	/**
	 * Notification of a hover event.
	 * @param pt - the current location of the mouse.
	 */
	virtual void handleHover( const QPointF& pt );

	/**
	 * Notification of a click event.
	 * @param pt - the current location of the mouse.
	 */
	virtual void handleTouch( const QPointF& pt );

	/**
	 * Notification of a double click event.
	 * @param pt - the current location of the mouse.
	 */
	virtual void handleTapDouble( const QPointF& pt );

	/**
	 * Handle an unspecified event.
	 * @param ev - an unspecified event.
	 */
	virtual void handleEvent( Carta::Lib::InputEvents::JsonEvent & ev );

	/**
	 * Returns whether or not the shape is participating in event delivery.
	 * @return - true if the shape is participating in event delivery; false otherwise.
	 */
	virtual bool isActive() const;

	/**
	 * Returns whether or not the region can be dragged.
	 * @return - true if the region can be dragged; false, otherwise.
	 */
	bool isDraggable() const;

	/**
	 * Returns whether or not the shape is being edited.
	 * @return - true if the shape is being edited; false, otherwise.
	 */
	virtual bool isEditMode() const;

	/**
	 * Returns whether or not the shape is being hovered.
	 * @return - true if the shape is hovered; false otherwise.
	 */
	virtual bool isHovered() const;

	/**
	 * Returns whether or not the shape is selected.
	 * @return - true if the shape is selected; false otherwise.
	 */
	virtual bool isSelected() const;

	/**
	 * Set whether or not the shape participates in user events.
	 * @param flag - true if the shape participates in user events; false,
	 * 	otherwise.
	 */
	virtual void setActive( bool flag );

	/**
	 * Set the center of the region.
	 * @param center - the center point of the region.
	 * @return - true if the center has been changed; false, otherwise.
	 */
	virtual bool setCenter( const QPointF& center ) = 0;

	/**
	 * Set information about the current cursor position in the shape.
	 * @param value - information about the cursor position in the shape.
	 */
	virtual void setCursor( const QString & value );

	/**
	 * Set whether or not the shape can be deleted.
	 * @param value - true if the shape can be deleted; false otherwise.
	 */
	virtual void setDeletable( bool value );

	/**
	 * Set whether or not the shape is being edited.
	 * @param editable - true if the shape is being edited; false otherwise.
	 */
	virtual void setEditMode( bool editable );

	/**
	 * Set the height of the bounding box of the region.
	 * @param value - the height of the region bounding box.
	 * @return - true if the height was successfully set; false, otherwise.
	 */
	virtual bool setHeight( double value );

	/**
	 * Sets whether or not the shape is in a hovered state.
	 * @param hovered - true if the shape is hovered; false otherwise.
	 * @return - true if the underlying VG graphics need to be updated; false,
	 * 	otherwise.
	 */
	virtual bool setHovered( bool hovered );

	/**
	 * Set the underlying model for the region.
	 * @param model - the region model.
	 */
	virtual void setModel( Carta::Lib::Regions::RegionBase* model );

	/**
	 * Set the major radius of the region.
	 * @param length - the length of the region's major radius.
	 * @param changed - set to true if the major radius is changed; false otherwise.
	 * @return - an error message if there was a problem setting the major radius; an
	 * 		empty string otherwise.
	 */
	//Default implementation does nothing since most shapes do not have a radius.
	virtual QString setRadiusMajor( double length, bool* changed );

	/**
	 * Set the minor radius of the region.
	 * @param length - the length of the region's minor radius.
	 * @param changed - set to true if the minor radius is changed; false otherwise.
	 * @return - an error message if there was a problem setting the minor radius; an
	 * 		empty string otherwise.
	 */
	//Default implementation does nothing since most shapes do not have a radius.
	virtual QString setRadiusMinor( double length, bool* changed );

	/**
	 * Set a user-customized name for the region.
	 * @param name - a user name for the region.
	 * @return - an error message if the region name was not correctly set; an empty string
	 *  otherwise.
	 */
	QString setRegionName( const QString& name );

	/**
	 * Set whether or not the shape is selected;
	 * @param value - true if the shape is selected; false otherwise.
	 */
	void setSelected( bool value );

	/**
	 * Set the width of the bounding box of the region.
	 * @param value - the width of the region bounding box.
	 * @return - true if the width was successfully set; false, otherwise.
	 */
	virtual bool setWidth( double value );

	/**
	 * Set user data associated with the shape.
	 * @param value - user data associated with the shape.
	 */
	virtual void setUserData( void * value );

	/**
	 * Returns a representation of the region as a json object.
	 * @return - a representation of the region as a json objeect.
	 */
	virtual QJsonObject toJSON() const;

	virtual ~Region();

signals:

	/**
	 * Notification that the region has been created.
	 */
	void editDone();

	/**
	 * Notification that the region has changed its selection status.
	 * @param id - an identifier for this region.
	 */
	void regionSelectionChanged( const QString& id );

	/**
	 * Notification that the shape of the region changed.
	 */
	void regionShapeChanged( );

protected:

	double _getErrorMargin() const;

	/**
	 * Restore the region state.
	 * @param state - a string representation of the state to restore.
	 */
	virtual void _restoreState( const QString& state );

	void _updateShapeFromState();

	void _updateName();

	/**
	 * Construct a region.
	 */
	Region( const QString& className, const QString& path, const QString& id );

	const static QString ACTIVE;
	const static QString CUSTOM_NAME;
	const static QString HOVERED;
	const static QString REGION_TYPE;
	const static int SIGNIFICANT_DIGITS;
	std::shared_ptr<Shape::ShapeBase> m_shape;

private:



	/**
	 * Return the region state as a string.
	 * @return - the region state as a string.
	 */
	QString _getStateString() const;

	void _initializeState();



	Region( const Region& other);
	Region& operator=( const Region& other );
};
}
}
