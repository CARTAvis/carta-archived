/// This plugin can read image formats that casacore supports.

#pragma once

#include "common/IPlugin.h"
#include <QObject>
#include <QString>
#include <vector>
#include "CartaLib/IColormapScalar.h"

class Colormap1 : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.cartaviewer.IPlugin" )
    Q_INTERFACES( IPlugin )
    ;

public:

    Colormap1( QObject * parent = 0 );
    virtual bool
    handleHook( BaseHook & hookData ) override;

    virtual std::vector < HookId >
    getInitialHookList() override;

protected:

    /// get the actual list
    std::vector < Carta::Lib::IColormapScalar::SharedPtr >
    getColormaps();
};
