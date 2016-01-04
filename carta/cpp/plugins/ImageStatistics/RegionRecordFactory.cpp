#include <QDebug>
#include "RegionRecordFactory.h"
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/images/Regions/RegionManager.h>

#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/BasicSL/String.h>


RegionRecordFactory::RegionRecordFactory( ){
}

std::pair<casa::Quantity,casa::Quantity>
RegionRecordFactory::_getWorldVertex( double xPixel, double yPixel,
        const casa::CoordinateSystem& csys){
    std::pair<casa::Quantity,casa::Quantity> worldVertices;

    const casa::IPosition dirAxes = csys.directionAxesNumbers();
    if ( dirAxes.nelements() >= 2 ){
        casa::String xUnit = csys.worldAxisUnits()[dirAxes[0]];
        casa::String yUnit = csys.worldAxisUnits()[dirAxes[1]];
        int pixelAxesCount = csys.nPixelAxes();
        if ( pixelAxesCount >= 2 ){
            casa::Vector<casa::Double> pixel( pixelAxesCount, 0);
            pixel[dirAxes[0]] = xPixel;
            pixel[dirAxes[1]] = yPixel;
            casa::Vector<casa::Double> world;
            csys.toWorld(world, pixel);

            worldVertices.first = casa::Quantity(world[dirAxes[0]], xUnit);
            worldVertices.second = casa::Quantity(world[dirAxes[1]], yUnit);
        }
    }
    return worldVertices;
}

casa::Record RegionRecordFactory::getRegionRecord( Carta::Lib::RegionInfo::RegionType type,
        const casa::CoordinateSystem& cSys,
    std::vector<std::pair<double,double> >& corners){
    const casa::String units( "rad");
    const casa::String absStr( "abs");
    casa::Record regionRecord;
    casa::Int directionIndex = cSys.findCoordinate(casa::Coordinate::DIRECTION);
    if ( directionIndex >= 0 ){
        casa::Vector<casa::Int> dirPixelAxis = cSys.pixelAxes(directionIndex);
        casa::RegionManager regMan;
        int cornerCount = corners.size();
        if ( type == Carta::Lib::RegionInfo::RegionType::Polygon ){
            //Rectangular region or point
            if ( cornerCount == 4 || cornerCount == 1 ){
                casa::Vector<casa::Quantity> blc(2);
                casa::Vector<casa::Quantity> trc(2);
                double minX = corners[0].first;
                double maxX = corners[0].first;
                double minY = corners[0].second;
                double maxY = corners[0].second;
                for ( int i = 1; i < cornerCount; i++ ){
                    if ( corners[i].first < minX ){
                        minX = corners[i].first;
                    }
                    else if ( corners[i].first > maxX ){
                        maxX = corners[i].first;
                    }
                    if ( corners[i].second < minY ){
                        minY = corners[i].second;
                    }
                    else if ( corners[i].second > maxY ){
                        maxY = corners[i].second;
                    }
                }
                std::pair<casa::Quantity,casa::Quantity> blcPt = _getWorldVertex( minX, minY, cSys);
                blc(0) = blcPt.first;
                blc(1) = blcPt.second;

                std::pair<casa::Quantity,casa::Quantity> trcPt = _getWorldVertex( minX, minY, cSys);
                trc(0) = trcPt.first;
                trc(1) = trcPt.second;

                casa::Vector<casa::Int> pixax(2);
                pixax(0) = dirPixelAxis[0];
                pixax(1) = dirPixelAxis[1];

                casa::Record* imagregRecord = regMan.wbox(blc, trc, pixax, cSys, absStr, units);
                regionRecord = *imagregRecord;
                delete imagregRecord;
            }
            else if ( cornerCount > 2 ){
                /*ImageRegion* polygon = getPolygon( cSys, x, y );
                if ( polygon != NULL ){
                    regionRecord = polygon->toRecord(String(""));
                    delete polygon;
                }*/
            }
        }
        else if ( type == Carta::Lib::RegionInfo::RegionType::Ellipse ){
            /*ImageRegion* ellipsoid = getEllipsoid( cSys, x, y );
            if ( ellipsoid != NULL ){
                regionRecord = ellipsoid->toRecord("");
                delete ellipsoid;
            }*/
            qDebug() << "Ellipse not implemented yet";
        }
        else {
            qDebug() <<"RegionRecordFactory::getRegionRecord unrecognized region type.";
        }
    }

    return regionRecord;
}


RegionRecordFactory::~RegionRecordFactory() {
}

