#include "ImageRegionGenerator.h"
#include "CartaLib/Regions/Rectangle.h"
#include "CartaLib/Regions/Ellipse.h"
#include "CartaLib/Regions/Point.h"

#include <QDebug>
#include <QRectF>


#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/images/Regions/WCBox.h>
#include <casacore/images/Regions/WCEllipsoid.h>
#include <casacore/images/Regions/WCPolygon.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/measures/Measures/MDirection.h>

const QString ImageRegionGenerator::RAD_UNITS = "rad";

std::pair<casa::Vector<casa::Quantity>,casa::Vector<casa::Quantity> > ImageRegionGenerator::_getEllipsePositionRadii(
		Carta::Lib::Regions::Ellipse* ellipse, const casa::CoordinateSystem &cs ){


	QRectF outlineBox = ellipse->outlineBox();
	QPointF topLeftPt = outlineBox.topLeft();
	QPointF centerPt = outlineBox.center();
	bool validCenter = false;
	casa::Vector<casa::Double> centerWorld = _toWorld( cs, centerPt.x(), centerPt.y(), &validCenter );
	bool validTopLeft = false;
	casa::Vector<casa::Double> topLeft = _toWorld( cs, topLeftPt.x(), topLeftPt.y(), &validTopLeft );

	casa::Vector<casa::Quantum<casa::Double> > qcenter, qtlc;
	casa::Vector<casa::Quantity> radii_(2);

	if ( validCenter && validTopLeft ){
		qcenter.resize(2);
		qtlc.resize(2);

		casa::String units( RAD_UNITS.toStdString().c_str() );

		qcenter[0] = casa::Quantum<casa::Double>(centerWorld[0], units);
		qcenter[1] = casa::Quantum<casa::Double>(centerWorld[1], units);

		qtlc[0] = casa::Quantum<casa::Double>(topLeft[0], units);
		qtlc[1] = casa::Quantum<casa::Double>(topLeft[1], units);

		int directionIndex = cs.directionCoordinateNumber();
		casa::MDirection::Types cccs = casa::MDirection::N_Types;
		if ( directionIndex < 0 ){
			// no direction coordinate was found...
			double c0 = qcenter[0].getValue();
			double lc0 = qtlc[0].getValue();
			double c1 = qcenter[1].getValue();
			double lc1 = qtlc[1].getValue();
			radii_[0] = qAbs( c0 - lc0 );
			radii_[1] = qAbs( c1 - lc1 );
		}
		else {
			cccs = cs.directionCoordinate( directionIndex).directionType( true );
			// a direction coordinate was found...

			casa::Vector<casa::Double> center_rad(2);
			center_rad[0] = qcenter[0].getValue( units );
			center_rad[1] = qcenter[1].getValue( units );
			casa::MDirection mdcenter( casa::Quantum<casa::Vector<casa::Double> > (center_rad,units), cccs  );

			casa::Vector<casa::Double> tlc_rad_x(2);
			tlc_rad_x[0] = qtlc[0].getValue( units);
			tlc_rad_x[1] = qcenter[1].getValue( units );
			casa::MDirection mdtlc_x( casa::Quantum<casa::Vector<casa::Double> >(tlc_rad_x,units),cccs );

			casa::Vector<casa::Double> tlc_rad_y(2);
			tlc_rad_y[0] = qcenter[0].getValue(units);
			tlc_rad_y[1] = qtlc[1].getValue(units);
			casa::MDirection mdtlc_y( casa::Quantum<casa::Vector<casa::Double> >(tlc_rad_y,units),cccs );

			double xdistance = mdcenter.getValue( ).separation(mdtlc_x.getValue( ));
			double ydistance = mdcenter.getValue( ).separation(mdtlc_y.getValue( ));
			radii_[0] = casa::Quantity(xdistance, units);
			radii_[1] = casa::Quantity(ydistance, units);
		}
	}
	return std::make_pair(qcenter,radii_);
}


