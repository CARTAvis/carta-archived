/***
 * A elliptical region of an image.
 */

#pragma once

#include "Region.h"

namespace Carta {

namespace Data {

class RegionEllipse : public Region {

	Q_OBJECT

    friend class DataFactory;

public:

	/**
	 * Return the position angle of the ellipse.
	 * @return - the position angle of the ellipse.
	 */
    double getAngle() const;

    /**
     * Return the center of the ellipse.
     * @return - the center of the ellipse.
     */
    QPointF getCenter() const;

    /**
     * Return the model associated with this region.
     * @return - a model for the region.
     */
    virtual std::shared_ptr<Carta::Lib::Regions::RegionBase> getModel() const Q_DECL_OVERRIDE;

    /**
     * Return the major radius of the ellipse.
     * @return - the major radius of the ellipse.
     */
    double getRadiusMajor() const;

    /**
     * Return the minor radius of the ellipse.
     * @return - the minor radius of the ellipse.
     */
    double getRadiusMinor() const;

	/**
	 * Notification that a drag event has started on the shape.
	 * @param pt - the starting point of the drag.
	 */
	virtual void handleDragStart( const QPointF & pt ) Q_DECL_OVERRIDE;

	/**
	 * Notification of a drag as it progresses.
	 * @param pt - the current location of the drag.
	 */
	virtual void handleDragDone( const QPointF & pt ) Q_DECL_OVERRIDE;

	/**
	 * Set the center of the ellipse.
	 * @param center - the center of the ellipse.
	 * @return - true if the center was changed; false otherwise.
	 */
	virtual bool setCenter( const QPointF& center ) Q_DECL_OVERRIDE;

	/**
	 * Set the underlying model for the region.
	 * @param model - the region model.
	 */
    virtual void setModel( Carta::Lib::Regions::RegionBase* model );

    /**
     * Set the major radius of the ellipse.
     * @param length - the length of the ellipse's major radius.
     * @param changed - set to true if the major radius is changed; false otherwise.
     * @return - an error message if there was a problem setting the major radius.
     */
    virtual QString setRadiusMajor( double length, bool* changed ) Q_DECL_OVERRIDE;

    /**
     * Set the minor radius of the ellipse.
     * @param length - the length of the ellipse's minor radius.
     * @param changed - set to true if the minor radius is changed; false otherwise.
     * @return - an error message if there was a problem setting the minor radius.
     */
    virtual QString setRadiusMinor( double length, bool* changed ) Q_DECL_OVERRIDE;

    /**
     * Returns a json representation of the region.
     * @return - a json representation of the region.
     */
    virtual QJsonObject toJSON() const Q_DECL_OVERRIDE;

    virtual ~RegionEllipse();

    const static QString CLASS_NAME;

private slots:

	void _updateStateFromJson( const QJsonObject& json );

private:

    void _initializeState();

    static bool m_registered;

    /**
     * Construct a region.
     */
    RegionEllipse( const QString& path, const QString& id );
    class Factory;

    void _initializeCallbacks();

    RegionEllipse( const RegionEllipse& other);
    RegionEllipse& operator=( const RegionEllipse& other );
};
}
}
