/**
 * Hook for adding histogram to the system.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include <vector>
#include "CartaLib/Hooks/HistogramResult.h"

namespace Image {
class ImageInterface;
}
namespace Carta
{
namespace Lib
{
namespace Hooks
{
class HistogramHook : public BaseHook
{
    CARTA_HOOK_BOILER1( HistogramHook )

public:

    /**
     * @brief Result is histogram data.
     */
    typedef HistogramResult ResultType;

    /**
     * @brief Params
     */
     struct Params {

            Params( std::vector<std::shared_ptr<Image::ImageInterface>> p_dataSource,
                    int p_binCount, int p_minChannel, int p_maxChannel, double p_minFrequency, double p_maxFrequency,
                    const QString& p_rangeUnits, double p_minIntensity, double p_maxIntensity){
                dataSource = p_dataSource;
                binCount = p_binCount;
                minChannel = p_minChannel;
                maxChannel = p_maxChannel;
                minIntensity = p_minIntensity;
                maxIntensity = p_maxIntensity;
                minFrequency = p_minFrequency;
                maxFrequency = p_maxFrequency;
                rangeUnits = p_rangeUnits;
            }

            std::vector<std::shared_ptr<Image::ImageInterface>> dataSource;
            int binCount;
            int minChannel;
            int maxChannel;
            double minIntensity;
            double maxIntensity;
            double minFrequency;
            double maxFrequency;
            QString rangeUnits;
        };

    /**
     * @brief PreRender
     * @param pptr
     *
     * @todo make hook constructors protected, so that only hook helper can create them
     */
    HistogramHook( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr;
};
}
}
}
