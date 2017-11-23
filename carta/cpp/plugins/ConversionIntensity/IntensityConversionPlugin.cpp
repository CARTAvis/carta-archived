#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/ConversionIntensityHook.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "plugins/ConversionIntensity/IntensityConversionPlugin.h"
#include "plugins/ConversionIntensity/ConverterIntensity.h"
#include <QDebug>

IntensityConversionPlugin::IntensityConversionPlugin( QObject * parent ) : QObject( parent ) {
}


bool IntensityConversionPlugin::handleHook( BaseHook & hookData ){
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
                qWarning() << "Current units are empty. The intensity conversion plugin cannot proceed.";
                return false;
            }
            
            CCImageBase * base = dynamic_cast<CCImageBase*>( image.get() );
            
            if ( base ){
                casacore::ImageInfo information = base->getImageInfo();
                casacore::GaussianBeam beam;
                
                if ( !information.hasMultipleBeams() ){
                    beam = information.restoringBeam();
                }
                else {
                    beam = information.restoringBeam( 0, -1 );
                }
    
                casacore::Double beamArea = beam.getArea( "arcsec2");
                

                double maxValue = hook.paramsPtr->m_maxValueY;
                QString maxUnits = hook.paramsPtr->m_maxUnit;
                
                try {
                    hook.result = ConverterIntensity::converters(oldUnits, newUnits, maxValue, maxUnits, beamArea);
                } catch (const QString& error) {
                    qWarning() << error;
                    return false;
                }
                return true;
            }
        }
    }
    
    qWarning() << "Conversion intensity doesn't know how to handle this hook";
    return false;
} // handleHook


std::vector < HookId > IntensityConversionPlugin::getInitialHookList() {
    return {
        Carta::Lib::Hooks::Initialize::staticId,
        Carta::Lib::Hooks::ConversionIntensityHook::staticId
    };
}


IntensityConversionPlugin::~IntensityConversionPlugin() {
}
