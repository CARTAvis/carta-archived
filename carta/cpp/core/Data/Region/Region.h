/***
 * A region of an image.
 */

#pragma once

#include "State/StateInterface.h"
#include "State/ObjectManager.h"
#include "Shape/IShape.h"

namespace Carta {

namespace Lib {

namespace Regions {
	class RegionBase;
}
}

namespace Data {

class Region : public Carta::State::CartaObject, public Carta::Shape::IShape {

	friend class RegionFactory;
	friend class RegionControls;

public:

	/**
	 * Notification that the editable state of the shape has changed.
	 */
	virtual void editableChanged();

	/**
	 * Return information about the cursor position withen the shape.
	 * @retun - information about the cursor position in the shape.
	 */
	virtual QString getCursor() const;

	/**
	 * Return the information associated with this region.
	 * @return - information about the region.
	 */
	virtual std::shared_ptr<Carta::Lib::Regions::RegionBase> getModel() const = 0;


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
	 * Returns whether or not the shape can be deleted.
	 * @return - whether or not the shape can be deleted.
	 */
	virtual bool isDeletable() const;


	/**
	 * Returns whether or not the shapee is editable.
	 * @return - true if the shape is editable; false, otherwise.
	 */
	virtual bool isEditable() const;

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
	 * Set whether or not the shape is editable.
	 * @param editable - true if the shape is editable; false otherwise.
	 */
	virtual void setEditable( bool editable );

	/**
	 * Sets whether or not the shape is in a hovered state.
	 * @param hovered - true if the shape is hovered; false otherwise.
	 * @return - true if the underlying VG graphics need to be updated; false,
	 * 	otherwise.
	 */
	virtual bool setHovered( bool hovered );

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
	virtual void setSelected( bool value );

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

protected:

	/**
	 * Restore the region state.
	 * @param state - a string representation of the state to restore.
	 */
	virtual void _restoreState( const QString& state );

	/**
	 * Construct a region.
	 */
	Region( const QString& className, const QString& path, const QString& id );

	const static QString ACTIVE;
	const static QString HOVERED;
	const static QString REGION_TYPE;
	bool m_regionNameSet;
	std::shared_ptr<IShape> m_shape;

private:

	/**
	 * Return the region state as a string.
	 * @return - the region state as a string.
	 */
	QString _getStateString() const;

	void _initializeCallbacks();
	void _initializeState();


	Region( const Region& other);
	Region& operator=( const Region& other );
};
}
}
