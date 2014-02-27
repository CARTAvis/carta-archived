#include <iostream>
#include <QTime>
#include <QMetaType>
#include <QDir>
#include "ProfileService.h"
#include "common.h"
#include "FitsParser.h"
#include "RaiCache.h"

namespace ProfileService {

static const int FIRST_REPORT_TIME = 200;
static const int REPORT_TIME = 1500;

// register metatypes with QT so that we can pass Results and InputParameters via signals/slots
static bool registerMetaTypes() {
    std::cerr << "Registering metatype\n";
    std::cerr << "ProfileService::Results = "
              << qRegisterMetaType<ProfileService::Results>("Results") << "\n";
    std::cerr << "ProfileService::InputParameters = "
              << qRegisterMetaType<ProfileService::InputParameters>("InputParameters")
              << "\n";
    return true;
}
static bool initOnce_ = registerMetaTypes ();

// private exception class
//class Interrupt : public std::runtime_error
//{
//public:
//    Interrupt() : std::runtime_error("worker-interrupt") {}
//};

// private exception class
class Interrupt
{
};


Results::Results()
{
//    std::cerr << "Results constructor from thread " << QThread::currentThread () << "\n";
    status_ = Error;
    total_ = 0;
}

/*

Results::Results(const Results & r)
{
    std::cerr << "Results copy-constructor " << r.testImage.width ()
              << "x" << r.testImage.height () << " from thread " << QThread::currentThread () << "\n";

    testImage = r.testImage;
}
*/


Manager::Manager(QObject *parent) :
    QObject(parent),
    worker_(interruptRequested_)
{
    // initialize internal fields
    interruptRequested_ = false;
    hasPendingInput_ = false;
    workerBusy_ = false;

    // connect worker and manager
    connect ( & worker_, SIGNAL( progress(Results)),
              this, SLOT(onProgress(Results)));
    connect ( & worker_, SIGNAL( done(Results)),
              this, SLOT(onDone(Results)));
    connect ( & worker_, SIGNAL( error(QString)),
              this, SLOT(onError(QString)));
    connect ( & worker_, SIGNAL( interrupt()),
              this, SLOT(onInterrupt()));
    connect ( this, SIGNAL(privateGo(InputParameters)),
              & worker_, SLOT(onGoFromService(InputParameters)));


    // move the worker to a separate thread
    worker_.moveToThread( & workerThread_);
    workerThread_.start ();
}


// TODO: optimize for different BITPIX
// right now this is hard-wired for bitpix=64 (double), since the class uses
// the inefficient FITS parser...

qint8 Results::bitpix() const {
    return 64;
}

void Manager::request (InputParameters input)
{
//    std::cerr << "Manager::request " << input.toString() << "\n";

    // if worker is running, ask for interrupt
    if( workerBusy_) {
        interruptRequested_ = true;
    }
    pendingInput_ = input;
    hasPendingInput_ = true;
    schedulePending ();
}

// if there is a pending request, and worker is not busy, ask worker
// to process the pending request
// otherwise do nothing
void Manager::schedulePending ()
{
    if( ! hasPendingInput_) return;
    if( workerBusy_) return;

    hasPendingInput_ = false;
    interruptRequested_ = false;
    workerBusy_ = true;
    // tell worker to start working on the request
    emit privateGo ( pendingInput_);
}

void Manager::go ()
{
}

Worker::Worker( bool & interruptFlagRef)
    : interruptFlag_( interruptFlagRef)
//    , m_parser( nullptr)
{
//    fitsParser_ = 0;
    //    currentFitsLocation_ = "";
}

Worker::~Worker()
{
//    if( m_parser != nullptr) {
//        delete m_parser;
//    }
}

// throws an exception if the interrupt flag is set
void Worker::checkForInterrupts ()
{
    if( interruptFlag_ == true) {
//        dbg(1) << "#" << QThread::currentThreadId() << " interrupting\n";
        throw Interrupt();
    }
}


void Worker::onGoFromService (InputParameters input)
{
    try {

        if( m_cc.isNull () || m_cc-> getInitParameters () != input.ccinit) {
            m_cc = QSharedPointer<RaiLib::Cache::Controller>(
                        new RaiLib::Cache::Controller( input.ccinit));
        }
        if( input.fitsLocation != m_handle.floc ()) {
            m_handle = m_cc-> getFileHandle ( input.fitsLocation);
        }


        if( input.direction == 2)
            doWorkZ( input);
        else
            doWorkXY ( input);

    } catch ( Interrupt & ) {
        // std::cerr << "*** Worker interrupted ***\n";
        emit interrupt ();
    } catch ( std::runtime_error & e) {
        std::cerr << "Worker runtime-exception: " << e.what() << "\n";
        emit error( QString("runtime-exception: %1").arg(e.what()));
    } catch ( std::exception & e) {
        std::cerr << "Worker std-exception: " << e.what() << "\n";
        emit error( QString("std-exception: %1").arg(e.what()));
    } catch ( std::string & s) {
        std::cerr << "Worker exception: " << s << "\n";
        emit error( QString("exception: %1").arg(s. c_str()));
    } catch ( QString & s) {
        std::cerr << "Worker exception: " << s << "\n";
        emit error( QString("exception: %1").arg(s));
    } catch ( char * s) {
        std::cerr << "Worker exception: " << s << "\n";
        emit error( QString("exception: %1").arg(s));
    } catch (...) {
        std::cerr << "Worker uncaught exception...\n";
        dbg(1) << "Worker uncaught exception\n";
        emit error ( "Uncaught exception");
//        throw;
    }
}

// TODO: refactor doWorkXY and doWorkZ to combine some of the common functionality

void Worker::doWorkXY (InputParameters input)
{
    // check for input errors
    if( input.direction < 0 || input.direction > 2 || input.position.size () > 3) {
        throw std::runtime_error( "profile extraction currently only supports 3 axes");
    }

    // initialize the parser if it's not already initialized
//    if( ! m_parser)
//        m_parser = std::make_shared<FitsParser>();

    // if the parser is working on a different filename, reopen it
    if( currentFitsLocation_.uniqueId () != input.fitsLocation.uniqueId ()) {
        bool res = m_parser.loadFile ( input.fitsLocation);
        if( ! res)
            throw std::runtime_error("Could not open file");
        else
            currentFitsLocation_ = input.fitsLocation;
    }

    // check for more input errors now that we have fits header
    const FitsParser::HeaderInfo & hdr = m_parser.getHeaderInfo ();
    if( int(input.position[0]) >= hdr.naxis1 ||
            int(input.position[1]) >= hdr.naxis2 ||
            int(input.position[2]) >= hdr.naxis3)
    {
        throw std::runtime_error("position out of range");
    }

    // prepare results (partial)
    Results r;
    r.input = input; // save reference to input
    r.status_ = Results::Partial;
    r.rawData_.clear ();

    // make sure minPos < maxPos (swap if necessary)
    if( r.input.minPos > r.input.maxPos)
        std::swap( r.input.minPos, r.input.maxPos);

    // figure out the max. index of direction
    if( r.input.direction == 0) {
        if( r.input.minPos < 0) r.input.minPos = 0;
        if( r.input.maxPos < 0) r.input.maxPos = hdr.naxis1 - 1;
        r.input.minPos = clamp<qint64>( r.input.minPos, 0, hdr.naxis1 - 1);
        r.input.maxPos = clamp<qint64>( r.input.maxPos, 0, hdr.naxis1 - 1);
    } else if( r.input.direction == 1) {
        if( r.input.minPos < 0) r.input.minPos = 0;
        if( r.input.maxPos < 0) r.input.maxPos = hdr.naxis2 - 1;
        r.input.minPos = clamp<qint64>( r.input.minPos, 0, hdr.naxis2 - 1);
        r.input.maxPos = clamp<qint64>( r.input.maxPos, 0, hdr.naxis2 - 1);
    } else {
        if( r.input.minPos < 0) r.input.minPos = 0;
        if( r.input.maxPos < 0) r.input.maxPos = hdr.totalFrames - 1;
        r.input.minPos = clamp<qint64>( r.input.minPos, 0, hdr.totalFrames - 1);
        r.input.maxPos = clamp<qint64>( r.input.maxPos, 0, hdr.totalFrames - 1);
    }
    r.total_ = r.input.maxPos - r.input.minPos + 1;
#if QT_VERSION >= 0x040700
    r.rawData_.reserve ( r.total_);
#endif

    // initialize timer for reporting progress
    QTime progressTimer; progressTimer.restart ();
    QVector<qint64> currPos = input.position;
    bool firstTime = true;

    // do the actual extraction
    for(qint64 i = r.input.minPos ; i <= r.input.maxPos ; i ++ ) {

        if( 0){
            // busy sleep for debugging
            QTime t; t.start();
            while( t.elapsed() < 1) {}
        }
        // terminate loop (via exception) if there is an interrupt pending
        checkForInterrupts ();
        // report progress:
        //   - after 100ms if it's the first time
        //   - after that every 1000ms
        if( progressTimer.elapsed () > REPORT_TIME
                || (progressTimer.elapsed () > FIRST_REPORT_TIME && firstTime))
        {
//            dbg(1) << r.input.direction << " reporting partial "
//                   << r.rawData().length() << " / " << r.total()
//                      << "\n";
            firstTime = false;
            progressTimer.restart ();
            r.status_ = r.Partial;
            checkForInterrupts ();
            emit progress( r);
        }
        // extract the next value
        currPos[r.input.direction] = i;
//        const char * ptr = m_parser. srcRaw ( currPos[0], currPos[1], currPos[2]);
//        double val = m_parser.raw2double ( ptr);
        double val = m_parser.src ( currPos[0], currPos[1], currPos[2]);
        r.rawData_.push_back ( val);
    }
    r.status_ = Results::Complete;
    r.status_ = r.Complete;
    checkForInterrupts ();
    emit done (r);
}


// same as doWork but specialized for Z profile
void Worker::doWorkZ (InputParameters input)
{
    // check for input errors
    if( input.direction < 0 || input.direction > 2 || input.position.size () > 3) {
        throw std::runtime_error( "profile extraction currently only supports 3 axes");
    }

    // initialize the parser if it's not already initialized
//    if( ! m_parser)
//        m_parser = new FitsParser();
    // if the parser is working on a different filename, reopen it
    if( currentFitsLocation_.uniqueId () != input.fitsLocation.uniqueId ()) {
        bool res = m_parser.loadFile ( input.fitsLocation);
        if( ! res)
            throw std::runtime_error("Could not open file");
        else
            currentFitsLocation_ = input.fitsLocation;
    }

    // check for more input errors now that we have fits header
    const FitsParser::HeaderInfo & hdr = m_parser.getHeaderInfo ();
    if( int(input.position[0]) >= hdr.naxis1 ||
            int(input.position[1]) >= hdr.naxis2 ||
            int(input.position[2]) >= hdr.naxis3)
    {
        throw std::runtime_error("position out of range");
    }

    // prepare results (partial)
    Results r;
    r.input = input; // save reference to input
    r.status_ = Results::Partial;
    r.rawData_.clear ();

    // figure out the max. index of direction
    if( r.input.maxPos < 0) r.input.maxPos = hdr.totalFrames - 1;
    if( r.input.minPos > r.input.maxPos)
        std::swap( r.input.minPos, r.input.maxPos);
    r.input.minPos = clamp<qint64>( r.input.minPos, 0, hdr.totalFrames - 1);
    r.input.maxPos = clamp<qint64>( r.input.maxPos, 0, hdr.totalFrames - 1);
    if( r.input.minPos > r.input.maxPos)
        std::swap( r.input.minPos, r.input.maxPos);

    r.total_ = r.input.maxPos - r.input.minPos + 1;

    /// TODO: make these variables class members, and recompute only if
    /// the flocation changes
    RaiLib::ByteBlock rawData;
    RaiLib::Cache::FileHandle handle;
    bool useCache = RaiLib::Cache::shouldCacheZprofile ( hdr);

    // try to get the raw data out of cache if appropriate
    if( useCache) {
//        handle = m_cc-> getFileHandle ( input.fitsLocation);
        handle = m_handle;
        rawData = m_cc-> getRawZprofile ( handle, r.input.position[0], r.input.position[1]);
    }
    checkForInterrupts ();
    // if we were able to extract the raw pfofile from cache, use it to report results
    if( qint64(rawData.size ()) == hdr.totalFrames * hdr.bitpixSize) {
//        dbg(1) << "Got profile from cache " << input.fitsLocation.getLocalFname () << "\n";
        for(qint64 i = r.input.minPos ; i <= r.input.maxPos ; i ++ ) {
            // extract the next value
            const char * ptr = rawData.data () + i * hdr.bitpixSize;
            double val = m_parser.raw2double ( ptr);
            r.rawData_.push_back ( val);
        }
        r.status_ = Results::Complete;
        r.status_ = r.Complete;
        checkForInterrupts ();
        emit done (r);
        return;
    }
    if( rawData.size () > 0) {
        dbg(1) << "!!!!!!!!!!! bad profile data !!!!!!!!!!!\n";
        rawData.clear ();
    }

    // data was not found in cache, we need to compute the profile
    //    dbg(1) << "Computing z profile " << input.fitsLocation.getLocalFname () << "\n";

    rawData.reserve ( (r.input.maxPos - r.input.minPos + 1) * hdr.bitpixSize);

    // initialize timer for reporting progress
    QTime progressTimer; progressTimer.restart ();
    QVector<qint64> currPos = input.position;
    bool firstTime = true;

    // do the actual extraction
    for(qint64 i = r.input.minPos ; i <= r.input.maxPos ; i ++ ) {
        // terminate loop (via exception) if there is an interrupt pending
        checkForInterrupts ();
        // report progress:
        //   - after 100ms if it's the first time
        //   - after that every 1000ms
        if( progressTimer.elapsed () > REPORT_TIME
                || (progressTimer.elapsed () > FIRST_REPORT_TIME && firstTime))
        {
            firstTime = false;
            progressTimer.restart ();
            r.status_ = r.Partial;
            checkForInterrupts ();
            emit progress( r);
        }
        // extract the next value
        currPos[r.input.direction] = i;
        const char * ptr = m_parser. srcRaw ( currPos[0], currPos[1], currPos[2]);
        rawData.insert (
                    rawData.end (),
                    const_cast<char *> (ptr),
                    const_cast<char *> (ptr + hdr.bitpixSize));
//        rawData.append ( ptr, hdr.bitpixSize);
//        double val = fitsParser_->src ( currPos[0], currPos[1], currPos[2]);
        double val = m_parser.raw2double ( ptr);
        r.rawData_.push_back ( val);
    }
    r.status_ = Results::Complete;
    r.status_ = r.Complete;
    checkForInterrupts ();
    emit done (r);

    // and also save the data into cache (in case it's not there yet)
    if( useCache && r.input.minPos == 0 && r.input.maxPos == handle.nframes ()-1) {
            m_cc-> setRawZprofile ( handle, r.input.position[0], r.input.position[1], rawData);
    }
}

void Manager::onProgress (Results r)
{
//    std::cerr << "Manager::onProgress: " << r.toString() << "\n";
    emit progress (r);
}

void Manager::onDone (Results r)
{
//    std::cerr << "Manager::onDone: " << r.toString() << "\n";
    emit done (r);
    workerBusy_ = false;
    schedulePending ();
}

void Manager::onError (QString s)
{
//    std::cerr << "Manager::onError: " << s << "\n";
    emit error (s);
    workerBusy_ = false;
    schedulePending ();
}

void Manager::onInterrupt ()
{
//    std::cerr << "Manager::onInterrupt\n";
    workerBusy_ = false;
    schedulePending ();
}



}; // namespace ProfileService
