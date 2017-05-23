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

std::pair<casacore::Vector<casacore::Quantity>,casacore::Vector<casacore::Quantity> > ImageRegionGenerator::_getEllipsePositionRadii(
		Carta::Lib::Regions::Ellipse* ellipse, const casacore::CoordinateSystem &cs ){


	QRectF outlineBox = ellipse->outlineBox();
	QPointF topLeftPt = outlineBox.topLeft();
	QPointF centerPt = outlineBox.center();
	bool validCenter = false;
	casacore::Vector<casacore::Double> centerWorld = _toWorld( cs, centerPt.x(), centerPt.y(), &validCenter );
	bool validTopLeft = false;
	casacore::Vector<casacore::Double> topLeft = _toWorld( cs, topLeftPt.x(), topLeftPt.y(), &validTopLeft );

	casacore::Vector<casacore::Quantum<casacore::Double> > qcenter, qtlc;
	casacore::Vector<casacore::Quantity> radii_(2);

	if ( validCenter && validTopLeft ){
		qcenter.resize(2);
		qtlc.resize(2);

		casacore::String units( RAD_UNITS.toStdString().c_str() );

		qcenter[0] = casacore::Quantum<casacore::Double>(centerWorld[0], units);
		qcenter[1] = casacore::Quantum<casacore::Double>(centerWorld[1], units);

		qtlc[0] = casacore::Quantum<casacore::Double>(topLeft[0], units);
		qtlc[1] = casacore::Quantum<casacore::Double>(topLeft[1], units);

		int directionIndex = cs.directionCoordinateNumber();
		casacore::MDirection::Types cccs = casacore::MDirection::N_Types;
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

			casacore::Vector<casacore::Double> center_rad(2);
			center_rad[0] = qcenter[0].getValue( units );
			center_rad[1] = qcenter[1].getValue( units );
			casacore::MDirection mdcenter( casacore::Quantum<casacore::Vector<casacore::Double> > (center_rad,units), cccs  );

			casacore::Vector<casacore::Double> tlc_rad_x(2);
			tlc_rad_x[0] = qtlc[0].getValue( units);
			tlc_rad_x[1] = qcenter[1].getValue( units );
			casacore::MDirection mdtlc_x( casacore::Quantum<casacore::Vector<casacore::Double> >(tlc_rad_x,units),cccs );

			casacore::Vector<casacore::Double> tlc_rad_y(2);
			tlc_rad_y[0] = qcenter[0].getValue(units);
			tlc_rad_y[1] = qtlc[1].getValue(units);
			casacore::MDirection mdtlc_y( casacore::Quantum<casacore::Vector<casacore::Double> >(tlc_rad_y,units),cccs );

			double xdistance = mdcenter.getValue( ).separation(mdtlc_x.getValue( ));
			double ydistance = mdcenter.getValue( ).separation(mdtlc_y.getValue( ));
			radii_[0] = casacore::Quantity(xdistance, units);
			radii_[1] = casacore::Quantity(ydistance, units);
		}
	}
	return std::make_pair(qcenter,radii_);
}


