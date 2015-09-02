/**
 * Defines a hook that is executed once, when core is starting up.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include <vector>

namespace Carta
{
namespace Lib
{
namespace Hooks
{
/// \brief Hook for loading a plugin of an unknown type
///
class Initialize : public BaseHook
{
    CARTA_HOOK_BOILER1( Initialize );

public:

    typedef FakeVoid ResultType;
    typedef struct { } Params;

    /// standard constructor (could be probably a macro)
    Initialize( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
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
