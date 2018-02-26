/// Plugin for calculating approximate percentiles.

#pragma once

#include "CartaLib/IPlugin.h"
#include "casacore/images/Images/ImageInfo.h"
#include <QObject>

class PercentileManku99Plugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.cartaviewer.IPlugin" )
    Q_INTERFACES( IPlugin );

public:

    /**
     * Constructor.
     */
    PercentileManku99Plugin( QObject * parent = 0 );
    
    virtual bool handleHook( BaseHook & hookData ) override;
    virtual std::vector < HookId > getInitialHookList() override;
    virtual ~PercentileManku99Plugin();

    virtual void initialize( const InitInfo & initInfo ) override;

private:
    int m_numBuffers;
    int m_bufferCapacity;
    int m_sampleAfter;
};
