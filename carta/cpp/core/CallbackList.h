/** The idea of this data structure is to provide a general container for callbacks.
 *  It provides the following API:
 *
 *  CallbackID add( callback)
 *  bool remove( CallbackID)
 *  void callEveryone()
 *  destructor
 *
 *  What is special about this data structure? The fact that all of the methods that
 *  modify the list (add, remove, destructor) can be called within callEveryone(), which
 *  is not the most trivial thing to implement. I.e. if inside one of the callbacks we
 *  remove some other callback, that callback should not be called. If we add a new
 *  callback inside one of the callbacks, that new callback should not be called immediately.
 *  Finally, if we destroy the container in one of the callbacks, the rest of the callbacks
 *  should not be called, and the application should not crash or result undefined
 *  behavior.
 *
 *
 **/


#ifndef COMMON_CALLBACKLIST_H
#define COMMON_CALLBACKLIST_H

#include <QtGlobal>
#include <QDebug>
#include <functional>
#include <cstdint>
#include <map>

/*

template <typename ...Params>
class Callback {
public:

    void add( std::function< void(Params ...)> cb){
        m_cbs . emplace_back( cb);
    }
    void call( Params ...p1) {

        for( auto const & cb : m_cbs) {
            cb( std::move(p1) ...);
        }
    }

    std::vector< std::function< void(Params ...)> > m_cbs;

};

*/

/**
  *
  * Internal class for handling callback lists (lists of functions with arbitrary parameters).
  *
  * \todo THIS NEEDS SOME DOCUMENTATION!!!! At least high-level stuff.
  */
template <typename ...Params>
class CallbackList
{

public:

    /// callback signature
    typedef std::function< void (Params ...)> CallbackSig;

    /// callback ID
    typedef int64_t CallbackID;

    /// initializes data structures
    CallbackList()
    {
        m_pimpl = new PIMPL;
//        qDebug() << "constructor this=" << this << " m_pimpl=" << m_pimpl;
    }

    /// adds a callback
    CallbackID add( const CallbackSig & cb)
    {
        CallbackInfo info;
        info.cb = cb;
        info.id = m_pimpl->nextId();

        // if we are inside the callback loop, don't activate the callback
        // and set the cleanup flag
        if( m_pimpl->insideCallEveryone) {
            m_pimpl->pendingCleanup = true;
            info.status = INSERTED;
        }
        // otherwise the callback is activated immediately
        else {
            info.status = ACTIVE;
        }

        // add the callback to the container
        m_pimpl-> callbacks[ info.id] = info;

        // return the callback ID
        return info.id;
    }

    /// remove a callback
    void remove( const CallbackID & id)
    {
        qDebug() << "=================== Removing callback " << id;
        // find the callback in the container
        auto iter = m_pimpl->callbacks.find( id);

        // if the callback could not be found, log the problem
        if( iter == std::end( m_pimpl->callbacks)) {
            qWarning() << "invalid callback id to remove: " << id;
            return;
        }

        // if we are inside deletions, just mark the callback as deleted
        // otherwise delete the callback right here
        if( m_pimpl-> insideCallEveryone) {
            iter-> second.status = DELETED;
            m_pimpl->pendingCleanup = true;
        }
        else {
            m_pimpl->callbacks.erase( iter);
        }
    }

    /// call all callbacks in the list
    void callEveryone(Params ... params)
    {
//        qDebug() << "Calling all callbacks";

        // we don't allow recursive calls
        Q_ASSERT( ! m_pimpl->insideCallEveryone);

        // remember the pimpl pointer in case we are destroyed inside one of the callbacks
        // then we access everything through our own copy of the pointer, because
        // m_pimpl might be invalid
        auto d = m_pimpl;

        // indicate we are inside the callback loop
        d-> insideCallEveryone = true;

        // remember the number of callbacks, no need to iterate over inserted callbacks...
//        auto n = d-> callbacks.size();

//        for( size_t i = 0 ; i < n ; ++ i) {
//            CallbackInfo & cbinfo = d-> callbacks[i];

        // iterate through all callbacks
        for( auto & entry : d->callbacks) {
            const CallbackInfo & cbinfo = entry.second;
//            qDebug() << "  cb #" << cbinfo.id << cbinfo.status;
            // skip any callback that is not active
            if( cbinfo.status != ACTIVE) continue;
            // call the callback
            cbinfo.cb( params ...);
            // if we were destroyed while inside the callback, abort the loop, clean up
            // pimpl and return immediately
            if( d->destroyed) {
                qDebug() << "Destroyed inside a callEveryone() ... aborting callback loop";
                delete d;
                return;
            }
        }

        // paranoia
        Q_ASSERT( ! d->destroyed);

        // if we had insertions/deletions while inside one of the callbacks
        // clean up the deletions and update insertions to active status
        // TODO: is there a faster way to do this?
        if( d->pendingCleanup) {
            qDebug() << "  cleaning up callback list";
            auto iter = std::begin( d->callbacks);
            while (iter != std::end(d->callbacks)) {
                CallbackInfo & cbinfo = iter->second;
                // do some stuff
                if (cbinfo.status == DELETED) {
                    iter = d->callbacks.erase(iter);
                }
                else {
                    cbinfo.status = ACTIVE;
                    ++iter;
                }
            }

//            decltype( d->callbacks) newList;

//            for( auto & entry : d->callbacks) {
//                CallbackInfo & cbinfo = entry.second;
//                if( cbinfo.status == DELETED) continue;
//                cbinfo.status = ACTIVE;
//                newList[ cbinfo.id] = cbinfo;
//            }
//            d->callbacks = newList;
        }

        // we are not inside the loop anymore
        m_pimpl-> insideCallEveryone = false;

        // clear the cleanup flag
        m_pimpl-> pendingCleanup = false;
    }

    /// safe destructor
    ~CallbackList()
    {
        qDebug() << "============== CallbackList destructor!!!!";
        qDebug() << "destructor this=" << this << " m_pimpl=" << m_pimpl;

        // if were are executing callbacks, just indicate destruction
        // the pointer will be destroyed after the loop
        if( m_pimpl->insideCallEveryone) {
            m_pimpl->destroyed = true;
        }
        else {
            // otherwise destroy pimpl immediately
            delete m_pimpl;
            m_pimpl = nullptr;
        }
    }

private:

    /// possible states of callbacks
    enum Status {
        ACTIVE = 0,
        DELETED = 1,
        INSERTED = 2
    };

    /// we keep this info for each callback
    struct CallbackInfo
    {
        CallbackID id;
        CallbackSig cb;
        Status status;
    };

    /// private implementation detail
    struct PIMPL
    {
        /// was the destructor called ?
        bool destroyed;

        /// are we executing callback loop?
        bool insideCallEveryone;

        /// do we have pending cleanup after callback loop?
        bool pendingCleanup;

        /// ID counter
        CallbackID idCounter;

        /// container with callback infos
        /// we want fast insertions, deletions, lookup by id, and iterations
        /// with iterations being the most important...
        /// TODO: is map the best container for this? Probably does not matter too...
        /// much unless we start to have excessive number of callbacks
//        std::vector < CallbackInfo > callbacks;
        std::map< CallbackID, CallbackInfo > callbacks;

        PIMPL() {
            destroyed = false;
            insideCallEveryone = false;
            pendingCleanup = false;
            idCounter = 0;
        }

        /// return the next unique ID
        CallbackID nextId() {
            return idCounter ++;
        }

    };

    /// private implementation detail
    PIMPL * m_pimpl;

    CallbackList( const CallbackList &) = delete;

};

#endif // COMMON_CALLBACKLIST_H
