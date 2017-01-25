/// @todo move everything here Carta::Lib namespace

#pragma once

#include "CartaLib/Hooks/HookIDs.h"
#include "IImage.h"
#include <QImage>
#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <cstdint>

/// Every hook as a unique ID and we are using 64bit integers for hook IDs
/// The IDs will allow us to do static_cast<> downcasting inside plugins.
typedef int64_t HookId;

/// base hook event
/// Currently the purpose of this base class is to implement IDs
/// TODO: does this need inheritance from QObject
class BaseHook // : public QObject
{
//    Q_OBJECT

public:

    /// TODO: I have not yet figured out how to specialize some of the templates for
    /// void return type... hence the FakeVoid aliased to char
    typedef char FakeVoid;

    BaseHook() = delete;

    /// only one constructor, requires a hook ID
    explicit
    BaseHook( const HookId & id ) : m_hookId( id ) { }

    /// get the dynamic hook ID
    HookId
    hookId() const
    {
        return m_hookId;
    }

    /// a convenience (and preferred way) to compare whether a given hook instance is
    /// of a particular hook type. By using this we can hide the implementation details...
    template < typename HookType >
    static bool
    isHook( const BaseHook & hook )
    {
        return hook.hookId() == HookType::staticId;
    }

    /// returns treu if this hook is an instace of HookType
    template < typename HookType >
    bool
    is() const
    {
        return hookId() == HookType::staticId;
    }

protected:

    /// dynamic hook ID
    HookId m_hookId;
};

/// parsed plugin.json information
struct PluginJson {
    /// API version against which
    QString api;

    /// name of the plugin
    QString name;

    /// version of the plugin
    QString version;

    /// type of the plugin (e.g. "cpp")
    QString typeString;

    /// description of the plugin
    QString description;

    /// about the plugin
    QString about;

    /// list of depenencies of the plugin
    QStringList depends;
};

/// plugin interface
/// Every plugin must implement this interface
class IPlugin
{
public:

    /// information passed to plugins during initialize()
    struct InitInfo {
        /// full path to the directory from where the plugin was loaded
        QString pluginPath;

        /// parsed json
        QJsonObject json;
    };

    /// called immediately after the plugin was loaded
    /// TODO: should be pure virtual, don't be lazy!
    ///
    /// This is different from the Initialize hook, which is delivered after
    /// all plugins have been loaded and after core is started.
    virtual void
    initialize( const InitInfo & initInfo )
    {
        Q_UNUSED( initInfo );
    }

    /// at startup plugins will be asked to return a list of hook ids they are
    /// interested in listening to
    virtual std::vector < HookId >
    getInitialHookList() = 0;

    /// this is what we end up calling to do all work
    virtual bool
    handleHook( BaseHook & hookData ) = 0;

    /// virtual empty destructor
    virtual
    ~IPlugin() { }
};

#define CARTA_HOOK_BOILER1( name ) \
    CLASS_BOILERPLATE( name ); \
    enum { staticId = static_cast < HookId > ( Carta::Lib::Hooks::UniqueHookIDs::name ## _ID ) }

/// just before rendering a view, plugins are given a chance to modify the rendered image
/// \todo remove this, it was only a proof of concept hook
class PreRender : public BaseHook
{
    CARTA_HOOK_BOILER1( PreRender );

public:

    typedef FakeVoid ResultType;
    struct Params {
        Params( QString p_viewName, QImage * p_imgPtr )
        {
            imgPtr = p_imgPtr;
            viewName = p_viewName;
        }

        QImage * imgPtr;
        QString viewName;
    };
    PreRender( Params * pptr ) : BaseHook( staticId ), paramsPtr( pptr ) { }

    ResultType result;
    Params * paramsPtr;
};

// This is needed to setup the Qt metatype system to enable qobject_cast<> downcasting.
// It must be outside of any namespace!!!
Q_DECLARE_INTERFACE( IPlugin, "org.cartaviewer.IPlugin" )
