
#include <iostream>
#include <QTime>
#include <QMetaType>
#include <QDir>
#include "common.h"
#include "FitsParser.h"
#include "RaiCache.h"
#include "Gaussian2dFitService.h"
#include "Optimization/LMGaussFitter2d.h"
//#include "Optimization/TAGauss2dFitter.h"
#include "Optimization/LBTAGauss2dFitter.h"
#include "Optimization/HeuristicGauss2dFitter.h"

namespace Gaussian2dFitService {

// register metatypes with QT so that we can pass Results and InputParameters via signals/slots
static bool registerMetaTypes() {
    std::cerr << "Registering metatype\n";
    std::cerr << "Gaussian2dFitService::Results = "
              << qRegisterMetaType<Gaussian2dFitService::ResultsG2dFit>("ResultsG2dFit") << "\n";
    std::cerr << "Gaussian2dFitService::InputParameters = "
              << qRegisterMetaType<Gaussian2dFitService::InputParametersG2dFit>("InputParametersG2dFit")
              << "\n";
    return true;
}
static bool initOnce_ = registerMetaTypes ();

// private exception class
class Interrupt
{
};


ResultsG2dFit::ResultsG2dFit()
{
//    std::cerr << "Results constructor from thread " << QThread::currentThread () << "\n";
    status_ = Error;
    chisq = -1;
    rms = 0;
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
    connect ( & worker_, SIGNAL( progress(ResultsG2dFit)),
              this, SLOT(onProgress(ResultsG2dFit)));
    connect ( & worker_, SIGNAL( done(ResultsG2dFit)),
              this, SLOT(onDone(ResultsG2dFit)));
    connect ( & worker_, SIGNAL( error(QString)),
              this, SLOT(onError(QString)));
    connect ( & worker_, SIGNAL( interrupt()),
              this, SLOT(onInterrupt()));
    connect ( this, SIGNAL(privateGo(InputParametersG2dFit)),
              & worker_, SLOT(onGoFromService(InputParametersG2dFit)));


    // move the worker to a separate thread
    worker_.moveToThread( & workerThread_);
    workerThread_.start ();
}


void Manager::request (InputParametersG2dFit input)
{
//    std::cerr << "Manager::request " << input.toString() << "\n";

    // if worker is running, ask for interrupt
    if( workerBusy_) {
        interruptRequested_ = true;
    }
    pendingInput_ = input;
//    if( input.isNull ) {
//        hasPendingInput_ = false;
//    } else {
//        hasPendingInput_ = true;
//        schedulePending ();
//    }

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
    , m_parser( 0)
{
    m_parser = 0;
}

Worker::~Worker()
{
    delete m_parser;
}

// throws an exception if the interrupt flag is set
void Worker::checkForInterrupts ()
{
    if( interruptFlag_ == true)
        throw Interrupt();
}


void Worker::onGoFromService (InputParametersG2dFit input)
{
    try {
        doWork( input);
    } catch ( Interrupt & ) {
        // std::cerr << "*** Worker interrupted ***\n";
        emit interrupt ();
    } catch ( std::runtime_error & e) {
        dbg(0) << "Worker runtime-exception: " << e.what() << "\n";
        emit error( QString("runtime-exception: %1").arg(e.what()));
    } catch ( std::exception & e) {
        dbg(0) << "Worker std-exception: " << e.what() << "\n";
        emit error( QString("std-exception: %1").arg(e.what()));
    } catch ( std::string & s) {
        dbg(0) << "Worker exception: " << s << "\n";
        emit error( QString("exception: %1").arg(s.c_str()));
    } catch ( QString & s) {
        dbg(0) << "Worker exception: " << s << "\n";
        emit error( QString("exception: %1").arg(s));
    } catch ( char * s) {
        dbg(0) << "Worker exception: " << s << "\n";
        emit error( QString("exception: %1").arg(s));
    } catch (...) {
        dbg(0) << "Worker uncaught exception...\n";
        dbg(1) << "Worker uncaught exception\n";
        emit error ( "Uncaught exception");
//        throw;
    }
}

void Worker::doWork (InputParametersG2dFit input)
{
    ResultsG2dFit res;
    res.input = input;

    // handle null input right off the bat
    if( input.isNull || input.nGaussians <= 0) {
        res.status_ = ResultsG2dFit::Complete;
        res.chisq = -1;
        emit done( res);
        return;
    }

    // initialize the parser if it's not already initialized
    if( ! m_parser)
        m_parser = new FitsParser();
    // if the parser is working on a different filename, reopen it
    if( currentFitsLocation_.uniqueId () != input.fitsLocation.uniqueId()) {
        bool res = m_parser->loadFile ( input.fitsLocation);
        if( ! res)
            throw std::runtime_error("Could not open file");
        imageWidth = m_parser->getHeaderInfo().naxis1;
        imageHeight = m_parser->getHeaderInfo().naxis2;
        currentFitsLocation_ = input.fitsLocation;
    }

    // remember the requested frame (for gauss fitting)
    frame = input.currentFrame;
    // make sure frame is valid
    if( frame < 0 || frame >= m_parser->getHeaderInfo().totalFrames)
        throw std::runtime_error( "Gaussian2dFitService::bad frame");

    // pre-cache the frame if not already cached
    m_parser->cacheFrame( frame);

    // prepare results (partial)
    res.input = input; // save reference to input
    res.status_ = ResultsG2dFit::Partial;
    res.params.resize( res.input.nGaussians * 6 + 1, 0.0);

    // initialize timer for reporting progress
    QTime progressTimer; progressTimer.restart ();
    bool firstTime = true;

    int iteration = 0;

    int dof;
    // ----------------------------------------------------------------------
    // run the heuristic fitter
    // ----------------------------------------------------------------------
    Optimization::HeuristicGauss2dFitter < Worker > hFitter;
    hFitter.setImage( this);
    hFitter.setNumGaussians( input.nGaussians);
    hFitter.setRect(
                input.left, input.right,
                input.top, input.bottom);
    dof = hFitter.calculateDof();
    if( dof < 0) dof = 1;

    // start fitting
    iteration = 0;
    while( true) {
        iteration ++;
        checkForInterrupts ();
        bool fitDone = hFitter.iterate();

        // report progress:
        //   - after 100ms if it's the first time
        //   - after that every 1000ms
        bool reportProgress = progressTimer.elapsed () > 1000 || (progressTimer.elapsed () > 100 && firstTime);
        bool copyResults = fitDone || reportProgress;
        if( copyResults) {
            dbg(1) << "Copying results\n";
            res.params = hFitter.getResults();
            res.chisq = hFitter.getChiSq();
            res.rms = sqrt( res.chisq / dof);
            dbg(1) << "Chi^2 = " << res.chisq << "\n";
            if( int(res.params.size()) != input.nGaussians * 6 + 1)
                throw std::runtime_error("Gaussian2dFitService::Worker - mismatch in result size");
        }

        if( reportProgress) {
            firstTime = false;
            progressTimer.restart ();
            res.status_ = res.Partial;
            res.info = QString( "Heuristics #%1").arg(iteration);
            checkForInterrupts ();
            emit progress( res);
        }

        if( fitDone) break;
    }

    dbg(1) << "Heuristic fitter produced chiSquare = " << hFitter.getChiSq() << "\n";


    // ----------------------------------------------------------------------
    // setup the LBTA fitter
    // ----------------------------------------------------------------------
    Optimization::LBTAGauss2dFitter < Worker > taFitter;
    taFitter.setImage( this);
    taFitter.setNumGaussians( input.nGaussians);
    taFitter.setRect(
                input.left, input.right,
                input.top, input.bottom);
    taFitter.setInitialParams( hFitter.getResults());

    // start fitting
    iteration = 0;
    while( true) {
        iteration ++;
        checkForInterrupts ();
//        dbg(1) << "Calling taFitter.iterate()\n";
        bool taDone = taFitter.iterate();

        // report progress:
        //   - after 100ms if it's the first time
        //   - after that every 1000ms
        bool reportProgress = progressTimer.elapsed () > 1000 || (progressTimer.elapsed () > 100 && firstTime);
        bool copyResults = taDone || reportProgress;
        if( copyResults) {
            dbg(1) << "Copying results\n";
            res.params = taFitter.getResults();
            res.chisq = taFitter.getChiSq();
            res.rms = sqrt( res.chisq / dof);
            dbg(1) << "Chi^2 = " << res.chisq << "\n";
            if( int(res.params.size()) != input.nGaussians * 6 + 1)
                throw std::runtime_error("Gaussian2dFitService::Worker - mismatch in result size");
        }

        if( reportProgress) {
            firstTime = false;
            progressTimer.restart ();
            res.status_ = res.Partial;
            res.info = QString( "TA #%1").arg(iteration);
            checkForInterrupts ();
            emit progress( res);
        }

        if( taDone) break;
    }

    dbg(1) << "LBTA fitter produced chiSquare = " << taFitter.getChiSq() << "\n";


    // ----------------------------------------------------------------------
    // setup up the lev-mar fitter
    // ----------------------------------------------------------------------
    Optimization::LMGaussFitter2d < Worker > lmfitter;
    lmfitter.setImage( this);
    lmfitter.setNumGaussians( input.nGaussians);
    lmfitter.setRect(
                input.left, input.right,
                input.top, input.bottom);
    lmfitter.setInitialParams( taFitter.getResults());

    // start fitting
    iteration = 0;
    while( true) {
        iteration ++;
        checkForInterrupts ();
        dbg(1) << "Calling g2fitter.iterate()\n";
        Optimization::LMGaussFitter2d<Worker>::IterateStatus status = lmfitter.iterate();

        // report progress:
        //   - after 100ms if it's the first time
        //   - after that every 1000ms
        bool reportProgress = progressTimer.elapsed () > 1000 || (progressTimer.elapsed () > 100 && firstTime);
        bool copyResults = ( status == lmfitter.Done) || reportProgress;
        if( copyResults) {
            dbg(1) << "Copying results\n";
            res.params = lmfitter.getResults();
            res.chisq = lmfitter.getChiSq();
            res.rms = sqrt( res.chisq / dof);
            dbg(1) << "Chi^2 = " << res.chisq << "\n";
            if( int(res.params.size()) != input.nGaussians * 6 + 1)
                throw std::runtime_error("Gaussian2dFitService::Worker - mismatch in result size");
        }

        if( reportProgress) {
            firstTime = false;
            progressTimer.restart ();
            res.status_ = res.Partial;
            res.info = QString( "LevMar #%1").arg(iteration);
            checkForInterrupts ();
            emit progress( res);
        }

        if( status == lmfitter.Done) break;
    }

    dbg(1) << "LevMar fitter produced chiSquare = " << lmfitter.getChiSq() << "\n";


    res.status_ = res.Complete;
    res.info = "All done.";
    checkForInterrupts ();
    emit done (res);
}

void Manager::onProgress (ResultsG2dFit r)
{
//    std::cerr << "Manager::onProgress: " << r.toString() << "\n";
    emit progress (r);
}

void Manager::onDone (ResultsG2dFit r)
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

double Worker::imageValue(int x, int y)
{
    return m_parser->src( x, y, frame);
}



}; // namespace Gaussian2dFitService
