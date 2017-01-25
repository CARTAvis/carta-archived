/**
 * Defines a hook for obtaining a persistent cache object.
 *
 **/

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/IPlugin.h"
#include "CartaLib/IPCache.h"

namespace Carta
{
namespace Lib
{
namespace Hooks
{
/// \brief Hook for loading a plugin of an unknown type
///
class GetPersistentCache : public BaseHook
{
    CARTA_HOOK_BOILER1( GetPersistentCache );

public:

    /// Result of the hook is an instance of IPCache object.
    /// This hook should only be ever called once, since the persistent cache
    /// should be treated as a singleton. The reason I am not implementing this
    /// as a singleton is because in the future, we may addd a mechanism (see below)
    /// to be able to create multiple independent caches.
    typedef IPCache::SharedPtr ResultType;

    /// input parameters are:
    /// nothing
    /// All necessary parameters are retrieved from carta.config.
    ///
    /// In the future we could make this contain overrides of some sort, in case
    /// some code would like to create additional independent caches...
    struct Params {
        Params( )
        {
        }
    };

    /// standard constructor (could be probably a macro)
    GetPersistentCache( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
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
