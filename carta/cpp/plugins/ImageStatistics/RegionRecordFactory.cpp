#include <QDebug>
#include <QtCore/qmath.h>
#include "RegionRecordFactory.h"
#include "CartaLib/Regions/Ellipse.h"
#include "CartaLib/Regions/Point.h"
#include "CartaLib/Regions/Rectangle.h"
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


casacore::ImageRegion*
RegionRecordFactory::_getEllipsoid(
        const casacore::CoordinateSystem& cSys,
        const casacore::Vector<casacore::Double>& corner1,
        const casacore::Vector<casacore::Double>& corner2 ){
    int imageDims = 2;
    casacore::Vector<casacore::Quantity> center(imageDims);
    casacore::Vector<casacore::Quantity> radius(imageDims);
    casacore::ImageRegion* imageRegion = NULL;
    int directionIndex = cSys.findCoordinate( casacore::Coordinate::DIRECTION );
    if ( directionIndex >= 0 ){
        const casacore::String radUnits( "rad");
        casacore::Vector<casacore::String> axisUnits = cSys.worldAxisUnits();
        for ( int i = 0; i < imageDims; i++ ){
            center[i] = casacore::Quantity( (corner1[i] +corner2[i]) / 2, axisUnits[i]);
        }

        casacore::uInt dirIndex = static_cast<casacore::uInt>(directionIndex);
        casacore::MDirection::Types type = cSys.directionCoordinate(dirIndex).directionType(true);


        casacore::Vector<casacore::Double> qCenter(imageDims);
        casacore::Vector<casacore::Int> dirPixelAxis = cSys.pixelAxes(directionIndex);
        qCenter[0] = center[0].getValue();
        qCenter[1] = center[1].getValue();
        casacore::MDirection mdcenter( casacore::Quantum<casacore::Vector<casacore::Double> >(qCenter,radUnits), type );

        casacore::Vector<casacore::Double> blc_rad_x(imageDims);
        blc_rad_x[0] = corner1[0];
        blc_rad_x[1] = center[1].getValue();
        casacore::MDirection mdblc_x( casacore::Quantum<casacore::Vector<casacore::Double> >(blc_rad_x,radUnits),type );

        casacore::Vector<casacore::Double> blc_rad_y(imageDims);
        blc_rad_y[0] = center[0].getValue();
        blc_rad_y[1] = corner1[1];
        casacore::MDirection mdblc_y( casacore::Quantum<casacore::Vector<casacore::Double> >(blc_rad_y,radUnits),type );

        double xdistance = mdcenter.getValue( ).separation(mdblc_x.getValue( ));
        double ydistance = mdcenter.getValue( ).separation(mdblc_y.getValue( ));
        const float ERR = 0;
        if ( xdistance > ERR && ydistance > ERR ){
            radius[0] = casacore::Quantity( xdistance, axisUnits[0]);
            radius[1] = casacore::Quantity( ydistance, axisUnits[1]);

            casacore::Vector<casacore::Int> pixax(imageDims);
            pixax[0] = dirPixelAxis[0];
            pixax[1] = dirPixelAxis[1];
            casacore::WCEllipsoid ellipsoid( center, radius, pixax, cSys);
            imageRegion = new casacore::ImageRegion( ellipsoid );
        }
    }
    else {
        qDebug() << "Image had missing direction index";
    }
    return imageRegion;
}


void
RegionRecordFactory::_getMinMaxCorners( const QPolygonF & corners,
        std::pair<double,double>& minCorner, std::pair<double,double>& maxCorner){
    int cornerCount = corners.size();
    if ( cornerCount > 0 ){
        QPointF firstCorner = corners.value( 0 );
        int minX = qRound(firstCorner.x());
        int maxX = qRound(firstCorner.x());
        int minY = qRound(firstCorner.y());
        int maxY = qRound(firstCorner.y());
        for ( int i = 1; i < cornerCount; i++ ){
            QPointF corner = corners.value( i );
            if ( corner.x() < minX ){
                minX = corner.x();
            }
            else if ( corner.x() > maxX ){
                maxX = corner.x();
            }
            if ( corner.y() < minY ){
                minY = corner.y();
            }
            else if ( corner.y() > maxY ){
                maxY = corner.y();
            }
        }
        minCorner.first = minX;
        minCorner.second = minY;
        maxCorner.first = maxX;
        maxCorner.second = maxY;
    }
}


