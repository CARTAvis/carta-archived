/**
 * Hook loading astronomical images.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "CartaLib/IWcsGridRenderService.h"
#include <QImage>
#include <QColor>
#include <vector>
#include <functional>
#include <memory>

namespace Carta
{
namespace Lib
{
namespace Hooks
{

/// get an instance of an synchronous grid renderer
class GetWcsGridRendererHook : public BaseHook
{
    // all hooks should have this:
    CARTA_HOOK_BOILER1( GetWcsGridRendererHook )

public:

    /// the result of this hook is a pointer to a render object
    typedef IWcsGridRenderService::SharedPtr ResultType;

    /// input parameters: none are needed for this hook
    struct Params { };

    GetWcsGridRendererHook( Params * pptr )
        : BaseHook( staticId )
          , paramsPtr( pptr )
    {
        // force instantiation of templates
        CARTA_ASSERT( is < Me > () );
    }

    ResultType result;
    Params * paramsPtr = nullptr;
};
}
}
}
