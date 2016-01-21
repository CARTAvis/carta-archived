/**
 * Created by pfederl on 26/04/14.
 */

/** The idea of this data structure is to provide a general container for callbacks.
 *  It provides the following API:
 *
 *  CallbackID add( callback)
 *  bool remove( CallbackID)
 *  void callEveryone()
 *  destroy
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

/* global mImport, mExport */

(function() {

    "use strict";

    var console = mImport( "console");
    var assert = mImport( "assert");

    // private ID counter
    var idCounter = 0;
    function nextId() {
        return idCounter ++;
    }

    /*var CallbackList = */function CallbackList()
    {
        this.m_insideLoop = false;
        this.m_pendingCleanup = false;
        this.m_cbList = {};
        this.m_destroyed = false;
    }


    /**
     * Add a callback and return it's unique ID for removal later.
     * @param cb
     * @returns {*}
     */
    CallbackList.prototype.add = function add( cb)
    {
        var info = {
            cb: cb,
            id: nextId()
        };

        // if we are inside the callback loop, don't activate the callback
        // and set the cleanup flag
        if( this.m_insideLoop) {
            this.m_pendingCleanup = true;
            info.status = "inserted";
        }
        // otherwise the callback is activated immediately
        else {
            info.status = "active";
        }

        // add the callback to the container
        this.m_cbList[ info.id] = info;

        // return the callback ID
        return info.id;
    };

    /**
     * Remove a callbak by its ID.
     * @param id
     */
    CallbackList.prototype.remove = function remove( id)
    {
        // find the callback in the container
        var iter = this.m_cbList[id];

        // if the callback could not be found, log the problem
        if( iter === undefined) {
            console.error( "invalid callback id to remove: ", id);
            return;
        }

        // if we are inside deletions, just mark the callback as deleted
        // otherwise delete the callback right here
        if( this.m_insideLoop) {
            iter.status = "deleted";
            this.m_pendingCleanup = true;
        }
        else {
            delete this.m_cbList[id];
        }
    };

    /**
     * Calls every callback with the given parameters
     */
    CallbackList.prototype.callEveryone = function callEveryone( /* args ... */)
    {
        if( this.m_insideLoop) {
            console.error( "recursive call of CallbackList.callEveryone()");
            return;
        }

        assert( ! this.m_insideLoop, "You are calling callEveryone recursively");

        // indicate we are inside the callback loop
        this.m_insideLoop = true;

        // iterate through all callbacks
        for (var key in this.m_cbList) {
            if (! this.m_cbList.hasOwnProperty(key)) {
                console.log( "early exit");
                continue;
            }
            var info = this.m_cbList[key];
            // only call active callbacks
            if( info.status === "active") {
                info.cb.apply( this, arguments);
            }
            // if we were destroyed while inside the callback, abort the loop, clean up
            // pimpl and return immediately
            if( this.m_destroyed) {
                console.log( "Destroyed inside a callEveryone() ... aborting callback loop");
                this.m_cbList = {};
                return;
            }
        }


        // if we had insertions/deletions while inside one of the callbacks
        // clean up the deletions and update insertions to active status
        if( this.m_pendingCleanup) {
            for (var key in this.m_cbList) {
                if (! this.m_cbList.hasOwnProperty(key)) {
                    continue;
                }
                var info = this.m_cbList[key];
                // only call active callbacks
                if( info.status === "deleted") {
                    delete this.m_cbList[key];
                }
                else {
                    info.status = "active";
                }
            }
            this.m_pendingCleanup = false;
        }

        // we are not inside the loop anymore
        this.m_insideLoop = false;
    };

    /**
     * mark as destroyed and remove all callbacks
     */
    CallbackList.prototype.destroy = function destroy()
    {
        // if were are executing callbacks, just indicate destruction
        if( this.m_insideCallEveryone) {
            this.m_destroyed = true;
        }
        else {
            // otherwise destroy now
            this.m_cbList = {};
            this.m_destroyed = true;
        }
    };

    mExport( "CallbackList", CallbackList);

})();

/*

(function(){

    "use strict";

    function testCBL() {
        window.CL = mImport( "CallbackList");
        window.cl = new window.CL;
        window.logfn = function() { console.log( "logfn:", arguments); };

        cl.add( function() {
            console.log('callback1 - you should see this once');
        });
        cl.add( function() {
            console.log('callback2 - you should see this both times');
            cl.remove(0);
            cl.add( function() {
                console.log('callback3 - you should see this only on 2nd run');
            });
        });
    }

    mExport( "testCBL", testCBL);

})();


*/





