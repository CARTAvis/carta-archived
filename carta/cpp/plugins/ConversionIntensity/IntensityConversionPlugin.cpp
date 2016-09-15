#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/ConversionIntensityHook.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "plugins/ConversionIntensity/IntensityConversionPlugin.h"
#include "plugins/ConversionIntensity/ConverterIntensity.h"
#include <QDebug>

IntensityConversionPlugin::IntensityConversionPlugin( QObject * parent ) :
    QObject( parent )
{ }


void IntensityConversionPlugin::_getBeamInfo( casa::ImageInfo& information,
        casa::Double& beamAngle, casa::Double& beamArea) const {
    //Get the major and minor axis beam widths.
    casa::GaussianBeam beam;
    bool multipleBeams = information.hasMultipleBeams();
    if ( !multipleBeams ){
        beam = information.restoringBeam();
    }
    else {
        beam = information.restoringBeam( 0, -1 );
    }
    casa::Quantity majorQuantity = beam.getMajor();
    casa::Quantity minorQuantity = beam.getMinor();
    double arcsecArea = beam.getArea( "arcsec2");
    beamArea = arcsecArea;

    //Calculate:  PI * (half power width)^2 * ARCSEC^2_SR_CONVERSIONFACTOR / 4 ln 2
    double halfPowerWidthSquared = (majorQuantity.getValue() * minorQuantity.getValue() );
    const double ARCSEC2_SR_CONVERSION = 0.0000000000235045;
    const double PI = 3.1415926535;
    double solidAngle = PI * halfPowerWidthSquared * ARCSEC2_SR_CONVERSION / (4 * log(2));
    if ( solidAngle > 0 ){
        beamAngle = solidAngle;
    }
    else {
        beamAngle = 0;
    }
}


bool
IntensityConversionPlugin::handleHook( BaseHook & hookData ){
    if ( hookData.is < Carta::Lib::Hooks::Initialize > () ) {
        return true;
    }
    else if ( hookData.is < Carta::Lib::Hooks::ConversionIntensityHook > () ) {
        Carta::Lib::Hooks::ConversionIntensityHook & hook
            = static_cast <Carta::Lib::Hooks::ConversionIntensityHook & > ( hookData );
        std::shared_ptr<Carta::Lib::Image::ImageInterface> image = hook.paramsPtr->m_dataSource;
        if ( image ){
            QString newUnits = hook.paramsPtr->m_newUnit;
            QString oldUnits = hook.paramsPtr->m_oldUnit;
            if ( oldUnits.isEmpty() || oldUnits.trimmed().length() == 0 ){
                return true;
            }
            CCImageBase * base = dynamic_cast<CCImageBase*>( image.get() );
            if ( base ){
                casa::ImageInfo information = base->getImageInfo();
                casa::Double beamAngle;
                casa::Double beamArea;
                _getBeamInfo( information, beamAngle, beamArea );
                std::vector<double> valsX = hook.paramsPtr->m_inputListX;
                std::vector<double> valsY = hook.paramsPtr->m_inputListY;
                double maxValue = hook.paramsPtr->m_maxValueY;
                QString maxUnits = hook.paramsPtr->m_maxUnit;
                if ( valsY.size() > 0 ){
					ConverterIntensity::convert( valsY, valsX,
							oldUnits, newUnits, maxValue, maxUnits,
							beamAngle, beamArea );
                }
                hook.result = valsY;
            }
            return true;
        }
    }
    qWarning() << "Conversion intensity doesn't know how to handle this hook";
    return false;
} // handleHook

std::vector < HookId >
IntensityConversionPlugin::getInitialHookList(){
    return {
        Carta::Lib::Hooks::Initialize::staticId,
        Carta::Lib::Hooks::ConversionIntensityHook::staticId
    };
}

IntensityConversionPlugin::~IntensityConversionPlugin() {

}
