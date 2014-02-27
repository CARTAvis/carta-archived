#ifndef GAUSSIAN2DFITSERVICE_H
#define GAUSSIAN2DFITSERVICE_H

#include <stdexcept>

#include <QObject>
#include <QVector>
#include <QThread>
#include <QImage>
#include <QTextStream>

#include "common.h"
#include "RaiCache.h"
#include "RegionStatsService.h"
//#include "Optimization/GaussFitter2d.h"

class FitsParser;

namespace Gaussian2dFitService {

// input parameters for extraction
struct InputParametersG2dFit {
    InputParametersG2dFit() {
//        stamp = -1;
        nGaussians = 0;
        left = 0, right = 0, top = 0, bottom = 0;
        currentFrame = 0;
        isNull = true;
    }

    // to make things simpler we just re-use the region stats service's parameters
    // TODO: maybe we need to fix this to make the gaussian fit not depend on
    // region stats service.
//    RegionStatsService::InputParametersRS rsInput;

    // number of gaussians to extract
    int nGaussians;
    // which rectangle to extract
    int left, right, top, bottom;
    int currentFrame;
    bool isNull;
    // file location
    FitsFileLocation fitsLocation;


    // some user specified id flag, in case the results need to be synchronized
//    qint64 stamp;

    QString toString() {
        QString s; QTextStream o( & s);
        o << fitsLocation.getLocalFname () << " "
          << left << ".." << right << " " << top << ".." << bottom
          << " null: " << isNull
          << " ng:" << nGaussians;
        return s;
    }
};

// output type for extraction
struct ResultsG2dFit {
    enum Status { Error, Partial, Complete };

    // constructor
    ResultsG2dFit();
    // copy constructor
//    Results( const Results &);

    // returns the status of completion: Error, Partial or Complete
    Status status() { return status_; }

    // copy of the input parameters
    InputParametersG2dFit input;

    // the fitted gaussian parameters, concatenated together, including the constant term
    // the total number is nGaussians * 6 + 1
    // each 6-tuple is:
    //   height, x-center, y-center, fwmh, ratio, angle
    // angle is in degrees, counter-clockwise
    std::vector<double> params;
    // chisq of the solution
    double chisq, rms;
    // some extra progress string
    QString info;

    // debugging stuff
    QString toString() {
        QString s; QTextStream o( & s);
        o << "s:" << status() <<
             " chi:" << chisq <<
             " vals:[";
        for( size_t i = 0 ; i < params.size() ; i ++ ) {
            o << params[i];
            if( i < params.size() - 1) o << ",";
        }
        o << " inp:" << input.toString();
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
    /// required interface for *GaussFitter2d<> templates
    int imageWidth;
    int imageHeight;
    double imageValue( int x, int y);
    /// values needed by interface to GaussFitter2d
    int frame;

protected:
    // constructor, only really meant to be used internally by the Manager class
    Worker( bool & interruptFlagRef);
    ~Worker();
    friend class Manager;

public slots:
    void onGoFromService(InputParametersG2dFit);

signals:
    // emitted periodically with results
    void progress( ResultsG2dFit);
    // emitted when done
    void done( ResultsG2dFit);
    // called when some error occurred
    void error( QString);
    // called when worker interrupted job
    void interrupt();

protected:
    bool & interruptFlag_;
    void checkForInterrupts();

    void doWork( InputParametersG2dFit);

    // info about the parser
    FitsParser * m_parser;
    FitsFileLocation currentFitsLocation_;

//    Optimization::GaussFitter2d < Worker > m_g2Fitter;
};

class Manager : public QObject
{
    Q_OBJECT
public:
    // constructor
    explicit Manager( QObject *parent = 0);

    // test method
    void go();


signals:

    // emitted when complete profile is available
    void done( Gaussian2dFitService::ResultsG2dFit);
    // emitted when partial result is ready
    void progress( Gaussian2dFitService::ResultsG2dFit);
    // emitted when some sort of error occurs
    void error( QString msg);

    void privateGo( InputParametersG2dFit);

public slots:

    void request( InputParametersG2dFit);

protected slots:

    void onProgress( ResultsG2dFit);
    void onDone( ResultsG2dFit);
    void onError( QString);
    void onInterrupt();

    // implementation details:
    // -----------------------


protected:

    void schedulePending();

    // worker & its thread
    Worker worker_;
    QThread workerThread_;
    bool workerBusy_;
    // shared variable to indicate to worker if it should interrupt itself
    bool interruptRequested_;
    // pending work
    InputParametersG2dFit pendingInput_;
    bool hasPendingInput_;

};

}; // namespace Gaussian2dFitService


#endif // GAUSSIAN2DFITSERVICE_H
