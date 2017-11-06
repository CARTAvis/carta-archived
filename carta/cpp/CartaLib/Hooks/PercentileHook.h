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
class PercentileHook : public BaseHook
{
    CARTA_HOOK_BOILER1( PercentileHook );

public:

   typedef Carta::Lib::IPercentilesToPixels::SharedPtr ResultType;

    /**
     * @brief Params
     */
     struct Params {

            Params( std::shared_ptr<Image::ImageInterface> dataSource ){
                m_dataSource = dataSource;
            }

            std::shared_ptr<Image::ImageInterface> m_dataSource;
        };

    /**
     * @brief PreRender
     * @param pptr
     *
     * @todo make hook constructors protected, so that only hook helper can create them
     */
    PercentileHook( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr;
};
}
}
}
