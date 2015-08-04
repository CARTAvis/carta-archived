/// This plugin can read image formats that casacore supports.

#pragma once

#include "CartaLib/IPlugin.h"
#include <QObject>
#include <QString>

class CasaTest1 : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.cartaviewer.IPlugin")
    Q_INTERFACES( IPlugin)

public:

    CasaTest1(QObject *parent = 0);
    virtual bool handleHook(BaseHook & hookData) override;
    virtual std::vector<HookId> getInitialHookList() override;

private:

    bool loadImage( const QString & fname, int channel, QImage & result);
};