casacore::ImageRegion*
RegionRecordFactory::_getPolygon( casacore::ImageInterface<casacore::Float>* casaImage,
        const QPolygonF& corners, const std::vector<int>& slice ){
    casacore::ImageRegion* imageRegion = NULL;

    casacore::CoordinateSystem cSys = casaImage->coordinates();
    casacore::Vector<casacore::String> axisUnits = cSys.worldAxisUnits();
    int imageDim = casaImage->shape().nelements();
    int cornerCount = corners.size();
    casacore::Vector<casacore::Quantity> worldVertexX(cornerCount);
    casacore::Vector<casacore::Quantity> worldVertexY(cornerCount);
    int directionIndex = cSys.findCoordinate( casacore::Coordinate::DIRECTION );
    if ( directionIndex >= 0 ){
        for ( int i = 0; i < cornerCount; i++ ){
            casacore::Vector<casacore::Double> worldVertex(imageDim);
            QPointF corner = corners.value( i );
            bool validVertex = _getWorldVertex( corner.x(), corner.y(), cSys,
                slice, worldVertex );
            if ( validVertex ){
                worldVertexX[i] = casacore::Quantity( worldVertex[0], axisUnits[0] );
                worldVertexY[i] = casacore::Quantity( worldVertex[1], axisUnits[1] );
            }
            else {
                qWarning() << "Could not convert vertex: ("<<corner.x()<< ", "<<corner.y()<<")";
            }
        }
        casacore::Vector<casacore::Int> dirPixelAxis = cSys.pixelAxes( directionIndex );
        casacore::Vector<casacore::Int> pixAx(2);
        pixAx[0] = dirPixelAxis[0];
        pixAx[1] = dirPixelAxis[1];
        casacore::RegionManager regMan;
        imageRegion = regMan.wpolygon( worldVertexX, worldVertexY,  pixAx, cSys, "abs");
    }
    return imageRegion;
}


casacore::ImageRegion*
RegionRecordFactory::_getRectangle( casacore::ImageInterface<casacore::Float>* casaImage,
        const QPolygonF& corners, const std::vector<int>& slice ){
    casacore::ImageRegion* imageRegion = NULL;

    std::pair<double,double> minCorners;
    std::pair<double,double> maxCorners;
    _getMinMaxCorners( corners, minCorners, maxCorners );
    casacore::CoordinateSystem cSys = casaImage->coordinates();
    int imageDim = casaImage->shape().nelements();
    casacore::Vector<casacore::Double> worldVerticesBLC( imageDim );
    casacore::Vector<casacore::Double> worldVerticesTRC( imageDim );

    bool blcValid = _getWorldVertex( minCorners.first, minCorners.second, cSys,
            slice, worldVerticesBLC );
    bool trcValid = _getWorldVertex( maxCorners.first, maxCorners.second, cSys,
            slice, worldVerticesTRC );

    if ( blcValid && trcValid ){
        casacore::Vector<casacore::String> axisUnits = cSys.worldAxisUnits();
        casacore::Vector<casacore::Quantity> blc(imageDim);
        casacore::Vector<casacore::Quantity> trc(imageDim);
        for ( int ax = 0; ax < imageDim; ax++) {
            blc[ax] = casacore::Quantity( worldVerticesBLC[ax], axisUnits[ax] );
            trc[ax] = casacore::Quantity( worldVerticesTRC[ax], axisUnits[ax] );
        }
        casacore::WCBox box(blc, trc, cSys, casacore::Vector<casacore::Int>());
        imageRegion = new casacore::ImageRegion(box);
    }
    return imageRegion;
}


casacore::Record RegionRecordFactory::getRegionRecord(
        casacore::ImageInterface<casacore::Float>* casaImage,
        std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
    const std::vector<int>& slice,
    QString& typeStr ){
    casacore::Record regionRecord;
    //Do a quick check to make sure the slice is actually in the image.
    casacore::IPosition imageShape = casaImage->shape();
    if ( imageShape.nelements() != slice.size() ){
        //Slice does not match image shape.
        return regionRecord;
    }
    else {
        int dims = imageShape.nelements();
        for ( int i = 0; i < dims; i++ ){
            if ( slice[i] >= imageShape[i] ){
                //Slice is not in image;
                return regionRecord;
            }
        }
    }
    QString regionType = region->typeName();
    if ( regionType == Carta::Lib::Regions::Polygon::TypeName ){
    	regionRecord = _getRegionRecordPolygon( casaImage, region, slice, typeStr );
    }
    else if ( regionType == Carta::Lib::Regions::Ellipse::TypeName ){
        typeStr = "Ellipse";
        regionRecord = _getRegionRecordEllipse( casaImage, region, slice );
    }
    else if ( regionType == Carta::Lib::Regions::Rectangle::TypeName ){
    	regionRecord = _getRegionRecordRectangle( casaImage, region, slice, typeStr );
    }
    else if ( regionType == Carta::Lib::Regions::Point::TypeName ){
    	regionRecord = _getRegionRecordPoint( casaImage, region, slice, typeStr );
    }
    else {
        qDebug() <<"RegionRecordFactory::getRegionRecord unrecognized region type: "+regionType;
    }
    return regionRecord;
}


