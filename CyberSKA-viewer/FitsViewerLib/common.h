#ifndef CONFIG_H
#define CONFIG_H

//#define DO_PRAGMA(x) _Pragma (#x)
//#define TODO(x) DO_PRAGMA(message ("TODO - " #x))

//TODO(Remember to fix this)


#undef CPP11_FULL_SUPPORT

#ifdef CPP11_FULL_SUPPORT
#define CPP11_FINAL final
#define CPP11_OVERRIDE override
#define CPP11_FINAL_OVERRIDE final override
#else
#define CPP11_FINAL
#define CPP11_OVERRIDE
#define CPP11_FINAL_OVERRIDE
#endif



#include <execinfo.h>

#include <ostream>
#include <istream>
#include <iostream>
#include <sstream>
#include <QString>
#include <stdexcept>
#include <QImage>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QHostInfo>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <thread>

#include "ConsoleColors.h"

namespace RaiLib {

void print_trace (std::ostream & out);
};


QString image2base64( const QImage & pimg, int quality, int frame);


/// convenience function to sleep for the given number of seconds
void qsleep( double seconds);

// convenience overload for std::ostream and QString
std::basic_ostream<char> & operator << ( std::basic_ostream<char> & out, const QString & s);
std::basic_istream<char> & operator >> ( std::basic_istream<char> & inp, QString & s);

// convenience overload for QTextStream << std::string
QTextStream & operator << ( QTextStream & out, const std::string & s);
// convenience overload for QTextStream << QPoint and QPointF
QTextStream & operator << ( QTextStream & out, const QPoint & pt);
QTextStream & operator << ( QTextStream & out, const QPointF & pt);

void fatalError( const QString & s);
void fatalError( const char * s);
void fatalError( const std::string & s);

// convenience random number generator
inline double rnd( double min, double max) {
    return drand48() * (max - min) + min;
}



//// we reimplement qcoreapp class so that we can trap exceptions thrown in slots...
//class TrappedApplication : public QApplication
//{
//    Q_OBJECT

//public:
//    TrappedApplication( int & argc, char ** argv)
//        : QApplication( argc, argv)
//    {}

//    virtual bool notify(QObject * o, QEvent *ev) {
//        try {
//            return QCoreApplication::notify( o, ev);
//        } catch (QString s) {
//            fatalError( s);
//        } catch (const char * s) {
//            fatalError( s);
//        } catch (const std::string & s) {
//            fatalError(s);
//        } catch (...) {
//            fatalError( "Unknown exception occured");
//        }
//        return false;
//    }

////    virtual ~TrappedApplication() {}
//};

/// unfortunately this has to be a macro if we want __FILE__ and __LINE__ macros to work
#define dbg(level) ( Debug::debug((level),__FILE__,__LINE__,__PRETTY_FUNCTION__))

/// warning macro (same as dbg(-1) with console colors set to warning
#define warn() ( Debug::debug((-1),__FILE__,__LINE__,__PRETTY_FUNCTION__) \
    << ConsoleColors::warning() )

#define dbgHere \
    do { \
    dbg(-1) << "HERE: " << __FILE__ << ":" << __LINE__ << "\n"; \
    } while(0)

/// convenience macro to throw exception with file/line number
#define LTHROW(x)                                                                     \
    do {                                                                              \
    std::stringstream _s;                                                             \
    _s<< __FILE__ << ":" << __LINE__ << ":" << (x);                                   \
    dbg(0) << ConsoleColors::error () << _s.str() << ConsoleColors::reset () << "\n"; \
    throw std::runtime_error( _s.str());                                              \
    } while(0)

class Debug {
public:

    class DebugReceiver {
    public:
        virtual void appendDebugString( const QString & ) = 0;
        virtual ~DebugReceiver() {}
    };

    class Stream {
    public:
        Stream( Stream && other) {
            m_buff = other.m_buff;
            m_on = other.m_on;
            m_dr = other.m_dr;

            other.m_buff = "";
            other.m_dr = nullptr;
        }

        Stream( bool on, DebugReceiver * dr) {
            m_on = on;
            m_dr = dr;
        }

