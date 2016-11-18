/**
 *
 **/

#pragma once

#include "CartaLib/IPlugin.h"
#include "CartaLib/Nullable.h"

#include <QImage>
#include <QString>
#include <vector>
#include <functional>
#include <utility>
#include <memory>

// helper to convert hooks to hookid's so that we can group them all in one place
// all work is done in specialization

template <typename T>
struct ResTrait {
    typedef std::vector< T> Type;
};

template <>
struct ResTrait <void> {
    typedef BaseHook::FakeVoid Type;
};

class PluginManager;

///
/// @todo this should be an inner class of PluginManager
template <typename T>
class HookHelper
{
public:

    /// call all plugins and execute func with the result of the plugin's hook (if
    /// the plugin answered)
    /// the return value of func() will be used
    /// to determine whether to continue the loop (true = continue, false = abort)
    void forEachCond( std::function< bool(typename T::ResultType)> func);

    /// as above, but all plugins are processed
//    void forEach( std::function< void(typename T::ResultType)> func) {
//        // code reuse by wrapping the supplied function into one that always returns true
//        auto wrapper = [=] (typename T::ResultType && res) -> bool {
//            func( std::forward(res));
//            return true;
//        };
//        forEachCond( wrapper);
//    }

    /// for each plugin: call plugin, and then call func() on the result
    void forEach( std::function< void(const typename T::ResultType & )> func) {
        // code reuse by wrapping the supplied function into one that always returns true
        auto wrapper = [=] (const typename T::ResultType & res) -> bool {
            func( res);
            return true;
        };
        forEachCond( wrapper);
    }


    /// execute all plugins and ignore results
    void executeAll() {
        auto wrapper = [=] (typename T::ResultType &&) -> bool {
            return true;
        };
        forEachCond( wrapper);
    }

    /// execute all plugins and return an array of results (for every plugin that answered)
//    std::vector<typename T::ResultType> vector();

    /// keep executing plugins until one answers
    Nullable<typename T::ResultType> first() {
        Nullable<typename T::ResultType> result;
        auto wrapper = [& result] (typename T::ResultType && hookResult) -> bool {
            result = hookResult;
            return false;
        };
        forEachCond( wrapper);
        // return unset value
        return result;
    }

protected:

    friend class PluginManager;

    // only PluginManager can construct this
    HookHelper() = delete;
    HookHelper( typename T::Params&& params)
        : m_params( std::forward<typename T::Params>( params))
    {}

    typename T::Params m_params;
    PluginManager * m_pm;

};

class PluginManager
{
    CLASS_BOILERPLATE( PluginManager);
    Q_DISABLE_COPY(PluginManager)

public:

    /// this is what the plugin manager keeps about each plugin
    struct PluginInfo {
        /// we keep all of plugin.json
        PluginJson json;
        /// pointer to the actual plugin implementation once loaded
        IPlugin * rawPlugin = nullptr;
        /// full path to the .so (shared library)
        QString soPath;
        /// full directory of the plugin from where it was loaded
        QString dirPath;
        /// errors indicating why plugin could not be loaded
        /// this means that if errors is not empty, the plugin could not be
        /// parsed
        QStringList errors;
        /// library paths (for "cpp" and "lib" type plugins)
        QStringList libPaths;
    };

    /// constructor - does not currently do anything interesting at all
    PluginManager();

    /// set the plugin search directories
    void setPluginSearchPaths( const QStringList & pathList);

    /// find and load plugins from the specified directories
    void loadPlugins();

    /// return information about all plugins
    const std::vector<PluginInfo> & getInfoList();

    ///
    /// Prepare the execution of the hook
    ///
    template <typename Hook, typename ... Args>
    HookHelper<Hook> prepare( Args&& ... args)
    {
        // construct the parameters for this hook
        typename Hook::Params params( std::forward<Args>(args)...);

        // create a helper
        HookHelper<Hook> helper( std::move(params));
        helper.m_pm = this;

        // return the helper
        return std::move( helper);
    }

    ~PluginManager() {
        qDebug() << "~PluginManager is getting called";
    }

protected:

    /// return a list of plugins that registered the given hook
    std::vector<PluginInfo *> & listForHook( HookId id) {
        return m_hook2plugin[ id];
    }

    /// find all plugins in the provided search paths and parse their
    /// cooresponding .json files
    std::vector< PluginInfo > findAllPlugins();

    /// parse a plugin directory i.e. the json file
    /// if cpp or lib plugin, see if the plugin
    /// has .so file and/or any libraries
    PluginInfo parsePluginDir( const QString & dirName);

    /// attempt to load a native plugin
    bool loadNativePlugin( PluginInfo & pInfo);

    /// list of plugins registered per hook
    /// \todo we should probably use std::vector for little more performance
    /// but that means we'll need to ensure consecutive numbering of hooks...
    std::map< HookId, std::vector< PluginInfo *> > m_hook2plugin;

    /// list of all discovered plugins
    std::vector< PluginInfo > m_discoveredPlugins;

    template<typename T> friend class HookHelper;

    /// list of plugin search paths
    QStringList m_pluginSearchPaths;

};

/// the workhorse - keep calling each plugin that implementes the hook, followed
/// by calling the supplied callback function, until we run out of plugins
/// or the callback return 'false'
template <typename T>
void HookHelper<T>::forEachCond( std::function< bool(typename T::ResultType)> func)
{
    HookId hookId = T::staticId;

    // get the list of plugins that claim they handle this hook
    auto pluginList = m_pm-> listForHook( hookId);

    // make an actual instance of the Hook on the stack and give it a pointer
    // to the parameters
    T hookData( & m_params);

    for( auto pluginInfo : pluginList) {
        bool handled = pluginInfo-> rawPlugin-> handleHook( hookData);
        // skip to the next plugin immediately if this hook was not handled by
        // this plugin
        if( ! handled) {
            continue;
        }
        // call the func() with the result of the hook
        bool shouldContinue = func( hookData.result);
        // if we should not continue, abort right here
        if( ! shouldContinue) {
            break;
        }
    }
}


//template <typename T>
//std::vector<typename T::ResultType> HookHelper<T>::vector() {
//    std::vector<typename T::ResultType> res;
//    auto wrapper = [& res] (typename T::ResultType && hookResult) -> bool {
//        res.push_back( res);
//        return true;
//    };
//    forEachCond( wrapper);
//    return res;
//}
