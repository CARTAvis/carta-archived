#ifndef RAICACHERSERVICE_H
#define RAICACHERSERVICE_H

#include <QMutex>
#include <QThread>
#include <deque>

#include "RaiCache.h"



namespace RaiLib { namespace Cache {


class RaiCacherService;

/// the worker thread class
class CacherWorker : public QThread
{
    Q_OBJECT

public:
    CacherWorker( RaiCacherService & rcs);
    RaiCacherService & m_rcs;
    bool m_interruptFlag;

    virtual void run ();

    /// internal class for CacherWorker for signalling interrupt
    struct Interrupt {
        Interrupt( const FitsFileLocation & floc)
            : m_floc( FitsFileLocation::invalid())
        {
            m_floc = floc;
            m_hasJob = true;
        }
        Interrupt()
            : m_floc( FitsFileLocation::invalid())
        {
            m_hasJob = false;
        }

        FitsFileLocation m_floc;
        bool m_hasJob;
    };

    /// check for interrupt, if present, throw an Interrupt exception
    void checkForInterruptAndThrow();

    /// check for interrupt, if present, throw an Interrupt exception with the
    /// job attached to it, so that it can be re-queued
    void checkForInterruptAndThrow( const FitsFileLocation &);

    /// the actual cache generator
//    void generateCache( const FitsFileLocation & floc);

    Q_DISABLE_COPY( CacherWorker)

};


/// provides a cacher service for generating cache in background (in a separate thread)
class RaiCacherService
{
public:
    /// Constructor takes arguments
    /// ccinit - input parameters to construct cache
    /// maxMem - maximum memory to use (for allocating internal buffers)
    RaiCacherService(
            const RaiLib::Cache::Controller::InitParameters & ccinit,
            qint64 maxMem = 1L * 1024 * 1024 * 1024 );

    /// ask the service to process the file ASAP, i.e. interrupting the current
    /// caching and switching to this file
    void doNow( const FitsFileLocation & );
    /// schedule processing of this file in the future
    void doLater( const FitsFileLocation &);
    /// get status
    QString getStatus();

    /// destructor
    ~RaiCacherService();

protected:
    friend class CacherWorker;

    std::deque< FitsFileLocation> m_jobs;

    QSharedPointer<CacherWorker> m_worker;
    QSharedPointer<RaiLib::Cache::Controller> m_cc;
    qint64 m_maxMem;
    QMutex m_queueMutex;

    // to satisfy requirements of forward declaring worker and using it with
    // shared pointer, we need to implement non-inline destructor and copies
    Q_DISABLE_COPY( RaiCacherService)
//    RaiCacherService( const RaiCacherService &);
//    RaiCacherService & operator = ( const RaiCacherService &);
//    RaiCacherService();



};

}; }; // namespace RaiLib::Cache


/*
 * worker algorithm
 *
 * loop start
 *   try {
 *     get the top job from queue
 *     if this was unsuccessful (queue empty), sleep a bit, then continue from top
 *     process job, periodically checking for interrupts
 *     continue at the top
 *   } catch {
 *     if interrupted put the current (unfinished) job at the back of the queue and
 *     report other exceptions to the log
 *   }
 *   continue from the top
 * loop end
 *
 * doNow() algorithm
 *   put a job at the beginning of the queue
 *   indicate interrupt
 *
 * doLater() algorithm
 *   put a job at the end of the queue
 */


//void test() {
//    // TODO: extract
//    RaiLib::Cache::Controller::InitParameters init =
//            RaiLib::Cache::Controller::InitParameters::fromIniFile ( "x.ini");
//    QSharedPointer<RaiCacherService> cs( new RaiCacherService( init));
//    cs-> doNow ( FitsFileLocation::fromLocal ( "/scratch/cube260M.fits"));
//    while(1) {
//        std::cout << "Status :\n";
//        std::cout << cs-> getStatus ();
//        qsleep(5);
//    }
//}


#endif // RAICACHERSERVICE_H
