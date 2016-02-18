#pragma once

#include "CartaLib/RegionInfo.h"
#include <casacore/casa/Containers/Record.h>
#include <casacore/images/Images/ImageInterface.h>
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
     * @param casaImage - the image containing the regions.
     * @param corners - a list of region corner points (in pixels).
     * @param slice - information about the current frames of the image.
     * @param typeStr - an identifier for the type of region (return value).
     * @return - a record representation of the region.
     */
	static casa::Record getRegionRecord( Carta::Lib::RegionInfo::RegionType regionType,
	        casa::ImageInterface<casa::Float>* casaImage,
	        std::vector< std::pair<double,double> >& corners,
	        const std::vector<int>& slice, QString& typeStr );

	virtual ~RegionRecordFactory();

private:
	RegionRecordFactory();
	RegionRecordFactory( const RegionRecordFactory& other );
	RegionRecordFactory operator=( const RegionRecordFactory& other );

	static casa::ImageRegion*
	_getEllipsoid(const casa::CoordinateSystem& cSys,
	            const casa::Vector<casa::Double>& x, const casa::Vector<casa::Double>& y);

	static void _getMinMaxCorners( const std::vector<std::pair<double,double> > & corners,
	        std::pair<double,double>& minCorner, std::pair<double,double>& maxCorner);

	static casa::ImageRegion*
	_getPolygon( casa::ImageInterface<casa::Float>* casaImage,
	        const std::vector<std::pair<double,double> >& corners, const std::vector<int>& slice );

	static casa::ImageRegion*
	_getRectangle( casa::ImageInterface<casa::Float>* casaImage,
	        const std::vector<std::pair<double,double> >& corners,
	        const std::vector<int>& slice );

	static casa::Record _getRegionRecordEllipse(
	        casa::ImageInterface<casa::Float>* casaImage,
	        std::vector<std::pair<double,double> >& corners,
	        const std::vector<int>& slice);

	static casa::Record _getRegionRecordPolygon(
	        casa::ImageInterface<casa::Float>* casaImage,
	        std::vector<std::pair<double,double> >& corners,
	        const std::vector<int>& slice, QString& typeStr );

	static bool _getWorldVertex( int pixelX, int pixelY, const casa::CoordinateSystem& cSys,
	        const std::vector<int>& slice, casa::Vector<casa::Double>& worldVertices );


};
