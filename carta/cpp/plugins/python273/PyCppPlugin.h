/// this is a plugin returned as a result of the hook call when loading a pythong plugin

#pragma once

#include "CartaLib/IPlugin.h"
#include "CartaLib/Hooks/LoadPlugin.h"

///
/// this is a plugin returned as a result of the hook call when loading a python plugin
///
/// \warning do not add Q_PLUGIN_METADATA to this class, because we are not exporting
/// this one to the qpluginloader
///
class PyCppPlug : public QObject, public IPlugin
{
    Q_OBJECT
    Q_INTERFACES( IPlugin)

public:

    PyCppPlug( const Carta::Lib::Hooks::LoadPlugin::Params & params);
    virtual bool handleHook(BaseHook & hookData) override;
    virtual std::vector<HookId> getInitialHookList() override;
    virtual void initialize( const InitInfo & InitInfo) override;

    Carta::Lib::Hooks::LoadPlugin::Params m_params;
    int m_pyModId = -1;
};
