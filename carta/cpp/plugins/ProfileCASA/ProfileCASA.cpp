#include "ProfileCASA.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "plugins/CasaImageLoader/CCMetaDataInterface.h"
#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/ProfileHook.h"
#include "CartaLib/ProfileInfo.h"
#include "CartaLib/Regions/IRegion.h"
#include "CartaLib/Regions/Ellipse.h"
#include "CartaLib/Regions/Point.h"
#include "CartaLib/Regions/Rectangle.h"
#include "CartaLib/IImage.h"
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <images/Regions/WCEllipsoid.h>
#include <images/Regions/RegionManager.h>
#include <imageanalysis/ImageAnalysis/ImagePolarimetry.h>

#include <iterator>

using namespace std;
#include <imageanalysis/ImageAnalysis/PixelValueManipulator.h>
#include <imageanalysis/ImageAnalysis/PixelValueManipulatorData.h>
#include <imageanalysis/ImageAnalysis/ImageCollapserData.h>

#include <iostream>
#include <QDebug>


ProfileCASA::ProfileCASA(QObject *parent) :
    QObject(parent),
    PIXEL_UNIT( "pix"),
	RADIAN_UNIT( "rad"){
}


casacore::MFrequency::Types ProfileCASA::_determineRefFrame(
        std::shared_ptr<casacore::ImageInterface<casacore::Float> > img ) const {
    casacore::MFrequency::Types freqtype = casacore::MFrequency::DEFAULT;
    if ( img ){
        casacore::CoordinateSystem cSys=img->coordinates();
        casacore::Int specAx=cSys.findCoordinate(casacore::Coordinate::SPECTRAL);
        if ( specAx >= 0 ) {
            casacore::SpectralCoordinate specCoor=cSys.spectralCoordinate(specAx);
            casacore::MFrequency::Types tfreqtype;
            casacore::MEpoch tepoch;
            casacore::MPosition tposition;
            casacore::MDirection tdirection;
            specCoor.getReferenceConversion(tfreqtype, tepoch, tposition, tdirection);
            freqtype = specCoor.frequencySystem(casacore::False); // false means: get the native type
        }
    }
    return freqtype;
}

