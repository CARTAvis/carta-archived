/**
 * PureWeb connector. See IConnector.js for documentation regarding the API. This is just
 * an implementation.
 */

/* JsHint options */
/*global pureweb, mExport, mImport */
/*jshint eqnull:true */


(function()
{
    "use strict";

    if( pureweb === undefined ) {
        throw "PureWeb not loaded. Aborting!";
    }

    var connector = mExport( "connector", {} );
    var setZeroTimeout = mImport( "setZeroTimeout" );
    var console = mImport( "console" );
    var defer = mImport( "defer" );
    var CallbackList = mImport( "CallbackList");

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

    connector.VIEW_CALLBACK_REASON = {
        UPDATED   : 1,
        TX_CHANGED: 2
    };

    // private variables
    // =================
    var m_initialUrl = null;
    var m_connectionStatus = connector.CONNECTION_STATUS.DISCONNECTED;
    var m_connectionCB = null;
    var m_lastShareUrl = null;
    var m_client = pureweb.getClient();
    // counter used to assign a unique ID to containers that don't have one (because
    // PureWeb requires one)
    var m_containerUIDcounter = 0;
    // cache of shared variables
    var m_sharedVars = {};

    // the View class
    var View = function( container, viewName )
    {
        // obtain the ID of the container element. This is unfortunately needed by
        // current PureWeb API. If there is no ID, assign one.
        if( ! container.id ) {
            // TODO: this assumes pwUID<number> will be unique, but to be certain we should...
            // probably check
            container.id = "pwUID" + m_containerUIDcounter;
            console.log( "generating id for container", container);
            m_containerUIDcounter ++;
        }
        console.log( "container id = ", container.id);

        // get an instance of the PureWeb View by linking the container to the view on the server
        this.m_pwview = new pureweb.client.View( {id: container.id, viewName: viewName } );
        // set the view's encoding parameters
        this.setQuality( 90 );
        // tell PureWeb to fit the canvas to the container element
        this.m_pwview.resize();
        // listen for PureWeb update events
        pureweb.listen( this.m_pwview, pureweb.client.View.EventType.VIEW_UPDATED,
            function( e )
            {
                var params = e.args.getEncodingParameters();
                // tell the server side we updated view, and send back also the attached refreshId
                pureweb.getClient().queueCommand( "viewrefreshed",
                    { viewName: viewName, id: params.refreshId }, function() {} );
                // also call any user registered callbacks
                this._callViewCallbacks();

            }, false, this );
        // no user callbacks
        this.m_viewCallbacks = new CallbackList();
        pureweb.listen( this.m_pwview, pureweb.client.View.EventType.TRANSFORMS_CHANGED,
            function( )
            {
                console.log( "View '" + this.m_pwview.getViewName() + "' txupdate");
            }, false, this );
    };

    View.prototype.setQuality = function setQuality( quality )
    {
        if( quality < 1) {
            quality = 1;
        }
        else if ( typeof( quality) == "undefined" ){
        	quality = connector.supportsRasterViewQuality() ? 90 : 101;
        }
        this.m_quality = quality;
        var params = {};
        // support for safari... (browsers that don't support binary format?)
        if( ! m_client.supportsBinary() ) {
            params = {'UseBase64': true};
        }
        var ef;
        if( quality <= 100 ) {
            ef = new pureweb.client.EncoderFormat( pureweb.SupportedEncoderMimeType.JPEG, quality, params );
        }
        else if( quality == 101) {
            ef = new pureweb.client.EncoderFormat( pureweb.SupportedEncoderMimeType.PNG, null, params );
        }
        else {
            console.error( "server connector does not support mpeg quality yet");
            ef = new pureweb.client.EncoderFormat( pureweb.SupportedEncoderMimeType.JPEG, 100, params );
        }
        var ec = new pureweb.client.EncoderConfiguration( ef, ef );
        this.m_pwview.setEncoderConfiguration( ec );
        this.m_pwview.refresh();
    };
    View.prototype.getQuality = function() {
        return this.m_quality;
    };
    View.prototype.updateSize = function( )
    {
        this.m_pwview.resize();
    };
    View.prototype.getName = function()
    {
        return this.m_pwview.getViewName();
    };
    View.prototype.getServerSize = function()
    {
        return this.m_pwview.getImageSize();
    };
    View.prototype.local2server = function( coordinate )
    {
        return this.m_pwview.localToSource( coordinate );
    };
    View.prototype.server2local = function( coordinate )
    {
        return this.m_pwview.sourceToLocal( coordinate );
    };
    View.prototype.addViewCallback = function( callback )
    {
        console.log( "Adding view callback to", this.getName());
        return this.m_viewCallbacks.add( callback);
    };
    View.prototype._callViewCallbacks = function () {
        this.m_viewCallbacks.callEveryone();
    };

    connector.supportsRasterViewQuality = function()
    {
        return true;
    };

    connector.registerViewElement = function( divElement, viewName) {
        return new View( divElement, viewName);
    };

    connector.setInitialUrl = function( url )
    {
        m_initialUrl = url;
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
        var uri = m_initialUrl;
        if( uri == null ) {
            uri = location.href;
        }


        if( ! m_client.isaSessionUri( uri ) ) {
            uri = location.protocol + '//' + location.host + '/pureweb/app?name=' + pureweb.getServiceAppName( uri );
        }


        pureweb.listen( m_client, pureweb.client.WebClient.EventType.SESSION_STATE_CHANGED,
            function( /*e*/ )
            {
                var st = connector.CONNECTION_STATUS.UNKNOWN;
                console.log( "internal cb initialized status from ", m_connectionStatus );
                switch( m_client.getSessionState() ) {
                    case pureweb.client.SessionState.ACTIVE :
                        st = connector.CONNECTION_STATUS.CONNECTED;
                        break;
                    case pureweb.client.SessionState.CONNECTING :
                        st = connector.CONNECTION_STATUS.CONNECTING;
                        break;
                    case pureweb.client.SessionState.DISCONNECTED :
                        st = connector.CONNECTION_STATUS.DISCONNECTED;
                        break;
                    case pureweb.client.SessionState.FAILED :
                        st = connector.CONNECTION_STATUS.FAILED;
                        break;
                    case pureweb.client.SessionState.STALLED :
                        st = connector.CONNECTION_STATUS.STALLED;
                        break;
                    default:
                        st = connector.CONNECTION_STATUS.UNKNOWN;
                        break;
                }
                m_connectionStatus = st;
                console.log( "internal cb set connection status to ", m_connectionStatus );
                // we'll fire CONNECTED event only if state is initialized, which is
                // most likely not the case if we just connected
                if( st !== connector.CONNECTION_STATUS.CONNECTED
                    || pureweb.getFramework().isStateInitialized()) {
                    cb();
                }
            }
        );

        console.log( "framework = ", pureweb.getFramework());
        console.log( "STATE_INITIALIZED right now = ", pureweb.getFramework().isStateInitialized());
//        pureweb.listen( m_client, pureweb.client.Framework.EventType.IS_STATE_INITIALIZED,
        pureweb.listen( pureweb.getFramework(), pureweb.client.Framework.EventType.IS_STATE_INITIALIZED,
            function() {
                window.console.log( "IS_STATE_INITIALIZED cb -->", pureweb.getFramework().isStateInitialized());
                if( pureweb.getFramework().isStateInitialized()
                    && m_connectionStatus == connector.CONNECTION_STATUS.CONNECTED) {
                    cb();
                }
            }
        );

        // setup the window.onbeforeunload callback to disconnect from the service application
        window.onbeforeunload = window.onunload = function( /*e*/ )
        {
            if( m_client.isConnected() ) {
                m_client.disconnect( false );
            }
            return null;
        };

        console.log( "calling pureweb.connect with uri=", uri );
        pureweb.connect( uri );
    };

    connector.disconnect = function()
    {
        var m_client = pureweb.getClient();
        if( m_client.isConnected() ) {
            pureweb.disconnect();
        }
    };

    connector.canShareSession = function()
    {
        return true;
    };

    connector.shareSession = function( callback, username, password, timeout )
    {
        if( m_lastShareUrl != null ) {
            throw "Session already shared.";
        }
        var m_client = pureweb.getClient();
        m_client.getSessionShareUrlAsync(
            password, '', timeout, '', function( getUrl, exception )
            {
                console.warn( 'shareurlcb', getUrl, exception, arguments );
                if( getUrl != null ) {
                    m_lastShareUrl = getUrl;
                    getUrl = getUrl + "?client=html5&share_password=";
//                    window.prompt( "Here is your collaboration URL:", getUrl );
                    callback( getUrl );
                }
                else {
                    var msg="An error occurred creating the share URL";
                    if ( exception !== null ){
                        msg = msg + ": "+exception.description;
                    }
                    window.alert( msg );
                    callback( null, msg );
                }
            } );
        m_lastShareUrl = "";

    };

    connector.unShareSession = function( errorCallback )
    {
        if( m_lastShareUrl == null ) {
            throw "No session is shared.";
        }
        var m_client = pureweb.getClient();
        m_client.invalidateSessionShareUrlAsync( m_lastShareUrl, function( exception )
            {
                if( exception != null ) {
                    window.alert( 'An error occurred invalidating the share URL: ' + exception );
                    errorCallback( exception );
                }
                else {
                    m_lastShareUrl = null;
                }
            }
        );
        m_lastShareUrl = null;
    };

    function SharedVar( path )
    {
        console.log( "Creating pureweb variable:", path );

        // make a copy of this to use in private/priviledged functions
        var m_that = this;
        // copy of the path
        var m_path = path;
        // list of callbacks
        var m_callbacks = [];
        // last callback ID
        var m_lastCallbackID = 1;
        // last retrieved value
        var m_cachedValue = null;
        // state manager convenience
        var m_pwStateManager = pureweb.getFramework().getState().getStateManager();
        // whether we have internal callback set or not
        var m_internalCallbackRegistered = false;

        // register PureWeb callback if not already registered
        function registernInternalCallback()
        {
            if( m_internalCallbackRegistered === true ) {
                return;
            }
            m_internalCallbackRegistered = true;
            m_pwStateManager.addValueChangedHandler( m_path, internalCallback );
        }

        // unregister PureWeb callback, unless not registered
        function unregisterInternalCallback()
        {
            if( m_internalCallbackRegistered === false ) {
                return;
            }
            m_internalCallbackRegistered = false;
            m_pwStateManager.removeValueChangedHandler( m_path, internalCallback );
        }

        // add a callback for the variable
        this.addNamedCB = function( callback )
        {
            if( typeof callback !== "function" ) {
                throw "callback is not a function!!";
            }
            var cbId = "cb" + (m_lastCallbackID ++);
            m_callbacks.push( { cb: callback, id: cbId } );
            // since we now have at least 1 callback, register internal callback
            registernInternalCallback();
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
            m_pwStateManager.setValue( m_path, value );
            m_cachedValue = value;

            return m_that;
        };

        this.get = function()
        {
            // if we have don't have internal callback registered, re-read the value
            if( !m_internalCallbackRegistered ) {
                m_cachedValue = m_pwStateManager.getValue( m_path );
            }
            return m_cachedValue;
        };

        // call all callbacks
        function callEveryone()
        {
            m_callbacks.forEach( function( cb )
            {
                cb.cb( m_that.get() );
            } );
        }

        // internal pureweb callback
        function internalCallback( ev )
        {
//            console.log( "internal callback" );
            m_cachedValue = ev.getNewValue();
            setZeroTimeout( callEveryone );
        }

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
            if( m_callbacks.length === 0 ) {
                unregisterInternalCallback();
            }
            return this;
        };

        // retrieve current value
        m_cachedValue = m_pwStateManager.getValue( m_path );

        console.log( "current value:", m_cachedValue );
    }

    connector.getSharedVar = function( path )
    {
        var sv = m_sharedVars[path];
        if( sv != null) return sv;
        m_sharedVars[path] = new SharedVar( path);
        return m_sharedVars[path];
//        return new SharedVar( path );
    };

    connector.sendCommand = function( cmd, params, callback )
    {
        m_client.queueCommand( "generic", { cmd: cmd, params: params}, function( caller, data) {
            var response = data.getResponse();
            if ( response.textContent ){
                response = response.textContent.replace(/(\r\n|\n|\r)/gm,"");
                response = response.trim();
            }
            callback && callback( response );
        } );
    };

})();

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
