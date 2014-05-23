/**
 *
 **/


#include "Globals.h"

Globals * Globals::m_instance = nullptr;

IPlatform *Globals::platform()
{
    Q_ASSERT( instance()-> m_platform != nullptr);

    return instance()-> m_platform;
}

void Globals::setPlatform(IPlatform *platform)
{
    Q_ASSERT_X( instance()-> m_platform == nullptr, "Globals", "redefining platform");

    instance()-> m_platform = platform;
}

PluginManager *Globals::pluginManager()
{
    Q_ASSERT( instance()-> m_pluginManager != nullptr);

    return instance()-> m_pluginManager;
}

void Globals::setPluginManager(PluginManager * pluginManager)
{
    Q_ASSERT_X( instance()-> m_pluginManager == nullptr, "Globals", "redefining platform manager");

    instance()-> m_pluginManager = pluginManager;
}

Globals *Globals::instance()
{
    if( ! m_instance) {
        m_instance = new Globals;
    }
    return m_instance;
}


QString Globals::fname()
{
    return instance()-> m_fname;
}

void Globals::setFname(const QString &fname)
{
    instance()-> m_fname = fname;
}

Globals::Globals()
{
    m_connector = nullptr;
    m_platform = nullptr;
    m_pluginManager = nullptr;
}

IConnector * Globals::connector()
{
    Q_ASSERT( instance()->m_connector != nullptr);

    return instance()-> m_connector;
}

void Globals::setConnector(IConnector *connector)
{
    Q_ASSERT_X( instance()->m_connector == nullptr, "Globals", "redefining connector");

    instance()->m_connector = connector;
}
