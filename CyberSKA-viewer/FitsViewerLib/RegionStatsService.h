#ifndef REGIONSTATSSERVICE_H
#define REGIONSTATSSERVICE_H

#include <stdexcept>

#include <QObject>
#include <QVector>
#include <QThread>
#include <QImage>
#include <QTextStream>

#include "common.h"
#include "RaiCache.h"

class FitsParser;

namespace RegionStatsService {

// input parameters for extraction
struct InputParametersRS {
    InputParametersRS() {
//        stamp = -1;
        left = 0, right = 0, top = 0, bottom = 0;
        currentFrame = 0;
        isNull = true;
    }

    // file location
    FitsFileLocation fitsLocation;
    // which rectangle to extract
    int left, right, top, bottom;
    int currentFrame;
    bool isNull;

    QString toString() {
        QString s; QTextStream o( & s);
        o << fitsLocation.getLocalFname () << " "
          << left << " " << right << " " << top << " " << bottom
          << " null: " << isNull;
        return s;
    }
};

// output type for extraction
struct ResultsRS {
    enum Status { Error, Partial, Complete, NullInput };

    // constructor
    ResultsRS();
    // copy constructor
//    Results( const Results &);

    // returns the status of completion
    Status status() { return status_; }

    // copy of the input parameters
    InputParametersRS input;

    // computed values for the current frame
    int width, height, depth, currentFrame;
    qint64 totalPixels, nanPixels;
    double min, max, average, sum, rms, bkgLevel, sumMinusBkg, maxMinusBkg;
    double beamArea, totalFluxDensity, aboveBackground;
    QPoint maxPos; // position of maximum, image coordinates (0-based)

    // computed values for all other frames
    struct FrameRes {
        qint64 totalPixels, nanPixels;
        int width, height;
        double min, max, average, sum, rms, bkgLevel, sumMinusBkg, maxMinusBkg;
        QPoint maxPos; // position of maximum, image coordinates (0-based)
    };
    QVector<FrameRes> frames;

    // number of frames computed
    int nFramesComputed;

    // debugging stuff
    QString toString() { return QString("s:%1 %2x%3 inp:%4")
                .arg(status ())
                .arg(width).arg(height)
                .arg(input.toString());  }
protected:

    friend class Worker;

    Status status_;

};

class Worker : public QObject
{
    Q_OBJECT

protected:
    // constructor, only really meant to be used internally by the Manager class
    Worker( bool & interruptFlagRef);
    friend class Manager;

public slots:
    void onGoFromService(InputParametersRS);

signals:
    // emitted periodically with results
    void progress( ResultsRS);
    // emitted when done
    void done( ResultsRS);
    // called when some error occurred
    void error( QString);
    // called when worker interrupted job
    void interrupt();

protected:
    bool & interruptFlag_;
    void throwIfInterrupt();

    void doWork( InputParametersRS);
    void doWorkOld( InputParametersRS);

    // compute stats for a single frame
    ResultsRS::FrameRes computeFrame( InputParametersRS, int frame);

    // cached info from the previous job
    ResultsRS m_currRes;
    InputParametersRS m_lastInput;


    // info about the parser
    FitsParser * m_parser;
    FitsFileLocation currentFitsLocation_;



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

    // emitted when complete result is available
    void done( RegionStatsService::ResultsRS);
    // emitted when partial result is ready
    void progress( RegionStatsService::ResultsRS);
    // emitted when some sort of error occurs
    void error( QString msg);

    void privateGo( InputParametersRS);

public slots:

    void request( InputParametersRS);

protected slots:

    void onProgress( ResultsRS);
    void onDone( ResultsRS);
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
    InputParametersRS pendingInput_;
    bool hasPendingInput_;

};

}; // namespace RegionStatsService

#endif // REGIONSTATSSERVICE_H


