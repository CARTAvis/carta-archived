/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 06/04/13
 * Time: 10:49 PM
 * To change this template use File | Settings | File Templates.
 */

/* global fv, goog */

"use strict";

/*
goog.provide("fv.lib.Hub");
goog.require("fv.console");
*/

(function () {

    /** @const */
    var DEBUG = false && fv.GLOBAL_DEBUG;

    fv.lib.Hub = function (hubName) {

        DEBUG && fv.console.log("Creating hub", hubName);

        this.hubName = hubName;

        this.m_callbacks = {};

        this.m_buffer = [];

        this.m_isBuffered = true;

        if (DEBUG) {
            this.m_allEmits = {};
            this.m_allSubscribes = {};
            this.m_log = [];
        }
    };


    /**
     * hub is initially in buffered mode, i.e. all emits are queued but not delivered.
     * this call will deliver all buffered emits
     */
    fv.lib.Hub.prototype.releaseBuffer = function () {
//        DEBUG && fv.console.log("releasing hub buffer " + this.hubName);
        if (!this.m_isBuffered) {
            return;
        }
        this.m_isBuffered = false;
        this.m_buffer.forEach(function (e) {
            e();
        }, this);
        this.m_buffer = [];
    };


    /**
     * subscribe to messages with the given path, trailing wildcards supported
     * @param path
     * @param callback
     * @param [pscope] - scope for the callback
     * @param [popts] - currently unused
     */
    fv.lib.Hub.prototype.subscribe = function (path, callback, pscope, popts) {

        if (DEBUG) {
            if (!this.m_allSubscribes[ path]) {
                this.m_allSubscribes[ path ] = true;
            }
            this.m_log.push("subs " + path);
        }

        var scope = pscope;
        var opts = popts;
        DEBUG && fv.assert( typeof path == "string" , "path must be a string");
        DEBUG && fv.assert( callback != null, "callback not defined");
        if ( scope == null) {
            scope = null;
        }
        if ( opts == null) {
            opts = {};
        }

        if ( this.m_callbacks[path] == null) {
            this.m_callbacks[path] = [];
        }

        this.m_callbacks[path].push({
            path: path,
            callback: callback,
            scope: scope,
            opts: opts
        });

    };

    /**
     * sends a message to all registered receivers
     * @param path
     * @param [data=undefined]
     */
    fv.lib.Hub.prototype.emit = function (path, data) {
        if (DEBUG) {
            if (!this.m_allEmits[ path]) {
                this.m_allEmits[ path ] = true;
            }
            this.m_log.push("emit " + path);
        }

        var count = 0;
        var parts = path.split('.');

        // try to call the '*' handlers first
        var pf = '', pth = '';
        parts.forEach(function (part, ind) {
            pth = pf + '*';
//            fv.console.count("trying to call", pth);
            count += this.callAll(pth, path, data);
            pf += part + '.';
        }, this);

        // now call specific handlers
        count += this.callAll(path, path, data);

        if (count < 1) {
/*
            fv.console.log("%cHUB warning: message %s has no listener... %O",
                "color: orange;", path, data);
*/
            fv.console.warn("HUB: message %s has no listener... %O",
                path, data);
        }
    };

    /**
     * private function - call all messages with given raw path
     * @param rawpath
     * @param origpath
     * @param data
     * @returns {Number} - number or delivered messages
     */
    fv.lib.Hub.prototype.callAll = function (rawpath, origpath, data) {
        var cb;
        var cbs = this.m_callbacks[ rawpath] || [];
        cbs.forEach(function (e) {
            cb = function () {
                fv.lib.setZeroTimeout(function () {
                    try {
                        e.callback.call(e.scope, data, origpath, e.opts);
                    } catch (err) {
                        fv.console.group("ERROR: exception in hub listener:");
                        fv.console.error("Could not deliver message: ", origpath);
                        fv.console.info("data=", data);
                        fv.console.info("rethrowing exception, more details below");
                        fv.console.groupEnd();
                        throw err;
                    }
                });
            };
            if (this.m_isBuffered) {
                this.m_buffer.push(cb);
            } else {
                cb();
            }
        }, this);
        return cbs.length;
    };

    /**
     * Helper function - to make a simple forwarder. Any message with path 'pathIn' will be
     * re-emitted with path 'pathOut' to hub 'dstHub'.
     * @param pathIn {String} incoming path to forward (trailing wildcards are OK)
     * @param pathOut {String} outgoing path
     * @param [dstHub] {Hub} optional destination hub, if not provided, current hub is used
     */
    fv.lib.Hub.prototype.forward = function (pathIn, pathOut, dstHub) {
        var DEBUG = false;
        DEBUG && fv.console.log("Setting up forward: " + pathIn + " --> " + pathOut);
        var dHub = dstHub;
        if (dHub === undefined) {
            dHub = this;
        }
        fv.assert(dHub !== undefined && dHub !== null);
        this.subscribe(pathIn, function (val) {
            DEBUG && fv.console.log("forwarding: " + pathIn + " --> " + pathOut);
            // find position of * in pathIn
            dHub.emit(pathOut, val);
        }, this);
    };

    /**
     * Same as forward, but will replace * with * in destination
     * @param pathIn {String} incoming path to forward (trailing wildcards are OK)
     * @param pathOut {String} outgoing path
     * @param [dstHub] {Hub} optional destination hub, if not provided, current hub is used
     */

    fv.lib.Hub.prototype.forwardMulti = function (pathIn, pathOut, dstHub) {
        var DEBUG = false;
        DEBUG && fv.console.log("Setting up forwardMulti: " + pathIn + " --> " + pathOut);

        var pos1 = pathIn.indexOf("*");

        var dHub = dstHub;
        if (dHub === undefined) {
            dHub = this;
        }
        fv.assert(dHub !== undefined && dHub !== null);
        this.subscribe(pathIn, function (val, currPath) {
            var dstPath = pathOut.replace("*", currPath.slice( pos1));
            DEBUG && fv.console.log("multiforwarding: " + currPath + " -> " + dstPath);
            dHub.emit(dstPath, val);
        }, this);
    };


})();

