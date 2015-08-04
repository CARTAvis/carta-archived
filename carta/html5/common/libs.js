/**
 * Created by pfederl on 26/03/14.
 *
 * Various useful functions.
 */

/* global console, mImport, mExport, window, window.postMessage */

/**
 * Define mExport and mImport
 */
(function()
{
    "use strict";

    var m_cache = {};

    window.mExport = function( name, value ) {
        if( m_cache[name] !== undefined) {
            throw "export symbol already taken: " + name;
        }
        m_cache[name] = value;
        return m_cache[name];

    };

    window.mImport = function( name) {
        var res = m_cache[name];
        if( res === undefined) {
            console.warn( "mImport: '" + name + "' not available!");
        }
        return m_cache[name];
    };

}());


// export console
(function()
{
    "use strict";
    mExport( "console", window.console);

})();

// export zeroTimeout
(function()
{

    "use strict";

    var console = mImport( "console");

    if( window.addEventListener !== undefined && window.postMessage !== undefined ) {

        var timeouts = [];
        var messageName = "zTimeoutMessage";

        window.addEventListener(
            "message",
            function handleMessage( event )
            {
                if( event.source === window && event.data === messageName ) {
                    event.stopPropagation();
                    if( timeouts.length > 0 ) {
                        var fn = timeouts.shift();
                        fn();
                    }
                }
            }, true );

        console.log( "using fast addZeroTimeout");
        mExport( "setZeroTimeout", function( fn )
        {
            timeouts.push( fn );
            window.postMessage( messageName, "*" );
        });
    }
    else {
        console.warn( "setZeroTimeout defaulting to setTimeout(...,0) !!!" );
        // if postMessage functionality is not available default to regular setTimeout
        mExport( "setZeroTimeout", function( fn )
        {
            setTimeout( fn, 0 );
        });
    }
})();


// export defer
(function()
{
    "use strict";

    var console = mImport( "console");
    var szt = mImport( "setZeroTimeout");

    function defer( fn, scope) {
        var args = Array.prototype.slice.call(arguments, 2);
        szt( function(){
            fn.apply( scope, args);
        });
    }

    mExport( "defer", defer);

})();


// export assert
(function()
{
    "use strict";

    function assert(condition, message) {
        if (!condition) {
            throw message || "Assertion failed";
        }
    }

    mExport( "assert", assert);

})();


