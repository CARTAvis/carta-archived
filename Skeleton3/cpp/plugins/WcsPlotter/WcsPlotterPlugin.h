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
    setInputImage( Image::ImageInterface::SharedPtr image )
    { }

    virtual void
    setOutputSize( const QSize & size )
    { }

    virtual void
    setImageRect( const QRectF & rect )
    { }

    virtual void
    setOutputRect( const QRectF & rect )
    { }

    virtual void
    setLineColor( QColor color )
    { }

    virtual void
    startRendering()
    {
        emit done( m_img);
    }
private:
    QImage m_img;

};

class WcsPlotterPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.cartaviewer.IPlugin" )
    Q_INTERFACES( IPlugin )

public :
        WcsPlotterPlugin( QObject * parent = 0 );
    virtual bool
    handleHook( BaseHook & hookData ) override;

    virtual std::vector < HookId >
    getInitialHookList() override;

private:
};
}
