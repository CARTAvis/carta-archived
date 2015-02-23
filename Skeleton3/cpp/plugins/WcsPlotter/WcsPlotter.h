/// This plugin can read image formats that casacore supports.

#pragma once

#include "CartaLib/IPlugin.h"
#include <QObject>
#include <QString>

namespace wcsplotter
{
class WcsPlotter : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.cartaviewer.IPlugin" )
    Q_INTERFACES( IPlugin )

public :
        WcsPlotter( QObject * parent = 0 );
    virtual bool
    handleHook( BaseHook & hookData ) override;

    virtual std::vector < HookId >
    getInitialHookList() override;

private:
};
}
