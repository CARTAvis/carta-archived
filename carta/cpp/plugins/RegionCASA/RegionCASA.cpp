#include "RegionCASA.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "plugins/CasaImageLoader/CCMetaDataInterface.h"
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/LoadRegion.h"
#include "CartaLib/RegionInfo.h"
#include "CartaLib/IImage.h"
#include "casacore/coordinates/Coordinates/DirectionCoordinate.h"
#include "casacore/measures/Measures/MCDirection.h"
#include "imageanalysis/Annotations/RegionTextList.h"
#include "imageanalysis/Annotations/AnnEllipse.h"

#include <QDebug>


RegionCASA::RegionCASA(QObject *parent) :
    QObject(parent){
}


void RegionCASA::_addCorners( std::shared_ptr<Carta::Lib::RegionInfo>& rInfo,
        const std::vector<std::pair<double,double> >& corners ){
    int cornerCount = corners.size();
    for ( int j = 0; j < cornerCount; j++ ){
        rInfo->addCorner( corners[j].first, corners[j].second );
    }
}


bool RegionCASA::handleHook(BaseHook & hookData){
    qDebug() << "RegionCASA plugin is handling hook #" << hookData.hookId();
    if( hookData.is<Carta::Lib::Hooks::Initialize>()) {
        return true;
    }
    else if( hookData.is<Carta::Lib::Hooks::LoadRegion>()) {
        Carta::Lib::Hooks::LoadRegion & hook
                = static_cast<Carta::Lib::Hooks::LoadRegion &>( hookData);
        QString fileName = hook.paramsPtr->fileName;
        if ( fileName.length() > 0 ){
            std::shared_ptr<Carta::Lib::Image::ImageInterface> imagePtr = hook.paramsPtr->image;
            hook.result = _loadRegion( fileName, imagePtr );
            return true;
        }
    }
    qWarning() << "Sorry, RegionCASA doesn't know how to handle this hook";
    return false;
}

std::vector<HookId> RegionCASA::getInitialHookList(){
    return {
        Carta::Lib::Hooks::Initialize::staticId,
        Carta::Lib::Hooks::LoadRegion::staticId
    };
}


std::vector<std::pair<double,double> >
RegionCASA::_getPixelVertices( const casa::AnnotationBase::Direction& corners,
        const casa::CoordinateSystem& csys, const casa::Vector<casa::MDirection>& directions ) const {
    std::vector<casa::Quantity> xx, xy;
    _getWorldVertices(xx, xy, csys, directions );
    casa::Vector<casa::Double> world = csys.referenceValue();
    const casa::IPosition dirAxes = csys.directionAxesNumbers();
    casa::String xUnit = csys.worldAxisUnits()[dirAxes[0]];
    casa::String yUnit = csys.worldAxisUnits()[dirAxes[1]];
    int cornerCount = corners.size();

    std::vector<std::pair<double,double> > pixelVertices( cornerCount );
    for (int i=0; i<cornerCount; i++) {
        world[dirAxes[0]] = xx[i].getValue(xUnit);
        world[dirAxes[1]] = xy[i].getValue(yUnit);
        casa::Vector<casa::Double> pixel;
        csys.toPixel(pixel, world);
        pixelVertices[i]= std::pair<double,double>( pixel[dirAxes[0]], pixel[dirAxes[1]] );
    }
    return pixelVertices;
}


std::vector< std::shared_ptr<Carta::Lib::RegionInfo> >
RegionCASA::_loadRegion( const QString & fname, std::shared_ptr<Carta::Lib::Image::ImageInterface> imagePtr ){
    std::vector<std::shared_ptr<Carta::Lib::RegionInfo> > regionInfos;
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
                std::shared_ptr<Carta::Lib::RegionInfo> rInfo( new Carta::Lib::RegionInfo());

                casa::Vector<casa::MDirection> directions = ann->getConvertedDirections();
                casa::AnnotationBase::Direction points = ann->getDirections();
                std::vector<std::pair<double,double> > corners =
                            _getPixelVertices( points, *cs.get(), directions );
                int annType = ann->getType();
                switch( annType ){
                case casa::AnnotationBase::RECT_BOX : {
                    _addCorners( rInfo, corners );
                    rInfo->setRegionType( Carta::Lib::RegionInfo::RegionType::Polygon );
                }
                break;
                case casa::AnnotationBase::ELLIPSE : {
                    rInfo->setRegionType( Carta::Lib::RegionInfo::RegionType::Ellipse );
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
                    const bool x_is_major = ((pos_angle > 45.0 && pos_angle < 135.0) ||
                            (pos_angle > 225.0 && pos_angle < 315.0));
                    const double xradius = (x_is_major ? major_radius : minor_radius);
                    const double yradius = (x_is_major ? minor_radius : major_radius);

                    casa::Vector<casa::Double> world0(2, 0);
                    casa::Vector<casa::Double> world1(2, 0);
                    world0[0] = center[0] - xradius;
                    world0[1] = center[1] - yradius;
                    world1[0] = center[0] + xradius;
                    world1[1] = center[1] + yradius;
                    casa::Vector<casa::Double> pixel0(2, 0);
                    casa::Vector<casa::Double> pixel1(2, 0);
                    std::vector<std::pair<double,double> > ellipseCorners(2);

                    const casa::CoordinateSystem ellipsCoord = ellipse->getCsys();
                    bool firstConvert = ellipsCoord.directionCoordinate().toPixel( pixel0, world0 );
                    bool secondConvert = ellipsCoord.directionCoordinate().toPixel( pixel1, world1 );
                    if ( firstConvert && secondConvert ){
                        ellipseCorners[0] = std::pair<double,double>( pixel0[0], pixel0[1] );
                        ellipseCorners[1] = std::pair<double,double>( pixel1[0], pixel1[1] );
                        _addCorners( rInfo, ellipseCorners );
                    }
                    else {
                        qDebug() << "Problem storing ellipse corners: "<<ellipsCoord.errorMessage().c_str();
                    }
                }
                break;
                case casa::AnnotationBase::POLYGON : {
                    _addCorners( rInfo, corners );
                    rInfo->setRegionType( Carta::Lib::RegionInfo::RegionType::Polygon );
                }
                break;
                //Point????
                case casa::AnnotationBase::SYMBOL : {
                    _addCorners( rInfo, corners );
                    rInfo->setRegionType( Carta::Lib::RegionInfo::RegionType::Polygon );
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
