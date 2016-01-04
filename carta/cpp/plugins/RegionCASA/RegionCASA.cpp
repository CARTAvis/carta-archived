#include "RegionCASA.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "plugins/CasaImageLoader/CCMetaDataInterface.h"
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/LoadRegion.h"
#include "CartaLib/RegionInfo.h"
#include "CartaLib/IImage.h"
#include "imageanalysis/Annotations/RegionTextList.h"
#include "imageanalysis/Annotations/AnnRegion.h"

#include <QDebug>


RegionCASA::RegionCASA(QObject *parent) :
    QObject(parent){
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
            qDebug() << "Loading: "<<fileName;
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

void RegionCASA::_getWorldVertices(std::vector<casa::Quantity>& x, std::vector<casa::Quantity>& y,
        const casa::CoordinateSystem& csys,
        const casa::Vector<casa::MDirection>& directions ) const {

    const casa::IPosition dirAxes = csys.directionAxesNumbers();
    casa::String xUnit = csys.worldAxisUnits()[dirAxes[0]];
    casa::String yUnit = csys.worldAxisUnits()[dirAxes[1]];
    //Vector<MDirection> corners = getConvertedDirections();
    int directionCount = directions.size();
    x.resize( directionCount );
    y.resize( directionCount );
    for (int i = 0; i < directionCount; i++) {
        x[i] = casa::Quantity(directions[i].getAngle(xUnit).getValue(xUnit)[0], xUnit);
        y[i] = casa::Quantity(directions[i].getAngle(yUnit).getValue(yUnit)[1], yUnit);
    }
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
    qDebug() << "RegionCASA plugin trying to load image: " << fname;
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
                if ( aaregions[i].getType() == casa::AsciiAnnotationFileLine::ANNOTATION ){
                    std::shared_ptr<Carta::Lib::RegionInfo> rInfo( new Carta::Lib::RegionInfo());
                    const casa::AnnRegion* reg = dynamic_cast<const casa::AnnRegion*>
                        (aaregions[i].getAnnotationBase().get() );
                    casa::Vector<casa::MDirection> directions = reg->getConvertedDirections();
                    casa::AnnotationBase::Direction points = reg->getDirections();
                    std::vector<std::pair<double,double> > corners =
                            _getPixelVertices( points, *cs.get(), directions );
                    int cornerCount = corners.size();
                    qDebug() << "Corner count="<<cornerCount;
                    for ( int j = 0; j < cornerCount; j++ ){
                        qDebug() << "j="<<j;
                        /*std::pair<casa::Quantity,casa::Quantity> corner = points[j];
                        casa::Unit pixelUnit( "pixel");
                        qDebug() << "First corner unit="<<corner.first.getUnit().c_str();
                        double radValFirst = corner.first.getValue( pixelUnit);
                        double radValSecond = corner.second.getValue( pixelUnit );
                        qDebug() << "j="<<j<<" first="<<radValFirst<<" second="<<radValSecond;
                        rInfo->addCorner( std::pair<double,double>( radValFirst, radValSecond ));
                        */
                        qDebug() << "Pixel corner="<<corners[j].first<<" and "<<corners[j].second;
                        rInfo->addCorner( corners[j].first, corners[j].second );
                    }
                    int annType = reg->getType();
                    qDebug() << "Region type="<<annType;
                    switch( annType ){
                    case casa::AnnotationBase::RECT_BOX : {
                        qDebug() << "Read box region";
                        rInfo->setRegionType( Carta::Lib::RegionInfo::RegionType::Polygon );
                    }
                    break;
                    case casa::AnnotationBase::ELLIPSE : {
                        qDebug() << "Read ellipse region";
                        rInfo->setRegionType( Carta::Lib::RegionInfo::RegionType::Ellipse );
                    }
                    break;
                    case casa::AnnotationBase::POLYGON : {
                        rInfo->setRegionType( Carta::Lib::RegionInfo::RegionType::Polygon );
                    }
                    break;
                    //Point????
                    case casa::AnnotationBase::SYMBOL : {
                        rInfo->setRegionType( Carta::Lib::RegionInfo::RegionType::Polygon );
                    }
                    break;
                    }
                    regionInfos.push_back( rInfo );
                }
            }
        }
    }
    return regionInfos;
}
