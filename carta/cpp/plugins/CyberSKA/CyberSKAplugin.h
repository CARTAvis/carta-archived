/// This plugin can read image formats that Qt supports.

#pragma once

#include "CartaLib/IPlugin.h"
#include <QObject>
#include <QString>

class CyberSKAplugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.cartaviewer.IPlugin" )
    Q_INTERFACES( IPlugin )

public :

    CyberSKAplugin( QObject * parent = 0 );

    virtual bool
    handleHook( BaseHook & hookData ) override;

    virtual std::vector < HookId >
    getInitialHookList() override;

    virtual void
    initialize( const InitInfo & ) override;

protected:

    QStringList getParamsFromVizMan(QString sessionKey);


    QString m_urlPattern;
    double m_timeout = 5.0;
};
