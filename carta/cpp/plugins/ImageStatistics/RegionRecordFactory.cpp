#include <QDebug>
#include <QtCore/qmath.h>
#include "RegionRecordFactory.h"
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/images/Regions/RegionManager.h>

#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Slicer.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/images/Regions/WCEllipsoid.h>
#include <casacore/images/Regions/WCBox.h>
#include <casacore/images/Images/SubImage.h>


RegionRecordFactory::RegionRecordFactory( ){
}


casa::ImageRegion*
RegionRecordFactory::_getEllipsoid(
        const casa::CoordinateSystem& cSys,
        const casa::Vector<casa::Double>& corner1,
        const casa::Vector<casa::Double>& corner2 ){
    int imageDims = 2;
    casa::Vector<casa::Quantity> center(imageDims);
    casa::Vector<casa::Quantity> radius(imageDims);
    casa::ImageRegion* imageRegion = NULL;
    int directionIndex = cSys.findCoordinate( casa::Coordinate::DIRECTION );
    if ( directionIndex >= 0 ){
        const casa::String radUnits( "rad");
        casa::Vector<casa::String> axisUnits = cSys.worldAxisUnits();
        for ( int i = 0; i < imageDims; i++ ){
            center[i] = casa::Quantity( (corner1[i] +corner2[i]) / 2, axisUnits[i]);
        }

        casa::uInt dirIndex = static_cast<casa::uInt>(directionIndex);
        casa::MDirection::Types type = cSys.directionCoordinate(dirIndex).directionType(true);


        casa::Vector<casa::Double> qCenter(imageDims);
        casa::Vector<casa::Int> dirPixelAxis = cSys.pixelAxes(directionIndex);
        qCenter[0] = center[0].getValue();
        qCenter[1] = center[1].getValue();
        casa::MDirection mdcenter( casa::Quantum<casa::Vector<casa::Double> >(qCenter,radUnits), type );

        casa::Vector<casa::Double> blc_rad_x(imageDims);
        blc_rad_x[0] = corner1[0];
        blc_rad_x[1] = center[1].getValue();
        casa::MDirection mdblc_x( casa::Quantum<casa::Vector<casa::Double> >(blc_rad_x,radUnits),type );

        casa::Vector<casa::Double> blc_rad_y(imageDims);
        blc_rad_y[0] = center[0].getValue();
        blc_rad_y[1] = corner1[1];
        casa::MDirection mdblc_y( casa::Quantum<casa::Vector<casa::Double> >(blc_rad_y,radUnits),type );

        double xdistance = mdcenter.getValue( ).separation(mdblc_x.getValue( ));
        double ydistance = mdcenter.getValue( ).separation(mdblc_y.getValue( ));
        const float ERR = 0;
        if ( xdistance > ERR && ydistance > ERR ){
            radius[0] = casa::Quantity( xdistance, axisUnits[0]);
            radius[1] = casa::Quantity( ydistance, axisUnits[1]);

            casa::Vector<casa::Int> pixax(imageDims);
            pixax[0] = dirPixelAxis[0];
            pixax[1] = dirPixelAxis[1];
            casa::WCEllipsoid ellipsoid( center, radius, pixax, cSys);
            imageRegion = new casa::ImageRegion( ellipsoid );
        }
    }
    else {
        qDebug() << "Image had missing direction index";
    }
    return imageRegion;
}


void
RegionRecordFactory::_getMinMaxCorners( const std::vector<std::pair<double,double> > & corners,
        std::pair<double,double>& minCorner, std::pair<double,double>& maxCorner){
    int cornerCount = corners.size();
    if ( cornerCount > 0 ){
        int minX = qRound(corners[0].first);
        int maxX = qRound(corners[0].first);
        int minY = qRound(corners[0].second);
        int maxY = qRound(corners[0].second);
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
        minCorner.first = minX;
        minCorner.second = minY;
        maxCorner.first = maxX;
        maxCorner.second = maxY;
    }
}


