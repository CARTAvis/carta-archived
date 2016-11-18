/***
 * A point region of an image.
 */

#pragma once

#include "Region.h"

namespace Carta {

namespace Data {

class RegionPoint : public Region {

	Q_OBJECT

    friend class DataFactory;

public:

    /**
     * Return the model associated with this region.
     * @return - a model for the region.
     */
    virtual std::shared_ptr<Carta::Lib::Regions::RegionBase> getModel() const Q_DECL_OVERRIDE;

    /**
     * Notification that a drag event has ended.
     * @param pt - the ending point of the drag.
     */
    virtual void handleDragDone( const QPointF & pt ) Q_DECL_OVERRIDE;

	/**
	 * Notification that a drag event has started on the shape.
	 * @param pt - the starting point of the drag.
	 */
	virtual void handleDragStart( const QPointF & pt ) Q_DECL_OVERRIDE;

	/**
	 * Handle a touch event.
	 * @param pt - the touch event.
	 */
	virtual void handleTouch( const QPointF & pt ) Q_DECL_OVERRIDE;

	/**
	 * Set the underlying model for the region.
	 * @param model - the region model.
	 */
    virtual void setModel( Carta::Lib::Regions::RegionBase* model );

    /**
     * Set the center of the rectangle.
     * @param pt - the center of the rectangle.
     */
    virtual bool setCenter( const QPointF& pt ) Q_DECL_OVERRIDE;


    /**
     * Returns a json representation of the region.
     * @return - a json representation of the region.
     */
    virtual QJsonObject toJSON() const Q_DECL_OVERRIDE;

    virtual ~RegionPoint();

    const static QString CLASS_NAME;

private slots:

    	void _updateStateFromJson( const QJsonObject& json );

private:

    void _initializeState();

    static bool m_registered;

    /**
     * Construct a region.
     */
    RegionPoint( const QString& path, const QString& id );
    class Factory;

    RegionPoint( const RegionPoint& other);
    RegionPoint& operator=( const RegionPoint& other );
};
}
}
