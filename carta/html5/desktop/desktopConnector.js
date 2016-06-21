/**
 * Implementation of IConnector.js for the desktop version. See the
 * IConnector.js for documentation of the API.
 */

/* JsHint options */
/* global mExport, mImport, QtConnector, QtConnector.* */
/* jshint eqnull:true */


(function()
{
    "use strict";

    var connector = mExport( "connector", {} );
    var setZeroTimeout = mImport( "setZeroTimeout" );
    var console = mImport( "console" );
    var defer = mImport( "defer" );
    var assert = mImport( "assert" );
    var CallbackList = mImport( "CallbackList");

    /**
     * Numerical constants representing status of the connection.
     * 
     * @type {{}}
     */
    connector.CONNECTION_STATUS = {
        CONNECTED   : 1,
        CONNECTING  : 2,
        FAILED      : 3,
        STALLED     : 4,
        DISCONNECTED: 5,
        UNKNOWN     : 6
    };

    connector.VIEW_CALLBACK_REASON = {
        UPDATED   : 1,
        TX_CHANGED: 2
    };

    // private variables
    var m_connectionStatus = connector.CONNECTION_STATUS.DISCONNECTED;
    var m_connectionCB = null;
    // we keep following information for every state:
    // - path (so that individual shared variables don't need to keep their own
    //   copies)
    // - value
    // - callback list
    // We start with an empty state
    var m_states = {};
    // array of callbacks for commands, these are used to report back results
    var m_commandCallbacks = [];
    // map of views
    var m_views = {};
    // cache of shared variables
    var m_sharedVars = {};

    // listen for command results callbacks and always invoke the top callback
    // in the list
    // the command results always arrive in the same order they were sent
    QtConnector.jsCommandResultsSignal.connect( function( result )
    {
        try {
            if( m_commandCallbacks.length < 1 ) {
                console.warn( "Received command results but no callbacks for this!!!" );
                console.warn( "The result: ", result );
                return;
            }
            var cb = m_commandCallbacks.shift();
            if( cb == null ) {
                return;
            }
            if( typeof cb !== "function" ) {
                console.warn( "Registered callback for command is not a function!" );
                return;
            }
            cb( result );
        }
        catch( error ) {
            window.console.error( "Caught error in command callback ", error );
            window.console.trace();
        }
    });

    // listen for jsViewUpdatedSignal to render the image
    QtConnector.jsViewUpdatedSignal.connect( function(viewName, buffer, refreshId)
    {
        try {
            var view = m_views[viewName];
            if( view == null ) {
                console.warn( "Ignoring update for unconnected view '" + viewName + "'" );
                return;
            }
            if ( buffer != null ){
                buffer.assignToHTMLImageElement( view.m_imgTag );
                QtConnector.jsViewRefreshedSlot( view.getName(), refreshId );
                view._callViewCallbacks();
            }
        }
        catch( error ) {
            window.console.error( "Caught error in view updated callback "+viewName, error );
            window.console.trace();
        }
    });

    // convenience function to create & get or just get a state
    function getOrCreateState(path) {
        var st = m_states[path];
        if (st !== undefined) {
            return st;
        }
        st = {
            path : path,
            value : null,
            callbacks : new CallbackList()
        };
        m_states[path] = st;
        return st;
    }

    /**
     * The View class
     * 
     * @param container
     * @param viewName
     * @constructor
     */
    var View = function(container, viewName) {
        // QtWebKit does not support drawing to the canvas (they claim they do,
        // but it coredumps). So we'll use <img> tag instead. That works well
        // enough.
        // TODO: investigate performance using QWebFactoryPlugin vs <img> tag

        // create an image tag inside the container
        this.m_container = container;
        this.m_viewName = viewName;
        this.m_imgTag = document.createElement( "img" );
        this.m_imgTag.setAttribute( "max-width", "100%");
        this.m_imgTag.setAttribute( "max-height", "100%");
        this.m_container.appendChild( this.m_imgTag );

        // register mouse move event handler
        this.m_imgTag.onmousemove = this.mouseMoveCB.bind(this);

        // extra data to handle mouse move throttling

        // delay in milliseconds ( -1 means no delay, 0 means zero timeout
        this.MouseMoveDelay = -1;
        this.m_mouseMoveTimeoutHandle = null;
        this.m_mousePos = { x : 0, y: 0 };
        this.m_mousePosSlotScheduled = false;
        this.m_viewCallbacks = new CallbackList();
    };

    /**
     * direct callback for mouse moves. We remember the coordinates, and then
     * make sure a timeout is scheduled to actually send the coordinates.
     * 
     * @param ev
     */
    View.prototype.mouseMoveCB = function mouseMoveCB(ev) {
        var x = ev.pageX - this.m_imgTag.getBoundingClientRect().left;
        var y = ev.pageY - this.m_imgTag.getBoundingClientRect().top;

        // remember the last mouse position
        this.m_mousePos = {
            x : x,
            y : y
        };

        // if throttling of mouse move events not enabled, send the event
        // directly
        if (this.MouseMoveDelay < 0) {
            QtConnector.jsMouseMoveSlot(this.m_viewName, this.m_mousePos.x,
                    this.m_mousePos.y);
        } else {
            // we want to throttle the mouse move events
            this.m_mousePos = {
                x : x,
                y : y
            };
            if (!this.m_mousePosSlotScheduled) {
                console.log("scheduling mouse move slot");
                this.m_mousePosSlotScheduled = true;
                this.m_mouseMoveTimeoutHandle = window
                        .setTimeout(this.mouseMoveTimeoutCB.bind(this),
                                this.MouseMoveDelay);
            } else {
                console.log("throttling mouse move");
            }
        }
    };

    View.prototype.mouseMoveTimeoutCB = function mouseMoveTimeoutCB() {
        this.m_mousePosSlotScheduled = false;
        // console.log( "calling jsMouseMoveSlot", this.m_viewName,
        // this.m_mousePos.x, this.m_mousePos.y );
        QtConnector.jsMouseMoveSlot(this.m_viewName, this.m_mousePos.x,
                this.m_mousePos.y);
    };

    View.prototype.setQuality = function setQuality() {
        // desktop only supports quality 101
    };
    View.prototype.getQuality = function setQuality() {
        // desktop only supports quality 101
        return 101;
    };
    View.prototype.updateSize = function() {
        // this.m_imgTag.width = this.m_container.offsetWidth;
        // this.m_imgTag.height = this.m_container.offsetHeight;
        /*console.log("about to call jsUpdateViewSlot", this.m_viewName,
                this.m_container.offsetWidth, this.m_container.offsetHeight);*/
        QtConnector.jsUpdateViewSlot(this.m_viewName,
                this.m_container.offsetWidth, this.m_container.offsetHeight);
    };
    View.prototype.getName = function() {
        return this.m_viewName;
    };
    View.prototype.getServerSize = function() {
        return {
            width : 99,
            height : 101
        };
    };
    View.prototype.local2server = function(coordinate) {
        return coordinate;
    };
    View.prototype.server2local = function(coordinate) {
        return coordinate;
    };
    View.prototype.addViewCallback = function(callback) {
        return this.m_viewCallbacks.add( callback);
    };
    View.prototype._callViewCallbacks = function () {
        this.m_viewCallbacks.callEveryone();
    };

    connector.supportsRasterViewQuality = function()
    {
        return false;
    };

    connector.registerViewElement = function( divElement, viewName )
    {
        var view = m_views[ viewName];
        if( view !== undefined) {
            throw new Error("Trying to re-register existing view '" + viewName + "'");
        }
        view = new View( divElement, viewName );
        m_views[ viewName] = view;
        return view;
    };

    connector.setInitialUrl = function( /*url*/ )
    {
        // we don't need urls
    };

    connector.getConnectionStatus = function()
    {
        return m_connectionStatus;
    };


    connector.setConnectionCB = function( callback )
    {
        m_connectionCB = callback;
    };

    connector.connect = function()
    {
        if( m_connectionCB == null ) {
            console.warn( "No connection callback specified!!!" );
        }

        if( window.QtPlatform !== undefined || window.QtConnector !== undefined ) {
            m_connectionStatus = connector.CONNECTION_STATUS.CONNECTED;
        }

        // listen for changes to the state
        QtConnector.stateChangedSignal.connect(function(key, val)
        {
            try {
                var st = getOrCreateState( key );
                // save the value
                st.value = val;
                // now go through all callbacks and call them
                    st.callbacks.callEveryone( st.value );
            }
            catch( error ) {
                window.console.error( "Caught error in state callback ", error );
                window.console.trace();
            }
        });

        // let the c++ connector know we are ready
        QtConnector.jsConnectorReadySlot();

        if (m_connectionCB != null) {
            setZeroTimeout(m_connectionCB);
        }

    };

    connector.disconnect = function() {
    };

    connector.canShareSession = function() {
        return false;
    };

    connector.shareSession = function( /* callback, username, password, timeout */) {
    };

    connector.unShareSession = function( /* errorCallback */) {
    };

    function SharedVar(path) {
        // make a copy of this to use in private/priviledged functions
        var m_that = this;
        // save a pointer to the state info associated with path
        var m_statePtr = getOrCreateState(path);

        // add a callback for the variable
        this.addNamedCB = function(callback) {
            if (typeof callback !== "function") {
                throw "callback is not a function!!";
            }
            // add callback to the list of all callbacks for this state key
            var cbId = m_statePtr.callbacks.add(callback);
            // return the id
            return cbId;
        };

        // add an anonymous callback
        this.addCB = function(callback) {
            m_that.addNamedCB(callback);
            return m_that;
        };

        this.set = function(value) {
            if (typeof value === "boolean") {
                value = value ? "1" : "0";
            }
            else if( typeof value === "string" ) {
                // do nothing, this will be verbatim
                value = value;
            }
            else if( typeof value === "number" ) {
                // convert number
                value = "" + value;
            }
            else {
                console.error( "value has weird type: ", value, m_statePtr.path );
                throw "don't know how to set value";
            }
            QtConnector.jsSetStateSlot(m_statePtr.path, value);

            return m_that;
        };

        this.get = function() {
            return m_statePtr.value;
        };

        // this should be called when the variable will no longer be used, so
        // that
        // callbacks associated with this state are all cleared
        this.destroy = function() {
            m_statePtr.callbacks.destroy();
        };

        this.path = function() {
            return m_statePtr.path;
        };

        this.removeCB = function(cbid) {
            m_statePtr.callbacks.remove(cbid);
            return m_that;
        };

        console.log("new var[" + path + "] = ", m_statePtr.value);
    }

    // create or get a cached copy of a shared variable for this path
    connector.getSharedVar = function(path) {
        var sv = m_sharedVars[path];
        if (sv != null) {
            return sv;
        }
        var newVar = new SharedVar(path);
        m_sharedVars[path] = newVar;
        return newVar;
    };

    connector.sendCommand = function(cmd, params, callback) {
        if (callback != null && typeof callback !== "function") {

            throw new Error("callback must be a function, null, or undefined");
        }
        m_commandCallbacks.push( callback);
        QtConnector.jsSendCommandSlot( cmd, params);
    };

})();
