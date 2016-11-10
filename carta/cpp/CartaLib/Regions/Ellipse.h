/**
 * Model for an elliptical region.
 **/

#pragma once
#include "IRegion.h"
#include "CartaLib/CartaLib.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include <QJsonObject>
#include <QRectF>

namespace Carta {
namespace Lib {
namespace Regions {

class Ellipse : public RegionBase
{
public:

	static constexpr auto TypeName = "ellipse";

	/**
	 * Constructor.
	 */
	Ellipse( RegionBase * parent = nullptr );

	/**
	 * Constructor.
	 * @param center - the center of the ellipse.
	 * @param radiusMajor - the length of the major radius of the ellipse.
	 * @param radiusMinor - the length of the minor radius of the ellipse.
	 * @param angle - the tilt angle of the ellipse.
	 */
	Ellipse( const RegionPoint & center, double radiusMajor, double radiusMinor, double angle );

	/**
	 * Return the center of the ellipse.
	 * @return - the center of the ellipse.
	 */
	const RegionPoint & getCenter() const;


	/**
	 * Return the tilt angle of the ellipse.
	 * @return - the tilt angle of the ellipse.
	 */
	double getAngle() const;


	/**
	 * Return the minor radius of the ellipse.
	 * @return - the minor radius of the ellipse.
	 */
	double getRadiusMajor() const;


	/**
	 * Return the major radius of the ellipse.
	 * @return - the major radius of the ellipse.
	 */
	double getRadiusMinor() const;

	/**
	 * Initialize the ellipse from json.
	 * @param obj - the json object holding initialization information.
	 * @return - true if the ellipse was correctly initialized; false if there
	 * 	was a problem.
	 */
	virtual bool initFromJson( QJsonObject obj ) override;

	/**
	 * Returns whether or not the point is inside the ellipse.
	 * @return - true if the point is inside the ellipse; false, otherwise.
	 */
	virtual bool isPointInside( const RegionPointV & pts ) const override;

	/**
	 * Return whether or not the point is inside the union.
	 * @return - true if the point is in the union; false, otherwise.
	 */
	virtual bool isPointInsideUnion( const RegionPointV & pts ) const override;

	/**
	 * Return the bounding rectangle for the ellipse.
	 * @return - the ellipse's bounding rectangle.
	 */
	virtual QRectF outlineBox() const override;

	/**
	 * Set the tilt angle of the ellipse.
	 * @param angle - the tilt angle of the ellipse.
	 */
	//Assumed to be a degree major.  The angle the major axis of the ellipse makes with North
	void setAngle( double angle );

	/**
	 * Set the center of the ellipse.
	 * @param pt - the center of the ellipse.
	 */
	void setCenter( const RegionPoint & pt );

	/**
	 * Set the length of the major radius of the ellipse.
	 * @param radius - the length of the major radius of the ellipse.
	 */
	void setRadiusMajor( double radius );

	/**
	 * Set the length of the minor radius of the ellipse.
	 * @param radius - the length of the minor radius of the ellipse.
	 */
	void setRadiusMinor( double radius );

	/**
	 * Return a json representation of the ellipse.
	 * @return - a json representation of the ellipse.
	 */
	virtual QJsonObject toJson() const override;

	/**
	 * Returns a string indicating the type.
	 * @return - a string indicating the type.
	 */
	QString typeName() const;

	/**
	 * Return vector graphics for the ellipse.
	 * @return - vector graphics for the ellipse.
	 */
	virtual VectorGraphics::VGList vgList() const override;
	const static QString RADIUS_MAJOR;
	const static QString RADIUS_MINOR;
	const static QString ANGLE;

private:

	RegionPoint m_center { 0, 0 };
	double m_radiusMajor = 1;
	double m_radiusMinor = 1;
	double m_angle = 0;
};


}
}
}
