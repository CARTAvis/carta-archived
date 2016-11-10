/**
 * Model for a point region.
 **/

#pragma once

#include "IRegion.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include <QJsonObject>
#include <QRectF>

namespace Carta {
namespace Lib {
namespace Regions {

class Point : public RegionBase {

public:

	/**
	 * Constructor.
	 */
	Point( RegionBase * parent = nullptr );

	static constexpr auto TypeName = "Point";

	/**
	 * Initialize the Point from json.
	 * @param obj - the json object holding initialization information.
	 * @return - true if the Point was correctly initialized; false if there
	 * 	was a problem.
	 */
	virtual bool initFromJson( QJsonObject obj ) override;

	/**
	 * Returns whether or not the point is inside the region.
	 * @return - true if the point is inside the region; false, otherwise.
	 */
	virtual bool isPointInside( const RegionPointV & pts ) const override;

	/**
	 * Return whether or not the point is inside the union.
	 * @return - true if the point is in the union; false, otherwise.
	 */
	virtual bool isPointInsideUnion( const RegionPointV & pts ) const override;

	/**
	 * Returns a small box containing the point.
	 * @return - a small box containing the point.
	 */
	virtual QRectF outlineBox() const override;

	/**
	 * Set the location of the point.
	 * @param pt - the location of the underlying point.
	 */
	void setPoint( const QPointF & pt );

	/**
	 * Return a json representation of the region.
	 * @return - a json representation of the region.
	 */
	virtual QJsonObject toJson() const override;

	/**
	 * Returns a string indicating the type.
	 * @return - a string indicating the type.
	 */
	virtual QString typeName() const override;

	/**
	 * Return vector graphics for the region.
	 * @return - vector graphics for the region.
	 */
	VectorGraphics::VGList vgList() const override;

private:
	QPointF m_point;
};


}
}
}
