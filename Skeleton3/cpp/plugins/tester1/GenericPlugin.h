#ifndef GENERICPLUGIN_H
#define GENERICPLUGIN_H

#include "CartaLib/IPlugin.h"
#include <QObject>
#include <QString>

class GenericPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.cartaviewer.IPlugin" FILE "tester1.json")
    Q_INTERFACES( IPlugin)

public:
    GenericPlugin(QObject *parent = 0);

public:
    virtual bool handleHook(BaseHook &hookData) override;
    virtual std::vector<HookId> getInitialHookList() override;
};



#endif // GENERICPLUGIN_H
