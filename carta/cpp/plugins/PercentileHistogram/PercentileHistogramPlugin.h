/// Plugin for calculating approximate percentiles.

#pragma once

#include "CartaLib/IPlugin.h"
#include "casacore/images/Images/ImageInfo.h"
#include <QObject>

class PercentileHistogramPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.cartaviewer.IPlugin" )
    Q_INTERFACES( IPlugin );

public:

    /**
     * Constructor.
     */
    PercentileHistogramPlugin( QObject * parent = 0 );
    
    virtual bool handleHook( BaseHook & hookData ) override;
    virtual std::vector < HookId > getInitialHookList() override;
    virtual ~PercentileHistogramPlugin();

    virtual void initialize( const InitInfo & initInfo ) override;

private:
    int m_numberOfBins;

};
