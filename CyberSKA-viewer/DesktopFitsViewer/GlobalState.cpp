/**
 * This is the license for the CyberSKA image viewer.
 *
 **/

#include "GlobalState.h"

#include "FitsViewerLib/common.h"
#include "PureWeb.h"
#include <map>
#include <tuple>
#include <functional>

namespace GlobalState {

RawVariable::RawVariable(const QString & pathPrefix, const QString & pathPostfix)
{
    m_currId = 0;

    m_prefix = pathPrefix;
    m_postfix = pathPostfix;
    m_internalCbRegistered = false;
    m_insideInternalCb = false;

    QString p1 = pathPrefix;
    QString p2 = pathPostfix;
    if( ! p1.startsWith( '/')) p1.prepend('/');
    if( ! p1.endsWith('/')) p1.append('/');
    m_fullPath = p1 + p2;
    // discard trailing slash
    if( m_fullPath.length() > 1 && m_fullPath.endsWith( '/')) {
        m_fullPath = m_fullPath.left( m_fullPath.length() - 1);
    }

    // sync the cache
    get(true);
//    dbg(1) << "gv: " << m_fullPath << " initially= " << get();

    // register us with pureweb
//    GetStateManager().XmlStateManager().AddValueChangedHandler(
//                m_fullPath.toStdString(),
//                CSI::Bind(this, &RawVariable::internalCb));
}

RawVariable::~RawVariable()
{
    dbg(1) << "Destroying GlobalVar " << m_fullPath;
    if( m_insideInternalCb) {
        dbg(-1) << ConsoleColors::warning()
                << "Destroying global variable " << m_fullPath
                << " while in it's callback!!!!"
                << ConsoleColors::resetln();
    }
    // deregister with pureweb
    if( m_internalCbRegistered) {
        GetStateManager().XmlStateManager().RemoveValueChangedHandler(
                    m_fullPath.toStdString(),
                    CSI::Bind(this, &RawVariable::internalCb));
        m_internalCbRegistered = false;
    }
}

RawVariable::CallbackID RawVariable::addCallback(CallbackType fn)
{
    // register us with pureweb now that we have a callback
    if( ! m_internalCbRegistered) {
        m_internalCbRegistered = true;
        GetStateManager().XmlStateManager().AddValueChangedHandler(
                    m_fullPath.toStdString(),
                    CSI::Bind(this, &RawVariable::internalCb));
    }
    auto thisId = m_currId;
    m_currId ++;

    m_cbmap[thisId] = fn;

    return thisId;
}

void RawVariable::removeCallback(const RawVariable::CallbackID &cbId)
{
    auto ind = m_cbmap.find( cbId);
    if( ind == m_cbmap.end()) {
        dbg(1) << "callback not found: " << cbId;
        LTHROW( "callback not found");
    }
    m_cbmap.erase( ind);
}

void RawVariable::set(const QString &val)
{
    GetStateManager().XmlStateManager()
            .SetValueAs( m_fullPath.toStdString(), val.toStdString());
}

const QString &RawVariable::get(bool sync)
{
    if( sync) {
        std::string stdstr = GetStateManager().XmlStateManager()
                .GetValue( m_fullPath.toStdString()).ConvertOr<std::string>("");
        m_cachedValue = stdstr.c_str();
    }
    return m_cachedValue;
}

const QString & RawVariable::prefix() const
{
    return m_prefix;
}

const QString & RawVariable::postfix() const
{
    return m_postfix;
}

void RawVariable::internalCb(const CSI::ValueChangedEventArgs &val)
{
    m_insideInternalCb = true;

    // store the value
    std::string stdstr = val.NewValue().ConvertOr<std::string>("");
    m_cachedValue = stdstr.c_str();
    dbg(1) << "cached(" << m_fullPath << ")=" << m_cachedValue;

    // TODO: is this necessary? the reasoning is that a callback might be removed
    // while we are iterating through callbacks...
    auto cbcopy = m_cbmap;
    // call all our callbacks
    for( auto c : cbcopy) {
        dbg(1) << "Calling cb " << c.first;
//        CallbackParams parms;
//        parms.postfix = m_postfix;
//        parms.prefix = m_prefix;
//        parms.val = val.NewValue().ConvertOr<std::string>("").c_str();

        c.second( * this);
        dbg(1) << "Calling cb done" << c.first;
    }

    m_insideInternalCb = false;
}

struct V3d { double x, y, z; };
template <>
struct Converter<V3d> {
    static QString convertValToString( const V3d & val) {
        return QString("%1 %2 %3").arg(val.x).arg(val.y).arg(val.z);
    }
    static V3d convertStringToVal( const QString & s) {
        QString copy = s;
        QTextStream inp( & copy);
        V3d res;
        inp >> res.x >> res.y >> res.z;
        return res;
    }
};

// ------------------------------------------------------------------------
// commands
// ------------------------------------------------------------------------

// private stuff to handle commands
struct CommandStuff {
    static bool genericRegistered;
    static int64_t callbackIdCounter;
    struct CallbackData {
        Command::CallbackSignature cb;
        QString alias;
        Command::CallbackId id;
    };

//    static std::multimap< QString, std::tuple< Command::CallbackSignature, Command::CallbackId > > cbMap;
    static std::multimap< QString, CallbackData > cbMap;
    static void registerGenericListenerOnce() {
        if( genericRegistered) return;
        genericRegistered = true;
        GetStateManager().CommandManager().AddUiHandler(
                    "generic", & CommandStuff::genericListenerCB);
    }
    static void genericListenerCB (CSI::Guid /*sessionid*/, CSI::Typeless command, CSI::Typeless responses)
    {
//        std::string cmd = command["cmd"].As<std::string>();
        QString cmd = command["cmd"].As<QString>();
        QString value = command["value"].As<QString>();

        dbg(1) << "Generic command: " << cmd << " " << value << "\n";

        // invoke all callbacks
        Command::CallbackParameter param;
        param.value = value;
        auto range = cbMap.equal_range( cmd);
        bool foundAtLeastOne = false;
        QStringList results;
        for( auto i = range.first ; i != range.second ; i ++) {
            CallbackData & d = i-> second;
//            Command::CallbackSignature cb = std::get<0>(i-> second);
//            Command::CallbackId id = std::get<1>(i-> second);
            param.name = d.alias;
            results += d.cb( param);
            foundAtLeastOne = true;
        }
        if( ! foundAtLeastOne) {
            dbg(-1) << ConsoleColors::warning()
                    << "Generic command: " << cmd << " has no callback registered."
                    << ConsoleColors::resetln();
        }
        responses["result"] = results.join("|").toStdString();
    }

};

bool CommandStuff::genericRegistered = false;
int64_t CommandStuff::callbackIdCounter = 0;
//std::multimap< QString, std::tuple< Command::CallbackSignature, Command::CallbackId > > CommandStuff::cbMap;
std::multimap< QString, CommandStuff::CallbackData > CommandStuff::cbMap;

Command::CallbackId Command::addCallback(const QString &cmd, Command::CallbackSignature cb)
{
    // delegate to the version with alias, setting the alias to cmd
    return addCallback( cmd, cmd, cb);
}

Command::CallbackId Command::addCallback(const QString &cmd, const QString & alias, Command::CallbackSignature cb)
{
    // make sure generic listener is registered with PureWeb
    CommandStuff::registerGenericListenerOnce();

    // add this callback
    CallbackId id = CommandStuff::callbackIdCounter ++;
    CommandStuff::CallbackData data;
    data.alias = alias;
    data.cb = cb;
    data.id = id;

//    CommandStuff::cbMap.insert( std::make_pair( cmd, std::make_tuple(cb, id)));
    CommandStuff::cbMap.insert( std::make_pair( cmd, data ));

    return id;

}


#ifdef DONT_COMPILE

static int test() {
    return 0;
    TypedVariable<int> dotSize( "/x/y/dotSize");
    dotSize.set( 7);
    int x = dotSize.get();
    dbg(1) << x;
    dotSize.addCallback( [](int x) { dbg(1) << "cb: dotSize = " << x; });

    TypedVariable<double> transparency( "/x/y/transp");
    transparency.set( 1.2);
    x = transparency.get();


    V3d v1 { 1.0, 2.0, 3.0 };
    TypedVariable<V3d> upVector( "/upv");
    upVector.addCallback( [](const V3d & v) {
        dbg(1) << "cb: upVector = " << v.x << "," << v.y << "," << v.z;
    });
    upVector.set( v1);
    V3d v2 = upVector.get();
    v2.x = v2.y;

    // setup ranged double (with min/max)
//    TGlobalVar<double,RangedDouble> rdouble( "/a/b/c", RangedDouble(-1.1, 2.3));

    return 7;
}

static int foo = test();
#endif

}
