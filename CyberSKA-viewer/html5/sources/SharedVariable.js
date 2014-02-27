/**
 * Simplify interaction with PureWeb.
 *
 * Example:
 *
 *   var title = fv.makePureWebVar( "/title", { cb: callbackFunction, skipInitialCB: false });
 *   console.log( "current title = ", title.get());
 *
 *   initialCB: if true, callback will be dispatched immediately (with timeout 0)
 */

/* JsHint options */
/* global fv, pureweb, -console */

(function() {

    "use strict";

    // we'll pollute the namespace here:
    window.fv = window.fv || {};

    var MAX_RECURSION = 15; // ought to be enough for everyone :)

    if( typeof pureweb === "undefined") {
        fv.console.error( "pureweb is not defined");
        return;
    }

    var pwState = pureweb.getFramework().getState();
    var pwStateManager = pwState.getStateManager();
    var connectedVerified = false;

    function pwget( key) {
        if( ! connectedVerified) {
            if( ! pureweb.getClient().isConnected()) {
                fv.console.error( "Trying to read value from pureweb state without connection");
                fv.console.error( "key:", key);
            }
            else {
                connectedVerified = true;
            }
        }
        return pwState.getValue( key);
    }

    function pwset( key, value) {
        if( ! connectedVerified) {
            if( ! pureweb.getClient().isConnected()) {
                fv.console.error( "Trying to set value to pureweb state without connection");
                fv.console.error( "key:", key, "value:", value);
            }
            else {
                connectedVerified = true;
            }
        }
        pwStateManager.setValue( key, value);
    }

    fv.makeGlobalVariable = function( path, firstCB, skipInitialCB /* = false */)
    {
        fv.console.group( "Creating pureweb variable:", path);

        // copy of the path
        var m_path = path;
        // list of callbacks
        var m_callbacks = [];
        // infinite recursion guard
        var m_recursion = 0;
        // last retrieved value
        var m_value = null;

        // add a callback for the variable
        var addCallbackFn = function (fn) {
            if( typeof fn !== "function") {
                throw "callback must be a function!!";
            }
            m_callbacks.push( fn);
        };

        if( typeof firstCB !== "undefined" && typeof firstCB !== "function") {
            throw "Callback must be a function!";
        }
        if( typeof firstCB !== "undefined") {
            addCallbackFn( firstCB);
        }

        var setFn = function(value) {
            fv.console.log( "shareVar.set", m_path, value);
            if( typeof value === "boolean") {
                value = value ? "1" : "0";
            }
            else if( typeof value === "string") {
                // do nothing, this will be verbatim
            }
            else if( typeof value === "number") {
                // convert number
                value = "" + value;
            }
            else {
                fv.console.error( "value has weird type: ", value, m_path);
                throw "don't know how to set value";
            }
            pwset( m_path, value);
        };

        var getFn = function(psync /* = false */ ) {
            var sync = ( typeof psync === "boolean") ? psync : false;
            if( sync) {
                m_value = pwget( m_path);
            }
            return m_value;
        };

        var isSetFn = function() {
            return m_value != null;
        };

        // this should be called when the variable will no longer be used, so that the purweb
        // callback can be deactivated
        var destroyFn = function() {
            pwStateManager.removeChildChangedHandler( m_path, internalCallback);
        };


        // call all callbacks with the new value
        var callEveryone = function() {
            fv.lib.setZeroTimeout( function() {
                m_callbacks.forEach( function(cb) { cb(m_value); });
            });
        };

        // the pureweb callback
        var internalCallback = function(ev) {
//            fv.console.log( "internal callback", m_path, ev.getNewValue());
            m_recursion ++;
            if( m_recursion > MAX_RECURSION) {
                throw "Infinite recursion on callback for PureWeb state " + m_path + " ?!?!";
            }
            m_value = ev.getNewValue();
            // call all registered callbacks
            callEveryone();
            m_recursion --;
        };

        // ask pureweb to notify us of changes
        pwStateManager.addValueChangedHandler( m_path, internalCallback);

        // retrieve current value
        getFn( true);
        fv.console.log( "current value:", m_value);


        // if we want the initial callback, do it now
        if( ! skipInitialCB) {
            // if the value is not undefined, notify the initial callback
            if( typeof m_value !== "undefined") {
                callEveryone();
            }
        }

        fv.console.groupEnd();

        // return the object to access this variable
        return {
            set: setFn,
            get: getFn,
            destroy: destroyFn,
            addCallback: addCallbackFn,
            invokeCallbacks: callEveryone,
            isSet: isSetFn
        };
    };

})();

/*
// commands
(function(){
    fv.sendCommand = function( name, value) {
        pureweb.getClient().queueCommand( "generic", { cmd: name, value: value });
    };
})();

(function(){

    return;

    fv.console.group("testing sharedvariable.js");
    function titleCB( title) {
        fv.console.log( "titleCB: ", title);
    }
    var title = fv.makeGlobalVariable( "/titlexxxx", titleCB, false);
    fv.console.log( "current title nosync = ", title.get(false));
    fv.console.log( "current title = ", title.get());
    title.set( "x.fits");
    fv.console.log( "current title = ", title.get());
    fv.console.log( "current title sync = ", title.get(true));

    window.setTimeout( function(){
        fv.console.group( "after 5 seconds later...");
        fv.console.log( "current title = ", title.get());
        fv.console.log( "current title sync = ", title.get(true));
        fv.console.groupEnd();
    }, 5000);
    fv.console.groupEnd();

})();
*/