        // convenience overload for printing vectors
        template < typename T >
        Stream & operator << ( const std::vector<T> & vec)
        {
            QTextStream out( & m_buff);
            bool needSpace = false;
            for( const auto & val : vec) {
                out << (needSpace ? " " : "") << val;
                needSpace = true;
            }
            return * this;
        }

        template < typename T >
        Stream & operator <<  ( const T & val) {
            if( m_on)
            {
//                QString s; QTextStream out( & s);
//                out << val;
//                dr_-> appendDebugString( s);
                QTextStream out( & m_buff);
                out << val;
            }
            return * this;
        }



        ~Stream() {
            if( ! m_buff.endsWith('\n')) m_buff += "\n";
            std::string stdstr = ConsoleColors::reset();
            m_buff += stdstr.c_str();
            if( m_dr)
                m_dr-> appendDebugString( m_buff);
        }

        QString m_buff;

        bool m_on;
        DebugReceiver * m_dr;
    };

    static Stream debug( int level, const QString & fname, int lineno, const QString & function) {
        QMutexLocker locker( & m_mutex);

        if( debugReceiver_ == 0) setReceiver( new CerrDebugReceiver);
        if( level <= verbocity) {
            Stream stream( true, debugReceiver_);
            stream << QString( "%1 ").arg( m_time.elapsed(), 5, 10);
            m_time.restart();
            if( displayLevel) {
                stream << level << ":";
            }
            if( displayThreadId) {
                static std::map< std::thread::id, int > threadMap;
                static int myid = 1;
                std::thread::id tid = std::this_thread::get_id();
                if( threadMap.find( tid) == threadMap.end())
                    threadMap[tid] = myid ++;
                stream << "#" << threadMap[tid] << ":";
            }
            if( displaySource)
            {
                stream << QFileInfo(fname).fileName() << ":" << lineno << ":";

                if( displayFunctionName)
                {
                    stream << function << ":";
                }
            }
            return std::move(stream);
        } else {
            return Stream( false, debugReceiver_);
        }
    }

    static int verbocity;
    static bool displaySource;
    static bool displayLevel;
    static bool displayFunctionName;
    static bool displayThreadId;
    static QTime m_time;

    static void setReceiver( DebugReceiver * dr) {
        if( debugReceiver_) delete debugReceiver_;
        debugReceiver_ = dr;
    }

    class CerrDebugReceiver : public DebugReceiver {
    public:
        virtual void appendDebugString (const QString & s) {
            static QMutex mutex;
            QMutexLocker locker( & mutex);
            std::cerr << s.toStdString ();
        }
    };

protected:
    static DebugReceiver * debugReceiver_;
    static QMutex m_mutex;

};

struct ScopedDebug {
    ScopedDebug( const std::string & str, int level = 1)
    {
        m_str = str;
        m_level = level;
        dbg(1) << "+Entering scope: " << m_str << "\n";
    }
    ~ScopedDebug()
    {
        dbg(m_level) << "-Exiting scope: " << m_str << "\n";
    }

    std::string m_str;
    int m_level;
};


class FitsFileLocation {
public:

    // public constructors with pretty names
    static FitsFileLocation fromElgg( QString elggGuid, QString localFname, QString stamp) {
        FitsFileLocation loc;
        loc.elggGuid_ = elggGuid;
        QFileInfo inf( localFname);
        loc.localFname_ = inf.canonicalFilePath();
        loc.stamp_ = stamp;
        return loc;
    }

    static FitsFileLocation fromLocal( QString localFname) {
        FitsFileLocation loc;
        QFileInfo inf( localFname);
        loc.localFname_ = inf.canonicalFilePath();
        if( loc.localFname_.isEmpty()) loc.localFname_ = localFname;
        loc.stamp_ = inf.lastModified().toString( Qt::ISODate);
        return loc;
    }

