#ifndef CLOCK1PLUGIN_H
#define CLOCK1PLUGIN_H

#include "CartaLib/IPlugin.h"
#include <QObject>
#include <QString>

class Clock1Plugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.cartaviewer.IPlugin")
    Q_INTERFACES( IPlugin)

public:
    Clock1Plugin(QObject *parent = 0);

public:
    virtual bool handleHook(BaseHook &hookData) override;
    virtual std::vector<HookId> getInitialHookList() override;
};



#endif // CLOCK1PLUGIN_H
