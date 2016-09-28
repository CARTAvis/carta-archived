/***
 * A rectangular region of an image.
 */

#pragma once

#include "Region.h"

namespace Carta {

namespace Data {

class RegionRectangle : public Region {

	Q_OBJECT

    friend class DataFactory;

public:

    /**
     * Returns the height of the rectangular region.
     * @return - the height of the rectangle.
     */
    double getHeight() const;

    /**
     * Return the model associated with this region.
     * @return - a model for the region.
     */
    virtual std::shared_ptr<Carta::Lib::Regions::RegionBase> getModel() const Q_DECL_OVERRIDE;

    /**
     * Returns the top left corner of the rectangle.
     * @return - the top left corner of the rectangle.
     */
    QPointF getTopLeft() const;

    /**
     * Returns the width of the rectangle.
     * @return - the width of the rectangle.
     */
    double getWidth() const;

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
	 * Set the height of the bounding box of the region.
	 * @param value - the height of the region bounding box.
	 * @return - true if the height was successfully set; false, otherwise.
	 */
	virtual bool setHeight( double value ) Q_DECL_OVERRIDE;

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
     * Set the width of the bounding box of the region.
     * @param value - the width of the region bounding box.
     * @return - true if the width was successfully set; false, otherwise.
     */
    virtual bool setWidth( double value ) Q_DECL_OVERRIDE;

    /**
     * Returns a json representation of the region.
     * @return - a json representation of the region.
     */
    virtual QJsonObject toJSON() const Q_DECL_OVERRIDE;

    virtual ~RegionRectangle();

    const static QString CLASS_NAME;

private slots:

    	void _updateStateFromJson( const QJsonObject& json );

private:

    void _initializeState();

    static bool m_registered;

    /**
     * Construct a region.
     */
    RegionRectangle( const QString& path, const QString& id );
    class Factory;

    RegionRectangle( const RegionRectangle& other);
    RegionRectangle& operator=( const RegionRectangle& other );
};
}
}
