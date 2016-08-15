/**
 *
 **/

#pragma once

#include "CartaLib/IImage.h"

#include <QTime>
#include <QTimer>

namespace Carta {
namespace Lib {

namespace Profiles
{
typedef std::vector < int > VI;
typedef std::vector < qint64 > VI64;
typedef std::vector < double > VD;

/// list of profile types we'll support...
/// At the moment only the Principal will be supported for sure, the others are up for
/// discussion.
enum class ProfilePathType
{
    Principal = 0,
    Line,
    Polyline,
    Spline,
    Other
};

/// profile along principal axes of an image
class PrincipalAxisProfilePath
{
public:

    PrincipalAxisProfilePath( int axis, const VI & pos )
        : m_axis( axis )
          , m_pos( pos )
    { }

    int
    axis() const { return m_axis; }

    const VI &
    pos() const { return m_pos; }

private:

    int m_axis;
    VI m_pos;
};

/// describes a profile path that is a straight line through the n-dimensional data cube
///
/// this is not implemented anywhere yet, it may even be removed
///
/// the sole purpose of this class is to make sure the APIs for profile extraction will work
/// on multiple different profile path types...
class LineProfilePath
{
public:

    LineProfilePath( const VD & p1, const VD & p2, double radius )
        : m_p1( p1 )
          , m_p2( p2 )
          , m_radius( radius )
    { }

    const VD &
    p1() const { return m_p1; }

    const VD &
    p2() const { return m_p2; }

    double
    radius() const { return m_radius; }

private:

    VD m_p1, m_p2;
    double m_radius;
};

/// Container for one of the supported profile path types. It should essentially act as
/// a type-safe union of the profile paths, but since std::variant<> is not yet in the
/// standard, we'll have to hack it ourselves...
///
/// I tried with unrestricted c++11 unions, but it's way too much effort for the potential
/// benefit, so we'll just go with an inefficient approach where we'll store all possible
/// path types.
///
/// \todo Once c++17 std::variant is available, we can fix this. (N4542)
/// Or we could get eggs.variant implementation.
/// Or we could use inefficient QVariant.
///
class ProfilePath
{
public:

    /// named constructor for Principal Axis
    static ProfilePath
    principal( int axis, const VI & pos )
    {
        return ProfilePath( PrincipalAxisProfilePath( axis, pos ) );
    }

    /// named constructor for Line
    static ProfilePath
    line( const VD & p1, const VD & p2, double radius )
    {
        return ProfilePath( LineProfilePath( p1, p2, radius ) );
    }

    ProfilePath()
    {
        m_type = ProfilePathType::Other;
    }

    ProfilePath( const PrincipalAxisProfilePath & profile )
    {
        m_type = ProfilePathType::Principal;
        m_principal = profile;
    }

    ProfilePath( const LineProfilePath & profile )
    {
        m_type = ProfilePathType::Line;
        m_line = profile;
    }

    ProfilePathType
    type() const { return m_type; }

    const PrincipalAxisProfilePath &
    getPrincipalProfile() const
    {
        CARTA_ASSERT( m_type == ProfilePathType::Principal );
        return m_principal;
    }

    LineProfilePath &
    getLineProfile()
    {
        CARTA_ASSERT( m_type == ProfilePathType::Line );
        return m_line;
    }

private:

    ProfilePathType m_type;

    /// \todo these should be std::variant<> ....
    PrincipalAxisProfilePath m_principal = PrincipalAxisProfilePath( 0, { } );
    LineProfilePath m_line = LineProfilePath( { }, { }, 0 );
};

/// this is the API that a plugin must implement to provide its own profile extraction
class IProfileExtractor : public QObject
{
    Q_OBJECT
    CLASS_BOILERPLATE(IProfileExtractor);

public:

    IProfileExtractor( QObject * parent = nullptr ) : QObject( parent ) { }

    virtual
    ~IProfileExtractor() { }

    /// priority of this algorithm to help core decide which one to use if multiple
    /// are available
//    virtual int priority () = 0;

public slots:

    /// start the extraction, if possible the last extraction will be cancelled but
    /// this is not guaranteed
    virtual void
    start( Carta::Lib::NdArray::RawViewInterface * rv, const ProfilePath & profilePath,
           qint64 id ) = 0;

signals:

