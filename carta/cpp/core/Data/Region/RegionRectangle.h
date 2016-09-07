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
	 * Notification that a drag event has started on the shape.
	 * @param pt - the starting point of the drag.
	 */
	virtual void handleDragStart( const QPointF & pt ) Q_DECL_OVERRIDE;

	/**
		 * Notification that a touch event has started on the shape.
		 * @param pt - the location of the touch.
		 */
	virtual void handleTouch( const QPointF& pt );

	/**
	 * Set the underlying model for the region.
	 * @param model - the region model.
	 */
    virtual void setModel( Carta::Lib::Regions::RegionBase* model );

    /**
     * Set the width and height of the rectangle.
     * @param width - the width of the rectangle.
     * @param height - the height of the rectangle.
     */
    void setRectangleSize( double width, double height );

    /**
     * Set the top left corner of the rectangle.
     * @param pt - the top left corner of the rectangle.
     */
    void setTopLeft( const QPointF& pt );

    /**
     * Returns a json representation of the region.
     * @return - a json representation of the region.
     */
    virtual QJsonObject toJSON() const Q_DECL_OVERRIDE;

    virtual ~RegionRectangle();

    const static QString CLASS_NAME;

protected:

    /**
     * Restore the region state.
     * @param state - a string representation of the state to restore.
     */
    virtual void _restoreState( const QString& state );

private slots:

    	void _updateStateFromJson( const QJsonObject& json );

private:

    void _initializeState();

    void _setRectPt( const QPointF& pt );


    static bool m_registered;
    bool m_constructed;

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