casa::ImageRegion*
RegionRecordFactory::_getPolygon( casa::ImageInterface<casa::Float>* casaImage,
        const std::vector<std::pair<double,double> >& corners, const std::vector<int>& slice ){
    casa::ImageRegion* imageRegion = NULL;

    casa::CoordinateSystem cSys = casaImage->coordinates();
    casa::Vector<casa::String> axisUnits = cSys.worldAxisUnits();
    int imageDim = casaImage->shape().nelements();
    int cornerCount = corners.size();
    casa::Vector<casa::Quantity> worldVertexX(cornerCount);
    casa::Vector<casa::Quantity> worldVertexY(cornerCount);
    int directionIndex = cSys.findCoordinate( casa::Coordinate::DIRECTION );
    if ( directionIndex >= 0 ){
        for ( int i = 0; i < cornerCount; i++ ){
            casa::Vector<casa::Double> worldVertex(imageDim);
            bool validVertex = _getWorldVertex( corners[i].first, corners[i].second, cSys,
                slice, worldVertex );
            if ( validVertex ){
                worldVertexX[i] = casa::Quantity( worldVertex[0], axisUnits[0] );
                worldVertexY[i] = casa::Quantity( worldVertex[1], axisUnits[1] );
            }
            else {
                qWarning() << "Could not convert vertex: ("<<corners[i].first<<", "<<corners[i].second;
            }
        }
        casa::Vector<casa::Int> dirPixelAxis = cSys.pixelAxes( directionIndex );
        casa::Vector<casa::Int> pixAx(2);
        pixAx[0] = dirPixelAxis[0];
        pixAx[1] = dirPixelAxis[1];
        casa::RegionManager regMan;
        imageRegion = regMan.wpolygon( worldVertexX, worldVertexY,  pixAx, cSys, "abs");
    }
    return imageRegion;
}


casa::ImageRegion*
RegionRecordFactory::_getRectangle( casa::ImageInterface<casa::Float>* casaImage,
        const std::vector<std::pair<double,double> >& corners, const std::vector<int>& slice ){
    casa::ImageRegion* imageRegion = NULL;
    std::pair<double,double> minCorners;
    std::pair<double,double> maxCorners;
    _getMinMaxCorners( corners, minCorners, maxCorners );

    casa::CoordinateSystem cSys = casaImage->coordinates();
    int imageDim = casaImage->shape().nelements();
    casa::Vector<casa::Double> worldVerticesBLC( imageDim );
    casa::Vector<casa::Double> worldVerticesTRC( imageDim );

    bool blcValid = _getWorldVertex( minCorners.first, minCorners.second, cSys,
            slice, worldVerticesBLC );
    bool trcValid = _getWorldVertex( maxCorners.first, maxCorners.second, cSys,
            slice, worldVerticesTRC );

    if ( blcValid && trcValid ){
        casa::Vector<casa::String> axisUnits = cSys.worldAxisUnits();
        casa::Vector<casa::Quantity> blc(imageDim);
        casa::Vector<casa::Quantity> trc(imageDim);
        for ( int ax = 0; ax < imageDim; ax++) {
            blc[ax] = casa::Quantity( worldVerticesBLC[ax], axisUnits[ax] );
            trc[ax] = casa::Quantity( worldVerticesTRC[ax], axisUnits[ax] );
        }
        casa::WCBox box(blc, trc, cSys, casa::Vector<casa::Int>());
        imageRegion = new casa::ImageRegion(box);
    }
    return imageRegion;
}


casa::Record RegionRecordFactory::getRegionRecord( Carta::Lib::RegionInfo::RegionType type,
        casa::ImageInterface<casa::Float>* casaImage,
    std::vector<std::pair<double,double> >& corners,
    const std::vector<int>& slice,
    QString& typeStr ){
    casa::Record regionRecord;
    if ( type == Carta::Lib::RegionInfo::RegionType::Polygon ){
        regionRecord = _getRegionRecordPolygon( casaImage, corners, slice, typeStr );
    }
    else if ( type == Carta::Lib::RegionInfo::RegionType::Ellipse ){
        typeStr = "Ellipse";
        regionRecord = _getRegionRecordEllipse( casaImage, corners, slice );
    }
    else {
        qDebug() <<"RegionRecordFactory::getRegionRecord unrecognized region type: "+
                QString::number((int)(type));
    }
    return regionRecord;
}


