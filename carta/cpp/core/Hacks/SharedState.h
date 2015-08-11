/***
 *
 * an experiment in making pureweb state setting/getting easier to use, using templates
 *
 * for example, making it easy to set/read doubles, or vectors of ints, etc...
 *
 */

#pragma once

#include "CartaLib/CartaLib.h"
#include "core/IConnector.h"
#include <cstdint>
#include <QString>
#include <functional>
#include <vector>
#include <map>
#include <cmath>
#include <limits>
#include <memory>
#include <QStringList>
#include <QObject>
#include <QPen>

namespace Carta
{
namespace Lib
{
namespace SharedState
{
class PathSegment
{
    CLASS_BOILERPLATE( PathSegment );

public:

    PathSegment( const QString & part )
    {
        CARTA_ASSERT( isValidLocalXmlName( part ) );
        m_part = part;
    }

    static bool
    isValidLocalXmlName( const QString & s ) { return ! s.isEmpty(); }

    const QString &
    name() const { return m_part; }

private:

    QString m_part;
};

class FullPath
{
    CLASS_BOILERPLATE( FullPath );

public:

    FullPath() { }

    Me &
    add( const PathSegment & part )
    {
        m_path.append( '/' );
        m_path.append( part.name() );
        return * this;
    }

    FullPath with( const QString & part) const {
        FullPath copy = * this;
        copy.add( PathSegment(part));
        return copy;
    }

    const QString &
    path() const { return m_path; }

    bool
    isValid() const
    {
        return ! m_path.isEmpty();
    }

    static FullPath
    fromQString( const QString & s )
    {
        auto parts = s.split( '/', QString::SkipEmptyParts );
        FullPath path;
        for ( auto & part : parts ) {
            path.add( PathSegment( part ) );
        }
        return path;
    }

private:

    QString m_path;
};

//static void
//testAPI()
//{
//    FullPath path;
//    path.add( PathSegment("root") );
//    CARTA_ASSERT( path.path() == "/root");
//}

typedef std::vector < int > VI;
typedef std::vector < double > VD;

//class RawVar;
//class RawVarManager {
//public:
//    static RawVar::SharedPtr get( QString path);
//};

/// this is the lowest level class - it holds an unmodified QString representation of the state
class RawVar : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE( RawVar );

public:

    virtual
    ~RawVar();

    /// singleton like constructor of vars, so that they can be re-used
    /// you will have to use this instead of a constructor
    static RawVar::SharedPtr
    make( const FullPath & path );

    /// Access the value
    /// \param sync if true, the value will be first read in from the connector, and
    /// the current value will be returned. if false, the old cached value will be returned
    /// \return the value associated with the state
    ///
    const QString &
    get( bool sync = false );

    const QString &
    path() const;

signals:

    /// emitted when the value changes
    /// \param newValue the new value of the variable
    void
    rawVarChanged( QString newValue );

    /// emitted when the value changes
    void
    rawVarChangedVoid();

public slots:

    /// set the value
    void
    set( const QString & val );

private:

    explicit
    RawVar( const QString & path );

    /// singleton like map of vars already created so we can re-use them
    static std::map < QString, RawVar::SharedPtr > m_varCache;

    // path to the state, with trailing slash removed
    QString m_fullPath;

    // cached value
    QString m_cachedValue;

    // internal callback for the connector
    void
    internalCb( const QString & path, const QString & newValue );

    // connector callback id, so that we can remove it when destroyed
    IConnector::CallbackID m_connectorCBId;
};

struct ClampedDouble {
    ClampedDouble( double min, double max )
    {
        m_min = min;
        m_max = max;
    }

    double m_min, m_max;
    QString
    convertValToString( double val )
    {
        val = Carta::Lib::clamp( val, m_min, m_max );
        return QString::number( val );
    }

    double
    convertStringToVal( const QString & s )
    {
        QVariant variant( s );
        double val = s.toDouble();
        val = Carta::Lib::clamp( val, m_min, m_max );
        return val;
    }
};

// converter helper
// needs to be specialized for new types
template < class T >
struct ConverterHelper {
    static QString
    convertValToString( const T & val );

