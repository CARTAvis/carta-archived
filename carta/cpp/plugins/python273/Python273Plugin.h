/// This plugin can read image formats that casacore supports.

#pragma once

#include "CartaLib/IPlugin.h"
#include <QObject>
#include <QString>


/// this is the main plugin that is exported from the dynamic library
class Python273Plugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.cartaviewer.IPlugin")
    Q_INTERFACES( IPlugin)

public:

    Python273Plugin(QObject *parent = 0);
    virtual bool handleHook(BaseHook & hookData) override;
    virtual std::vector<HookId> getInitialHookList() override;
    virtual void initialize( const InitInfo & InitInfo) override;
};

