/// This plugin can read image formats that Qt supports.

#pragma once

#include "CartaLib/IPlugin.h"
#include <QObject>
#include <QString>

class HpcImageRenderServicePlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.cartaviewer.IPlugin" )
    Q_INTERFACES( IPlugin );

public:

    HpcImageRenderServicePlugin( QObject * parent = 0 );

    virtual bool
    handleHook( BaseHook & hookData ) override;

    virtual std::vector < HookId >
    getInitialHookList() override;

    virtual void
    initialize( const InitInfo & initInfo ) override;
};