casacore::ImageRegion* ImageRegionGenerator::makeRegion( casacore::ImageInterface<casacore::Float> * casaImage,
		std::shared_ptr<Carta::Lib::Regions::RegionBase> region ){
	casacore::ImageRegion* imageRegion = nullptr;
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

casacore::ImageRegion* ImageRegionGenerator::_makeRegionRectangle( casacore::ImageInterface<casacore::Float> * image,
		const QRectF& outlineBox){
	casacore::ImageRegion* imageRegion = nullptr;
	if ( image ){
		casacore::Vector<casacore::Int> dispAxes(2);
		const casacore::CoordinateSystem &cs = image->coordinates( );
		int directionIndex = cs.findCoordinate( casacore::Coordinate::DIRECTION );
		if ( directionIndex >= 0 ){
			casacore::Vector<casacore::Int> dirPixelAxis = cs.pixelAxes(directionIndex);
			dispAxes(0) = dirPixelAxis[0];
			dispAxes(1) = dirPixelAxis[1];

			casacore::Vector<casacore::Quantum<casacore::Double> > qbrc(2);
			casacore::Vector<casacore::Quantum<casacore::Double> > qtlc(2);

			QPointF tlc = outlineBox.topLeft();
			QPointF brc = outlineBox.bottomRight();
			bool blcValid = false;
			casacore::Vector<casacore::Double> brcWorld = _toWorld( cs, brc.x(), brc.y(), &blcValid );
			bool trcValid = false;
			casacore::Vector<casacore::Double> tlcWorld = _toWorld( cs, tlc.x(), tlc.y(), &trcValid );
			if ( blcValid && trcValid ){
				casacore::String unitStr( RAD_UNITS.toStdString().c_str() );
				qtlc[0] = casacore::Quantum<casacore::Double>( tlcWorld[0], unitStr );
				qtlc[1] = casacore::Quantum<casacore::Double>( tlcWorld[1], unitStr );
				qbrc[0] = casacore::Quantum<casacore::Double>( brcWorld[0], unitStr );
				qbrc[1] = casacore::Quantum<casacore::Double>( brcWorld[1], unitStr );
				try {
					casacore::WCBox box( qtlc, qbrc, casacore::IPosition(dispAxes), cs, casacore::Vector<casacore::Int>() );
					imageRegion = new casacore::ImageRegion(box);
				}
				catch( const casacore::AipsError& error ) {
					qDebug() << "Could not make image region error="<<error.getMesg().c_str();
				}
			}
		}
	}
	return imageRegion;
}

casacore::ImageRegion* ImageRegionGenerator::_makeRegionEllipse( casacore::ImageInterface<casacore::Float> * image,
		Carta::Lib::Regions::Ellipse* ellipse){
	casacore::ImageRegion* imageRegion = nullptr;
	if ( image ){
		casacore::Vector<casacore::Int> dispAxes(2);
		const casacore::CoordinateSystem &cs = image->coordinates( );
		int directionIndex = cs.findCoordinate( casacore::Coordinate::DIRECTION );
		if ( directionIndex >= 0 ){
			casacore::Vector<casacore::Int> dirPixelAxis = cs.pixelAxes(directionIndex);
			dispAxes(0) = dirPixelAxis[0];
			dispAxes(1) = dirPixelAxis[1];
			std::pair<casacore::Vector<casacore::Quantity>,casacore::Vector<casacore::Quantity> > positionRadii =
					_getEllipsePositionRadii( ellipse, cs );
			if ( positionRadii.second.size( ) == 2 ) {
				try {
					casacore::WCEllipsoid ellipse( positionRadii.first, positionRadii.second,
							casacore::IPosition(dispAxes), cs );
					imageRegion = new casacore::ImageRegion(ellipse);
				}
				catch( const casacore::AipsError& error ) {
					qDebug() << "Could not calculate ellipse position and radii: "<<error.getMesg().c_str();
				}
			}
		}
	}
	return imageRegion;
}

casacore::ImageRegion* ImageRegionGenerator::_makeRegionPolygon( casacore::ImageInterface<casacore::Float> * image,
		Carta::Lib::Regions::Polygon* polygon ){
	casacore::ImageRegion* imageRegion = nullptr;
	if ( image && polygon ){
		QPolygonF poly = polygon->qpolyf();
		int cornerPointCount = poly.count();
		casacore::Vector<casacore::Double> x( cornerPointCount );
		casacore::Vector<casacore::Double> y( cornerPointCount );
		const casacore::CoordinateSystem &cs = image->coordinates( );
		bool successful = true;
		for ( int i = 0; i < cornerPointCount; ++i ) {
			QPointF polyPt = poly.value( i );
			casacore::Vector<casacore::Double> worldPt = _toWorld( cs, polyPt.x(), polyPt.y(), &successful );
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
			casacore::Vector<casacore::Int> dispAxes(2);
			const casacore::CoordinateSystem &cs = image->coordinates( );
			int directionIndex = cs.findCoordinate( casacore::Coordinate::DIRECTION );
			if ( directionIndex >= 0 ){
				casacore::Vector<casacore::Int> dirPixelAxis = cs.pixelAxes(directionIndex);
				dispAxes(0) = dirPixelAxis[0];
				dispAxes(1) = dirPixelAxis[1];
				const casacore::String units( RAD_UNITS.toStdString().c_str() );
				try {
					casacore::Quantum<casacore::Vector<casacore::Double> > qx( x, units );
					casacore::Quantum<casacore::Vector<casacore::Double> > qy( y, units );
					casacore::WCPolygon poly(qx, qy, casacore::IPosition(dispAxes), cs);
					imageRegion = new casacore::ImageRegion(poly);
				}
				catch( casacore::AipsError& error ) {
					qDebug() << "Error making image region: "<<error.getMesg().c_str();
				}
			}
		}
	}
	return imageRegion;
}

casacore::Vector<casacore::Double> ImageRegionGenerator::_toWorld( const casacore::CoordinateSystem& cSys,
		double x, double y, bool* successful ){
	int pixelCount = cSys.nPixelAxes();
	casacore::Vector<casacore::Double> pixelPt( pixelCount, 0 );
	casacore::Vector<casacore::Double> worldPt;
	if ( pixelCount >= 2 ){
		pixelPt[0] = x;
		pixelPt[1] = y;
		try {
			worldPt = cSys.toWorld( pixelPt );
			*successful = true;
		}
		catch( const casacore::AipsError& error ){
			qDebug() << error.getMesg().c_str();
			*successful = false;
		}
	}
	else {
		*successful = false;
	}
	return worldPt;
}
