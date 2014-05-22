/**
 * Global variables groupped together.
 **/


#pragma once

#include "IConnector.h"
#include "IPlatform.h"
#include "PluginManager.h"

class Globals {
    PluginManager * m_pluginManager;
    IPlatform * m_platform;
    IConnector * m_connector;

    static Globals * m_instance;

    // private constructor
    Globals();

    /// singleton pattern
    static Globals * instance();


public:

    /// get the connector
    static IConnector * connector();

    /// set the connector
    static void setConnector(IConnector *connector);

    /// get the platform
    static IPlatform * platform();

    /// set the platform
    static void setPlatform(IPlatform * platform);

    /// get the plugin manager
    static PluginManager * pluginManager();

    /// set the plugin manager
    static void setPluginManager(PluginManager * pluginManager);

};

//class Dummy {
//    const static int XXX = 7;
//    void foo(int x) { x = x/2; }
//    Dummy()  { foo( XXX); }
//};

///// just before rendering a view, plugins are given a chance to modify the rendered image
//class PreRender2 : public BaseHook
//{
//    Q_OBJECT

//public:

//    typedef FakeVoid ResultType;
//    struct Params {
//        Params( QString p_viewName, QImage * p_imgPtr) {
//            imgPtr = p_imgPtr;
//            viewName = p_viewName;
//        }
//        QImage * imgPtr;
//        QString viewName;
//    };
//    const static HookId StaticHookId /*= 3*/;
//    PreRender2(Params * pptr) : BaseHook( PreRender2::StaticHookId), paramsPtr( pptr) {}
//    ResultType result;
//    Params * paramsPtr;
//};

//static void foobar() {
//    auto pr2 = new PreRender2(nullptr);
//}

