#pragma once

#include "Gauss1d.h"
#include <QObject>
#include <QVector>
#include <QThread>
#include <QImage>
#include <QTextStream>

#include <stdexcept>

namespace Gaussian1dFitService
{
// input parameters for extraction
struct InputParametersG1dFit {
    InputParametersG1dFit()
    {
        stamp = - 1;
        nGaussians = 0;
        poly = 0;
        left = 0, right = 0;
        isNull = true;
        randomHeuristicsEnabled = true;
    }

    // number of gaussians to extract
    int nGaussians;

    // polynomial degree
    int poly;

    // data
    std::vector < double > data;

    // initial guess (if empty, it means no initial guess)
    std::vector < double > initGuess;

    // whether to perform random heuristics or not
    bool randomHeuristicsEnabled;

    // over which portion of data to optimize
    int left, right;

    // whether the request is null
    bool isNull;

    // some user specified id flag, in case the results need to be synchronized
    qint64 stamp;

    QString
    toString()
    {
        QString s;
        QTextStream o( & s );
        o << "data:" << data.size()
          << " " << left << " .. " << right
          << " isNull:" << isNull
          << " ng:" << nGaussians
          << " poly:" << poly;
        return s;
    }
};

// output type for extraction
struct ResultsG1dFit {
    enum Status { Error, Empty, Partial, Complete };

    // constructor
    ResultsG1dFit();

    // copy constructor
//    Results( const Results &);

    // returns the status of completion: Error, Partial or Complete
    Status
    status() { return status_; }

    // copy of the input parameters
    InputParametersG1dFit input;

    // the fitted gaussian parameters, concatenated together, including the polynomial terms
    // the total number is nGaussians * 3 + poly
    // each 3-tuple is:
    //   center, amplitude, variance
    // angle is in degrees, counter-clockwise
    std::vector < double > params;

    // chisq of the solution
    double diffSq, rms;

    // some extra progress string
    QString info;

    // debugging stuff
    QString
    toString()
    {
        QString s;
        QTextStream o( & s );
        o << "s:" << status() <<
            " diffSq:" << diffSq << " rms:" << rms <<
            " params:[";
        for ( size_t i = 0 ; i < params.size() ; i++ ) {
            o << params[i];
            if ( i < params.size() - 1 ) { o << ","; }
        }
        o << "] inp:" << input.toString();
        o << " info: " << info;
        return s;
    }

protected:

    Status status_;

    friend class Worker;
};

class Worker : public QObject
{
    Q_OBJECT

public:

    /// required interface for the various XXXGaussFitter1d
//    int dataSize; // size of data
//    std::vector < Optimization::RangeParam > ranges;
//    double
//    getData( int x );       // access to the data

//    std::vector < double > data; // raw access to the data

protected:

    // constructor, only really meant to be used internally by the Manager class
    Worker( bool & interruptFlagRef );
    ~Worker();
    friend class Manager;

public slots:

    void onGoFromService( InputParametersG1dFit );

signals:

    // emitted periodically with results
    void progress( ResultsG1dFit );

    // emitted when done
    void done( ResultsG1dFit );

    // called when some error occurred
    void error( QString );

    // called when worker interrupted job
    void
    interrupt();

protected:

    bool & interruptFlag_;
    void
    checkForInterrupts();

    void doWork( InputParametersG1dFit );
};

class Manager : public QObject
{
    Q_OBJECT

public:

    // constructor
    explicit
    Manager( QObject * parent = 0 );

    // test method
//    void go();
    virtual ~Manager();

signals:

    /// emitted when results (partial or complete) are available
    void
    results( Gaussian1dFitService::ResultsG1dFit );

    // emitted when complete profile is available
    void
    done( Gaussian1dFitService::ResultsG1dFit );

    // emitted when partial result is ready
    void
    progress( Gaussian1dFitService::ResultsG1dFit );

    // emitted when some sort of error occurs
    void
    error( QString msg );

    void privateGo( InputParametersG1dFit );

public slots:

    bool request( InputParametersG1dFit );

protected slots:

    void onProgress( ResultsG1dFit );
    void onDone( ResultsG1dFit );
    void onError( QString );
    void
    onInterrupt();

    // implementation details:
    // -----------------------

protected:

    bool schedulePending();

    // worker & its thread
    Worker worker_;
    QThread workerThread_;
    bool workerBusy_;

    // shared variable to indicate to worker if it should interrupt itself
    bool interruptRequested_;

    // pending work
    InputParametersG1dFit pendingInput_;
    bool hasPendingInput_;
};
}
