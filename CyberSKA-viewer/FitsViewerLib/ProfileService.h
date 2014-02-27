#ifndef PROFILESERVICE_H
#define PROFILESERVICE_H

#include <stdexcept>

#include <QObject>
#include <QVector>
#include <QThread>
#include <QImage>
#include <QTextStream>
#include <memory>

#include "common.h"
#include "RaiCache.h"
#include "FitsParser.h"


//class FitsParser;

namespace ProfileService {


// input parameters for computing a profile
struct InputParameters {
    InputParameters()
        : fitsLocation( FitsFileLocation::invalid())
    {
        stamp = -1;
    }

    // file location
    FitsFileLocation fitsLocation;
    // parameters to construct our own cache controller (in our own thread)
    RaiLib::Cache::Controller::InitParameters ccinit;
    // which position to extract (x,y,z)
    QVector<qint64> position;
    // along which axis to extract, 0 = X, 1 = Y, 2 = Z
    int direction;
    // range of extraction
    qint64 minPos, maxPos;
    // some user specified id flag, in case the results need to be synchronized
    qint64 stamp;

    QString toString() {
        QString s; QTextStream o( & s);
        o << fitsLocation.getLocalFname () << " " << direction << " ";
        for( int i = 0 ; i < position.size () ; i ++ )
            o << position[i] << ",";
        return s;
    }
};

// computed profile
struct Results {
    enum Status { Error, Partial, Complete };

    // constructor
    Results();
    // copy constructor
//    Results( const Results &);

    // returns the status of completion: Error, Partial or Complete
    Status status() { return status_; }
    /**
     * @brief bitpix returns the type of data
     * @return
     */
    qint8 bitpix() const;
//    /**
//     * @brief returns the data at position index converted to requested type
//     */
//    template <typename T> T get( quint64 index);
    /**
     * @brief total returns the total number of entries to expect once the results are complete
     *        (only if status is Partial or Complete)
     * @return
     */
    quint64 total() { return total_; }
    // returns number of extracted entries (only if status is Partial or Complete)
    // if status is Complete, then total() is equal to available()
    quint64 available() { return rawData_.size (); }
    // returns reference to raw data
    const QList<double> & rawData() const { return rawData_; }

    // copy of the input parameters
    InputParameters input;

    // debugging stuff
    QString toString() { return QString("s:%1 tl:%2 av:%3 inp:%4")
                .arg(status ())
                .arg(total ())
                .arg(available ())
                .arg(input.toString());  }
protected:
    // TODO: optimize for different BITPIX
    // right now this is hard-wired for bitpix=64 (double), since the class uses
    // the inefficient FITS parser...
    QList<double> rawData_;
    Status status_;
    quint64 total_;

    friend class Worker;

};

class Worker : public QObject
{
    Q_OBJECT

protected:
    // constructor, only really meant to be used internally by the Manager class
    Worker( bool & interruptFlagRef);
    ~Worker();
    friend class Manager;

public slots:
    void onGoFromService(InputParameters);

signals:
    // emitted periodically with results
    void progress( Results);
    // emitted when done
    void done( Results);
    // called when some error occurred
    void error( QString);
    // called when worker interrupted job
    void interrupt();

protected:
    bool & interruptFlag_;
    void checkForInterrupts();

    void doWorkXY( InputParameters);
    void doWorkZ( InputParameters);

    // info about the parser
    // FitsParser * m_parser;
//    std::shared_ptr<FitsParser> m_parser;
    FitsParser m_parser;

    QSharedPointer < RaiLib::Cache::Controller > m_cc;
    RaiLib::Cache::FileHandle m_handle;
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

    // emitted when complete profile is available
    void done( ProfileService::Results);
    // emitted when partial result is ready
    void progress( ProfileService::Results);
    // emitted when some sort of error occurs
    void error( QString msg);

    void privateGo( InputParameters);

public slots:

    void request( InputParameters);

protected slots:

    void onProgress( Results);
    void onDone( Results);
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
    InputParameters pendingInput_;
    bool hasPendingInput_;

};

}; // namespace ProfileService

#endif // PROFILESERVICE_H
