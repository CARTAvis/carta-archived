/**
 * Produces casacore ImageRegions based on region models passed in.
 */
#pragma once

#include <memory>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/casa/Arrays/Vector.h>
#include <QString>
#include <QRectF>

namespace casacore {
    class ImageRegion;
    template <class T> class ImageInterface;
}

namespace Carta {
	namespace Lib {
		namespace Regions {
			class RegionBase;
			class Rectangle;
			class Ellipse;
			class Polygon;
		}
	}
}

class ImageRegionGenerator {
public:
	/**
	 * Make a casacore ImageRegion based on the image and region model passed in.
	 * @param casaImage - an image.
	 * @param region - a region in the image.
	 */
	static casacore::ImageRegion* makeRegion( casacore::ImageInterface<casacore::Float> * casaImage,
			std::shared_ptr<Carta::Lib::Regions::RegionBase> region );

	virtual ~ImageRegionGenerator();

private:

	static std::pair<casacore::Vector<casacore::Quantity>,casacore::Vector<casacore::Quantity> > _getEllipsePositionRadii(
			Carta::Lib::Regions::Ellipse* ellipse, const casacore::CoordinateSystem &cs );
	static casacore::ImageRegion* _makeRegionRectangle( casacore::ImageInterface<casacore::Float> * casaImage,
			const QRectF& outlineBox);
	static casacore::ImageRegion* _makeRegionEllipse( casacore::ImageInterface<casacore::Float> * casaImage,
			Carta::Lib::Regions::Ellipse* ellipse);
	static casacore::ImageRegion* _makeRegionPolygon( casacore::ImageInterface<casacore::Float> * casaImage,
			Carta::Lib::Regions::Polygon* polygon );
	static casacore::Vector<casacore::Double> _toWorld( const casacore::CoordinateSystem& cSys,
			double x, double y, bool* successful );
	const static QString RAD_UNITS;
	ImageRegionGenerator();
	ImageRegionGenerator( const ImageRegionGenerator& other );
	ImageRegionGenerator operator=( const ImageRegionGenerator& other );

};