Carta::Lib::Hooks::ProfileResult ProfileCASA::_generateProfile( casacore::ImageInterface < casacore::Float > * imagePtr,
        std::shared_ptr<Carta::Lib::Regions::RegionBase> regionInfo,
        Carta::Lib::ProfileInfo profileInfo ) const {
    std::vector<std::pair<double,double> > profileData;
    casacore::CoordinateSystem cSys = imagePtr->coordinates();
    casacore::uInt spectralAxis = 0;
    if ( cSys.hasSpectralAxis()){
        spectralAxis = cSys.spectralAxisNumber();
    }
    else {
        int tabCoord = cSys.findCoordinate( casacore::Coordinate::TABULAR );
        if ( tabCoord >= 0 ){
            spectralAxis = tabCoord;
        }
    }
    Carta::Lib::Hooks::ProfileResult profileResult;

    //Get the requested rest frequency & unit
    double restFrequency = profileInfo.getRestFrequency();
    QString restUnit = profileInfo.getRestUnit();

    //No rest frequency was specified so use the rest frequency from the image.
    if ( restUnit.trimmed().length() == 0 ){

        //Fill in the image rest frequency & unit
        if ( cSys.hasSpectralAxis() ){
            double restFrequencyImage = cSys.spectralCoordinate().restFrequency();
            QString restUnitImage = cSys.spectralCoordinate().worldAxisUnits()[0].c_str();
            profileResult.setRestUnits( restUnitImage );
            profileResult.setRestFrequency( restFrequencyImage );
            restFrequency = restFrequencyImage;
            restUnit = restUnitImage;
        }
    }

    int stokesFrame = profileInfo.getStokesFrame();
    int stokesAxis = imagePtr->coordinates().polarizationAxisNumber();
    // qDebug() << stokesFrame << "\n" << stokesAxis << "\n" << imagePtr->ndim() << "\n" << imagePtr->shape().asStdVector();

    casacore::Record regionRecord;
    if ( regionInfo ){
    	QString shape = regionInfo->typeName();
    	if ( shape == "ellipse" || shape=="rectangle"){
    		QRectF box = regionInfo->outlineBox();
    		QPointF topLeft = box.topLeft();
    		QPointF bottomRight = box.bottomRight();
    		casacore::Vector<casacore::Double> x(2);
    		casacore::Vector<casacore::Double> y(2);
    		x[0] = topLeft.x();
    		y[0] = topLeft.y();
    		x[1] = bottomRight.x();
    		y[1] = bottomRight.y();
    		regionRecord = _getRegionRecord( shape, cSys, x, y );
    	}
    	else if ( shape == "Point"){
    		casacore::Vector<casacore::Double> x(1);
    		casacore::Vector<casacore::Double> y(1);
    		QRectF box = regionInfo->outlineBox();
    		x[0] = box.center().x();
    		y[0] = box.center().y();
    		regionRecord = _getRegionRecord( shape, cSys, x, y );
    	}
    	else {
			QPolygonF regionCorners = regionInfo->outlineBox();
			int cornerCount = regionCorners.size();
			casacore::Vector<casacore::Double> x(cornerCount);
			casacore::Vector<casacore::Double> y(cornerCount);
			for ( int i = 0; i < cornerCount; i++ ){
				QPointF corner = regionCorners.value( i );
				x[i] = corner.x();
				y[i] = corner.y();
			}
			regionRecord = _getRegionRecord( shape, cSys, x, y);
    	}


    }
    QString spectralType = profileInfo.getSpectralType();
    QString spectralUnit = profileInfo.getSpectralUnit();
    if ( spectralType == "Channel"){
        spectralUnit = PIXEL_UNIT;
        spectralType = "default";
    }
    casacore::String pixelSpectralType( spectralType.toStdString().c_str() );

    casacore::String unit( spectralUnit.toStdString().c_str() );
    casa::PixelValueManipulatorData::SpectralType specType
        = casa::PixelValueManipulatorData::spectralType( pixelSpectralType );
    casacore::Vector<casacore::Float> jyValues;
    casacore::Vector<casacore::Double> xValues;
    try {

        // This part is used to verify the result.
        // casa::ImagePolarimetry polarimage = casa::ImagePolarimetry(*imagePtr->cloneII());
        // std::shared_ptr<casacore::ImageInterface<casacore::Float> >image(polarimage.stokesQ().cloneII());

        casacore::Slicer slicer(casacore::IPosition(imagePtr->ndim(), 0), imagePtr->shape());
        if (stokesAxis != -1){
            casacore::IPosition blc(imagePtr->ndim(), 0);
            casacore::IPosition trc = imagePtr->shape();
            blc(stokesAxis) = stokesFrame;
            trc(stokesAxis) = 1;
            // qWarning() << blc.asStdVector() << "\n" << trc.asStdVector();
            slicer = casacore::Slicer( blc, trc, Slicer::endIsLength);
        }
        std::shared_ptr<casacore::ImageInterface<casacore::Float> >image
            = make_shared<casacore::SubImage<casacore::Float> > (*imagePtr->cloneII(), slicer, casacore::AxesSpecifier() );
        // qWarning() << image->shape().asStdVector();

        casa::PixelValueManipulator<casacore::Float> pvm(image, &regionRecord, "");
        casa::ImageCollapserData::AggregateType funct = _getCombineMethod( profileInfo );
        casacore::MFrequency::Types freqType = _determineRefFrame( image );
        casacore::String frame = casacore::String( casacore::MFrequency::showType( freqType));

        casacore::Quantity restFreq( restFrequency, casacore::Unit( restUnit.toStdString().c_str()));
        casacore::Record result = pvm.getProfile( spectralAxis, funct, unit, specType,
                &restFreq, frame );

        const casacore::String VALUE_KEY( "values");
        if ( result.isDefined( VALUE_KEY )){
            result.get( VALUE_KEY, jyValues );
        }

        const casacore::String X_KEY( "coords");
        if ( result.isDefined( X_KEY )){
            result.get( X_KEY, xValues );
        }

        int dataCount = jyValues.size();
        for ( int i = 0; i < dataCount; i++ ){
            std::pair<double,double> dataPair(xValues[i], jyValues[i]);
            profileData.push_back( dataPair );
        }
        profileResult.setData( profileData );
    }
    catch( casacore::AipsError& error ){
        qDebug() << "Could not generate profile: "<<error.getMesg().c_str();
    }
    return profileResult;
}


