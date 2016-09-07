/***
 * A polygonal region of an image.
 */

#pragma once

#include "Region.h"

namespace Carta {

namespace Data {

class RegionPolygon : public Region {

	Q_OBJECT

public:

	/**
	 * Add a corner to the region.
	 * @param pt - the specific corner to add.
	 */
	void addCorner( const QPointF& pt );

    /**
     * Return a specific corner of the region.
     * @param index - the index of the corner.
     * @param valid - set to true if there is a valid corner at the specified
     * 		index; false otherwise.
     */
    QPointF getCorner( int index, bool* valid ) const;

    /**
     * Return a list of region corners.
     * @return - a list of region corners.
     */
    std::vector<QPointF > getCorners() const;

    /**
     * Return the information associated with this region.
     * @return - information about the region.
     */
    virtual std::shared_ptr<Carta::Lib::Regions::RegionBase> getModel() const Q_DECL_OVERRIDE;

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
	 * Set the underlying model for the region.
	 * @param model - the region model.
	 */
    virtual void setModel( Carta::Lib::Regions::RegionBase* model );

    /**
     * Returns a json representation of the region.
     * @return - a json representation of the region.
     */
    virtual QJsonObject toJSON() const Q_DECL_OVERRIDE;

    virtual ~RegionPolygon();

    const static QString CLASS_NAME;

protected:

    /**
     * Restore the region state.
     * @param state - a string representation of the state to restore.
     */
    virtual void _restoreState( const QString& state ) Q_DECL_OVERRIDE;

private slots:

	void _updateStateFromJson( const QJsonObject& json );

private:

	/**
	 * Add corners to the region.
	 * @param corners - a list of corners to add to the region.
	 */
    void _addCorners( const std::vector< QPointF >& corners );
    void _closePolygon();
    void _initializeState();
    void _insertCorner( int index, const QPointF& corner );

    static bool m_registered;

    /**
     * Construct a region.
     */
    RegionPolygon( const QString& path, const QString& id );
    class Factory;

    RegionPolygon( const RegionPolygon& other);
    RegionPolygon& operator=( const RegionPolygon& other );

};
}
}
