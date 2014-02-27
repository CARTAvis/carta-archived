#include <QObject>
#include <QThread>
#include "RaiCacherService.h"
#include "RaiCacher.h"


namespace RaiLib { namespace Cache {

RaiCacherService::RaiCacherService(
        const RaiLib::Cache::Controller::InitParameters &ccinit,
        qint64 maxMem)
{
    m_cc = QSharedPointer<RaiLib::Cache::Controller> (
                new RaiLib::Cache::Controller( ccinit));
    m_maxMem = std::max<qint64>( 5L * 1024 * 1024, maxMem);
    m_worker = QSharedPointer<CacherWorker>( new CacherWorker( * this));
    m_worker-> start ( QThread::LowestPriority);

}

void RaiCacherService::doNow(const FitsFileLocation & loc)
{
    m_queueMutex.lock ();
    m_jobs.push_front ( loc);
    m_queueMutex.unlock ();
    m_worker-> m_interruptFlag = true;
}

void RaiCacherService::doLater(const FitsFileLocation & loc)
{
    m_queueMutex.lock ();
    m_jobs.push_back ( loc);
    m_queueMutex.unlock ();
}

QString RaiCacherService::getStatus()
{
    return "Unknown status";
}

RaiCacherService::~RaiCacherService()
{
}

CacherWorker::CacherWorker(RaiCacherService &rcs)
    : QThread()
    , m_rcs( rcs)
    , m_interruptFlag( false)
{
}

struct CacherWorkerInterruptor {
    class Interrupt{};
    CacherWorkerInterruptor( bool & flagRef) : m_flagRef( flagRef) {;}

    volatile bool & m_flagRef;
    void operator()() {
        if( m_flagRef) throw Interrupt();
    }
};


void CacherWorker::run()
{
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
    **/

    while(1) {
        try {
            // see if there is a job to do and pop it off the queue
            bool gotJob = false;
            FitsFileLocation floc;
            m_rcs.m_queueMutex.lock ();
            if( ! m_rcs.m_jobs.empty ()) {
                floc = m_rcs.m_jobs.front ();
                m_rcs.m_jobs.pop_front ();
                gotJob = true;
            }
            m_rcs.m_queueMutex.unlock ();
            // if there is no job, sleep a little and then try again
            if( ! gotJob) {
                this-> msleep ( 1000);
                this-> yieldCurrentThread ();
                continue;
            }

            // there is a job, so process it
            m_interruptFlag = false;
            CacherWorkerInterruptor intr( m_interruptFlag);
            try {
                dbg(1) << ConsoleColors::blue ()
                       << "Caching " << floc.getLocalFname ()
                       << ConsoleColors::reset ()
                       << "\n";
                Cacher cacher(m_rcs.m_maxMem);
                cacher.generateCacheForInterruptible( intr, * m_rcs.m_cc, floc);
            } catch (const CacherWorkerInterruptor::Interrupt &)  {
                checkForInterruptAndThrow ( floc);
            }
            dbg(1) << "Finished processing " << floc.uniqueId () << "\n";
        } catch ( const Interrupt & intr) {
            dbg(1) << "Cacher worker interrupted.\n";
            if( intr.m_hasJob) {
                dbg(1) << "Re-inserting interrupted job\n";
                m_rcs.doLater ( intr.m_floc);
            } else {
                dbg(1) << "Interrupt happened when there was no job.\n";
            }
        } catch ( ...) {
            dbg(1) << "Uknown exception in CacherWorker\n";
        }
    }
}

void CacherWorker::checkForInterruptAndThrow()
{
    if( m_interruptFlag) {
        m_interruptFlag = false;
        throw Interrupt();
    }
}

void CacherWorker::checkForInterruptAndThrow(const FitsFileLocation & loc)
{
    if( m_interruptFlag) {
        m_interruptFlag = false;
        throw Interrupt( loc);
    }
}



};};



