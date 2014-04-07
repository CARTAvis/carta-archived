/**
 * Desktop connector. See IConnector.js for documentation regarding the API. This is just
 * an implementation.
 */

/* JsHint options */
/*global mExport, mImport */
/*jshint eqnull:true */


(function()
{
    "use strict";

    var connector = mExport( "connector", {} );
    var setZeroTimeout = mImport( "setZeroTimeout" );
    var console = mImport( "console" );
    var defer = mImport( "defer" );

    /**
     * Numerical constants representing status of the connection.
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

    connector.VIEW_CALLBACK_RESON = {
        UPDATED   : 1,
        TX_CHANGED: 2
    };

    // private variables
    var m_connectionStatus = connector.CONNECTION_STATUS.DISCONNECTED;
    var m_connectionCB = null;
    // last callback ID, so we can generate unique ones
    var m_lastCallbackID = 1;
    // we keep following information for every state:
    //  - path (indirectly as an index)
    //  - value
    //  - callbacks
    // for each callback we keep
    //    - callback {function}
    //    - id
    // We start with an empty state
    var m_states = {};
    // and make a create empty state convenience function
    function getOrCreateState( path) {
        var st = m_states[path];
        if( st !== undefined) return st;
        st = { value: null, callbacks: [] };
        m_states[path] = st;
        return st;
    }

    // the View class
    var View = function( container, viewName )
    {
        // QtWebKit does not support drawing to the canvas (they claim they do, but
        // it coredumps). So we'll use <img> tag instead. That works well enough.
        // TODO: investigate performance using QWebFactoryPlugin vs <img> tag

        // create an image tag inside the container
        this.m_container = container;
        this.m_viewName = viewName;
        this.m_imgTag = document.createElement( "img");
        console.log( "imgTag = ", this.m_imgTag);
        this.m_container.appendChild( this.m_imgTag);
    };
    View.prototype.setQuality = function setQuality( )
    {
        // desktop does not have quality
    };
    View.prototype.updateSize = function( newWidth, newHeight )
    {
        this.m_imgTag.width = this.m_container.width;
        this.m_imgTag.height = this.m_container.height;
    };
    View.prototype.getName = function()
    {
        return this.m_viewName;
    };
    View.prototype.getServerSize = function()
    {
        return { width: 99, height: 101}
    };
    View.prototype.local2server = function( coordinate )
    {
        return coordinate;
    };
    View.prototype.server2local = function( coordinate )
    {
        return coordinate;
    };
    View.prototype.addViewCallback = function( callback )
    {
    };

    connector.registerViewElement = function( divElement, viewName) {
        return new View( divElement, viewName);
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
        var cb = m_connectionCB || function()
        {
        };

        if( window.QtPlatform !== undefined || window.QtConnector !== undefined) {
            m_connectionStatus = connector.CONNECTION_STATUS.CONNECTED;
        }

        if( m_connectionCB) {
            m_connectionCB();
        }

        // listen for changes to the state
        QtConnector.stateChangedSignal.connect( function( key, val) {
            console.log( "qt.stateChanged", key, val);
            var st = getOrCreateState( key);
            st.value = val;
            // now go through all callbacks and call them
            st.callbacks.forEach( function( cb) {
                cb.callback( st.value);
            });
        });
    };

    connector.disconnect = function()
    {
    };

    connector.canShareSession = function()
    {
        return false;
    };

    connector.shareSession = function( /*callback, username, password, timeout*/ )
    {
    };

    connector.unShareSession = function( /*errorCallback*/ )
    {
    };

    function SharedVar( path )
    {
        console.log( "Creating shared variable:", path );

        // make a copy of this to use in private/priviledged functions
        var m_that = this;
        // copy of the path
        var m_path = path;

        // add a callback for the variable
        this.addNamedCB = function( callback )
        {
            if( typeof callback !== "function" ) {
                throw "callback is not a function!!";
            }
            var cbId = "cb" + (m_lastCallbackID ++);
            var st = getOrCreateState( m_path);
            st.callbacks.push( { callback: callback, id: cbId });
            return cbId;
        };

        // add an anonymous callback
        this.addCB = function( callback )
        {
            m_that.addNamedCB( callback );
            return m_that;
        };

        this.set = function( value )
        {
            if( typeof value === "boolean" ) {
                value = value ? "1" : "0";
            }
            else if( typeof value === "string" ) {
                // do nothing, this will be verbatim
            }
            else if( typeof value === "number" ) {
                // convert number
                value = "" + value;
            }
            else {
                console.error( "value has weird type: ", value, m_path );
                throw "don't know how to set value";
            }
            QtConnector.jsSetStateSlot( m_path, value);

            return m_that;
        };

        this.get = function()
        {
            return getOrCreateState(m_path ).value;
        };

        // this should be called when the variable will no longer be used, so that pureweb
        // callback can be deactivated
        this.destroy = function()
        {
            unregisterInternalCallback();
            m_callbacks = [];
        };

        this.path = function()
        {
            return m_path;
        };

        this.removeCB = function( cbid )
        {
            var m_callbacks = m_states[ m_path].callbacks;
            var ind = - 1;
            for( var i = 0 ; i < m_callbacks.length ; i ++ ) {
                if( m_callbacks[i].id === cbid ) {
                    ind = i;
                    break;
                }
            }
            if( ind < 0 ) {
                throw "no such callback found";
            }
            else {
                m_callbacks.splice( ind, 1 );
            }
            return m_that;
        };


        console.log( "current value:", getOrCreateState( m_path ).value );
    }

    connector.getSharedVar = function( path )
    {
        return new SharedVar( path );
    };

    connector.sendCommand = function( cmd, params, callback )
    {
    };

    connector.getViewElementInfo = function()
    {

    };

})();
/*

(function( scope )
{
    "use strict";

    return;

    var connector = mImport( "connector" );
    var console = mImport( "console" );
    connector.setConnectionCB( function( )
    {
        console.log( "connectionCB", connector, connector.getConnectionStatus() )
    } );
    connector.connect();

    return;

    scope.connector = {};

    scope.connector.setState = function( key, val )
    {
        pureweb.getFramework().getState().setValue( key, val );
    };

    scope.connector.clearState = function( prefix )
    {
        pureweb.getFramework().getState().getStateManager().deleteTree( prefix );
    };

    // rewrite uri if sharing session
    var uri = location.href;
    if( ! pureweb.getClient().isaSessionUri( uri ) ) {
        uri = location.protocol + '//' + location.host + '/pureweb/app?name=' + pureweb.getServiceAppName( uri );
    }

    var client = pureweb.getClient();
    pureweb.listen( client, pureweb.client.WebClient.EventType.CONNECTED_CHANGED, function onConnectedChanged( e )
        {
            if( ! e.target.isConnected() ) {
                return;
            }
            var diagnosticsPanel = document.getElementById( 'pwDiagnosticsPanel' );
            if( diagnosticsPanel ) {
                pureweb.client.diagnostics.initialize();
            }
        }
    );

    pureweb.listen( client, pureweb.client.WebClient.EventType.SESSION_STATE_CHANGED,
        function sscCB( e )
        {
            console.log( "pureweb session state changed", e );
            console.log( "  state:", client.getSessionState() );
        }
    );


    // setup the window.onbeforeunload callback to disconnect from the service application
    window.onbeforeunload = window.onunload = function( e )
    {
        if( client.isConnected() ) {
            client.disconnect( false );
        }
        return null;
    };

    pureweb.connect( uri );


})( window );
*/

/*
 var img = Qt.img;
 if( imgEl == null) {
 imgEl = document.getElementById("mimg");
 }
 Qt.img.assignToHTMLImageElement( imgEl);

 */