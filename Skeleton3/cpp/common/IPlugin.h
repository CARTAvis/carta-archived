#pragma once

#include <QImage>
#include <QObject>
#include <cstdint>
#include <QDebug>

/// Every hook as a unique ID and we are using 64bit integers for hook IDs
/// The IDs will allow us to do static_cast<> downcasting inside plugins.
typedef int64_t HookId;

/// base hook event
/// Currently the purpose of this base class is to implement IDs
/// TODO: does this need inheritance from QObject
class BaseHook : public QObject
{

    Q_OBJECT

public:

    BaseHook() = delete;

    /// only one constructor, requires a hook ID
    explicit BaseHook( const HookId & id) : m_hookId(id) {}

    /// getter
    HookId hookId() const
    {
        return m_hookId;
    }

    virtual void debug() {
        qDebug() << "BaseHook::debug()";
    }

protected:

    /// the actual hook ID
    HookId m_hookId;
};


/// plugin interface
/// Every plugin must implement this interface
class IPlugin
{

public:

    /// this is what we end up calling to do all work
    virtual bool handleHook( BaseHook & hookData) = 0;

    /// at startup plugins will be asked to return a list of hook ids they are
    /// interested in listening to
    virtual std::vector<HookId> getInitialHookList() = 0;

    virtual ~IPlugin() {}

};

// this is needed to setup the Qt metatype system to enable qobject_cast<> downcasting
Q_DECLARE_INTERFACE(IPlugin, "org.cartaviewer.IPlugin")


/// TODO: I have not yet figured out how to specialize some of the templates for
/// void return type... hence the FakeVoid aliased to char
typedef char FakeVoid;

struct EmptyParams { EmptyParams() {} };

/// initialize hook is called once at the beginning of the application
class Initialize : public BaseHook {
    Q_OBJECT
public:
    typedef FakeVoid ResultType;
    typedef EmptyParams Params;
//    constexpr static HookId StaticHookId = 1;
    enum { StaticHookId = 1 };
    Initialize( Params *) : BaseHook( Initialize::StaticHookId) {}

    ResultType result;

    virtual void debug() override {
        qDebug() << "Initialize::debug()";
    }
};

/// just before rendering a view, plugins are given a chance to modify the rendered image
class PreRender : public BaseHook
{
    Q_OBJECT

public:

    typedef FakeVoid ResultType;
    struct Params {
        Params( QString p_viewName, QImage * p_imgPtr) {
            imgPtr = p_imgPtr;
            viewName = p_viewName;
        }
        QImage * imgPtr;
        QString viewName;
    };
    enum { StaticHookId = 3 };
    PreRender(Params * pptr) : BaseHook( PreRender::StaticHookId), paramsPtr( pptr) {}
    ResultType result;
    Params * paramsPtr;
};

/// load image and convert it to QImage
class LoadImage : public BaseHook
{
    Q_OBJECT

public:

    typedef QImage ResultType;
    struct Params {
        Params( QString p_fileName) {
            fileName = p_fileName;
        }
        QString fileName;
    };
    enum { StaticHookId = 5 };
    LoadImage(Params * pptr) : BaseHook( StaticHookId), paramsPtr( pptr) {}
    ResultType result;
    Params * paramsPtr;
};


