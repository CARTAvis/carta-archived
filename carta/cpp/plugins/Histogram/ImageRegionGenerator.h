/**
 * Produces casacore ImageRegions based on region models passed in.
 */
#pragma once

#include <memory>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/casa/Arrays/Vector.h>
#include <QString>
#include <QRectF>

namespace casa {
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
	static casa::ImageRegion* makeRegion( casa::ImageInterface<casa::Float> * casaImage,
			std::shared_ptr<Carta::Lib::Regions::RegionBase> region );

	virtual ~ImageRegionGenerator();

private:

	static std::pair<casa::Vector<casa::Quantity>,casa::Vector<casa::Quantity> > _getEllipsePositionRadii(
			Carta::Lib::Regions::Ellipse* ellipse, const casa::CoordinateSystem &cs );
	static casa::ImageRegion* _makeRegionRectangle( casa::ImageInterface<casa::Float> * casaImage,
			const QRectF& outlineBox);
	static casa::ImageRegion* _makeRegionEllipse( casa::ImageInterface<casa::Float> * casaImage,
			Carta::Lib::Regions::Ellipse* ellipse);
	static casa::ImageRegion* _makeRegionPolygon( casa::ImageInterface<casa::Float> * casaImage,
			Carta::Lib::Regions::Polygon* polygon );
	static casa::Vector<casa::Double> _toWorld( const casa::CoordinateSystem& cSys,
			double x, double y, bool* successful );
	const static QString RAD_UNITS;
	ImageRegionGenerator();
	ImageRegionGenerator( const ImageRegionGenerator& other );
	ImageRegionGenerator operator=( const ImageRegionGenerator& other );

};
