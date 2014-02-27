/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 07/04/13
 * Time: 9:02 PM
 * To change this template use File | Settings | File Templates.
 */

//"use strict";

// define bind for obsolete JavaScript
if (!Function.prototype.bind) {
    console.warn("Old JavaScript... Implementing my own bind.");
    Function.prototype.bind = function (oThis) {
        if (typeof this !== "function") {
            // closest thing possible to the ECMAScript 5 internal IsCallable function
            throw new TypeError("Function.prototype.bind - what is trying to be bound is not callable");
        }

        var aArgs = Array.prototype.slice.call(arguments, 1),
            fToBind = this,
            fNOP = function () {},
            fBound = function () {
                return fToBind.apply(this instanceof fNOP && oThis
                    ? this
                    : oThis,
                    aArgs.concat(Array.prototype.slice.call(arguments)));
            };

        fNOP.prototype = this.prototype;
        fBound.prototype = new fNOP();

        return fBound;
    };
}


//if( window.fv == null) window.fv = {};
//if( window.fv.lib == null) window.fv.lib = {};

//var fv = { lib: {}};
window.fv = { lib: {}};

(function () {

    if( window.fvGlobalDebug )
        fv.GLOBAL_DEBUG = window.fvGlobalDebug;
    else
        fv.GLOBAL_DEBUG = false;

    var methods = 'error warn info debug log assert clear count dir dirxml group groupCollapsed groupEnd profile profileEnd table time timeEnd trace';
    methods = methods.split(' ');


    if (fv.GLOBAL_DEBUG) {

        // copy all console methods to fv.console
        fv.console = {};
        methods.forEach( function(m) {
            if( window.console[m] == null) return;
            fv.console[ m] = window.console[m].bind( window.console);
        });

        /*
        fv.startup = Date.now();

        fv.console.log = function() {
            var args = [ "[" + ((Date.now() - fv.startup) / 1000).toFixed(3) + "]" ];
            for( var i = 0 ; i < arguments.length ; i ++ )
                args.push( arguments[i]);
            window.console.log.apply( window.console, args);
        };
*/
        fv.slog = function() {
            var str = "";
            for( var i = 0 ; i < arguments.length ; i ++ )
                str += "" + arguments[i];
            pureweb.getClient().queueCommand( "debug", { text: str });
            console.log( "slog: ", str);
        }

    } else {
        var dummy = function () {
        };

        fv.console = {};
        for (var i = 0; i < methods.length; i++) {
            fv.console[ methods[i]] = dummy;
        }
        fv.slog = dummy;
    }

    // assert with exception throwing
    fv.assert = function (expr, pmsg) {
        if (!expr) {
            fv.console.assert.apply(fv.console, arguments);
            var msg = pmsg;
            if ( msg == null) msg = "unknown error";
            msg = "fv.assert: " + msg;
            throw new Error(msg);
        }
    };

//    fv.console.log("%cThis will be formatted with large, blue text", "color: blue; font-size: x-large");
})();


fv.lib.setZeroTimeout = (function () {

    if (window.addEventListener !== undefined && window.postMessage !== undefined) {

        var timeouts = [];
        var messageName = "fv-lib-zero-timeout-message";

        window.addEventListener(
            "message"
            , function handleMessage(event) {
                if (event.source == window && event.data == messageName) {
                    event.stopPropagation();
                    if (timeouts.length > 0) {
                        var fn = timeouts.shift();
                        fn();
                    }
                }
            }
            , true);

        return function (fn) {
            timeouts.push(fn);
            window.postMessage(messageName, "*");
        };
    } else {
        fv.console.warn("setZeroTimeout defaulting to setTimeout(...,0) !!!");
        // if postMessage functionality is not available default to regular setTimeout
        return function (fn) {
            setTimeout(fn, 0);
        }
    }

})();

fv.lib.now = (Date.now) || (function () {
    // Unary plus operator converts its operand to a number which in the case of
    // a date is done by calling getTime().
    return +new Date();
});

/**
 * Convenience function to run the supplied function. Useful for scoped variables.
 * @param func
 * @param scope
 * @returns {*}
 */
fv.lib.closure = function( func, scope) {
    if( scope === undefined) {
        return func();
    } else {
        return func.call( scope);
    }
};

/**
 * Returns value 'val' clamped to min/max
 * @param val
 * @param min
 * @param max
 */
fv.lib.clamp = function( val, min, max) {
    if( val < min) return min
    if( val > max) return max;
    return val;
};

/**
 * Linear map between two coordinates.
 * Maps val from range (x1..x2) to range (y1..y2). Value x1 maps to y1, and x2 maps to y2.
 * @param x
 * @param x1
 * @param x2
 * @param y1
 * @param y2
 */
fv.lib.linMap = function( val, x1, x2, y1, y2)
{
    var r = (val - x1) / (x2 - x1);
    return r * (y2-y1) + y1;
}


/*
 fv.GLOBAL_DEBUG && (function(){
 fv.console.log( "Calling set zero timeout");
 fv.lib.setZeroTimeout( function(){
 fv.console.log( "Zero timeout being executed.");
 });
 })();
 */