casa::Record RegionRecordFactory::_getRegionRecordEllipse(
        casa::ImageInterface<casa::Float>* casaImage,
        std::vector<std::pair<double,double> >& corners,
        const std::vector<int>& slice){
    casa::Record regionRecord;
    int cornerCount = corners.size();
    if ( cornerCount == 2 ){
        std::pair<double,double> minCorner;
        std::pair<double,double> maxCorner;
        _getMinMaxCorners( corners, minCorner, maxCorner );

        //Get the bounding box corners for the ellipse in world coordinates.
        casa::CoordinateSystem cSys = casaImage->coordinates();
        casa::Vector<casa::Double> worldVertex1;
        bool valid1 = _getWorldVertex( minCorner.first, minCorner.second, cSys,
                slice, worldVertex1 );
        casa::Vector<casa::Double> worldVertex2;
        bool valid2 = _getWorldVertex( maxCorner.first, maxCorner.second, cSys,
                slice, worldVertex2 );
        if ( valid1 && valid2 ){
            //Make an elliptical region based on the corners.
            casa::ImageRegion* ellipsoid = _getEllipsoid( cSys, worldVertex1, worldVertex2 );
            if ( ellipsoid != NULL ){
                regionRecord = ellipsoid->toRecord("");
                delete ellipsoid;
            }
        }
        else {
            qDebug() << "Could not obtain valid world coordinates of ellipse.";
        }
    }
    return regionRecord;
}

casa::Record RegionRecordFactory::_getRegionRecordPolygon(
        casa::ImageInterface<casa::Float>* casaImage,
        std::vector<std::pair<double,double> >& corners,
        const std::vector<int>& slice, QString& typeStr ){

    int cornerCount = corners.size();
    casa::Record regionRecord;
    casa::ImageRegion* region = nullptr;
    //Rectangular region or point
    if ( cornerCount == 4 || cornerCount == 1 ){
        if ( cornerCount == 4 ){
            typeStr = "Rectangle";
        }
        else {
            typeStr = "Point";
        }
        region = _getRectangle( casaImage, corners, slice );
    }
    //Polygonal region
    else if ( cornerCount == 3 || cornerCount > 4 ){
        typeStr = "Polygon";
        region = _getPolygon( casaImage, corners, slice );
    }
    else {
        qWarning() << "Unknown region with: "<<cornerCount<<" corners.";
    }
    if ( region != nullptr ){
        regionRecord = region->toRecord("");
        delete region;
    }
    return regionRecord;
}


bool RegionRecordFactory::_getWorldVertex( int pixelX, int pixelY, const casa::CoordinateSystem& cSys,
        const std::vector<int>& slice, casa::Vector<casa::Double>& worldVertices ){
    int imageDim = slice.size();
    bool worldFilled = false;
    casa::Vector<casa::Double> pixelVertices( imageDim );
    casa::Int directionIndex = cSys.findCoordinate(casa::Coordinate::DIRECTION);
    if ( directionIndex >= 0 ){
        casa::Vector<casa::Int> dirPixelAxis = cSys.pixelAxes(directionIndex);
        for ( int ax = 0; ax < imageDim; ax++) {
            if ( ax == dirPixelAxis[0] ) {
                pixelVertices[ax] = pixelX;
            }
            else if ( ax == dirPixelAxis[1]){
                pixelVertices[ax] = pixelY;
            }
            else  {
                pixelVertices[ax] = slice[ax];
            }
        }
        worldVertices.resize( imageDim );
        worldFilled = cSys.toWorld( worldVertices, pixelVertices );
    }
    return worldFilled;
}


RegionRecordFactory::~RegionRecordFactory() {
}

