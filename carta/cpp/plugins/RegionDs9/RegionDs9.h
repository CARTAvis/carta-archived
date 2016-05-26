/**
 * This plugin can read ds9 region formats.
 */
#pragma once

#include "CartaLib/IPlugin.h"
#include <QObject>

namespace Carta {
    namespace Lib {
        class RegionInfo;
    }
}

class RegionDs9 : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.cartaviewer.IPlugin")
    Q_INTERFACES( IPlugin)

public:

    /**
     * Constructor.
     */
    RegionDs9(QObject *parent = 0);
    virtual bool handleHook(BaseHook & hookData) override;
    virtual std::vector<HookId> getInitialHookList() override;
    virtual ~RegionDs9();
};