casa::ImageCollapserData::AggregateType ProfileCASA::_getCombineMethod( Carta::Lib::ProfileInfo profileInfo ) const {
    Carta::Lib::ProfileInfo::AggregateType combineType = profileInfo.getAggregateType();
    casa::ImageCollapserData::AggregateType collapseType = casa::ImageCollapserData::AggregateType::MEAN;
    if ( combineType == Carta::Lib::ProfileInfo::AggregateType::MEDIAN ){
        collapseType = casa::ImageCollapserData::AggregateType::MEDIAN;
    }
    else if ( combineType == Carta::Lib::ProfileInfo::AggregateType::SUM ){
        collapseType = casa::ImageCollapserData::AggregateType::SUM;
    }
    else if ( combineType == Carta::Lib::ProfileInfo::AggregateType::VARIANCE ){
        collapseType = casa::ImageCollapserData::AggregateType::VARIANCE;
    }
    else if ( combineType == Carta::Lib::ProfileInfo::AggregateType::MIN ){
        collapseType = casa::ImageCollapserData::AggregateType::MIN;
    }
    else if ( combineType == Carta::Lib::ProfileInfo::AggregateType::MAX ){
        collapseType = casa::ImageCollapserData::AggregateType::MAX;
    }
    else if ( combineType == Carta::Lib::ProfileInfo::AggregateType::RMS ){
        collapseType = casa::ImageCollapserData::AggregateType::RMS;
    }
    else if ( combineType == Carta::Lib::ProfileInfo::AggregateType::FLUX_DENSITY ){
        collapseType = casa::ImageCollapserData::AggregateType::FLUX;
    }
    return collapseType;
}


casacore::ImageRegion* ProfileCASA::_getEllipsoid(const casacore::CoordinateSystem& cSys,
        const casacore::Vector<casacore::Double>& x, const casacore::Vector<casacore::Double>& y) const {
    casacore::Vector<casacore::Quantity> center(2);
    casacore::Vector<casacore::Quantity> radius(2);
    casacore::ImageRegion* imageRegion = NULL;
    if ( x.size() == 2 && y.size() == 2 ){
    	bool successful0 = false;
    	casacore::Vector<casacore::Double> world0 = _toWorld( cSys, x[0], y[0], &successful0 );
    	bool successful1 = false;
    	casacore::Vector<casacore::Double> world1 = _toWorld( cSys, x[1], y[1], &successful1 );
    	if ( successful0 && successful1 ){
			const casacore::String radUnits( RADIAN_UNIT.toStdString().c_str());
			center[0] = casacore::Quantity(  (world0[0]+world1[0])/2, radUnits );
			center[1] = casacore::Quantity( (world0[1]+world1[1])/2, radUnits );
			casacore::MDirection::Types type = casacore::MDirection::N_Types;
			int directionIndex = cSys.findCoordinate( casacore::Coordinate::DIRECTION );
			if ( directionIndex >= 0 ){
				casacore::uInt dirIndex = static_cast<casacore::uInt>(directionIndex);
				type = cSys.directionCoordinate(dirIndex).directionType(true);

				casacore::Vector<casacore::Double> qCenter(2);
				qCenter[0] = center[0].getValue();
				qCenter[1] = center[1].getValue();
				const casacore::String angleUnits( "rad");
				casacore::MDirection mdcenter( casacore::Quantum<casacore::Vector<casacore::Double> >(qCenter,angleUnits), type );

				casacore::Vector<casacore::Double> blc_pix_x(2);
				blc_pix_x[0] = world0[0];
				blc_pix_x[1] = center[1].getValue();
				casacore::MDirection mdblc_x( casacore::Quantum<casacore::Vector<casacore::Double> >(blc_pix_x,angleUnits),type );

				casacore::Vector<casacore::Double> blc_pix_y(2);
				blc_pix_y[0] = center[0].getValue();
				blc_pix_y[1] = world0[1];
				casacore::MDirection mdblc_y( casacore::Quantum<casacore::Vector<casacore::Double> >(blc_pix_y,angleUnits),type );

				double xdistance = mdcenter.getValue( ).separation(mdblc_x.getValue( ));
				double ydistance = mdcenter.getValue( ).separation(mdblc_y.getValue( ));
				const float ERR = 0;
				if ( xdistance > ERR && ydistance > ERR ){
					radius[0] = casacore::Quantity(xdistance, radUnits );
					radius[1] = casacore::Quantity(ydistance, radUnits );

					casacore::Vector<casacore::Int> pixax(2);
					casacore::Vector<casacore::Int> dirPixelAxis = cSys.pixelAxes(directionIndex);
					pixax(0) = dirPixelAxis[0];
					pixax(1) = dirPixelAxis[1];
					casacore::WCEllipsoid ellipsoid( center, radius, casacore::IPosition(pixax), cSys);
					imageRegion = new casacore::ImageRegion( ellipsoid );
				}
			}
        }
    	else {
    		qDebug() << "Could not convert pixels to world coordinates";
    	}
    }
    else {
        qDebug() << "Invalid size (2) for an ellipse: "<<x.size()<<" and "<<y.size();
    }
    return imageRegion;
}


