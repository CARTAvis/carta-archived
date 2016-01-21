/**
 * Defines a hook for obtaining an initial file list.
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
class GetInitialFileList : public BaseHook
{
    CARTA_HOOK_BOILER1( GetInitialFileList );

public:

    /// result of the hook is a list of filenames
    typedef QStringList ResultType;

    /// input parameters are:
    /// map of all url parameters
    struct Params {
        Params( const std::map< QString, QString> & p_urlParams )
        {
            urlParams = p_urlParams;
        }

        std::map< QString, QString> urlParams;
    };

    /// standard constructor (could be probably a macro)
    GetInitialFileList( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
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
