/**
 * This is the license for the CyberSKA image viewer.
 *
 **/

#pragma once

#include <cstdint>
#include <QString>
#include <functional>
#include <vector>
#include <map>
#include <cmath>
#include <limits>
#include <QStringList>
#include <memory>
#include "FitsViewerLib/common.h"

// TODO: hide this with PIMPL
namespace CSI {
class ValueChangedEventArgs;
};

namespace GlobalState {

typedef std::vector<int> VI;
typedef std::vector<double> VD;

class RawVariable
{
public:

    typedef int CallbackID;
//    using CallbackID = int;
    typedef std::function<void(RawVariable &)> CallbackType;

    explicit RawVariable( const QString & pathPrefix, const QString & pathPostfix = QString());
    ~RawVariable();
    CallbackID addCallback( CallbackType fn);
    void removeCallback( const CallbackID & cbId);
    void set( const QString & val);
    const QString & get( bool sync = false);

    const QString & prefix() const;
    const QString & postfix() const;

protected:
    // map of callback ids to the actual callbacks to make it possible to remove
    // callbacks by ID
    std::map< CallbackID, CallbackType > m_cbmap;
    // incremental ID counter
    CallbackID m_currId;
    // prefix/postfix and full path of the variable
    QString m_prefix, m_postfix, m_fullPath;
    // cached value
    QString m_cachedValue;