std::vector<HookId> ProfileCASA::getInitialHookList(){
    return {
        Carta::Lib::Hooks::Initialize::staticId,
        Carta::Lib::Hooks::ProfileHook::staticId
    };
}


casacore::ImageRegion* ProfileCASA::_getPolygon(const casacore::CoordinateSystem& cSys,
        const casacore::Vector<casacore::Double>& x, const casacore::Vector<casacore::Double>& y) const {
    casacore::ImageRegion* polygon = NULL;
    const casacore::String pixUnits( "pixel" );
    casacore::RegionManager regMan;
    int n = x.size();
    casacore::Vector<casacore::Quantity> xvertex(n);
    casacore::Vector<casacore::Quantity> yvertex(n);
    for (casacore::Int k = 0; k < n; ++k) {
        xvertex[k] = casacore::Quantity(x[k], pixUnits );
        yvertex[k] = casacore::Quantity(y[k], pixUnits );
    }
    int directionIndex = cSys.findCoordinate( casacore::Coordinate::DIRECTION );
    if ( directionIndex >= 0 ){
        casacore::Vector<casacore::Int> dirPixelAxis = cSys.pixelAxes(directionIndex);
        casacore::Vector<casacore::Int> pixax(2);
        pixax(0) = dirPixelAxis[0];
        pixax(1) = dirPixelAxis[1];
        polygon = regMan.wpolygon(xvertex, yvertex, pixax, cSys, "abs");
    }
    return polygon;
}