    static T
    convertStringToVal( const QString & s );
};

template < >
struct ConverterHelper < QString > {
    static QString
    convertValToString( const QString & s )
    {
        return s;
    }

    static QString
    convertStringToVal( const QString & s )
    {
        return s;
    }
};

template < >
struct ConverterHelper < int > {
    static QString
    convertValToString( int val )
    {
        return QString::number( val );
    }

    static int
    convertStringToVal( const QString & s )
    {
        bool ok;
        int result = s.toInt( & ok );
        if ( ! ok ) {
            qWarning() << "string " << s << " should be int";
            result = 0;
        }
        return result;
    }
};

// generic std::vector
template < class T >
struct ConverterHelper < std::vector < T > > {
    static QString
    convertValToString( const std::vector < T > & val )
    {
        QString res;
        for ( const auto & v : val ) {
            res += ( res.isEmpty() ? "" : " " ) + ConverterHelper < T >::convertValToString( v );
        }
        return res;
    }

    static std::vector < T >
    convertStringToVal( const QString & s )
    {
        auto list = s.split( " ", QString::SkipEmptyParts );
        std::vector < T > res;
        for ( auto & v : list ) {
            res.push_back( ConverterHelper < T >::convertStringToVal( v ) );
        }
        return res;
    }
};

/*
template <>
struct Converter<VI> {
    static QString convertValToString( const VI & val) {
        QString res;
        for( auto & v : val) {
            res += ( res.isEmpty() ? "" : " ") + QString::number( v);
        }
        return res;
    }
    static VI convertStringToVal( const QString & s) {
        auto list = s.split(" ", QString::SkipEmptyParts);
        VI res;
        for( auto & v : list) {
            res.push_back( Converter<int>::convertStringToVal(v));
        }
        return res;
    }
};
*/

template < >
struct ConverterHelper < bool > {
    static QString
    convertValToString( bool val )
    {
        return val ? "1" : "0";
    }

    static bool
    convertStringToVal( const QString & s )
    {
        if ( s == "1" ) { return true; }
        if ( s == "0" ) { return false; }
        qWarning() << "string " << s << " is not a boolean";
        return false;
    }
};

template < >
struct ConverterHelper < double > {
    static QString
    convertValToString( double val )
    {
        return QString::number( val );
    }

    static double
    convertStringToVal( const QString & s )
    {
        // try to convert s to double
        bool ok;
        double result = s.toDouble( & ok );
        if ( ! ok ) {
            // check for special string 'nan', otherwise output debug warning
            if ( s.toLower() != "nan" ) {
                qWarning() << "string " << s << " should be nan or valid double";
            }
            result = std::numeric_limits < double >::quiet_NaN();
        }
        return result;
    }
};

template < >
struct ConverterHelper < QPen > {
    static QString
    convertValToString( const QPen pen )
    {
        return QString( "%1 %2 %3")
                .arg( pen.color().name(QColor::HexRgb))
                .arg( pen.color().alphaF())
                .arg( pen.widthF());
    }

    static QPen
    convertStringToVal( const QString & s )
    {
        auto parts = s.split( ' ');
        if( parts.size() != 3) {
            qWarning() << s << "is not a valid qpen (need 3 parts)";
            return QPen();
        }
        QColor color( parts[0]);
        bool ok;
        double alpha = parts[1].toDouble( & ok);
        if( ! ok) {
            qWarning() << s << "is not a valid qpen (bad alpha)";
            return QPen();
        }
        double width = parts[2].toDouble( & ok);
        if( ! ok) {
            qWarning() << s << "is not a valid qpen (bad width)";
            return QPen();
        }

        color.setAlphaF( alpha);
        return QPen( color, width);
    }
};


/// Helper class so that typed vars can do signals/slots. This is needed because
/// Qt does not support template classes for sending/receiving signals...
class TypedVariableBase : public QObject
{
    Q_OBJECT

public:

