/*global performance, graph, mImport, mExport, $ *//*


// global variables
var glob = {
    varPrefix         : "/myVars",
    pingSentAt        : null,
    usedUpVarCount    : 0,
    continuePingPongFn: null,
    LastPingsMaxSize  : 10,
    lastPings         : []
};

// not all browsers have hig res performance timer...
if( typeof window.performance === "undefined" ) {
    window.performance = { now: Date.now.bind( Date ) };
}

var setZeroTimeout = mImport( "setZeroTimeout" );
var connector = mImport( "connector" );

// dummy logger with time-diff prefix
(function( scope )
{
    "use strict";

    var previousLogTime = performance.now();

    var scrollToBottomLater = (function()
    {
        var timeoutPending = false;
        return function()
        {
            if( timeoutPending ) {
                return;
            }
            timeoutPending = true;
            setZeroTimeout( function()
            {
                $( "#mylogdiv" ).scrollTop( $( "#mylogdiv" )[0].scrollHeight );
                timeoutPending = false;
            } );
        }
    })();

    var cursorAtBeginning = true;

    function common( pad, args )
    {
        var txt = "";
        if( cursorAtBeginning ) {
            txt = pad;
        }
        for( var i = 0 ; i < args.length ; i ++ ) txt += args[i] + ( i > 0 ? " " : "");
        var endsWithNL = txt.slice( - 1 ) === "\n";
        if( endsWithNL ) {
            txt = txt.slice( 0, - 1 );
        }
        txt = txt.split( "\n" ).join( "\n" + pad ) + ( endsWithNL ? "\n" : "");
        $( "#mylog" ).append( txt );
        cursorAtBeginning = endsWithNL;
    }

    function nextLine()
    {
        if( ! cursorAtBeginning ) {
            $( "#mylog" ).append( "\n" );
            cursorAtBeginning = true;
        }
    }

    scope.mylogln = function()
    {
        var now = performance.now();
        nextLine();
        common( formatMillis( now - previousLogTime, "+" ) + "ms:", arguments );
        nextLine();
        scrollToBottomLater();
        previousLogTime = performance.now();
    };

    scope.mylog = function()
    {
        common( "      ", arguments );
        scrollToBottomLater();
        previousLogTime = performance.now();
    };

    function formatMillis( t, prefix */
