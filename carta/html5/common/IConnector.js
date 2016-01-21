/**
 * Created by pfederl on 23/03/14.
 *
 * Interface for a connector between GUI and Application. Same connector can be used
 * for client/server version and for desktop-only version.
 *
 * The design of the connector is limited by the functionality offered by PureWeb, since
 * we are using PureWeb to implements the client/server communication.
 *
 * Note: this is not actual code. It is not used anywhere. It is just a place to document
 * the APIs of the real connectors: desktop & server at the moment.
 */

var IConnector;
IConnector = {};

/**
 * This is the url from the browser.
 * @param url
 */
IConnector.setInitialUrl = function( url) {};

/**
 * Numerical constants representing status of the connection.
 * @type {{}}
 */
IConnector.CONNECTION_STATUS = {
    CONNECTED: 1,
    CONNECTING: 2,
    FAILED: 3,
    STALLED: 4,
    DISCONNECTED: 5,
    UNKNOWN: 6
};

/**
 * Get the status of the connection.
 * @returns { code: IConnector.STATUS, text: String }
 *
 * Code is one of the constants from IConnector.CONNECTION_STATUS
 */
IConnector.getConnectionStatus = function() {};

/**
 * Sets the callback to be called whenever PureWeb connection changes state. Only one callback
 * is supported at the moment.
 * @param callback
 *
 * The callback signature is:
 *   function callback()
 */
IConnector.setConnectionCB = function (callback) {};

/**
 * Attempts a connection using the URL specified via setInitialUrl.
 * Connection success/failure is communicated via callback specified using setConnectionCB;
 * @returns bool (true on success)
 */
IConnector.connect = function() {};

/**
 * Disconnects from the server.
 */
IConnector.disconnect = function() {};

/**
 * @returns bool (if this connector supports session sharing)
 */
IConnector.canShareSession = function() {};

/**
 * Requests session sharing. Calling this is only meaningful if canShareSession returns true.
 * @param callback
 * @param username
 * @param password
 * @param timeout
 * callback is used to obtain the URL for sharing the session that should be shared with collaborators.
 * callback( url) in case of success
 * callback( null, description) in case of failure
 */
IConnector.shareSession = function( callback, username, password, timeout) {};

/**
 * If session sharing is active, it will be deactivated. Otherwise nothing happens.
 */
IConnector.unShareSession = function( errorCallback) {};

/**
 * Higher level API for reading/observing/modifying state.
 * @param path
 * @returns instance of SharedVar
 */
IConnector.getSharedVar = function( path) {};

/*

Sample usage:

Create a var, add callback and set the value. The callback will be called with the new value.
var v = connector.getSharedVar("/mainview/cursor").addCb(cb).set("0 0");
or
var v = connector.getSharedVar("/mainview/cursor").set("0 0").addCb(cb);

Create a var, add callback and set the value. The callback will be called twice.
var v = connector.getSharedVar("/mainview/cursor").addCb(cb, true).set("0 0");

Create a var, add callback and set the value. The callback will be called only after
something else changes the value..
var v = connector.getSharedVar("/mainview/cursor").set("0 0").addCb(cb);

*/

var SharedVar = {};
/**
 * Returns the current value.
 * @returns {String} current value
 */
SharedVar.get = function() {};

/**
 * Returns the path of this var.
 * @returns {String} path
 */
SharedVar.path = function() {};

/**
 * Sets the var to a new value.
 * @param {String|bool|Number} value
 * @returns this
 */
SharedVar.set = function( value) {};

/**
 * Adds a callback to be called when the value changes.
 * @param callback
 * @param callNow if true, callback will be scheduled
 * @returns {String} callbackID which can be used to remove the callback
 *
 * Callback signature is callback( variable)
 */
SharedVar.addNamedCB = function( callback, callNow) {};

/**
 * Adds an anonymous callback to be called when the value changes.
 * @param callback
 * @param callNow if true, callback will be scheduled
 * @returns this, used to chain methods
 *
 * Callback signature is callback( variable)
 */
SharedVar.addCB = function( callback, callNow) {};

/**
 * Removes an existing callback.
 * @param callbackId
 */
SharedVar.removeCB = function( callbackId) {};

/**
 * Returns true if the variable has a value associated with it.
 */
SharedVar.isSet = function() {};

/**
 * Removes all callbacks associated with this path.
 */
SharedVar.destroy = function() {};

/**
 * Send a command to the application.
 * @param cmd
 * @param params stringified parameters
 * @param callback (optional)
 *
 * callback has signature:
 * function callback( value:Object)
 */
IConnector.sendCommand = function( cmd, params, callback) {};

/**
 * Associates a div element as an 'output device' for the given view. As a result of this
 * operation, additional html content will be inserted into the div.
 * @param divElement is an existing div. It will server as a container for implementation
 *          specific html content.
 * @param viewName the name of the view to match server side
 * @returns {IView} instance of view
 */
IConnector.registerViewElement = function( divElement, viewName) {};

/**
 * returns true if the raster views support quality control (i.e. true for server, false
 * for desktop).
 */
IConnector.supportsRasterViewQuality = function() {};


/**
 * View interface. Instance returned by IConnector.registerViewElement
 */
var IView = {};

/**
 * Set the quality of the view.
 * Setting to 100 will attempt loss-less compression, all other levels are lossy.
 * For example quality=100 could be PNG.
 *
 * This has no effect for desktop connector. There the quality is always 100%.
 *
 * @param quality {int} 0 = lowest, 100 = highest quality jpeg, 101 = png, 102 = mpeg?
 */
IView.setQuality = function( quality) {};

/**
 * Returns the current quality (integer 0..102).
 *
 */
IView.getQuality = function() {};

/**
 * This needs to be called when the container of the view was resized.
 */
IView.updateSize = function() {};

/**
 * Returns the name associated with this view.
 * @returns {String}
 */
IView.getName = function() {};

/**
 * Returns the size of the view on the server.
 */
IView.getServerSize = function() {};

/**
 * Converts local coordinate to server.
 * Local coordinate has (0,0) in the top left corner of the element used to draw the view.
 * @param coordinate { x: Number, y: Number }
 * @returns { x: Number, y: Number }
 */
IView.local2server = function( coordinate) {};

/**
 * Converts server coordinate to local.
 * Local coordinates have (0,0) in the top left corner of the element used to draw the view.
 * @param coordinate { x: Number, y: Number }
 * @returns { x: Number, y: Number }
 */
IView.server2local = function( coordinate) {};

/**
 * Associate a callback with a view. This callback is called when the view is redrawn.
 * @param viewName
 * @param callback
 *
 * callback signature:
 *
 * callback( reason:int, params:Object)
 */
IView.addViewCallback = function( callback) {};