    TypedVariableBase( QObject * parent = nullptr ) : QObject( parent ) { }

    virtual
    ~TypedVariableBase() { }

protected:

    void
    emitValueChanged() { emit valueChanged(); }

signals:

    void
    valueChanged();
};

/**
 * TypedVariable is a convenience wrapper for RawVar. It has similar functionality
 * but an associated type, which is automatically used to convert to/from string.
 */
template < class T, class Converter = ConverterHelper < T > >
class TypedVariable : public TypedVariableBase
{
    CLASS_BOILERPLATE( TypedVariable < T > );

public:

    TypedVariable( const FullPath & path, Converter cvt = ConverterHelper < T >() ) :
        TypedVariableBase( nullptr ), m_cvt( cvt )
    {
        m_rawVar = RawVar::make( path );
        m_rawVar-> connect( m_rawVar.get(), & RawVar::rawVarChanged,
                            this, & Me::rawVarCB );
        (void) get( true );
    }

    /// get the parsed value
    const T &
    get( bool sync = false )
    {
        if ( sync ) {
            m_cachedVal = m_cvt.convertStringToVal( getRaw( true ) );
        }
        return m_cachedVal;
    }

    /// get the raw (i.e. unparsed) value as QString
    const QString &
    getRaw( bool sync = false )
    {
        return m_rawVar-> get( sync );
    }

    void
    set( const T & val )
    {
        QString str = m_cvt.convertValToString( val );
        m_rawVar-> set( str );
        m_cachedVal = val;
    }

    ~TypedVariable() { }

private:

    void
    rawVarCB( QString newVal )
    {
        m_cachedVal = m_cvt.convertStringToVal( newVal );

        emitValueChanged();
    }

    RawVar::SharedPtr m_rawVar = nullptr;
    T m_cachedVal;
    Converter m_cvt;
};

// convenience types:
typedef TypedVariable < int > IntVar;
typedef TypedVariable < double > DoubleVar;
typedef TypedVariable < QString > StringVar;
typedef TypedVariable < bool > BoolVar;
typedef TypedVariable < VI > VIVar;
typedef TypedVariable < VD > VDVar;

// command handlers

namespace Command
{
/**
 * Handle for registered callbacks, used for removing a callback
 */
typedef int64_t CallbackId;

struct CallbackParameter {
    QString name, value;
};

/**
 * @brief Signature of a callback function.
 *
 * The first parameters to the callback is the command's full name, or alias.
 * The second parameter is the value attached to the command.
 * The result of the callback is a string to be passed back to the caller.
 */

//typedef std::function< QString(const QString &, const QString &) > CallbackType;
typedef std::function < QString( const CallbackParameter & ) > CallbackSignature;

/**
 * @brief Adds another callback for the command.
 * @param cmd
 * @param cb
 * @return callback id that can be used with removeCallback()
 */
CallbackId
addCallback( const QString & cmd, CallbackSignature cb );

/**
 * @brief Same as addCallback(), but with an attached alias. The alias
 *        will be used to call the callback, instead of the full command name. This can
 *        be useful if your commands are starting to have very long names.
 * @param cmd
 * @param alias
 * @param cb
 * @return callback id that can be used with removeCallback()
 */
CallbackId
addCallback( const QString & cmd, const QString & alias, CallbackSignature cb );

/**
 * @brief Remove a single callback with the given id.
 * @param id
 */
void
removeCallback( const CallbackId & id );

/**
 * @brief Removes all callbacks for the given command cmd
 * @param cmd
 */
void
removeAllCallbacks( const QString & cmd );

/**
 * @brief utility class that provides automatic  callback de-registration in the destructor
 */
class CallbackGuard
{
public:

    CallbackGuard();
    CallbackGuard &
    operator= ( CallbackId );

    ~CallbackGuard();

protected:

    CallbackId m_id;
    bool m_hasCallback;
};
}
} // namespace GlobalState
}
}