/* = "" *//*
, width */
/* = 10 *//*
 )
    {
        if( typeof width !== "number" ) {
            width = 6;
        }
        prefix = "";
        var res = prefix + Math.round( t );
        while( res.length < width ) {
            res = " " + res;
        }
        return res;
    }

})( window );
//
//// setTimeout(fn,0) does not really fire in 0 ms for 'backwards' compatibility reasons, or
//// some other such nonsense. This function will try to simulate 0ms timeout by sending itself
//// a message if the browser supports. If support is not available, it'll default to setTimeout
//setZeroTimeout = (function () {
//
//    if (window.addEventListener !== undefined && window.postMessage !== undefined) {
//
//        var timeouts = [];
//        var messageName = "fv-lib-zero-timeout-message";
//
//        window.addEventListener(
//            "message"
//            , function handleMessage(event) {
//                if (event.source == window && event.data == messageName) {
//                    event.stopPropagation();
//                    if (timeouts.length > 0) {
//                        var fn = timeouts.shift();
//                        fn();
//                    }
//                }
//            }
//            , true);
//
//        return function (fn) {
//            timeouts.push(fn);
//            window.postMessage(messageName, "*");
//        };
//    } else {
//        fv.console.warn("setZeroTimeout defaulting to setTimeout(...,0) !!!");
//        // if postMessage functionality is not available default to regular setTimeout
//        return function (fn) {
//            setTimeout(fn, 0);
//        }
//    }
//
//})();


// sets the number of state variables to the given number
// if the number is less than the current number, deleteTree() will be used to clear
// all vars, and then they are repopulated with the desired number
function setStateSize( num )
{
    if( glob.usedUpVarCount > num ) {
//        connector.clearState( glob.varPrefix);
        if( window.pureweb) {
            pureweb.getFramework().getState().getStateManager().deleteTree( glob.varPrefix );
        } else {
//            QtConnector.
        }
        glob.usedUpVarCount = 0;
    }
    while( glob.usedUpVarCount < num ) {
        var varName = "v" + glob.usedUpVarCount;
//        connector.setState(glob.varPrefix + "/" + varName, "x");
        connector.getSharedVar( glob.varPrefix + "/" + varName ).set( "x" );
        glob.usedUpVarCount ++;
    }
    mylogln( "State size: " + glob.usedUpVarCount + " vars." );
    updateStatus();
}

// update the stats bar at the bottom
function updateStatus()
{
    var lblColor = "pink";

    function lbl( txt )
    {
        return "<span style='color:" + lblColor + ";'>" + txt + "</span>";
    }

    var txt = "";
    txt += lbl( " Vars:" ) + glob.usedUpVarCount;
    txt += lbl( " Graph:" ) + graph.nPoints() + "pts";
    txt += lbl( " Last pings:" ) + "[";
    glob.lastPings.forEach( function( e, i )
    {
        txt += (i === 0 ? "" : " ") + e
    } );
    txt += "]";
    $( "#status" ).html( txt );
}

*/
/**
 * Graphing functions
 *
 * highly inefficient but working scatter-plot routines that display x,y data as dots.
 * It even has a primitive axes with labels :)
 *//*

(function( scope )
{

    var canvas, ctx;
    var minx = null, maxx, miny, maxy;
    var pts = [];
    var fontSize = 10;
    var dotRadius = 4;
    var txtColor = "#007";
    var axisColor = "#007";
    var margin = {
        left  : fontSize + 11,
        right : 10,
        top   : 5,
        bottom: fontSize + 10
    };

    scope.graph = {};
    scope.graph.init = function()
    {
        canvas = $( "#plotCanvas" )[0];
        ctx = canvas.getContext( '2d' );

        scope.graph.clear();
    };

    scope.graph.clear = function()
    {
        pts = [];
        minx = null;
        scope.graph.redraw();
    };

    // linear transformations
    function tx( x )
    {
        return (x - minx) / (maxx - minx) * (canvas.width - margin.left - margin.right - dotRadius * 2) + margin.left + dotRadius;
    }

    function ty( y )
    {
        return (1 - (y - miny) / (maxy - miny)) * (canvas.height - margin.top - margin.bottom - dotRadius * 2) + margin.top + dotRadius;
    }

    function htext( txt, x, y )
    {
        ctx.fillText( txt, x, y );
    }

    function vtext( txt, x, y )
    {
        ctx.save();
        ctx.translate( x, y );
        ctx.rotate( - Math.PI / 2 );
        ctx.fillText( txt, 0, 0 );
        ctx.restore();
    }

    function drawAxes()
    {
        ctx.save();
        ctx.font = fontSize.toString() + "pt Arial";
        ctx.strokeStyle = axisColor;
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.moveTo( margin.left - 3, 3 );
        ctx.lineTo( margin.left - 3, canvas.height - margin.bottom + 3 );
        ctx.lineTo( canvas.width - 3, canvas.height - margin.bottom + 3 );
        ctx.stroke();

        ctx.textAlign = "center";
        ctx.fillStyle = txtColor;
        htext( "nVars", (canvas.width - margin.left - margin.right) / 2, canvas.height - 3 );
        vtext( "PingPong time", fontSize + 2, (canvas.height - margin.top - margin.bottom) / 2 );

        ctx.textAlign = "left";
        var minxstr = "", maxxstr = "", minystr = "", maxystr = "";
        if( minx !== null ) {
            minystr = miny.toFixed( 3 ) + "ms";
            maxystr = maxy.toFixed( 3 ) + "ms";
            minxstr = minx;
            maxxstr = maxx;
        }
        htext( minxstr, margin.left, canvas.height - 3 );
        vtext( minystr, fontSize + 2, canvas.height - margin.bottom );
        ctx.textAlign = "right";
        htext( maxxstr, canvas.width - margin.right, canvas.height - 3 );
        vtext( maxystr, fontSize + 2, margin.top );
        ctx.restore();
    }

    scope.graph.redraw = function()
    {
        canvas.width = canvas.clientWidth;
        canvas.height = canvas.clientHeight;

        ctx.fillStyle = "#eee";
        ctx.fillRect( 0, 0, canvas.width, canvas.height );
        drawAxes();
        if( pts.length === 0 ) {
            return;
        }

        if( maxx - minx < 1e-6 ) {
            maxx += 1;
        }
        if( maxy - miny < 1e-6 ) {
            maxy += 1;
        }

        ctx.fillStyle = "rgba(64,0,0,0.5)";
        ctx.strokeStyle = "rgba(64,0,0,1)";
        ctx.lineWidth = 1;
        pts.forEach( function( e, i )
        {
            ctx.beginPath();
            ctx.arc( tx( e.x ), ty( e.y ), dotRadius, 0, Math.PI * 2, false );
            ctx.fill();
        } );
    };

    scope.graph.addPoint = function( x, y )
    {
        if( pts.length === 0 ) {
            minx = maxx = x;
            miny = maxy = y;
        }
        else {
            minx = Math.min( minx, x );
            maxx = Math.max( maxx, x );
            miny = Math.min( miny, y );
            maxy = Math.max( maxy, y );
        }
        pts.push( { x: x, y: y } );
        scope.graph.redraw();
    };

    scope.graph.nPoints = function()
    {
        return pts.length;
    };
})( window );

// ad a data point to the graph and to the status bar
function addPing( nVars, tDiff )
{
    graph.addPoint( nVars, tDiff );
    glob.lastPings.push( Math.round( tDiff ) );
    glob.lastPings = glob.lastPings.slice( - glob.LastPingsMaxSize );
    updateStatus();
}

// generic callback for the buttons in the html document.
// btw, can you tell I am in love with closures? :)
function buttonCB( command, arg1, arg2 )
{

    if( command === "start" ) {
        (function()
        {
            mylogln( "Running ping-pongs. Click 'Stop' to abort." );
            var count = 0;
            glob.continuePingPongFn = function( tDiff )
            {
                mylog( Math.round( tDiff ) );
                count ++;
                if( count % 10 === 0 ) {
                    mylog( "\n" );
                }
                else {
                    mylog( " " );
                }
                addPing( glob.usedUpVarCount, tDiff );
                return true;
            };
            // start it up
            sendPing();
        })();
        return;
    }

    if( command === "clearGraph" ) {
        graph.clear();
        updateStatus();
        return;
    }

    if( command === "stop" ) {
        glob.continuePingPongFn = function()
        {
            return false;
        };
        mylogln( "Stopping" );
        return;
    }

    if( command === "longTest" ) {
        (function( varInc )
        {
            mylogln( "Running long test:" );
            if( varInc === 1 ) {
                mylog( "5 pings and varCount increases by 2% + 1" );
            }
            else {
                mylog( "5 pings and varCount increases by 10" );
            }
            setStateSize( 0 );
            mylog( "vars=", glob.usedUpVarCount, ":" );
            var count = 1;
            glob.continuePingPongFn = function( tDiff )
            {
                mylog( " ", Math.round( tDiff ) );
                addPing( glob.usedUpVarCount, tDiff );
                if( count % 5 === 0 ) {
                    if( glob.usedUpVarCount > 3000 ) {
                        mylogln( "Long test done." );
                        return false;
                    }
                    var nextSize;
                    if( varInc === 1 )
                    {
                        nextSize = Math.round( glob.usedUpVarCount * 1.02 + 1 );
                    }
                    else {
                        nextSize = glob.usedUpVarCount + 10;
                    }
                    setStateSize( nextSize );
                }
                count ++;
                return true;
            };
            sendPing();
        })( arg1 );
        return;
    }

    if( command === "clearLog" ) {
        $( "#mylog" ).text( "Log cleared\n" );
        return;
    }

    if( command === "add" ) {
        setStateSize( glob.usedUpVarCount + arg1 );
        return;
    }

    if( command === "clearState" ) {
        setStateSize( 0 );
        return;
    }

    mylogln( "Unknown command " + command );
}

// when a PONG is received from the server, this is the function that gets called.
function pongHandler( e )
{
    // calculate how long it took since we sent the ping
    var tDiff = performance.now() - glob.pingSentAt;

    // should we continue running?
    if( ! glob.continuePingPongFn( tDiff ) ) {
        return;
    }

    // send another ping...
    // we must delay set state because if we'd do it directly from this callback,
    // the server would not get notified..., i.e. the following line would not work
    // sendPing();

    // So we have to delay it until after we exit the callback:
    setZeroTimeout( sendPing );
}

function pongHandler2()
{
    // calculate how long it took since we sent the ping
    var tDiff = performance.now() - glob.pingSentAt;

    // should we continue running?
    if( ! glob.continuePingPongFn( tDiff ) ) {
        return;
    }

    // send another ping...
    // we must delay set state because if we'd do it directly from this callback,
    // the server would not get notified..., i.e. the following line would not work
    // sendPing();

    // So we have to delay it until after we exit the callback:
    setZeroTimeout( sendPing );
}


// send a unique ping to the server
var sendPing = (function()
{
    var uniquePingId = 0;
    return function()
    {
        glob.pingSentAt = performance.now();
//        connector.setState(glob.varPrefix + "/ping", uniquePingId);
        connector.getSharedVar( glob.varPrefix + "/ping" ).set( uniquePingId );
        uniquePingId ++;
    };
})();


//connector.setConnectionCB( function( s )
//{
//    console.log( "connectionCB", connector, connector.getConnectionStatus() )
//} );
//connector.connect();
//

// setup various things when document is loaded
function onLoadCB()
{
    var console = mImport( "console");
    var defer = mImport( "defer");
    console.log( "onLoad running");

    connector.setConnectionCB( function( s )
    {
        console.log( "connectionCB", connector, connector.getConnectionStatus() )
    } );
    connector.connect();

    // test
//    $("#ccon1").append("<div class='yoyo'>hello world</div>");
//    var el = $(".yoyo")[0];
    var el = $("#ccon1")[0];

    console.log( "container = ", el);

    var view = connector.registerViewElement( el, "view1");
    view.updateSize();
    window.gview = view;


    // initialize layout
    $( 'body' ).layout( {
        applyDemoStyles : true,
        center__onresize: graph.redraw,
        center__size    : "70%",
        west__size      : 70,
        east__minSize   : 400,
        west__onresize  : view.updateSize.bind(view),
        west__onshow  : view.updateSize.bind(view),
        west__onopen  : view.updateSize.bind(view)
    } );

    defer( view.updateSize.bind(view));


    connector.getSharedVar( glob.varPrefix + "/pong" ).addCB( pongHandler2 );

    // initialize graph
    graph.init();

    // initialize tooltips
    $( function()
    {
        $( document ).tooltip();
    } );

    // update status
    updateStatus();

//    window.con = mImport("connector");
//    window.a1 = con.getSharedVar("x");
//    window.a2 = con.getSharedVar("x");
//    console.log( "a1cb1 = ", a1.addNamedCB( console.log.bind(console,"a1.cb1")));
//    console.log( "a1cb1 = ", a1.addNamedCB( console.log.bind(console,"a1.cb2")));
//    console.log( "a1cb1 = ", a2.addNamedCB( console.log.bind(console,"a2.cb1")));
//    console.log( "a1cb1 = ", a2.addNamedCB( console.log.bind(console,"a2.cb2")));
}


*/