casa::ImageRegion* ImageRegionGenerator::makeRegion( casa::ImageInterface<casa::Float> * casaImage,
		std::shared_ptr<Carta::Lib::Regions::RegionBase> region ){
	casa::ImageRegion* imageRegion = nullptr;
	if ( region ){
		QString regionType = region->typeName();
		if ( regionType == "rectangle" ){
			Carta::Lib::Regions::Rectangle* rect =
					dynamic_cast<Carta::Lib::Regions::Rectangle*>( region.get() );
			QRectF box = rect->outlineBox();
			imageRegion = _makeRegionRectangle( casaImage, box );
		}
		else if ( regionType == "ellipse" ){
			Carta::Lib::Regions::Ellipse* ellipse =
					dynamic_cast<Carta::Lib::Regions::Ellipse*>( region.get() );
			imageRegion = _makeRegionEllipse( casaImage, ellipse );
		}
		else if ( regionType == "polygon" ){
			Carta::Lib::Regions::Polygon* poly =
					dynamic_cast<Carta::Lib::Regions::Polygon*>( region.get() );
			imageRegion = _makeRegionPolygon( casaImage, poly );
		}
		else if ( regionType == "Point" ){
			Carta::Lib::Regions::Point* point =
					dynamic_cast<Carta::Lib::Regions::Point*>( region.get() );
			QPointF center = point->outlineBox().center();
			QRectF box( center.x(), center.y(), center.x(), center.y());
			imageRegion = _makeRegionRectangle( casaImage, box );
		}
		else {
			qDebug() << "RegionGenerator::makeRegion unsupported regionType: "<<regionType;
		}
	}
	return imageRegion;
}

casa::ImageRegion* ImageRegionGenerator::_makeRegionRectangle( casa::ImageInterface<casa::Float> * image,
		const QRectF& outlineBox){
	casa::ImageRegion* imageRegion = nullptr;
	if ( image ){
		casa::Vector<casa::Int> dispAxes(2);
		const casa::CoordinateSystem &cs = image->coordinates( );
		int directionIndex = cs.findCoordinate( casa::Coordinate::DIRECTION );
		if ( directionIndex >= 0 ){
			casa::Vector<casa::Int> dirPixelAxis = cs.pixelAxes(directionIndex);
			dispAxes(0) = dirPixelAxis[0];
			dispAxes(1) = dirPixelAxis[1];

			casa::Vector<casa::Quantum<casa::Double> > qbrc(2);
			casa::Vector<casa::Quantum<casa::Double> > qtlc(2);

			QPointF tlc = outlineBox.topLeft();
			QPointF brc = outlineBox.bottomRight();
			bool blcValid = false;
			casa::Vector<casa::Double> brcWorld = _toWorld( cs, brc.x(), brc.y(), &blcValid );
			bool trcValid = false;
			casa::Vector<casa::Double> tlcWorld = _toWorld( cs, tlc.x(), tlc.y(), &trcValid );
			if ( blcValid && trcValid ){
				casa::String unitStr( RAD_UNITS.toStdString().c_str() );
				qtlc[0] = casa::Quantum<casa::Double>( tlcWorld[0], unitStr );
				qtlc[1] = casa::Quantum<casa::Double>( tlcWorld[1], unitStr );
				qbrc[0] = casa::Quantum<casa::Double>( brcWorld[0], unitStr );
				qbrc[1] = casa::Quantum<casa::Double>( brcWorld[1], unitStr );
				try {
					casa::WCBox box( qtlc, qbrc, casa::IPosition(dispAxes), cs, casa::Vector<casa::Int>() );
					imageRegion = new casa::ImageRegion(box);
				}
				catch( const casa::AipsError& error ) {
					qDebug() << "Could not make image region error="<<error.getMesg().c_str();
				}
			}
		}
	}
	return imageRegion;
}

