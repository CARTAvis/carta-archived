/// This plugin can read image formats that casacore supports.

#pragma once

#include "common/IPlugin.h"
#include <QObject>
#include <QString>


class CasaImageLoader : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.cartaviewer.IPlugin")
    Q_INTERFACES( IPlugin)

public:

    CasaImageLoader(QObject *parent = 0);
    virtual bool handleHook(BaseHook & hookData) override;
    virtual std::vector<HookId> getInitialHookList() override;

private:

    Image::ImageInterface * loadImage(const QString & fname);
};
