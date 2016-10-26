#include "RegionCASA.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "plugins/CasaImageLoader/CCMetaDataInterface.h"
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/LoadRegion.h"
#include "CartaLib/Regions/Ellipse.h"
#include "CartaLib/Regions/Point.h"
#include "CartaLib/Regions/Rectangle.h"
#include "CartaLib/IImage.h"
#include "casacore/coordinates/Coordinates/DirectionCoordinate.h"
#include "casacore/measures/Measures/MCDirection.h"
#include "imageanalysis/Annotations/RegionTextList.h"
#include "imageanalysis/Annotations/AnnEllipse.h"

#include <QDebug>
#include <QFile>
#include <QtCore/qmath.h>


RegionCASA::RegionCASA(QObject *parent) :
    QObject(parent){
}


void RegionCASA::_addCorners( Carta::Lib::Regions::Polygon* rInfo,
        const std::vector<QPointF >& corners ){
    if ( rInfo ){
        QPolygonF polygonF = QPolygonF::fromStdVector( corners );
        rInfo->setqpolyf( polygonF );
    }
}


bool RegionCASA::handleHook(BaseHook & hookData){
    qDebug() << "RegionCASA plugin is handling hook #" << hookData.hookId();
    bool hookHandled = false;
    if( hookData.is<Carta::Lib::Hooks::Initialize>()) {
        hookHandled = true;
    }
    else if( hookData.is<Carta::Lib::Hooks::LoadRegion>()) {
        Carta::Lib::Hooks::LoadRegion & hook
                = static_cast<Carta::Lib::Hooks::LoadRegion &>( hookData);
        QString fileName = hook.paramsPtr->fileName;
        if ( fileName.length() > 0 ){
            //Before going to a lot of trouble, make sure we can open the file and that it has
            //the potential to be a CASA region.
            bool casaRegion = _isCASARegion( fileName );
            if ( casaRegion ){
                std::shared_ptr<Carta::Lib::Image::ImageInterface> imagePtr = hook.paramsPtr->image;
                hook.result = _loadRegion( fileName, imagePtr );
            }
            else {
                //Not a casa region so return an empty vector.
                hook.result = std::vector<Carta::Lib::Regions::RegionBase*>();
            }
            hookHandled = true;
        }
    }
    return hookHandled;
}

std::vector<HookId> RegionCASA::getInitialHookList(){
    return {
        Carta::Lib::Hooks::Initialize::staticId,
        Carta::Lib::Hooks::LoadRegion::staticId
    };
}


std::vector<QPointF>
RegionCASA::_getPixelVertices( const casa::AnnotationBase::Direction& corners,
        const casa::CoordinateSystem& csys, const casa::Vector<casa::MDirection>& directions ) const {
    std::vector<casa::Quantity> xx, xy;
    _getWorldVertices(xx, xy, csys, directions );
    casa::Vector<casa::Double> world = csys.referenceValue();
    const casa::IPosition dirAxes = csys.directionAxesNumbers();
    casa::String xUnit = csys.worldAxisUnits()[dirAxes[0]];
    casa::String yUnit = csys.worldAxisUnits()[dirAxes[1]];
    int cornerCount = corners.size();

    std::vector<QPointF> pixelVertices( cornerCount );
    for (int i=0; i<cornerCount; i++) {
        world[dirAxes[0]] = xx[i].getValue(xUnit);
        world[dirAxes[1]] = xy[i].getValue(yUnit);
        casa::Vector<casa::Double> pixel;
        csys.toPixel(pixel, world);
        pixelVertices[i]= QPointF( pixel[dirAxes[0]], pixel[dirAxes[1]] );
    }
    return pixelVertices;
}


bool RegionCASA::_isCASARegion( const QString& fileName ) const {
    bool casaRegion = false;
    QFile inputFile( fileName );
    if ( inputFile.open( QIODevice::ReadOnly ) ){
        QString firstLine = inputFile.readLine();
        if ( firstLine.contains( "#CRTF" ) ){
            casaRegion = true;
        }
    }
    return casaRegion;
}


