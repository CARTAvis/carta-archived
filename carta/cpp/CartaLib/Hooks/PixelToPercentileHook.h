/**
 * Hook for converting intensity units.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include "CartaLib/IPercentileCalculator.h"

namespace Carta
{
namespace Lib
{
namespace Image {
class ImageInterface;
}
namespace Hooks
{
class PixelToPercentileHook : public BaseHook
{
    CARTA_HOOK_BOILER1( PixelToPercentileHook );

public:

   typedef Carta::Lib::IPercentilesToPixels::SharedPtr ResultType;

    /**
     * @brief Params
     */
     struct Params {

            Params(
                std::shared_ptr<Image::ImageInterface> image,
                int spectralIndex,
                Carta::Lib::IntensityUnitConverter::SharedPtr converter,
                std::vector<double> hertzValues,
            ) {
                m_image = image;
            }

            std::shared_ptr<Image::ImageInterface> m_image;
        };

    /**
     * @brief PreRender
     * @param pptr
     *
     * @todo make hook constructors protected, so that only hook helper can create them
     */
    PixelToPercentileHook( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr;
};
}
}
}