casacore::Record ProfileCASA::_getRegionRecord( const QString& shape, const casacore::CoordinateSystem& cSys,
        const casacore::Vector<casacore::Double>& x, const casacore::Vector<casacore::Double>& y) const {
	const casacore::String radUnits( RADIAN_UNIT.toStdString().c_str());
    const casacore::String absStr( "abs");
    casacore::Record regionRecord;
    casacore::Int directionIndex = cSys.findCoordinate(casacore::Coordinate::DIRECTION);
    if ( directionIndex >= 0 ){
        casacore::Vector<casacore::Int> dirPixelAxis = cSys.pixelAxes(directionIndex);
        casacore::RegionManager regMan;
        if ( shape == Carta::Lib::Regions::Rectangle::TypeName ||
        		shape == Carta::Lib::Regions::Point::TypeName ){
            int ptCount = x.size();
            if ( ptCount == 2 ){
                casacore::Vector<casacore::Quantity> blc(2);
                casacore::Vector<casacore::Quantity> trc(2);
                bool successful0 = false;
                casacore::Vector<casacore::Double> w0 = _toWorld( cSys, x[0], y[0], &successful0 );
                bool successful1 = false;
                casacore::Vector<casacore::Double> w1 = _toWorld( cSys, x[1], y[1], &successful1 );
                if ( successful0 && successful1 ){
					blc(0) = casacore::Quantity(w0[0], radUnits);
					blc(1) = casacore::Quantity(w0[1], radUnits);
					trc(0) = casacore::Quantity(w1[0], radUnits);
					trc(1) = casacore::Quantity(w1[1], radUnits);
					casacore::Vector<casacore::Int> pixax(2);
					pixax(0) = dirPixelAxis[0];
					pixax(1) = dirPixelAxis[1];

					casacore::Record* imagregRecord = regMan.wbox(blc, trc, pixax, cSys, absStr, radUnits);
					regionRecord = *imagregRecord;
					delete imagregRecord;
                }
                else {
                	qWarning() << "Could not generate region profile";
                }
            }
            else if ( ptCount == 1 ){
                //Try a rectangle with blc=trc;
                casacore::Vector<casacore::Quantity> blc(2);
                casacore::Vector<casacore::Quantity> trc(2);
                bool successful0 = false;
                casacore::Vector<casacore::Double> w0 = _toWorld( cSys, x[0], y[0], &successful0 );
                if ( successful0 ){
					blc(0) = casacore::Quantity(w0[0], radUnits);
					blc(1) = casacore::Quantity(w0[1], radUnits);
					trc(0) = casacore::Quantity(w0[0], radUnits);
					trc(1) = casacore::Quantity(w0[1], radUnits);
					casacore::Vector<casacore::Int> pixax(2);
					pixax(0) = dirPixelAxis[0];
					pixax(1) = dirPixelAxis[1];

					casacore::Record* imagregRecord = regMan.wbox(blc, trc, pixax, cSys, absStr, radUnits);
					regionRecord=*imagregRecord;
					delete imagregRecord;
                }
            }
            else {
            	qWarning()<<"Unrecognized point count "<<ptCount;
            }
        }
        else if ( shape == Carta::Lib::Regions::Polygon::TypeName ){
        	casacore::ImageRegion* polygon = _getPolygon( cSys, x, y );
        	if ( polygon != NULL ){
        		regionRecord = polygon->toRecord(casacore::String(""));
        		delete polygon;
        	}

        }
        else if ( shape == Carta::Lib::Regions::Ellipse::TypeName ){
        	casacore::ImageRegion* ellipsoid = _getEllipsoid( cSys, x, y );
            if ( ellipsoid != NULL ){
                regionRecord = ellipsoid->toRecord("");
                delete ellipsoid;
            }
        }
        else {
        	qDebug() << "getRegion record shape not handled: "<<shape;
        }
    }
    return regionRecord;
}


bool ProfileCASA::handleHook(BaseHook & hookData){
    //qDebug() << "ProfileCASA plugin is handling hook #" << hookData.hookId();
    if( hookData.is<Carta::Lib::Hooks::Initialize>()) {
        return true;
    }
    else if ( hookData.is<Carta::Lib::Hooks::ProfileHook>()){
        Carta::Lib::Hooks::ProfileHook & hook
            = static_cast<Carta::Lib::Hooks::ProfileHook &>( hookData);

        std::shared_ptr<Carta::Lib::Image::ImageInterface> imagePtr = hook.paramsPtr->m_dataSource;

        if ( !imagePtr ) {
            return false;
        }

        casacore::ImageInterface < casacore::Float > * casaImage = cartaII2casaII_float( imagePtr );
        if( ! casaImage) {
            qWarning() << "Profile plugin: not an image created by casaimageloader...";
            return false;
        }

        std::shared_ptr<Carta::Lib::Regions::RegionBase> regionInfo = hook.paramsPtr->m_regionInfo;
        Carta::Lib::ProfileInfo profileInfo = hook.paramsPtr->m_profileInfo;
        hook.result = _generateProfile( casaImage, regionInfo, profileInfo );
        return true;
    }
    qWarning() << "Sorry, ProfileCASA doesn't know how to handle this hook";
    return false;
}

casacore::Vector<casacore::Double> ProfileCASA::_toWorld( const casacore::CoordinateSystem& cSys,
		double x, double y, bool* successful ) const {
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
			qDebug() << error.getMesg().c_str()<<" pixel=("<<x<<","<<y<<")";
			*successful = false;
		}
	}
	else {
		*successful = false;
	}
	return worldPt;
}

ProfileCASA::~ProfileCASA(){

}
