#include "RegionCASA.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "plugins/CasaImageLoader/CCMetaDataInterface.h"
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/LoadRegion.h"
#include "CartaLib/Regions/Ellipse.h"
#include "CartaLib/IImage.h"
#include "casacore/coordinates/Coordinates/DirectionCoordinate.h"
#include "casacore/measures/Measures/MCDirection.h"
#include "imageanalysis/Annotations/RegionTextList.h"
#include "imageanalysis/Annotations/AnnEllipse.h"

#include <QDebug>
#include <QFile>


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
                hook.result = std::vector<std::shared_ptr<Carta::Lib::Regions::RegionBase> >();
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


std::vector< std::shared_ptr<Carta::Lib::Regions::RegionBase> >
RegionCASA::_loadRegion( const QString & fname,
        std::shared_ptr<Carta::Lib::Image::ImageInterface> imagePtr ){
    std::vector<std::shared_ptr<Carta::Lib::Regions::RegionBase> > regionInfos;

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
                std::shared_ptr<Carta::Lib::Regions::RegionBase> rInfo( nullptr);

                casa::Vector<casa::MDirection> directions = ann->getConvertedDirections();
                casa::AnnotationBase::Direction points = ann->getDirections();
                std::vector<QPointF> corners =
                            _getPixelVertices( points, *cs.get(), directions );
                int annType = ann->getType();
                switch( annType ){
                case casa::AnnotationBase::RECT_BOX : {
                    Carta::Lib::Regions::Polygon* poly = new Carta::Lib::Regions::Polygon();
                    rInfo.reset( poly );
                    _addCorners( poly, corners );
                }
                break;
                case casa::AnnotationBase::ELLIPSE : {
                    Carta::Lib::Regions::Ellipse* regionEllipse = new Carta::Lib::Regions::Ellipse();
                    rInfo.reset( regionEllipse );
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

                    // convert to the viewer's world coordinates... <mdirection>
                    casa::MDirection dir_center = casa::MDirection::Convert(ellipse->getCenter( ), csType)();
                    casa::Vector<double> center = dir_center.getAngle("rad").getValue( );
                    // 90 deg around 0 & 180 deg
                    const double major_radius = ellipse->getSemiMajorAxis().getValue("rad");
                    const double minor_radius = ellipse->getSemiMinorAxis().getValue("rad");
                    const double pos_angle = ellipse->getPositionAngle( ).getValue("deg");
                    regionEllipse->setRadiusMajor( major_radius );
                    regionEllipse->setRadiusMinor( minor_radius );
                    regionEllipse->setAngle( pos_angle );
                    QPointF circleCenter( center[0], center[1] );
                    regionEllipse->setCenter( circleCenter );
                }
                break;
                case casa::AnnotationBase::POLYGON : {
                    Carta::Lib::Regions::Polygon* poly = new Carta::Lib::Regions::Polygon();
                    rInfo.reset( poly );
                    _addCorners( poly, corners );
                }
                break;
                //Point????
                case casa::AnnotationBase::SYMBOL : {
                    qWarning()<<"Symbol not currently supported";
                }
                break;
                }
                regionInfos.push_back( rInfo );

            }
        }
    }
    return regionInfos;
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

RegionCASA::~RegionCASA(){

}
