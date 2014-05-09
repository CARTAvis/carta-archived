#ifndef COMMON_IPLUGIN_H
#define COMMON_IPLUGIN_H

#include <QImage>
#include <QObject>
#include <cstdint>
#include <QDebug>

/// we are using 64bit integers for hook IDs
typedef int64_t HookId;

/// base hook event
class BaseHook : public QObject {
    Q_OBJECT
public:

    BaseHook() = delete;
    BaseHook( const HookId & id) : m_hookId(id) {}

//    static HookId staticHookId;
//    virtual HookId dynamicHoodId() const = 0;

    HookId hookId() const
    {
        return m_hookId;
    }

    virtual void debug() {
        qDebug() << "BaseHook::debug()";
    }

protected:

    HookId m_hookId;
};


/// plugin interface
class IPlugin {
public:

    /// this is what we end up calling to do all work
    virtual bool handleHook( BaseHook & hookData) = 0;

    /// at startup plugins will be asked to return a list of hook ids they are
    /// interested in listening to
    virtual std::vector<HookId> getInitialHookList() = 0;

    virtual ~IPlugin() {}

};

//#define IPlugin_iid "org.cartaviewer.IPlugin"
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
    const static HookId StaticHookId = 1;
    Initialize( Params *) : BaseHook( StaticHookId) {}

    ResultType result;

    virtual void debug() override {
        qDebug() << "Initialize::debug()";
    }
};

/// just before rendering a view, plugins are given a chance to modify the rendered image
class PreRender : public BaseHook {
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
    const static HookId StaticHookId = 3;
    PreRender(Params * pptr) : BaseHook( StaticHookId), paramsPtr( pptr) {}
    ResultType result;
    Params * paramsPtr;
};


class GetRandomNumber : public BaseHook  {
    Q_OBJECT
public:
    typedef double ResultType;
    typedef EmptyParams Params;
//    GetRandomNumber() {}
};

class MouseMove : public BaseHook {
    Q_OBJECT
public:
    typedef void ResultType;
    struct Params { Params( int, int ) {} };
//    MouseMove( int, int) {}
};



#endif // COMMON_IPLUGIN_H


