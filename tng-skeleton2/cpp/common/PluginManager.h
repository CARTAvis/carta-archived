/**
 *
 **/

#pragma once

#include "IPlugin.h"

#include <QImage>
#include <QString>
#include <vector>
#include <functional>
#include <utility>
#include <memory>

class IncompleteType;


// helper to convert hooks to hookid's so that we can group them all in one place
// all work is done in specialization
//template <typename Hook>
//constexpr HookId Hook2Int();
//template<> constexpr HookId Hook2Int<Initialize>() { return 1; }

template <typename T>
struct ResTrait {
    typedef std::vector< T> Type;
};

template <>
struct ResTrait <void> {
    typedef FakeVoid Type;
};

class PluginManager;

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
    void forEach( std::function< void(typename T::ResultType)> func) {
        // code reuse by wrapping the supplied function into one that always return true
        auto wrapper = [=] (typename T::ResultType && res) -> bool {
            func( std::forward(res));
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
//    std::vector<typename T::ResultType> collectResults() {
//        std::vector<typename T::ResultType>
//    }


protected:

    friend class PluginManager;

    // only PluginManager can construct this
    // TODO: this should be an inner class
    HookHelper() = delete;
//    HookHelper() {}
    HookHelper( typename T::Params&& params)
        : m_params( std::forward<typename T::Params>( params))
    {}

    typename T::Params m_params;
    PluginManager * m_pm;

};


class PluginManager
{
public:

    /// this is what the plugin manager keeps about each plugin
    struct PluginInfo {

        IPlugin * rawPlugin;
        QString path;

    };

    PluginManager();

    void loadConfig( const QString & /*fileName*/) {}
    void loadPlugins();
    const std::vector< PluginInfo *> & getInfoList() { return m_allPlugins; }

    template <typename Ev, typename ... Args>
    typename ResTrait<typename Ev::ResultType>::Type hookAll( Args ... args) {
//        typename Ev::Params params( args ...);
        typename ResTrait<typename Ev::ResultType>::Type res;
        return res;

    }

    template <typename Ev, typename ... Args>
    HookHelper<Ev> hookAll2( Args&& ... args)
    {
        // construct the parameters for this hook
        typename Ev::Params params( std::forward<Args>(args)...);

        // create a helper
        HookHelper<Ev> helper( std::move(params));
//        HookHelper<Ev> helper;
//        helper.m_params = std::move(params);
        helper.m_pm = this;

//        HookHelper( typename T::Params&& params)
//            : m_params( std::forward<typename T::Params>( params))
//        {}

        // return the helper
        return std::move( helper);
    }

    // keep calling all plugins until one answers
    template <typename Ev, typename ... Args>
    void hookFirst( Args ...);

protected:

    /// return a list of plugins that registered the given hook
//    template <typename Ev>
//    std::vector<PluginInfo *> & listForHook() {
//        int ind = Hook2Int<Ev>();
//        return m_hook2plugin[ ind];
//    }

    std::vector<PluginInfo *> & listForHook( HookId id) {
        return m_hook2plugin[ id];
    }


    void processPlugin( QObject * plugin, QString path = QString() );

private:


    /// list of plugins registered for a each hook
//    std::vector< std::vector< PluginInfo *> > m_hook2plugin;
    std::map< HookId, std::vector< PluginInfo *> > m_hook2plugin;

    /// unique list of all loaded plugins
    std::vector< PluginInfo *> m_allPlugins;

    template<typename T> friend class HookHelper;

};

template <typename T>
void HookHelper<T>::forEachCond( std::function< bool(typename T::ResultType)> func)
{
    qDebug() << "forEachCond";
    // get the list of plugins that claim they handle this hook
//    /*constexpr*/ int hookId = Hook2Int<T>();
    HookId hookId = T::StaticHookId;
    qDebug() << "  static hook id = " << hookId;
    qDebug() << "  m_pm = " << m_pm;
    auto pluginList = m_pm-> listForHook( hookId);
    qDebug() << "  plugins registered for this hook: " << pluginList.size();

    // make an actual instance of the Hook
    T hookData( & m_params);

    for( auto pluginInfo : pluginList) {
        qDebug() << "  info" << pluginInfo;
        qDebug() << "  calling" << pluginInfo-> path;
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
