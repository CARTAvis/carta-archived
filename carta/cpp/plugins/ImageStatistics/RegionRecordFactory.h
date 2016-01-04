#pragma once

#include "CartaLib/RegionInfo.h"
#include <casacore/casa/Containers/Record.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <vector>

namespace casa {
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
     * @param regionType - the type of region such as a Polygon.
     * @param cSys - the coordinate system of the image containing the regions.
     * @param corners - a list of region corner points (in pixels).
     */
	static casa::Record getRegionRecord( Carta::Lib::RegionInfo::RegionType regionType,
	        const casa::CoordinateSystem& cSys,
	        std::vector< std::pair<double,double> >& corners);

	virtual ~RegionRecordFactory();

private:
	RegionRecordFactory();
	RegionRecordFactory( const RegionRecordFactory& other );
	RegionRecordFactory operator=( const RegionRecordFactory& other );

	/**
	 * Translate a point in pixel coordinates into a point in world
	 * coordinates of an image.
	 * @param xPixel - an x-coordinate in pixels.
	 * @param yPixel - a y-coordinate in pixels.
	 * @param csys - the image coordinate system.
	 */
	static std::pair<casa::Quantity,casa::Quantity>
	    _getWorldVertex( double xPixel, double yPixel,
	            const casa::CoordinateSystem& csys);


};