    // internal callback for the PureWeb system
    // TODO: this should be hidden by PIMPL
    void internalCb( const CSI::ValueChangedEventArgs & val);
    // whether we are registered with pureweb for value changed callbacks
    bool m_internalCbRegistered;
    // internal guard for being inside a callback (big warning if we try to
    // destruct ourselves while in a callback)
    bool m_insideInternalCb;

};


struct RangedDouble {
    RangedDouble( double min, double max) {
        m_min = min; m_max = max;
    }
    double m_min, m_max;
    QString convertValToString( double val) {
        val = clamp( val, m_min, m_max);
        return QString::number( val);
    }
    double convertStringToVal( const QString & s) {
        QVariant variant( s);
        double val = s.toDouble();
        val = clamp( val, m_min, m_max);
        return val;
    }
};

// converter helper
// needs to be specialized for new types
template < class T >
struct Converter {
    static QString convertValToString( const T & val);
    static T convertStringToVal( const QString & s);
};

template <>
struct Converter<QString> {
    static QString convertValToString( const QString & s) {
        return s;
    }
    static QString convertStringToVal( const QString & s) {
        return s;
    }
};


template <>
struct Converter<int> {
    static QString convertValToString( int val) {
        return QString::number( val);
    }
    static int convertStringToVal( const QString & s) {
//        QVariant variant( s);
//        return s.toInt();
        bool ok;
        int result = s.toInt( & ok);
        if( ! ok) {
            dbg(0) << ConsoleColors::warning()
                   << "Warning: string " << s << " should be int"
                   << ConsoleColors::reset();
            result = 0;
        }
        return result;
    }
};

// generic std::vector
template <class T>
struct Converter< std::vector<T> > {
    static QString convertValToString( const std::vector<T> & val) {
        QString res;
        for( const auto & v : val) {
            res += ( res.isEmpty() ? "" : " ") + Converter<T>::convertValToString( v);
        }
        return res;
    }
    static std::vector<T> convertStringToVal( const QString & s) {
        auto list = s.split(" ", QString::SkipEmptyParts);
        std::vector<T> res;
        for( auto & v : list) {
            res.push_back( Converter<T>::convertStringToVal(v));
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

template <>
struct Converter<bool> {
    static QString convertValToString( bool val) {
        return val ? "1" : "0";
    }
    static bool convertStringToVal( const QString & s) {
        if( s == "1") return true;
        if( s == "0") return false;
        dbg(0) << ConsoleColors::warning()
               << "Warning: string " << s << " is not a boolean"
               << ConsoleColors::reset();
        return false;
    }
};


template <>
struct Converter<double> {
    static QString convertValToString( double val) {
        return QString::number( val);
    }
    static double convertStringToVal( const QString & s) {
        // try to convert s to double
        bool ok;
        double result = s.toDouble( & ok);
        if( ! ok) {
            // check for special string 'nan', otherwise output debug warning
            if( s.toLower() != "nan")  {
                dbg(0) << ConsoleColors::warning()
                       << "Warning: string " << s << " should be nan or valid double"
                       << ConsoleColors::reset();
            }
            result = std::numeric_limits<double>::quiet_NaN();
        }
        return result;
    }
};

/**
 * TypedVariable is a convenience wrapper for RawVariable. It has similar functionality
 * but an associated type, which is automatically used to convert to/from string.
 */
template < class T, class Converter = Converter<T> >
class TypedVariable {
public:
    typedef std::shared_ptr< TypedVariable<T> > SharedPtr;
    typedef std::unique_ptr< TypedVariable<T> > UniquePtr;

    TypedVariable( const QString & path, Converter cvt = Converter()) :
        m_cvt( cvt)
    {
        m_gv.reset( new RawVariable( path));
    }
    bool isSet() const;
    /// get the parsed value
    const T & get( bool sync = false) {
        QString s = m_gv-> get( sync);
        m_cachedVal = m_cvt.convertStringToVal(s);
        return m_cachedVal;
    }
    /// get the raw (i.e. unparsed) value as QString
    const QString & getRaw( bool sync = false) {
        return m_gv-> get( sync);
    }

    void set( const T & val) {
        QString str = m_cvt.convertValToString( val);
        m_gv-> set( str);
    }
    RawVariable::CallbackID addCallback( std::function<void(const T &)> fn) {
        auto closure = [this,fn] (RawVariable & gv) {
            m_cachedVal = m_cvt.convertStringToVal( gv.get());
            fn( m_cachedVal);
        };
        auto id = m_gv-> addCallback( closure);
        return id;
    }
    RawVariable::CallbackID addCallback( std::function<void()> fn) {
        auto closure = [this,fn] (RawVariable & gv) {
            m_cachedVal = m_cvt.convertStringToVal( gv.get());
            fn();
        };
        auto id = m_gv-> addCallback( closure);
        return id;
    }

    ~TypedVariable() {
    }
protected:
    std::unique_ptr< RawVariable > m_gv;
    T m_cachedVal;
    Converter m_cvt;
};

// convenience types:

// g++ 4.6.3 doesn't like all of c++11 syntax /cry
//using IntVar = TypedVariable<int>;
//using DoubleVar = TypedVariable<double>;
//using StringVar = TypedVariable<QString>;
//using BoolVar = TypedVariable<bool>;

typedef TypedVariable<int> IntVar;
typedef TypedVariable<double> DoubleVar;
typedef TypedVariable<QString> StringVar;
typedef TypedVariable<bool> BoolVar;
typedef TypedVariable< VI> VIVar;
typedef TypedVariable< VD> VDVar;

// command handlers

namespace Command {

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
typedef std::function< QString( const CallbackParameter &) > CallbackSignature;

/**
 * @brief Adds another callback for the command.
 * @param cmd
 * @param cb
 * @return callback id that can be used with removeCallback()
 */
CallbackId addCallback( const QString & cmd, CallbackSignature cb);

/**
 * @brief Same as addCallback(), but with an attached alias. The alias
 *        will be used to call the callback, instead of the full command name. This can
 *        be useful if your commands are starting to have very long names.
 * @param cmd
 * @param alias
 * @param cb
 * @return callback id that can be used with removeCallback()
 */
CallbackId addCallback( const QString & cmd, const QString & alias, CallbackSignature cb);

/**
 * @brief Remove a single callback with the given id.
 * @param id
 */
void removeCallback( const CallbackId & id);

/**
 * @brief Removes all callbacks for the given command cmd
 * @param cmd
 */
void removeAllCallbacks( const QString & cmd);

/**
 * @brief utility class that provides automatic  callback de-registration in the destructor
 */
class CallbackGuard {
public:
    CallbackGuard();
    CallbackGuard & operator= ( CallbackId);
    ~CallbackGuard();
protected:
    CallbackId m_id;
    bool m_hasCallback;
};

}



} // namespace GlobalState