    // parses following syntax:
    //    elgg:<GUID>:<timestamp>:<local filename>
    // or local:<local filename>
    // or <local filename>
    //
    // any of the <> parts can be actually arbitrary strings, as long as
    // <GUID> and <timestamp> don't contain colons ':', as the parser uses them as separators
    static FitsFileLocation fromCommandLine( QString s) {
        FitsFileLocation fLoc;
        if( s.startsWith ( "elgg:")) {
            int i1 = s.indexOf ( ':', 5);
            int i2 = s.indexOf ( ':', i1+1);
            if( i1 < 0 || i2 < 0) throw "Invalid syntax for file location";
            fLoc = fromElgg( s.mid (5, i1-5), s.mid(i2+1), s.mid (i1+1, i2-i1-1));
        } else if( s.startsWith ( "local:")) {
            fLoc = fromLocal ( s.mid(6));
        } else {
            fLoc = fromLocal ( s);
        }
        return fLoc;
    }

    static FitsFileLocation invalid() {
        FitsFileLocation loc;
        return loc;
    }

    bool isElgg() const {
        return ! elggGuid_.isNull();
    }

    /// returns the canonical path to the file
    const QString & getLocalFname() const {
        return localFname_;
    }

    const QString & getElggGuid() const {
        return elggGuid_;
    }

    const QString & getStamp() const {
        return stamp_;
    }

    QString uniqueId() const {
        if( isElgg())
            return "elgg:" + elggGuid_;
        else
            return QString("local:%1:%2")
                    .arg(QHostInfo::localHostName())
                    .arg(localFname_);
    }

    bool operator == ( const FitsFileLocation & loc) const {
        return elggGuid_ == loc.elggGuid_
                && localFname_ == loc.localFname_
                && stamp_ == loc.stamp_;
    }

    bool operator != ( const FitsFileLocation & loc) const {
        return ! (* this == loc);
    }


    QString toStr() const {
        return QString("[%1|%2|%3]").arg(elggGuid_).arg(localFname_).arg(stamp_);
    }

//    /**
//     * @brief copy constructor
//     * @param loc
//     */
//    FitsFileLocation( const FitsFileLocation & loc) {
//        this-> elggGuid_ = loc.elggGuid_;
//        this-> localFname_ = loc.localFname_;
//        this-> stamp_ = loc.stamp_;
//    }

    /**
     * @brief default constructor - constructs an invalid location
     */
    FitsFileLocation() {}

    /**
     * @brief returns true if the location is valid
     * @return
     */
    bool isValid() const {
        return ! localFname_.isNull();
    }

protected:

    QString elggGuid_;
    QString localFname_;
    QString stamp_;
};

// simple Rgb class (8 bit per color, 0..255)
struct Rgb {
    int r, g, b;
    Rgb() { r = g = b = 0; }
    Rgb( int red, int green, int blue) { r = red; g = green; b = blue; }

    QString toString() { return QString("(%1,%2,%3)").arg (r).arg (g).arg (b); }
};

// simple Rgb class (double per color, 0..1)
struct Rgbf {
    double r, g, b;
    Rgbf() { r = g = b = 0; }
    Rgbf( double red, double green, double blue) { r = red; g = green; b = blue; }

    QString toString() { return QString("(%1,%2,%3)").arg (r).arg (g).arg (b); }
};


template <typename T>
inline
T clamp( const T & v, const T & v1, const T & v2)
{
    if( v < v1) return v1;
    if( v > v2) return v2;
    return v;
}

template <typename T>
inline void
swap_ordered( T & v1, T & v2)
{
    if( v1 > v2) std::swap( v1, v2);
}

// this is the exception the gsl error trap will throw
class gsl_exception : public std::runtime_error
{
public:
    explicit gsl_exception (
            const std::string & p_reason,
            const std::string & p_file,
            int p_line,
            int p_gslErrno
            )
        : std::runtime_error( "GSL global error")
    {
        reason = p_reason;
        file = p_file;
        line = p_line;
        gslErrno = p_gslErrno;
    }

    virtual ~ gsl_exception() throw() {}

    virtual const char * what() const throw()
    {
        std::ostringstream out;
        out << "GSL-error:" << file << ":" << line << ":" << reason << "["
            << gslErrno << "]";
        std::string stdstr = out.str();
        return stdstr.c_str();
    }

    std::string reason, file;
    int line, gslErrno;

};



#endif // CONFIG_H
