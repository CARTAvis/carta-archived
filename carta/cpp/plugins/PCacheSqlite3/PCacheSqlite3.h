/// Implements plugin for persistent cache, using QT's sqlite3 database driver

/// This plugin can read image formats that Qt supports.

#pragma once

#include "CartaLib/IPlugin.h"
#include <QObject>
#include <QString>

class PCacheSQlite3Plugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.cartaviewer.IPlugin" )
    Q_INTERFACES( IPlugin )

public :
        PCacheSQlite3Plugin( QObject * parent = 0 );
    virtual bool
    handleHook( BaseHook & hookData ) override;

    virtual std::vector < HookId >
    getInitialHookList() override;

    virtual void
    initialize( const InitInfo & initInfo ) override;

private:

    QString m_dbPath;
};
