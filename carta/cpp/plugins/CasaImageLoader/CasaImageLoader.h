/// This plugin can read image formats that casacore supports.

#pragma once

#include "CartaLib/IPlugin.h"
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
    virtual ~CasaImageLoader();



//    void forgot_to_define_this();

private:

    Carta::Lib::Image::ImageInterface::SharedPtr loadImage(const QString & fname);
};
