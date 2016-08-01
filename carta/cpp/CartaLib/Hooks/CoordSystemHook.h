/**
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include "CartaLib/Regions/ICoordSystem.h"

namespace Cartab
{
namespace Lib
{
namespace Hooks
{
/// \brief Hook for creating a coordinate system from a JSON string
///
class CoordSystemHook : public BaseHook
{
    CARTA_HOOK_BOILER1( CoordSystemHook );

public:

    typedef Carta::Lib::Regions::ICoordSystem * ResultType;
    typedef struct {
        QString serializedCS;
    } Params;

    /// standard constructor (could be probably a macro)
    CoordSystemHook( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
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
