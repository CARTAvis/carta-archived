/**
 * Global variables groupped together.
 *
 * Globals are bad. Hiding them in singletons is probably even worse. So instead of that,
 * they are groupped here together in a single singleton.
 *
 * I add globals here when I have a mental block and cannot figure out a good
 * place for them elsewhere, and I hate it. I hope to eventually remove all of them...
 *
 **/


#pragma once

#include "PluginManager.h"

class IConnector;
class IPlatform;
namespace CmdLine { class ParsedInfo; }
namespace MainConfig { class ParsedInfo; }

class Globals {

    friend class StateInterface;
    friend class ImageView;
    friend class ObjectManager;
    friend class CartaObject;

public:

    /// singleton pattern
    static Globals * instance();

    /// get the connector
    IConnector * connector();

    /// set the connector
    void setConnector(IConnector *connector);

    /// get the platform
    IPlatform * platform();

    /// set the platform
    void setPlatform(IPlatform * platform);

    /// get the plugin manager
    PluginManager::SharedPtr pluginManager();

    /// set the plugin manager
    void setPluginManager(PluginManager::SharedPtr pluginManager);

    const CmdLine::ParsedInfo * cmdLineInfo() const;
    void setCmdLineInfo(const CmdLine::ParsedInfo * cmdLineInfo);

    const MainConfig::ParsedInfo * mainConfig() const;
    void setMainConfig(const MainConfig::ParsedInfo * mainConfig);

protected:

//    PluginManager * m_pluginManager = nullptr;
    PluginManager::SharedPtr m_pluginManager = nullptr;
    IPlatform * m_platform = nullptr;
    IConnector * m_connector = nullptr;
    const CmdLine::ParsedInfo * m_cmdLineInfo = nullptr;
    const MainConfig::ParsedInfo * m_mainConfig = nullptr;

    static Globals * m_instance;

    // private constructor
    Globals();
};

