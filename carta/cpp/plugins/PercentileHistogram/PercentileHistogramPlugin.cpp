#include "CartaLib/Hooks/Initialize.h"
#include "CartaLib/Hooks/PixelToPercentileHook.h"
#include "plugins/CasaImageLoader/CCImage.h"
#include "plugins/PercentileHistogram/PercentileHistogramPlugin.h"
#include "plugins/PercentileHistogram/PercentileHistogram.h"
#include <QDebug>


bool PercentileHistogramPlugin::handleHook( BaseHook & hookData ){
    if ( hookData.is < Carta::Lib::Hooks::Initialize > () ) {
        return true;
    }
    
    else if ( hookData.is < Carta::Lib::Hooks::PixelToPercentileHook > () ) {
        Carta::Lib::Hooks::PixelToPercentileHook & hook
            = static_cast <Carta::Lib::Hooks::PixelToPercentileHook & > ( hookData );
        
        // TODO this is currently unused, but we should use it to pick a plugin (maybe)
        std::shared_ptr<Carta::Lib::Image::ImageInterface> image = hook.paramsPtr->m_image;
        int spectralIndex = hook.paramsPtr->m_spectralIndex;
        Carta::Lib::IntensityUnitConverter::SharedPtr converter = hook.paramsPtr->m_converter;
        std::vector<double> hertzValues = hook.paramsPtr->m_hertzValues;
        std::vector<double> minMaxIntensities = hook.paramsPtr->m_minMaxIntensities;
        
//         numberOfBins = ???? // TODO this comes from the plugin config, but rename it first
        
        hook.result = PercentileHistogram(spectralIndex, converter, hertzValues, minMaxIntensities, numberOfBins);
        
        return true;
    }
    
    qWarning() << "Percentile histogram plugin doesn't know how to handle this hook";
    return false;
} // handleHook


std::vector < HookId > PercentileHistogramPlugin::getInitialHookList() {
    return {
        Carta::Lib::Hooks::Initialize::staticId,
        Carta::Lib::Hooks::PixelToPercentileHook::staticId
    };
}

