#pragma once

#include <casacore/casa/Containers/Record.h>
#include <casacore/images/Images/ImageInterface.h>
#include <vector>
#include "CartaLib/Regions/IRegion.h"

namespace casacore {
    class ImageRegion;
}

/**
 * Generates Records of regions.
 */

class RegionRecordFactory {
public:
    /**
     * Returns a Record of a region based on its type, the coordinate system of its containing
     * image, and a list of region corners (in pixels).
     * @param casaImage - the image containing the regions.
     * @param region - region information.
     * @param slice - information about the current frames of the image.
     * @param typeStr - an identifier for the type of region (return value).
     * @return - a record representation of the region.
     */
	static casacore::Record getRegionRecord(
	        casacore::ImageInterface<casacore::Float>* casaImage,
	        std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
	        const std::vector<int>& slice, QString& typeStr );

	virtual ~RegionRecordFactory();

private:
	RegionRecordFactory();
	RegionRecordFactory( const RegionRecordFactory& other );
	RegionRecordFactory operator=( const RegionRecordFactory& other );

	static casacore::ImageRegion*
	_getEllipsoid(const casacore::CoordinateSystem& cSys,
	            const casacore::Vector<casacore::Double>& x, const casacore::Vector<casacore::Double>& y);

	static void _getMinMaxCorners( const QPolygonF & corners,
	        std::pair<double,double>& minCorner, std::pair<double,double>& maxCorner);

	static casacore::ImageRegion*
	_getPolygon( casacore::ImageInterface<casacore::Float>* casaImage,
	        const QPolygonF& corners, const std::vector<int>& slice );

	static casacore::ImageRegion*
	_getRectangle( casacore::ImageInterface<casacore::Float>* casaImage,
	        const QPolygonF& corners,
	        const std::vector<int>& slice );

	static casacore::Record _getRegionRecordEllipse(
	        casacore::ImageInterface<casacore::Float>* casaImage,
	        std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
	        const std::vector<int>& slice);

	static casacore::Record _getRegionRecordPoint(
	        casacore::ImageInterface<casacore::Float>* casaImage,
	        std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
	        const std::vector<int>& slice, QString& typeStr );

	static casacore::Record _getRegionRecordPolygon(
	        casacore::ImageInterface<casacore::Float>* casaImage,
	        std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
	        const std::vector<int>& slice, QString& typeStr );

	static casacore::Record _getRegionRecordRectangle(
	        casacore::ImageInterface<casacore::Float>* casaImage,
	        std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
	        const std::vector<int>& slice, QString& typeStr );

	static bool _getWorldVertex( int pixelX, int pixelY, const casacore::CoordinateSystem& cSys,
	        const std::vector<int>& slice, casacore::Vector<casacore::Double>& worldVertices );
};