std::vector<Carta::Lib::Regions::RegionBase*>
RegionCASA::_loadRegion( const QString & fname,
		std::shared_ptr<Carta::Lib::Image::ImageInterface> imagePtr ){
	std::vector<Carta::Lib::Regions::RegionBase*> regionInfos;

	casa::String fileName( fname.toStdString().c_str() );
	CCImageBase * base = dynamic_cast<CCImageBase*>( imagePtr.get() );
	if ( base ){
		Carta::Lib::Image::MetaDataInterface::SharedPtr metaPtr = base->metaData();
		CCMetaDataInterface* metaData = dynamic_cast<CCMetaDataInterface*>(metaPtr.get());
		if ( metaData ){
			std::shared_ptr<casa::CoordinateSystem> cs = metaData->getCoordinateSystem();
			std::vector < int > dimensions = imagePtr->dims();
			int dimCount = dimensions.size();
			casa::IPosition shape(dimCount);
			for ( int i = 0; i < dimCount; i++ ){
				shape[i] = dimensions[i];
			}
			casa::RegionTextList regionList( fileName, *cs.get(), shape );
			casa::Vector<casa::AsciiAnnotationFileLine> aaregions = regionList.getLines();
			int regionCount = aaregions.size();
			for ( int i = 0; i < regionCount; i++ ){
				if ( aaregions[i].getType() != casa::AsciiAnnotationFileLine::ANNOTATION ){
					continue;
				}

				casa::CountedPtr<const casa::AnnotationBase> ann = aaregions[i].getAnnotationBase();
				Carta::Lib::Regions::RegionBase* rInfo = nullptr;

				casa::Vector<casa::MDirection> directions = ann->getConvertedDirections();
				casa::AnnotationBase::Direction points = ann->getDirections();
				std::vector<QPointF> corners =
						_getPixelVertices( points, *cs.get(), directions );

				int annType = ann->getType();
				switch( annType ){
				case casa::AnnotationBase::RECT_BOX : {
					Carta::Lib::Regions::Rectangle* rect = new Carta::Lib::Regions::Rectangle();
					rInfo = rect;
					int cornerCount = corners.size();
					double dataMinX = std::numeric_limits<double>::max();
					double dataMinY = std::numeric_limits<double>::max();
					double dataMaxX = -1 * dataMinX;
					double dataMaxY = -1 * dataMaxY;
					QRectF rectangle;
					for ( int i = 0; i < cornerCount; i++ ){
						if ( corners[i].x() < dataMinX ){
							dataMinX = corners[i].x();
						}
						if ( corners[i].y() < dataMinY ){
							dataMinY = corners[i].y();
						}
						if ( corners[i].x() > dataMaxX ){
							dataMaxX = corners[i].x();
						}
						if ( corners[i].y() > dataMaxY ){
							dataMaxY = corners[i].y();
						}
					}

					rectangle.setTopLeft( QPointF(dataMinX, dataMinY) );
					rectangle.setBottomRight( QPointF(dataMaxX, dataMaxY) );
					rect->setRectangle( rectangle );
				}
				break;
				case casa::AnnotationBase::ELLIPSE : {
					Carta::Lib::Regions::Ellipse* regionEllipse = new Carta::Lib::Regions::Ellipse();
					rInfo = regionEllipse;
					const casa::AnnEllipse* ellipse = dynamic_cast<const casa::AnnEllipse*>( ann.get() );
					casa::Int directionIndex = cs->findCoordinate(casa::Coordinate::Type::DIRECTION );

					casa::MDirection::Types csType = casa::MDirection::EXTRA;
					if ( directionIndex >= 0 ){
						casa::DirectionCoordinate  dCoord = cs->directionCoordinate(directionIndex);
						csType = dCoord.directionType();
					}

					if ( csType == casa::MDirection::EXTRA ){
						qWarning( "Unable to complete elliptical region, unspecified direction type.");
						continue;
					}


					casa::MDirection dir_center = casa::MDirection::Convert(ellipse->getCenter( ), csType)();
					casa::Vector<double> center = dir_center.getAngle("rad").getValue( );
					// 90 deg around 0 & 180 deg
					const double major_radius = ellipse->getSemiMajorAxis().getValue("rad");
					const double minor_radius = ellipse->getSemiMinorAxis().getValue("rad");
					const double pos_angle = ellipse->getPositionAngle( ).getValue("deg");
					QPointF centerRadian( center[0], center[1]);
					double majorRadiusPixel = _getRadiusPixel( centerRadian, corners[0],
							major_radius, pos_angle, *(cs.get()) );
					double minorRadiusPixel = _getRadiusPixel( centerRadian, corners[0],
							minor_radius, pos_angle, *(cs.get()) );

					regionEllipse->setRadiusMajor( majorRadiusPixel );
					regionEllipse->setRadiusMinor( minorRadiusPixel );
					regionEllipse->setAngle( pos_angle );
					QPointF circleCenter( corners[0].x(), corners[0].y() );
					regionEllipse->setCenter( circleCenter );
				}
				break;
				case casa::AnnotationBase::POLYGON : {
					Carta::Lib::Regions::Polygon* poly = new Carta::Lib::Regions::Polygon();
					rInfo = poly;
					_addCorners( poly, corners );
				}
				break;
				//Point????
				case casa::AnnotationBase::SYMBOL : {
					if ( corners.size() == 1 ){
						Carta::Lib::Regions::Point* point = new Carta::Lib::Regions::Point();
						point->setPoint( corners[0] );
						rInfo = point;
					}
					else {
						qWarning()<<"Point incorrectly specified with "<<corners.size()<<" corners.";
					}
				}
				break;
				}
				regionInfos.push_back( rInfo );

			}
		}
	}
	return regionInfos;
}