    /// this is emitted any time there is progress with the data
    /// the job is done when totalLength == data.length()
    /// \param id the id of the job
    /// \param totalLength the length (in pixels, not bytes) to expect of the data
    ///        -1 = not available, -2 = error condition
    /// \param data the raw data
    void
    progress( qint64 id, qint64 totalLength, QByteArray data );
};

/// the default implemenation of a principal axis profile extractor
/// this is slow, but it'll work for any image
///
/// \todo work is done using timers, in the same thread. A better solution would be to
/// move this into its own thread, but that will require thread support from RawViewInterface
/// and ImageInterface as well...
///
class DefaultPrincipalProfileExtractor : public IProfileExtractor
{
    Q_OBJECT
    CLASS_BOILERPLATE( DefaultPrincipalProfileExtractor );

public:

    DefaultPrincipalProfileExtractor( QObject * parent = nullptr )
        : IProfileExtractor( parent )
    {
        // when we emit internal delayedProgress, the real progress will be emitted
        // little later (this essentially allows us to emit progress directly from
        // the start method, but these signals will be delivered asynchronously)
        connect( this, & Me::_delayedProgress, this, & Me::progress, Qt::QueuedConnection );

        // setup work timer (to simulate extraction)
        m_workTimer.setInterval( 100 );
        connect( & m_workTimer, & QTimer::timeout, this, & Me::workTimerCB );
    }

public slots:

    virtual void
    start( Carta::Lib::NdArray::RawViewInterface * rv, const ProfilePath & profilePath,
           qint64 id ) override
    {
        m_id = id;

        CARTA_ASSERT( rv );
        if ( profilePath.type() != ProfilePathType::Principal ) {
            qCritical() << "DefaultPrincipalProfileExtractor can only handle Principal paths";

            // report the error but delayed, to make it async
            emit _delayedProgress( m_id, - 2, QByteArray() );
            return;
        }

        const PrincipalAxisProfilePath & pa = profilePath.getPrincipalProfile();

        // let's make sure the dimensions match up
        CARTA_ASSERT( rv->dims().size() == pa.pos().size() );
        CARTA_ASSERT( pa.axis() >= 0 && size_t( pa.axis() ) < rv->dims().size() );

        // remember the raw view
        m_rv = rv;

        // save profile path information
        m_currPos = pa.pos();
        m_axis = pa.axis();
        m_currPos[m_axis] = 0;

        // figure out pixel size
        m_pixelSize = Carta::Lib::Image::pixelType2size( m_rv-> pixelType() );

//        switch ( m_rv->pixelType() )
//        {
//        case Carta::Lib::Image::PixelType::Byte :
//            m_pixelSize =
//                Carta::Lib::Image::PixelType2CType < Carta::Lib::Image::PixelType::Byte >::size;
//            break;
//        case Carta::Lib::Image::PixelType::Int16 :
//            m_pixelSize =
//                Carta::Lib::Image::PixelType2CType < Carta::Lib::Image::PixelType::Int16 >::size;
//            break;
//        case Carta::Lib::Image::PixelType::Int32 :
//            m_pixelSize =
//                Carta::Lib::Image::PixelType2CType < Carta::Lib::Image::PixelType::Int32 >::size;
//            break;
//        case Carta::Lib::Image::PixelType::Int64 :
//            m_pixelSize =
//                Carta::Lib::Image::PixelType2CType < Carta::Lib::Image::PixelType::Int64 >::size;
//            break;
//        case Carta::Lib::Image::PixelType::Real32 :
//            m_pixelSize =
//                Carta::Lib::Image::PixelType2CType < Carta::Lib::Image::PixelType::Real32 >::size;
//            break;
//        case Carta::Lib::Image::PixelType::Real64 :
//            m_pixelSize =
//                Carta::Lib::Image::PixelType2CType < Carta::Lib::Image::PixelType::Real64 >::size;
//            break;
//        case Carta::Lib::Image::PixelType::Other :
//            break;
//        } // switch
        CARTA_ASSERT( m_pixelSize > 0 );

        // immediately report delayed progress (to establish total length of the result)
        emit _delayedProgress( m_id, m_rv->dims()[m_axis], QByteArray() );

        // and start the extraction
        m_workTimer.start();
    } // start

signals:

    /// internal signal - used to deliver progress asynchronously
    void
    _delayedProgress( qint64 id, qint64 totalLength, QByteArray data );

private slots:

    void
    workTimerCB()
    {
        qDebug() << "profile workTimerCB()";

        // note that from here we ca safely emit directly, since this is executed
        // as a callback of the work timer...
        QTime t;
        t.restart();

//        while ( t.elapsed() < 100 ) {
        while ( true ) {
            // if we are done, stop timer, report result
            qDebug() << "profile cmp" << m_currPos[m_axis] << "to" << m_rv->dims()[m_axis];
            if ( m_currPos[m_axis] >= m_rv-> dims()[m_axis] ) {
                qDebug() << "profile stop timer";
                m_workTimer.stop();
                break;
            }

            qDebug() << "profile pos" << m_currPos;

            // get the element at the current position
            const char * data = m_rv->get( m_currPos );
            m_buffer.append( data, m_pixelSize );

            m_currPos[m_axis]++;

            // only do little bit of work
            if ( t.elapsed() >= 10 ) {
                break;
            }
        }

        // report result
        emit progress( m_id, m_rv->dims()[m_axis], m_buffer );
    } // workTimerCB

private:

