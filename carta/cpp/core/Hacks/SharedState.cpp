#include "SharedState.h"
#include "core/Globals.h"
#include <QObject>
#include <map>
#include <tuple>
#include <functional>

namespace Carta
{
namespace Lib
{
namespace SharedState
{
RawVar::RawVar( const QString & path )
    : QObject( nullptr )
{
    // discard trailing slash from path
    m_fullPath = path;
    if ( m_fullPath.endsWith( '/' ) ) {
        m_fullPath = m_fullPath.left( m_fullPath.length() - 1 );
    }

    // register a callback with connector
    m_connectorCBId =
        Globals::instance()->connector()->addStateCallback(
            m_fullPath,
            std::bind( & Me::internalCb, this, std::placeholders::_1, std::placeholders::_2 ) );

    // sync the cache
    (void) get( true );
}

RawVar::~RawVar()
{
    // deregister with connector
    Globals::instance()-> connector()-> removeStateCallback( m_connectorCBId );
}

std::map<QString,RawVar::SharedPtr> RawVar::m_varCache;

//namespace anon {
//// promote protected constructor to public?
//class helper : public RawVar {
//public:
//    template<typename... Args>
//        helper(Args&&... args): RawVar(std::forward<Args>(args)...) {}
//};
//}

RawVar::SharedPtr
RawVar::make( const FullPath & fullPath )
{
    CARTA_ASSERT( fullPath.isValid());
    QString path = fullPath.path();
    auto it = m_varCache.find( path );
    if ( it != m_varCache.end() ) {
        return it-> second;
    }
    // I would like:
    // SharedPtr ptr = std::make_shared < RawVar > ( path );
    // while keeping constructor private, which needs a small hack, so let's do it
    // slightly less efficiently for the sake of readability

    SharedPtr ptr;
    ptr.reset( new RawVar( path ));
    m_varCache[path] = ptr;
    return ptr;

    m_varCache[path] = ptr;
    return ptr;
}

void
RawVar::set( const QString & val )
{
    Globals::instance()->connector()-> setState( m_fullPath, val );
}

const QString &
RawVar::get( bool sync )
{
    if ( sync ) {
        m_cachedValue = Globals::instance()-> connector()-> getState( m_fullPath );
    }
    return m_cachedValue;
}

const QString &
RawVar::path() const
{
    return m_fullPath;
}

void
RawVar::internalCb( const QString &, const QString & newValue )
{
    // store the value
    m_cachedValue = newValue;

    emit rawVarChanged( m_cachedValue);
    emit rawVarChangedVoid();

}

struct V3d {
    double x, y, z;
};
template < >
struct ConverterHelper < V3d > {
    static QString
    convertValToString( const V3d & val )
    {
        return QString( "%1 %2 %3" ).arg( val.x ).arg( val.y ).arg( val.z );
    }

    static V3d
    convertStringToVal( const QString & s )
    {
        QString copy = s;
        QTextStream inp( & copy );
        V3d res;
        inp >> res.x >> res.y >> res.z;
        return res;
    }
};

#ifdef DONT_COMPILE

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
    static std::multimap < QString, CallbackData > cbMap;
    static void
    registerGenericListenerOnce()
    {
        if ( genericRegistered ) { return; }
        genericRegistered = true;
        GetStateManager().CommandManager().AddUiHandler(
            "generic", & CommandStuff::genericListenerCB );
    }

    static void
    genericListenerCB( CSI::Guid /*sessionid*/, CSI::Typeless command, CSI::Typeless responses )
    {
//        std::string cmd = command["cmd"].As<std::string>();
        QString cmd = command["cmd"].As < QString > ();
        QString value = command["value"].As < QString > ();

        dbg( 1 ) << "Generic command: " << cmd << " " << value << "\n";

        // invoke all callbacks
        Command::CallbackParameter param;
        param.value = value;
        auto range = cbMap.equal_range( cmd );
        bool foundAtLeastOne = false;
        QStringList results;
        for ( auto i = range.first ; i != range.second ; i++ ) {
            CallbackData & d = i-> second;

//            Command::CallbackSignature cb = std::get<0>(i-> second);
//            Command::CallbackId id = std::get<1>(i-> second);
            param.name = d.alias;
            results += d.cb( param );
            foundAtLeastOne = true;
        }
        if ( ! foundAtLeastOne ) {
            dbg( - 1 ) << ConsoleColors::warning()
                       << "Generic command: " << cmd << " has no callback registered."
                       << ConsoleColors::resetln();
        }
        responses["result"] = results.join( "|" ).toStdString();
    } // genericListenerCB
};

bool CommandStuff::genericRegistered = false;
int64_t CommandStuff::callbackIdCounter = 0;

//std::multimap< QString, std::tuple< Command::CallbackSignature, Command::CallbackId > > CommandStuff::cbMap;
std::multimap < QString, CommandStuff::CallbackData > CommandStuff::cbMap;

Command::CallbackId
Command::addCallback( const QString & cmd, Command::CallbackSignature cb )
{
    // delegate to the version with alias, setting the alias to cmd
    return addCallback( cmd, cmd, cb );
}

Command::CallbackId
Command::addCallback( const QString & cmd, const QString & alias, Command::CallbackSignature cb )
{
    // make sure generic listener is registered with PureWeb
    CommandStuff::registerGenericListenerOnce();

    // add this callback
    CallbackId id = CommandStuff::callbackIdCounter++;
    CommandStuff::CallbackData data;
    data.alias = alias;
    data.cb = cb;
    data.id = id;

//    CommandStuff::cbMap.insert( std::make_pair( cmd, std::make_tuple(cb, id)));
    CommandStuff::cbMap.insert( std::make_pair( cmd, data ) );

    return id;
}


static int
test()
{
    return 0;

    TypedVariable < int > dotSize( "/x/y/dotSize" );
    dotSize.set( 7 );
    int x = dotSize.get();
    dbg( 1 ) << x;
    dotSize.addCallback([] (int x) { dbg( 1 ) << "cb: dotSize = " << x;
                        }
                        );

    TypedVariable < double > transparency( "/x/y/transp" );
    transparency.set( 1.2 );
    x = transparency.get();

    V3d v1 {
        1.0, 2.0, 3.0
    };
    TypedVariable < V3d > upVector( "/upv" );
    upVector.addCallback([] ( const V3d & v ) {
                             dbg( 1 ) << "cb: upVector = " << v.x << "," << v.y << "," << v.z;
                         }
                         );
    upVector.set( v1 );
    V3d v2 = upVector.get();
    v2.x = v2.y;

    // setup ranged double (with min/max)
//    TGlobalVar<double,RangedDouble> rdouble( "/a/b/c", RangedDouble(-1.1, 2.3));

    return 7;
} // test

static int foo = test();
#endif // ifdef DONT_COMPILE
}
}
}