casacore::Record RegionRecordFactory::_getRegionRecordEllipse(
        casacore::ImageInterface<casacore::Float>* casaImage,
        std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
        const std::vector<int>& slice){
    casacore::Record regionRecord;
    if ( region ){
        QRectF boundingRect = region->outlineBox();
        QPointF topLeftCorner = boundingRect.topLeft();
        QPointF bottomRightCorner = boundingRect.bottomRight();
        std::pair<double,double> minCorner( topLeftCorner.x(), topLeftCorner.y());
        std::pair<double,double> maxCorner( bottomRightCorner.x(), bottomRightCorner.y());

        //Get the bounding box corners for the ellipse in world coordinates.
        casacore::CoordinateSystem cSys = casaImage->coordinates();
        casacore::Vector<casacore::Double> worldVertex1;
        bool valid1 = _getWorldVertex( minCorner.first, minCorner.second, cSys,
                slice, worldVertex1 );
        casacore::Vector<casacore::Double> worldVertex2;
        bool valid2 = _getWorldVertex( maxCorner.first, maxCorner.second, cSys,
                slice, worldVertex2 );
        if ( valid1 && valid2 ){
            //Make an elliptical region based on the corners.
            casacore::ImageRegion* ellipsoid = _getEllipsoid( cSys, worldVertex1, worldVertex2 );
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

casacore::Record RegionRecordFactory::_getRegionRecordPolygon(
        casacore::ImageInterface<casacore::Float>* casaImage,
        std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
        const std::vector<int>& slice, QString& typeStr ){
    casacore::Record regionRecord;
    if ( region ){
        Carta::Lib::Regions::Polygon* polygonRegion = dynamic_cast<Carta::Lib::Regions::Polygon*>( region.get());
        QPolygonF polygon = polygonRegion->qpolyf();
        int cornerCount = polygon.size();

        casacore::ImageRegion* region = nullptr;
        //Polygonal region
        if ( cornerCount >= 3 ){
            typeStr = "Polygon";
            region = _getPolygon( casaImage, polygon, slice );
        }
        else {
            qWarning() << "Unknown polygon region with: "<<cornerCount<<" corners.";
        }
        if ( region != nullptr ){
            regionRecord = region->toRecord("");
            delete region;
        }
    }
    return regionRecord;
}

casacore::Record RegionRecordFactory::_getRegionRecordPoint(
        casacore::ImageInterface<casacore::Float>* casaImage,
        std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
        const std::vector<int>& slice, QString& typeStr ){
    casacore::Record regionRecord;
    if ( region ){
    	Carta::Lib::Regions::Point* pointRegion = dynamic_cast<Carta::Lib::Regions::Point*>( region.get());
    	QRectF polygon = pointRegion->outlineBox();
    	QPointF centerVal = polygon.center();
    	QRectF pointRect( centerVal.x(), centerVal.y(), centerVal.x(), centerVal.y() );
    	typeStr = "Point";
    	casacore::ImageRegion* region = _getRectangle( casaImage, pointRect, slice );
    	if ( region != nullptr ){
    		regionRecord = region->toRecord("");
    		delete region;
    	}
    }
    return regionRecord;
}


casacore::Record RegionRecordFactory::_getRegionRecordRectangle(
        casacore::ImageInterface<casacore::Float>* casaImage,
        std::shared_ptr<Carta::Lib::Regions::RegionBase> region,
        const std::vector<int>& slice, QString& typeStr ){
    casacore::Record regionRecord;
    if ( region ){
    	Carta::Lib::Regions::Rectangle* rectRegion = dynamic_cast<Carta::Lib::Regions::Rectangle*>( region.get());
    	QRectF polygon = rectRegion->outlineBox();
    	int width = polygon.width();
    	int height = polygon.height();

    	casacore::ImageRegion* region = nullptr;
    	//Rectangular region or point
    	if ( width > 0 || height > 0 ){
    		typeStr = "Rectangle";
    		region = _getRectangle( casaImage, polygon, slice );
    	}
    	if ( region != nullptr ){
    		regionRecord = region->toRecord("");
    		delete region;
    	}
    }
    return regionRecord;
}


bool RegionRecordFactory::_getWorldVertex( int pixelX, int pixelY, const casacore::CoordinateSystem& cSys,
        const std::vector<int>& slice, casacore::Vector<casacore::Double>& worldVertices ){
    int imageDim = slice.size();
    bool worldFilled = false;
    casacore::Vector<casacore::Double> pixelVertices( imageDim );
    casacore::Int directionIndex = cSys.findCoordinate(casacore::Coordinate::DIRECTION);
    if ( directionIndex >= 0 ){
        casacore::Vector<casacore::Int> dirPixelAxis = cSys.pixelAxes(directionIndex);
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

