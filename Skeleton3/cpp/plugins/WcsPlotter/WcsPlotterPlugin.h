/// This plugin can read image formats that casacore supports.

#pragma once

#include "CartaLib/IPlugin.h"
#include <QObject>
#include <QString>
#include "CartaLib/Hooks/DrawWcsGrid.h"

namespace WcsPlotterPluginNS
{
class AstGridRenderer : public Carta::Lib::IWcsGridRenderer
{
    Q_OBJECT
    CLASS_BOILERPLATE( AstGridRenderer );

public:

    AstGridRenderer();

    virtual void
    setInputImage( Image::ImageInterface::SharedPtr image ) override
    { Q_UNUSED( image ); }

    virtual void
    setOutputSize( const QSize & size ) override
    { Q_UNUSED( size ); }

    virtual void
    setImageRect( const QRectF & rect ) override
    { Q_UNUSED( rect ); }

    virtual void
    setOutputRect( const QRectF & rect ) override
    { Q_UNUSED( rect ); }

    virtual void
    setLineColor( QColor color ) override
    { Q_UNUSED( color ); }

    virtual void
    startRendering() override
    {
        emit done( m_img );
    }

private:

    VGList m_img;
};

class WcsPlotterPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.cartaviewer.IPlugin" )
    Q_INTERFACES( IPlugin )
    ;

public:

    WcsPlotterPlugin( QObject * parent = 0 );

    virtual bool
    handleHook( BaseHook & hookData ) override;

    virtual std::vector < HookId >
    getInitialHookList() override;
};
}
