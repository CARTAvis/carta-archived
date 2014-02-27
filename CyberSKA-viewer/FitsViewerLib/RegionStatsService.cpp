#include "RegionStatsService.h"

#include <iostream>
#include <QTime>
#include <QMetaType>
#include <QDir>
#include <cmath>
#include "RegionStatsService.h"
#include "common.h"
#include "FitsParser.h"
#include "RaiCache.h"

namespace RegionStatsService {

// register metatypes with QT so that we can pass Results and InputParameters via signals/slots
static bool registerMetaTypes() {
    std::cerr << "Registering metatype\n";
    std::cerr << " RegionStatsService::Results = "
              << qRegisterMetaType<RegionStatsService::ResultsRS>("ResultsRS") << "\n";
    std::cerr << " RegionStatsService::InputParameters = "
              << qRegisterMetaType<RegionStatsService::InputParametersRS>("InputParametersRS")
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


ResultsRS::ResultsRS()
{
    status_ = Error;

    width = height = totalPixels = nanPixels = 0;
    min = max = average = sum = rms = bkgLevel = sumMinusBkg = maxMinusBkg = 0.0;
    maxPos = QPoint( -1, -1);
    depth = 0;
    currentFrame = 0;
    nFramesComputed = 0;
    beamArea = std::numeric_limits<double>::quiet_NaN();
    totalFluxDensity = std::numeric_limits<double>::quiet_NaN();
    aboveBackground = std::numeric_limits<double>::quiet_NaN();
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
    connect ( & worker_, SIGNAL( progress(ResultsRS)),
              this, SLOT(onProgress(ResultsRS)));
    connect ( & worker_, SIGNAL( done(ResultsRS)),
              this, SLOT(onDone(ResultsRS)));
    connect ( & worker_, SIGNAL( error(QString)),
              this, SLOT(onError(QString)));
    connect ( & worker_, SIGNAL( interrupt()),
              this, SLOT(onInterrupt()));
    connect ( this, SIGNAL(privateGo(InputParametersRS)),
              & worker_, SLOT(onGoFromService(InputParametersRS)));


    // move the worker to a separate thread
    worker_.moveToThread( & workerThread_);
    workerThread_.start ();
}


void Manager::request (InputParametersRS input)
{
    dbg(1) << "Manager::request " << input.toString() << "\n";

    // if worker is running, ask for interrupt
    if( workerBusy_) {
        interruptRequested_ = true;
    }
    pendingInput_ = input;
    hasPendingInput_ = true;
    schedulePending ();
}

// if there is a pending request, and worker is not busy, ask worker
// to process the pending request, otherwise do nothing
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
    , m_parser( 0)
{
//    fitsParser_ = 0;
//    currentFitsLocation_ = "";
}

// throws an exception if the interrupt flag is set
void Worker::throwIfInterrupt ()
{
    if( interruptFlag_ == true)
        throw Interrupt();
}


void Worker::onGoFromService (InputParametersRS input)
{
    try {

        doWork( input);

    } catch ( Interrupt & ) {
        // std::cerr << "*** Worker interrupted ***\n";
        dbg(1) << "RS interrupted\n";
        emit interrupt ();
    } catch ( std::runtime_error & e) {
        std::cerr << "Worker runtime-exception: " << e.what() << "\n";
        emit error( QString("runtime-exception: %1").arg(e.what()));
    } catch ( std::exception & e) {
        std::cerr << "Worker std-exception: " << e.what() << "\n";
        emit error( QString("std-exception: %1").arg(e.what()));
    } catch ( std::string & s) {
        std::cerr << "Worker exception: " << s << "\n";
        emit error( QString("exception: %1").arg(s.c_str()));
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

void Worker::doWorkOld (InputParametersRS input)
{
    ResultsRS r;
    // for null input emit null output
    if( input.isNull) {
        r.status_ = ResultsRS::NullInput;
        emit done(r);
        return;
    }

    // initialize the parser if it's not already initialized
    if( ! m_parser)
        m_parser = new FitsParser();

    // if the parser is working on a different filename, reopen it
    if( currentFitsLocation_.uniqueId () != input.fitsLocation.uniqueId ()) {
        bool res = m_parser->loadFile ( input.fitsLocation);
        if( ! res)
            throw std::runtime_error("Could not open file");
        else
            currentFitsLocation_ = input.fitsLocation;
    }

    // get a reference to the fits header
    const FitsParser::HeaderInfo & hdr = m_parser->getHeaderInfo ();

    // check for more input errors now that we have fits header
    input.left = clamp( input.left, 0, hdr.naxis1-1);
    input.right = clamp( input.right, 0, hdr.naxis1-1);
    input.top = clamp( input.top, 0, hdr.naxis2-1);
    input.bottom = clamp( input.bottom, 0, hdr.naxis2-1);
    if( input.left > input.right) std::swap( input.left, input.right);
    if( input.top > input.bottom) std::swap( input.top, input.bottom);

    // prepare results (partial)
    r.input = input; // save reference to (fixed) input
    r.width = r.input.right - r.input.left + 1;
    r.height = r.input.bottom - r.input.top + 1;
    r.totalPixels = r.width * r.height;
    r.status_ = ResultsRS::Partial;
    r.depth = hdr.totalFrames;
    r.currentFrame = r.input.currentFrame;
    throwIfInterrupt();
//    emit progress (r);

    int frame = r.input.currentFrame;

    // get the frame results for the current frame
    r.frames.resize( hdr.totalFrames);
    r.frames[ frame] = computeFrame( input, frame);
    ResultsRS::FrameRes & cfr = r.frames[ frame];
//    cfr = computeForFrame( input, frame);

    // copy out the relevant bits
    r.nanPixels = cfr.nanPixels;
    r.min = cfr.min;
    r.max = cfr.max;
    r.average = cfr.average;
    r.sum = cfr.sum;
    r.rms = cfr.rms;
    r.bkgLevel = cfr.bkgLevel;
    r.sumMinusBkg = cfr.sumMinusBkg;
    r.maxMinusBkg = cfr.maxMinusBkg;
    r.maxPos = cfr.maxPos;

    emit progress( r);

    // initialize timer for reporting progress
    QTime progressTimer;
    progressTimer.restart ();

    // now extract all the other frames
    for( int i = 0 ; i < hdr.totalFrames ; i ++ ) {
        throwIfInterrupt();
        if( i == frame) continue; // skip the current frame, we already did that one
        // compute the frame
        r.frames[i] = computeFrame( input, i);
        r.nFramesComputed = i + 1;
        // report progress every second or so, but not for the last frame...
        if( progressTimer.elapsed () > 1000 && i+1 < hdr.totalFrames) {
            progressTimer.restart ();
            throwIfInterrupt ();
            emit progress( r);
        }
    }

    // we are done
    r.status_ = ResultsRS::Complete;
    r.nFramesComputed = r.depth;
    throwIfInterrupt ();
    emit done (r);
}

void Worker::doWork (InputParametersRS input)
{
    // if the last input is different from this input only in the frame number,
    // then we can re-use some of the results from previous invocation
    bool cacheValid = false;
    if( m_lastInput.fitsLocation.uniqueId() == input.fitsLocation.uniqueId()
            && ! m_lastInput.isNull
            && m_lastInput.bottom == input.bottom
            && m_lastInput.top == input.top
            && m_lastInput.left == input.left
            && m_lastInput.right == input.right)
    {
        cacheValid = true;
    } else {
        cacheValid = false;
        m_currRes = ResultsRS();
        m_currRes.nFramesComputed = 0;
    }

    m_lastInput = input;

    // for null input emit null output
    if( input.isNull) {
        m_currRes.status_ = ResultsRS::NullInput;
        emit done(m_currRes);
        return;
    }

    // initialize the parser if it's not already initialized
    if( ! m_parser)
        m_parser = new FitsParser();

    // if the parser is working on a different filename, reopen it
    if( currentFitsLocation_.uniqueId () != input.fitsLocation.uniqueId ()) {
        bool res = m_parser->loadFile ( input.fitsLocation);
        if( ! res)
            throw std::runtime_error("Could not open file");
        else
            currentFitsLocation_ = input.fitsLocation;
    }

    // get a reference to the fits header
    const FitsParser::HeaderInfo & hdr = m_parser->getHeaderInfo ();

    // check for more input errors now that we have fits header
    input.left = clamp( input.left, 0, hdr.naxis1-1);
    input.right = clamp( input.right, 0, hdr.naxis1-1);
    input.top = clamp( input.top, 0, hdr.naxis2-1);
    input.bottom = clamp( input.bottom, 0, hdr.naxis2-1);
    if( input.left > input.right) std::swap( input.left, input.right);
    if( input.top > input.bottom) std::swap( input.top, input.bottom);


    // prepare results (partial)
    m_currRes.input = input; // save reference to (fixed) input
    m_currRes.width = m_currRes.input.right - m_currRes.input.left + 1;
    m_currRes.height = m_currRes.input.bottom - m_currRes.input.top + 1;
    m_currRes.totalPixels = m_currRes.width * m_currRes.height;
    m_currRes.depth = hdr.totalFrames;
    m_currRes.currentFrame = m_currRes.input.currentFrame;
    throwIfInterrupt();

    int frame = m_currRes.input.currentFrame;

    // get the frame results for the current frame, unless we can retrieve this already
    // from the last time
    m_currRes.frames.resize( hdr.totalFrames);
    if( cacheValid && frame < m_currRes.nFramesComputed) {

    } else {
        m_currRes.frames[ frame] = computeFrame( input, frame);
    }
    ResultsRS::FrameRes & cfr = m_currRes.frames[ frame];

    // copy out the relevant bits
    m_currRes.nanPixels = cfr.nanPixels;
    m_currRes.min = cfr.min;
    m_currRes.max = cfr.max;
    m_currRes.average = cfr.average;
    m_currRes.sum = cfr.sum;
    m_currRes.rms = cfr.rms;
    m_currRes.bkgLevel = cfr.bkgLevel;
    m_currRes.sumMinusBkg = cfr.sumMinusBkg;
    m_currRes.maxMinusBkg = cfr.maxMinusBkg;
    m_currRes.maxPos = cfr.maxPos;

    { // total flux
        double bmin = 0.0, bmaj = 0.0;
        bool ok = true;
        if( ok) bmin = hdr.bmin.toDouble( & ok);
        if( ok) bmaj = hdr.bmaj.toDouble( & ok);
        if( ok) {
            m_currRes.beamArea = bmin * bmaj * 1.13309003545679845240692073642916670254
                    / (hdr.cdelt1 * hdr.cdelt2);
            m_currRes.beamArea = fabs( m_currRes.beamArea);
            m_currRes.totalFluxDensity = m_currRes.sum / m_currRes.beamArea;
            m_currRes.aboveBackground = m_currRes.sumMinusBkg / m_currRes.beamArea;
        } else {
            m_currRes.totalFluxDensity = std::numeric_limits<double>::quiet_NaN();
            m_currRes.aboveBackground = std::numeric_limits<double>::quiet_NaN();
            m_currRes.beamArea = std::numeric_limits<double>::quiet_NaN();
        }
    }

    // if the results are already complete, we are done
    if( m_currRes.nFramesComputed == hdr.totalFrames) {
        m_currRes.status_ = ResultsRS::Complete;
        emit done (m_currRes);
        return;
    }

    // otherwise we'll have to compute the remaining frames, but in any case,
    // report a partial result right now with the current frame
    m_currRes.status_ = ResultsRS::Partial;
    emit progress( m_currRes);

    // initialize timer for reporting progress
    QTime progressTimer;
    progressTimer.restart ();

    // now extract all the other frames
    int startFrame = m_currRes.nFramesComputed;
    for( int i = startFrame ; i < hdr.totalFrames ; i ++ ) {
        throwIfInterrupt();
        if( i == frame) continue; // skip the current frame, we already did that one
        // compute the frame
        m_currRes.frames[i] = computeFrame( input, i);
        m_currRes.nFramesComputed = i + 1;
        // report progress every second or so, but not for the last frame...
        if( progressTimer.elapsed () > 1000 && i+1 < hdr.totalFrames) {
            progressTimer.restart ();
            throwIfInterrupt ();
            emit progress( m_currRes);
        }
    }

    // we are done
    m_currRes.status_ = ResultsRS::Complete;
    m_currRes.nFramesComputed = m_currRes.depth;
    throwIfInterrupt ();
    emit done (m_currRes);
}

ResultsRS::FrameRes
Worker::computeFrame(InputParametersRS input, int frame)
{
    ResultsRS::FrameRes r;

    // prepare results (partial)
    r.width = input.right - input.left + 1;
    r.height = input.bottom - input.top + 1;
    r.totalPixels = r.width * r.height;

    // get the four corner average
    double cornerSum = 0.0; int cornerCount = 0;
    {
        double val = m_parser-> src( input.left, input.top, frame);
        if( ! isnan(val)) { cornerCount ++; cornerSum += val; }
    }{
        double val = m_parser-> src( input.left, input.bottom, frame);
        if( ! isnan(val)) { cornerCount ++; cornerSum += val; }
    }{
        double val = m_parser-> src( input.right, input.top, frame);
        if( ! isnan(val)) { cornerCount ++; cornerSum += val; }
    }{
        double val = m_parser-> src( input.right, input.bottom, frame);
        if( ! isnan(val)) { cornerCount ++; cornerSum += val; }
    }
    if( cornerCount > 0) {
        r.bkgLevel = cornerSum / cornerCount;
    } else {
        r.bkgLevel = std::numeric_limits<double>::quiet_NaN();
    }

//    const char * ptr = 0;
    double val;
    r.min = r.max = std::numeric_limits<double>::quiet_NaN();
    r.nanPixels = 0;
    r.sum = 0;
    for( int y = input.top ; y <= input.bottom ; y ++ ) {
        throwIfInterrupt();
        for( int x = input.left ; x <= input.right ; x ++ ) {
//            ptr = m_parser-> srcRaw ( x, y, frame);
//            val = m_parser-> raw2double( ptr);
            val = m_parser-> src( x, y, frame);
            if( std::isnan(val)) {
                r.nanPixels ++;
            } else {
                r.sum += val;
                if( isnan(r.min) || val < r.min) r.min = val;
                if( isnan(r.max) || val > r.max) {
                    r.max = val;
                    r.maxPos = QPoint( x, y);
                }
            }
        }
    }
    r.average = r.sum / (r.totalPixels - r.nanPixels);

    r.sumMinusBkg = r.sum - r.bkgLevel * (r.totalPixels - r.nanPixels);
    r.maxMinusBkg = r.max - r.bkgLevel;

    double sumSq = 0;
    for( int y = input.top ; y <= input.bottom ; y ++ ) {
        throwIfInterrupt();
        for( int x = input.left ; x <= input.right ; x ++ ) {
//            ptr = m_parser-> srcRaw ( x, y, frame);
//            val = m_parser-> raw2double( ptr);
            val = m_parser-> src( x, y, frame);
            if( ! std::isnan(val)) {
                sumSq += (val - r.average) * (val - r.average);
            }
        }
    }
    r.rms = sqrt( sumSq / (r.totalPixels - r.nanPixels - 1));

    return r;
}

void Manager::onProgress (ResultsRS r)
{
//    std::cerr << "Manager::onProgress: " << r.toString() << "\n";
    emit progress (r);
}

void Manager::onDone (ResultsRS r)
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




}; // namespace PSRVC