    VI m_currPos;
    int m_axis = - 1;
    QTimer m_workTimer;
    Carta::Lib::NdArray::RawViewInterface * m_rv = nullptr;
    QByteArray m_buffer;
    qint64 m_id = - 1;
    size_t m_pixelSize = 0;
};

/// this will somehow return the best algorithm available by combining built-in extractors
/// and those provided by plugins
///
/// \todo implement hook for obtaining best extractors from plugins
IProfileExtractor *
getBestProfileExtractor( Carta::Lib::NdArray::RawViewInterface * rv, ProfilePathType pt );

/// this is the extractor that encapsulates all profile extractions tidbits into one
/// convenient place. Most code should only use this single class for all profile
/// extraction needs
class ProfileExtractor : public QObject
{
    Q_OBJECT

public:

    ProfileExtractor( Carta::Lib::NdArray::RawViewInterface * rv, QObject * parent =
                          nullptr ) : QObject( parent )
    {
        m_rawView = rv;
    }

    /// start the extraction, if possible the last extraction will be cancelled but
    /// this is not guaranteed
    qint64
    start( /*Carta::Lib::NdArray::RawViewInterface * rv,*/
        const ProfilePath & profilePath,
        qint64 jobId = - 1 )
    {
        // delete the previous algorithm if the profile path type changed or views have changed
        if ( m_algorithm && m_profilePath.type() != profilePath.type() ) {
            m_algorithm->deleteLater();
            m_algorithm = nullptr;
        }

        // create a new algorithm based on raw view & profile type and connect it
        if ( ! m_algorithm ) {
            m_algorithm = getBestProfileExtractor( m_rawView, profilePath.type() );
            connect( m_algorithm, & IProfileExtractor::progress,
                     this, & ProfileExtractor::progressCB );
        }

        if ( jobId == - 1 ) {
            jobId = m_jobId + 1;
        }
        m_jobId = jobId;

        m_pixelSize = Carta::Lib::Image::pixelType2size( m_rawView->pixelType() );
        m_jobId++;
        m_profilePath = profilePath;
        m_algorithm->start( m_rawView, m_profilePath, m_jobId );
        return m_jobId;
    } // start

    // extracting results

    /// get the job ID for which the results are available
    qint64
    currentJobID();

    /// get available data as an array of doubles - this is a convenience function, since
    /// more likely than not we'll end up casting the data to doubles anyways
    const std::vector<double> getDataD();

    /// get the total profile length
    /// if unknown, returns -1
    qint64
    getTotalProfileLength() { return m_totalLength; }

    /// is the extraction finished?
    bool
    isFinished();

    // raw data accessors
    Carta::Lib::Image::PixelType
    dataRawDataType();

    /// get the raw data
    const QByteArray &
    getRawData() { return m_resultBuffer; }

    /// get the raw data length (in pixels!, not bytes)
    qint64
    getRawDataLength()
    {
        CARTA_ASSERT( m_resultBuffer.length() % m_pixelSize == 0 );
        return m_resultBuffer.length() / m_pixelSize;
    }

    virtual
    ~ProfileExtractor()
    {
        if ( m_algorithm ) { delete m_algorithm; }
    }

signals:

    void
    progress();

private slots:

    void
    progressCB( qint64 id, qint64 totalLength, QByteArray data )
    {
        qDebug() << "profile progressCB" << id << totalLength << data.size();

        // ignore this signal if it's for an older id
        if ( id != m_jobId ) {
            qDebug() << "profile progressCB old id" << id << "!=" << m_jobId;
            return;
        }

        m_totalLength = totalLength;
        m_resultBuffer = data;
        emit progress();
    }

private:

    Carta::Lib::NdArray::RawViewInterface * m_rawView = nullptr;
    ProfilePath m_profilePath = ProfilePath::principal( 0, { } );

    //    std::unique_ptr< IProfileExtractor> m_algorithm = nullptr;
    IProfileExtractor * m_algorithm = nullptr;

    qint64 m_jobId = 0;
    size_t m_pixelSize = 0;

    QByteArray m_resultBuffer;
    qint64 m_totalLength = - 1;
};
}
}
}
