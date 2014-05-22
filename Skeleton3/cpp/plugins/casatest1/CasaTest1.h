#ifndef CLOCK1PLUGIN_H
#define CLOCK1PLUGIN_H

#include "common/IPlugin.h"
#include <QObject>
#include <QString>

class CasaTest1 : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.cartaviewer.IPlugin")
    Q_INTERFACES( IPlugin)

public:
    CasaTest1(QObject *parent = 0);

public:
    virtual bool handleHook(BaseHook &hookData) override;
    virtual std::vector<HookId> getInitialHookList() override;
};



#endif // CLOCK1PLUGIN_H
