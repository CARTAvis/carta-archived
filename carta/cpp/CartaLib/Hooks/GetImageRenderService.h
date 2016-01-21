/**
 * Hook for adding new image render service.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include "CartaLib/IImageRenderService.h"
#include <vector>

namespace Carta
{
namespace Lib
{
namespace Hooks
{
class GetImageRenderService : public BaseHook
{
    CARTA_HOOK_BOILER1( GetImageRenderService );

public:

    /**
     * @brief Result is a render service.
     */
    typedef IImageRenderService::SharedPtr ResultType;

    /**
     * @brief No input
     */
    struct Params { };

    /**
     * @brief constructor
     * @param pptr pointer to the input parameters
     */
    GetImageRenderService( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
    {
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr;
};
}
}
}