double RegionCASA::_getRadiusPixel( const QPointF& centerRadian, const QPointF& centerPixel,
		double radius, double angleDegrees, const casa::CoordinateSystem& cSys ) const {
	double pointX = centerRadian.x() + radius * cos( angleDegrees * ( M_PI / 180));
	double pointY = centerRadian.y() + radius * sin ( angleDegrees * ( M_PI / 180));
	bool successful = false;
	casa::Vector<casa::Double> pixelPt = _toPixel( cSys, pointX, pointY, &successful );
	double xDiff = pixelPt[0] - centerPixel.x();
	double yDiff = pixelPt[1] - centerPixel.y();
	double radiusPixel = qSqrt( xDiff * xDiff + yDiff * yDiff );
	return radiusPixel;
}

void RegionCASA::_getWorldVertices(std::vector<casa::Quantity>& x, std::vector<casa::Quantity>& y,
        const casa::CoordinateSystem& csys,
        const casa::Vector<casa::MDirection>& directions ) const {
    const casa::IPosition dirAxes = csys.directionAxesNumbers();
    casa::String xUnit = csys.worldAxisUnits()[dirAxes[0]];
    casa::String yUnit = csys.worldAxisUnits()[dirAxes[1]];
    int directionCount = directions.size();
    x.resize( directionCount );
    y.resize( directionCount );
    for (int i = 0; i < directionCount; i++) {
        x[i] = casa::Quantity(directions[i].getAngle(xUnit).getValue(xUnit)[0], xUnit);
        y[i] = casa::Quantity(directions[i].getAngle(yUnit).getValue(yUnit)[1], yUnit);
    }
}

casa::Vector<casa::Double> RegionCASA::_toPixel( const casa::CoordinateSystem& cSys,
		double x, double y, bool* successful ) const {
	int pixelCount = cSys.nPixelAxes();
	casa::Vector<casa::Double> worldPt( pixelCount, 0 );
	worldPt = cSys.referenceValue();
	casa::Vector<casa::Double> pixelPt( pixelCount);
	pixelPt = cSys.referencePixel();
	if ( pixelCount >= 2 ){
		worldPt[0] = x;
		worldPt[1] = y;
		try {
			pixelPt = cSys.toPixel( worldPt );
			*successful = true;
		}
		catch( const casa::AipsError& error ){
			qDebug() << error.getMesg().c_str()<<" x="<<x<<" y="<<y;
			*successful = false;
		}
	}
	else {
		*successful = false;
	}
	return pixelPt;
}


RegionCASA::~RegionCASA(){

}
