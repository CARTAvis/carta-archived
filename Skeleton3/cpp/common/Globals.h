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
    QString m_fname;

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

    static QString fname();
    static void setFname(const QString & fname);

};

