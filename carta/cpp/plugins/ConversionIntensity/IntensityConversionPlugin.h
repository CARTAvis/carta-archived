/// Plugin for converting spectral units.

#pragma once

#include "CartaLib/IPlugin.h"
#include "casacore/images/Images/ImageInfo.h"
#include <QObject>

class IntensityConversionPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.cartaviewer.IPlugin" )
    Q_INTERFACES( IPlugin );

public:

    /**
     * Constructor.
     */
    IntensityConversionPlugin( QObject * parent = 0 );
    
    virtual bool handleHook( BaseHook & hookData ) override;
    virtual std::vector < HookId > getInitialHookList() override;
    virtual ~IntensityConversionPlugin();

private:

};