casa::ImageRegion* ImageRegionGenerator::_makeRegionEllipse( casa::ImageInterface<casa::Float> * image,
		Carta::Lib::Regions::Ellipse* ellipse){
	casa::ImageRegion* imageRegion = nullptr;
	if ( image ){
		casa::Vector<casa::Int> dispAxes(2);
		const casa::CoordinateSystem &cs = image->coordinates( );
		int directionIndex = cs.findCoordinate( casa::Coordinate::DIRECTION );
		if ( directionIndex >= 0 ){
			casa::Vector<casa::Int> dirPixelAxis = cs.pixelAxes(directionIndex);
			dispAxes(0) = dirPixelAxis[0];
			dispAxes(1) = dirPixelAxis[1];
			std::pair<casa::Vector<casa::Quantity>,casa::Vector<casa::Quantity> > positionRadii =
					_getEllipsePositionRadii( ellipse, cs );
			if ( positionRadii.second.size( ) == 2 ) {
				try {
					casa::WCEllipsoid ellipse( positionRadii.first, positionRadii.second,
							casa::IPosition(dispAxes), cs );
					imageRegion = new casa::ImageRegion(ellipse);
				}
				catch( const casa::AipsError& error ) {
					qDebug() << "Could not calculate ellipse position and radii: "<<error.getMesg().c_str();
				}
			}
		}
	}
	return imageRegion;
}

casa::ImageRegion* ImageRegionGenerator::_makeRegionPolygon( casa::ImageInterface<casa::Float> * image,
		Carta::Lib::Regions::Polygon* polygon ){
	casa::ImageRegion* imageRegion = nullptr;
	if ( image && polygon ){
		QPolygonF poly = polygon->qpolyf();
		int cornerPointCount = poly.count();
		casa::Vector<casa::Double> x( cornerPointCount );
		casa::Vector<casa::Double> y( cornerPointCount );
		const casa::CoordinateSystem &cs = image->coordinates( );
		bool successful = true;
		for ( int i = 0; i < cornerPointCount; ++i ) {
			QPointF polyPt = poly.value( i );
			casa::Vector<casa::Double> worldPt = _toWorld( cs, polyPt.x(), polyPt.y(), &successful );
			if ( successful ){
				x[i] = worldPt[0];
				y[i] = worldPt[1];
			}
			else {
				qDebug() << "i="<<i<<" could not convert x="<<polyPt.x()<<" y="<<polyPt.y()<<" to world.";
				break;
			}
		}

		if ( successful ){
			casa::Vector<casa::Int> dispAxes(2);
			const casa::CoordinateSystem &cs = image->coordinates( );
			int directionIndex = cs.findCoordinate( casa::Coordinate::DIRECTION );
			if ( directionIndex >= 0 ){
				casa::Vector<casa::Int> dirPixelAxis = cs.pixelAxes(directionIndex);
				dispAxes(0) = dirPixelAxis[0];
				dispAxes(1) = dirPixelAxis[1];
				const casa::String units( RAD_UNITS.toStdString().c_str() );
				try {
					casa::Quantum<casa::Vector<casa::Double> > qx( x, units );
					casa::Quantum<casa::Vector<casa::Double> > qy( y, units );
					casa::WCPolygon poly(qx, qy, casa::IPosition(dispAxes), cs);
					imageRegion = new casa::ImageRegion(poly);
				}
				catch( casa::AipsError& error ) {
					qDebug() << "Error making image region: "<<error.getMesg().c_str();
				}
			}
		}
	}
	return imageRegion;
}

casa::Vector<casa::Double> ImageRegionGenerator::_toWorld( const casa::CoordinateSystem& cSys,
		double x, double y, bool* successful ){
	int pixelCount = cSys.nPixelAxes();
	casa::Vector<casa::Double> pixelPt( pixelCount, 0 );
	casa::Vector<casa::Double> worldPt;
	if ( pixelCount >= 2 ){
		pixelPt[0] = x;
		pixelPt[1] = y;
		try {
			worldPt = cSys.toWorld( pixelPt );
			*successful = true;
		}
		catch( const casa::AipsError& error ){
			qDebug() << error.getMesg().c_str();
			*successful = false;
		}
	}
	else {
		*successful = false;
	}
	return worldPt;
}
