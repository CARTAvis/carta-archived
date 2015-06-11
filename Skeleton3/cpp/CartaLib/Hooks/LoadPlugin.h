/**
 * Defines a hook for loading plugins in other languages (other than c++).
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
/// This hook is used to implement support for plugins in languages other than C++. For example,
/// the python support plugin implements this, and then plugins written in Python can be loaded
/// by the core.
class LoadPlugin : public BaseHook
{
    CARTA_HOOK_BOILER1( LoadPlugin );

public:

    /// result of the hook is an instance of IPlugin
    typedef IPlugin * ResultType;

    /// parameters are:
    /// plugin directory
    /// parsed plugin.json from the directory
    /// The reason we pass the parsed json here (eventhough we could just read it ourselves) is
    /// because the plugin manager already had to parse it. The plugin is free to re-parse it
    /// again, in case there are some plugin-specific settings in the json...
    struct Params {
        Params( const QString & p_pluginDir, const PluginJson & p_json )
        {
            pluginDir = p_pluginDir;
            json = p_json;
        }

        QString pluginDir;
        PluginJson json;
    };

    /// standard constructor (could be probably a macro)
    LoadPlugin( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr )
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
