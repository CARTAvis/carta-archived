/// This plugin can read image formats that casacore supports.

#pragma once

#include "CartaLib/IPlugin.h"
#include "CartaLib/PixelPipeline/IPixelPipeline.h"
#include <QObject>
#include <QString>
#include <vector>

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

    /// get the actual list of colormaps
    std::vector < Carta::Lib::PixelPipeline::IColormapNamed::SharedPtr >
    getColormaps();
};
