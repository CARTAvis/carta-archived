/**
 * Model for a rectangular region.
 **/

#pragma once

#include "IRegion.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include <QJsonObject>
#include <QRectF>

namespace Carta {
namespace Lib {
namespace Regions {

class Rectangle : public RegionBase {

public:

	/**
	 * Constructor.
	 */
	Rectangle( RegionBase * parent = nullptr );

	static constexpr auto TypeName = "rectangle";

	/**
	 * Initialize the rectangle from json.
	 * @param obj - the json object holding initialization information.
	 * @return - true if the rectangle was correctly initialized; false if there
	 * 	was a problem.
	 */
	virtual bool initFromJson( QJsonObject obj ) override;

	/**
	 * Returns whether or not the point is inside the rectangle.
	 * @return - true if the point is inside the rectangle; false, otherwise.
	 */
	virtual bool isPointInside( const RegionPointV & pts ) const override;

	/**
	 * Return whether or not the point is inside the union.
	 * @return - true if the point is in the union; false, otherwise.
	 */
	virtual bool isPointInsideUnion( const RegionPointV & pts ) const override;

	/**
	 * Returns the underlying rectangle.
	 * @return - the rectangle.
	 */
	virtual QRectF outlineBox() const override;

	/**
	 * Set the underlying rectangle.
	 * @param rect - the underlying rectangle.
	 */
	void setRectangle( const QRectF & rect );

	/**
	 * Return a json representation of the rectangle.
	 * @return - a json representation of the rectangle.
	 */
	virtual QJsonObject toJson() const override;

	/**
	 * Returns a string indicating the type.
	 * @return - a string indicating the type.
	 */
	virtual QString typeName() const override;

	/**
	 * Return vector graphics for the rectangle.
	 * @return - vector graphics for the rectangle.
	 */
	VectorGraphics::VGList vgList() const override;

private:

	static constexpr auto WIDTH = "width";
	static constexpr auto HEIGHT = "height";
	QRectF m_rectf;
};


}
}
}
