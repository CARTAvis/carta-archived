/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 10/04/13
 * Time: 7:34 PM
 *
 * Global State module
 *
 * It is responsible for managing _everything_ PureWeb related, including:
 *   - connecting-to/disconnecting-from server
 *   - registering/resizing views
 *   - listening to state changes
 *   - parsing state changes
 *   - setting state
 *   - sending commands
 */

"use strict";

/* JsHint options */
/* global pureweb */
/* global fv */

(function () {

    var DEBUG = true && fv.GLOBAL_DEBUG;
    DEBUG && fv.console.log( "GlobalStatePW module loading" );

    /**
     *
     * @param hub {fv.lib.Hub} hub on which to send/receive messages
     * @constructor
     */
    fv.GlobalStatePW = function ( hub ) {

        /**
         * @type {fv.lib.Hub}
         */
        this.m_hub = hub;
        this.m_dc = 0;

        this.m_lastSessionState = null;

        this.m_shareUrl = null;

        /**
         * Map of registered PureWeb views, the key is the PureWeb view name (NOT DOM ID!!!)
         * @type {{}}
         */
        this.m_views = {};

        // public members - describing the state

        /**
         * parsed state
         *
         * Do not replace this object, other objects are likely to keep a reference to this
         */
        this.parsedState = {};

        this.parsedState.mainWindow = {
            tx: { xa: 1, xb: 0, ya: 1, yb: 0}
        };

        this.parsedState.mainWindow.tx.txObj = {
            mouse2serverImagef: this._mainViewMouse2serverImagef.bind( this ),
            serverImage2mousef: this._mainViewServerImage2mousef.bind( this )
        };

//        this.parsedState.histogram = {
////            preset: - 1,
////            lowerClip: NaN,
////            upperClip: NaN,
////            frameMin: NaN,
////            frameMax: NaN,
////            nNaNs: 0,
////            binValue: NaN,
////            nValuesInBin: 0,
////            trueCount: NaN,
//            marker1: 0,
//            marker2: 0
//        };

        this.parsedState.movie = {
            currentFrame: 0,
            numFrames: 1,
            frameDelay: 200,
            status: "stopped",
            isBouncing: false,
            startFrame: 0,
            endFrame: 0,
            frameSkip: 1
        };

        this.parsedState.regionStats = {
            isNull: true,
            width: 1,
            height: 1,
            totalPixels: 1,
            nanPixels: 0,
            average: 0,
            sum: 0,
            rms: 0,
            min: 0,
            max: 0,
            bkgLevel: 0,
            sumMinusBkg: 0,
            maxMinusBkg: 0,
            masPos1: "n/a",
            maxPos2: "n/a",
            depth: 1,
            currentFrame: 1,
            nFramesComputed: 0,
            totalFluxDensity: 0,
            aboveBackground: 0,
            beamArea: 0
        };

        this.parsedState.gauss2dFit = {
            ng: 0,
            isNull: true,
            status: "n/a",
            chisq: "n/a",
            rms: "n/a",
            frame: 0,
            constantTerm: "n/a",
            params: [
                {
                    peak: "n/a",
                    centerPix: "n/a",
                    centerWCS: "n/a",
                    sizePix: "n/a",
                    angle: "n/a"
                }
            ]
        };

        this.parsedState.plot3d = {
            isNull: true,
            nRows: 0,
            nCols: 0,
            localMin: 0,
            localMax: 1,
            values: []
        };

        // array of flags indicating which fitters are active
        this.parsedState.g1dfitters = [];

        // we'll keep parsed g1d fitter states here
        this.parsedState.g1dfs = {};

        // we'll keep parsed qu profiles here
        this.parsedState.quprofiles = {};

        this.parsedState.fileShortcuts = [];

        this.parsedState.warning = {
            text: ""
        };
        this.parsedState.error = {
            text: ""
        };
        this.parsedState.cachedColormap = {
            data: [ 0x000000, 0xffffff],
            histMin: 0,
            histMax: 1
        };
    };

    /**
     * prefixed emit
     * @param path
     * @param data
     */
    fv.GlobalStatePW.prototype.emit = function ( pathSuffix, data ) {
        this.m_hub.emit( "gs." + pathSuffix, data );
    };

    /**
     * Starts up the global state module
     *   - starts up pure web connection to client
     *     - when connection is established, emits 'gs.connected' event to the hub
     *   - listens to state change events from PureWeb and emits them to the hub
     *   - listens to message from the hub and translates them to PureWeb command/state changes
     * @returns void
     */
    fv.GlobalStatePW.prototype.run = function () {
        // if the client was downloaded using an app URI then at this point location.href
        // will be the session URI to connect with. Otherwise client was downloaded using a view
        // URI, so construct the app URI (without the client) to connect with.
        var uri = location.href;
        if ( ! pureweb.getClient().isaSessionUri( uri ) ) {
            uri = location.protocol + '//' + location.host + '/pureweb/app?name=' + pureweb.getServiceAppName( uri );
        }

        var client = pureweb.getClient();

        pureweb.listen( client, pureweb.client.WebClient.EventType.CONNECTED_CHANGED,
            this.onConnectedChanged, false, this );

        // TODO: this should probably be a hub message received
        // setup the window.onbeforeunload callback to disconnect from the service application
        window.addEventListener( "beforeunload", function () {
            if ( client.isConnected() ) {
                client.disconnect( false );
            }
        }, true );

        // register all pureweb state listeners
        // ====================================
        var serverStateChangeListeners = [
//            [ "/title", this._titleChangedCB],
//            [ "/VisibleImageRect", this._visibleImageRectCB],
//            [ "/Status/dc", this._statusCB],
//            [ "/FITSHeader", this._fitsHeaderCB],
            [ "/fileShortcuts/num", this._fileShortcutsCB ],
            [ "/RS/stamp", this._regionStatsCB ],
            [ "/Gauss2d/stamp", this._gauss2dFitCB ],
            [ "/Plot3d/stamp", this._plot3dCB ],
            [ "/error/dc", this._errorCB ],
            [ "/warning/dc", this._warningCB ],
            [ "/g1dfs/ix/dc", this._parseG1DfitterState.bind( this, "ix" ) ],
            [ "/g1dfs/iy/dc", this._parseG1DfitterState.bind( this, "iy" ) ],
            [ "/g1dfs/iz/dc", this._parseG1DfitterState.bind( this, "iz" ) ],
            [ "/g1dfs/qz/dc", this._parseG1DfitterState.bind( this, "qz" ) ],
            [ "/g1dfs/uz/dc", this._parseG1DfitterState.bind( this, "uz" ) ],
            [ "/g1dfs/pi/dc", this._parseG1DfitterState.bind( this, "pi" ) ],
            [ "/g1dfs/pa/dc", this._parseG1DfitterState.bind( this, "pa" ) ],
            [ "/g1dfs/rs-nan/dc", this._parseG1DfitterState.bind( this, "rs-nan" ) ],
            [ "/g1dfs/rs-min/dc", this._parseG1DfitterState.bind( this, "rs-min" ) ],
            [ "/g1dfs/rs-max/dc", this._parseG1DfitterState.bind( this, "rs-max" ) ],
            [ "/g1dfs/rs-avg/dc", this._parseG1DfitterState.bind( this, "rs-avg" ) ],
            [ "/g1dfs/rs-sum/dc", this._parseG1DfitterState.bind( this, "rs-sum" ) ],
            [ "/g1dfs/rs-std/dc", this._parseG1DfitterState.bind( this, "rs-std" ) ],
            [ "/g1dfs/rs-bkg/dc", this._parseG1DfitterState.bind( this, "rs-bkg" ) ],
            [ "/g1dfs/rs-sumbkg/dc", this._parseG1DfitterState.bind( this, "rs-sumbkg" ) ],
            [ "/g1dfs/rs-maxbkg/dc", this._parseG1DfitterState.bind( this, "rs-maxbkg" ) ],
//            [ "/quprofile-1/dc", this._parseQUProfileState.bind( this, "1") ],
            [ "/cachedColormap/stamp", this._parseCachedColormap ],
            [ "/g1dFittersList/dc", this._g1dFittersListCB ]

        ];
        serverStateChangeListeners.forEach( function ( e ) {
            pureweb.getFramework().getState().getStateManager().addValueChangedHandler(
                e[0], e[1], this );
        }, this );
//        pureweb.getFramework().getState().getStateManager().addChildChangedHandler(
//            "/Histogram", this._parseHistogramStateCB, this );
        pureweb.getFramework().getState().getStateManager().addChildChangedHandler(
            "/Movie", this._parseMovieStateCB, this );

        // register all hub listeners
        var hubListeners = [
            // TODO: replace this with more specific messages in controller?
            [ "togs.parsedStateRequest", this._parsedStateRequestHandler ],
            [ "togs.mainWindow.grid.*", this._gridHandler ],
            [ "togs.mainWindow.setWCS", this._gridHandler ],
            [ "togs.mainWindow.mouseMove", this._mainWindowMouseMoveHandler ],
            [ "togs.mainWindow.mouseClick", this._mainWindowMouseClickHandler ],
            [ "togs.mainWindow.mouseWheel", this._mainWindowMouseWheelHandler ],
            [ "togs.mainWindow.zoomRect", this._mainWindowZoomRectHandler ],
            [ "togs.mainWindow.regionSet", this._regionSetHandler ],
            [ "togs.mainWindow.zoomPreset", this._mainWindowZoomPresetHandler ],
            [ "togs.mainWindow.keyDown", this._mainWindowKeyDownHandler ],
            [ "togs.mainWindow.setQuality", this._setViewQuality.bind( this, "MainView" ) ],
            [ "togs.view.register", this._registerViewHandler ],
            [ "togs.view.resize", this._resizeViewHandler ],
            [ "togs.colormap.preset", this._colormapPresetHandler ],
            [ "togs.colormap.reverse", this._colormapReverseHandler ],
            [ "togs.colormap.invert", this._colormapInvertHandler ],
            [ "togs.colormap.sliders", this._colormapSlidersHandler ],
            [ "togs.histogram.preset", this._histogramPresetHandler ],
            [ "togs.histogram.setMarkers", this._histogramSetMarkersHandler ],
            [ "togs.histogram.zoom", this._histogramZoomHandler ],
            [ "togs.histogram.logScale", this._histogramLogScaleHandler ],
            [ "togs.histogram.smoothGraph", this._histogramSmoothGraphHandler ],
            [ "togs.histogram.setCursor", this._histogramSetCursorHandler ],
            [ "togs.histogram.setVisible", this._histogramSetSetVisibleHandler ],
            [ "togs.loadFileShortcut", this._loadFileShortcutHandler ],
            [ "togs.movie.request", this._movieHandler ],
            [ "togs.g1df.*", this._g1dfHandler ],
            [ "togs.guprofile.*", this._quProfileHandler ],
            [ "togs.shareRequest", this._shareRequestHandler ],
//            [ "togs.fractionalPolarization", this._fractionalPolarizationHandler ],
            [ "togs.plot3d.*", this._plot3dHandler],
            [ "togs.directCommand", this._directCommandHandler]

        ];
        hubListeners.forEach( function ( e ) {
            this.m_hub.subscribe( e[0], e[1], this );
        }, this );

        //everything is setup and ready to go - connect
        console.log( "connecting to pureweb using uri = ", uri );
        pureweb.connect( uri );
    };

    fv.GlobalStatePW.prototype._pwget = function ( key ) {
        return pureweb.getFramework().getState().getValue( key );
    };

    fv.GlobalStatePW.prototype._pwset = function ( key, value ) {
        return pureweb.getFramework().getState().setValue( key, value );
    };

    fv.GlobalStatePW.prototype._pwsetdc = function ( key, value ) {
        var valuedc = value + " " + this.m_dc.toString();
        this.m_dc ++;
        return this._pwset( key, valuedc );
    };

    fv.GlobalStatePW.prototype._sendCommand = function ( cmd, data ) {
        pureweb.getClient().queueCommand( cmd, data );
    };

    /*
     fv.GlobalStatePW.prototype._visibleImageRectCB = function () {
     var st = this.parsedState.mainWindow.visibleImageRect;
     var s = this._pwget( "/VisibleImageRect");
     var a = s.split(" ");
     if(a.length != 4) {
     st.x1 = 0;
     st.x2 = 0;
     st.y1 = 0;
     st.y2 = 0;
     }
     else {
     st.x1 = parseFloat( a[0]);
     st.x2 = parseFloat( a[1]);
     st.y1 = parseFloat( a[2]);
     st.y2 = parseFloat( a[3]);

     }
     this.emit("mainWindow.visibleImageRect", st);
     };
     */

//    fv.GlobalStatePW.prototype._statusCB = function () {
//        this.parsedState.mainWindow.cursor.part1 = this._pwget("/Status/Cursor");
//        this.parsedState.mainWindow.cursor.part2 = this._pwget("/Status/Cursor2");
//        this.parsedState.mainWindow.cursor.value = this._pwget("/Status/Value");
//        this.parsedState.mainWindow.cursor.frameString = this._pwget("/Status/Frame");
//        this.parsedState.mainWindow.cursor.wcs = this._pwget("/Status/WCS");
//        this.parsedState.mainWindow.cursor.isOutside = this._pwget("/Status/isOutside") === "1";
//        this.parsedState.mainWindow.cursor.cursorFrozen = this._pwget("/Status/cursorFrozen") === "1";
//        this.parsedState.mainWindow.cursor.mouseX = parseInt( this._pwget("/Status/mouseX"));
//        this.parsedState.mainWindow.cursor.mouseY = parseInt( this._pwget("/Status/mouseY"));
//        this.emit("mainWindow.cursor", this.parsedState.mainWindow.cursor);
//    };

//    fv.GlobalStatePW.prototype._fitsHeaderCB = function () {
//        this.parsedState.header = this._pwget("/FITSHeader");
//        this.emit("header", this.parsedState.header);
//    };

    fv.GlobalStatePW.prototype._fileShortcutsCB = function () {
        DEBUG && fv.console.log( "_fileShortcutsCB is running...." );
        this.parsedState.fileShortcuts = [];
        var s = this._pwget( "/fileShortcuts/num" );
        if ( s === null ) return;
        var num = parseInt( s );
        if ( isNaN( num ) ) return;
        for ( var i = 0 ; i < num ; i ++ ) {
            s = this._pwget( "/fileShortcuts/file" + i.toString() );
            this.parsedState.fileShortcuts.push( s );
        }
        this.emit( "fileShortcuts", this.parsedState.fileShortcuts );
    };

//    fv.GlobalStatePW.prototype._parseHistogramStateCB = function ( e ) {
////        var DEBUG = true;
////        DEBUG && fv.console.log("child changed %O", e);
////        this.parsedState.histogram.lowerClip = this._pwget( "/Histogram/LowerClip" );
////        this.parsedState.histogram.upperClip = this._pwget( "/Histogram/UpperClip" );
////        this.parsedState.histogram.frameMin = this._pwget( "/Histogram/GlobalMin" );
////        this.parsedState.histogram.frameMax = this._pwget( "/Histogram/GlobalMax" );
////        this.parsedState.histogram.preset = this._pwget( "/Histogram/Index" );
////        this.parsedState.histogram.marker1 = this._pwget( "/Histogram/marker1" );
////        this.parsedState.histogram.marker2 = this._pwget( "/Histogram/marker2" );
////        this.parsedState.histogram.nNaNs = this._pwget( "/Histogram/nNaNs" );
////        this.parsedState.histogram.binValue = this._pwget( "/Histogram/binValue" );
////        this.parsedState.histogram.nValuesInBin = this._pwget( "/Histogram/nValuesInBin" );
////        this.parsedState.histogram.trueCount = this._pwget( "/Histogram/trueCount" );
//
//        this.emit( "histogram.changed", this.parsedState.histogram );
//    };

    fv.GlobalStatePW.prototype._parseMovieStateCB = function () {
        var DEBUG = false;
        DEBUG && fv.console.group( "parsing movie state" );
        var st = this.parsedState.movie; // shortcut
        st.currentFrame = parseInt( this._pwget( "/Movie/CurrentFrame" ) );
        st.numFrames = parseInt( this._pwget( "/Movie/TotalFrames" ) );
        st.frameDelay = parseInt( this._pwget( "/Movie/frameDelay" ) );
        st.isBouncing = this._pwget( "/Movie/isBouncing" ) === "true";
        st.startFrame = parseInt( this._pwget( "/Movie/startFrame" ) );
        st.endFrame = parseInt( this._pwget( "/Movie/endFrame" ) );
        st.frameSkip = parseInt( this._pwget( "/Movie/frameSkip" ) );
        st.status = this._pwget( "/Movie/status" );

        DEBUG && fv.console.log( "st = %O", st );

        this.emit( "movie.changed", this.parsedState.movie );
        DEBUG && fv.console.groupEnd();
    };

    fv.GlobalStatePW.prototype._regionStatsCB = function () {
        var DEBUG = false;
        DEBUG && fv.console.group( "parsing region stats" );
        var st = this.parsedState.regionStats; // shortcut

        st.isNull = this._pwget( "/RS/isNull" ) === "1";
        st.width = this._pwget( "/RS/width" );
        st.height = this._pwget( "/RS/height" );
        st.totalPixels = this._pwget( "/RS/totalPixels" );
        st.nanPixels = this._pwget( "/RS/nanPixels" );
        st.average = this._pwget( "/RS/average" );
        st.sum = this._pwget( "/RS/sum" );
        st.rms = this._pwget( "/RS/rms" );
        st.min = this._pwget( "/RS/min" );
        st.max = this._pwget( "/RS/max" );
        st.bkgLevel = this._pwget( "/RS/bkgLevel" );
        st.sumMinusBkg = this._pwget( "/RS/sumMinusBkg" );
        st.maxMinuxBkg = this._pwget( "/RS/maxMinusBkg" );
        /*
         st.maxPosX = this._pwget( "/RS/maxPos/x");
         st.maxPosY = this._pwget( "/RS/maxPos/y");
         st.maxPosExtra = this._pwget( "/RS/maxPos/extra");
         */
        st.maxPos1 = this._pwget( "/RS/maxPos/line1" );
        st.maxPos2 = this._pwget( "/RS/maxPos/line2" );

        st.depth = parseInt( this._pwget( "/RS/depth" ) );
        st.currentFrame = this._pwget( "/RS/currentFrame" );
        st.nFramesComputed = parseInt( this._pwget( "/RS/nFramesComputed" ) );
        st.totalFluxDensity = this._pwget( "/RS/totalFluxDensity" );
        st.aboveBackground = this._pwget( "/RS/aboveBackground" );
        st.beamArea = this._pwget( "/RS/beamArea" );

        DEBUG && fv.console.log( "st = %O", st );

        this.emit( "regionStats.changed", st );
        DEBUG && fv.console.groupEnd();
    };

    fv.GlobalStatePW.prototype._warningCB = function () {
        var st = this.parsedState.warning;
        st.text = this._pwget( "/warning/text" );
        this.emit( "warning.changed", st );
    };

    fv.GlobalStatePW.prototype._errorCB = function () {
        var st = this.parsedState.error;
        st.text = this._pwget( "/error/text" );
        this.emit( "error.changed", st );
    };

    fv.GlobalStatePW.prototype._gauss2dFitCB = function () {
        DEBUG && fv.console.group( "parsing gauss 2d results" );
        var st = this.parsedState.gauss2dFit; // shortcut

        st.isNull = this._pwget( "/Gauss2d/isNull" ) === "1";
        st.chisq = this._pwget( "/Gauss2d/chisq" );
        st.rms = this._pwget( "/Gauss2d/rms" );
        st.frame = this._pwget( "/Gauss2d/frame" );
        st.status = this._pwget( "/Gauss2d/status" );
        st.ng = parseInt( this._pwget( "/Gauss2d/ng" ) );

        var params = this._pwget( "/Gauss2d/params" );
        var i = 0;

        // parse the parameters
        var arr = params.split( " " );
        for ( i = 0 ; i < arr.length ; i ++ ) {
            arr[i] = arr[i].split( "_" ).join( " " );
        }
        if ( arr.length > 0 ) {
            st.constantTerm = arr[ arr.length - 1].toString();
        } else {
            st.constantTerm = "n/a";
        }

        st.params = [];
        for ( i = 0 ; i < st.ng ; i ++ ) {
            st.params.push( {
                peak: arr[i * 5 + 0],
                centerPix: arr[i * 5 + 1],
                centerWCS: arr[i * 5 + 2],
                sizePix: arr[i * 5 + 3],
                angle: arr[i * 5 + 4]
            } );
        }

        DEBUG && fv.console.log( "st = %O", st );

        this.emit( "gauss2dFit.changed", st );
        DEBUG && fv.console.groupEnd();
    };

    fv.GlobalStatePW.prototype._plot3dCB = function () {
        var DEBUG = false;
        DEBUG && fv.console.group( "parsing plot 3d results" );
        var st = this.parsedState.plot3d; // shortcut

        st.isNull = this._pwget( "/Plot3d/isNull" ) === "1";
        if ( ! st.isNull ) {
            st.nCols = parseInt( this._pwget( "/Plot3d/nCols" ) );
            st.nRows = parseInt( this._pwget( "/Plot3d/nRows" ) );
            st.localMin = parseFloat( this._pwget( "/Plot3d/localMin" ) );
            st.localMax = parseFloat( this._pwget( "/Plot3d/localMax" ) );
            st.values = [];
            var remaining = st.nCols * st.nRows;
            var chunk = 0;
            while ( remaining > 0 ) {
                var str = this._pwget( "/Plot3d/dataChunk" + chunk );
                if ( str == null ) {
                    fv.console.warn( "Bad data... null" );
                    break;
                }
                var a = str.split( " " );
                if ( a.length < 1 ) {
                    fv.console.warn( "Bad data... not enough" );
                    break;
                }
                for ( var i = 0 ; i < a.length ; i ++ ) {
                    var val = a[i];
                    if ( val === "n" ) val = Number.NaN; else val = parseFloat( val );
                    st.values.push( val );
                    remaining = remaining - 1;
                }
                chunk = chunk + 1;
            }
            if ( st.values.length !== st.nCols * st.nRows ) {
                fv.console.warn( "Bad data... mismatch", st.values.length, st.nCols * st.nRows );
                st.isNull = true;
            } else {
                this.emit( "plot3d.changed", st );
            }
        }
        if ( st.isNull ) {
            st.nCols = 1;
            st.nRows = 1;
            st.data = [0];
        }

        DEBUG && fv.console.log( "st = %O", st, chunk );
        this.emit( "plot3d.changed", st );
        DEBUG && fv.console.groupEnd();
    };

    fv.GlobalStatePW.prototype._g1dFittersListCB = function () {
        DEBUG = true;
        DEBUG && fv.console.group( "parsing g1d fitters" );

        var i, activeFlag, id;

        var st = this.parsedState.g1dfitters; // shortcut

        var num = parseInt( this._pwget( "/g1dFittersList/max" ) );
        DEBUG && fv.console.log( "number = ", num );

        // register parsers for all g1d fitters (even the non-active ones), but do this only once
        if ( this.m_g1dFittersListCBAlreadyExecuted !== true ) {
            this.m_g1dFittersListCBAlreadyExecuted = true;
            DEBUG && fv.console.log( "registering g1d fitter state monitors" );
            for ( i = 0 ; i < num ; i ++ ) {
                id = "g1d" + i.toString();
                pureweb.getFramework().getState().getStateManager().addValueChangedHandler(
                    "/g1dfs/" + id + "/dc",
                    this._parseG1DfitterState.bind( this, id ),
                    this );
            }
        }

        st.length = num;

        for ( i = 0 ; i < num ; i ++ ) {
            activeFlag = this._pwget( "/g1dFittersList/x" + i + "/active" ) === "1";
            st[i] = activeFlag;
        }

        this.emit( "g1dFittersList.changed", st );
        DEBUG && fv.console.groupEnd();
    };

    fv.GlobalStatePW.prototype._parseG1DfitterState = function ( id ) {
        var DEBUG = false;
        var prefix = "g1dfs";
        DEBUG && fv.console.group( "parsing " + prefix + "/" + id + " g1df" );
        var pre = prefix + "/" + id;
        var st = this.parsedState.g1dfs[id]; // shortcut
        if ( st == null ) {
            st = {};
            this.parsedState.g1dfs[id] = st;
        }
//        st.extraStatus = this._pwget( pre + "/extraStatus");
//        st.rms = this._pwget( pre + "/rms");
//        st.mean = this._pwget( pre + "/mean");
//        st.autoX = this._pwget( pre + "/autoX") === "1";
//        st.autoY = this._pwget( pre + "/autoY") === "1";
//        st.cursor1pos = this._pwget( pre + "/cursor1/pos");
//        st.cursor1val = this._pwget( pre + "/cursor1/val");
//        st.cursor1mpos = this._pwget( pre + "/cursor1/mpos");
//        st.cursor1mval = this._pwget( pre + "/cursor1/mval");
//        st.cursor1mpos = parseFloat( st.cursor1mpos);
//        st.cursor1mval = parseFloat( st.cursor1mval);
//        st.showCursor1 = this._pwget( pre + "/showCursor1") === "1";
        st.plotStyle = this._pwget( pre + "/plotStyle" );
//        st.title = this._pwget( pre + "/title") + Math.random();
//        st.isFitter = this._pwget( pre + "/isFitter") === "1";
        st.nGauss = parseInt( this._pwget( pre + "/nGauss" ) );
        st.nPoly = parseInt( this._pwget( pre + "/nPoly" ) );
//        st.heuristics = this._pwget( pre + "/heuristics") === "1";
//        st.manualMode = this._pwget( pre + "/manualMode") === "1";
//        st.showManipulationBars = this._pwget( pre + "/showManipulationBars") === "1";
//        st.showResidual = this._pwget( pre + "/showResidual") === "1";
//        st.showLabelPeaks = this._pwget( pre + "/showLabelPeaks") === "1";
//        st.useWCS = this._pwget( pre + "/useWCS") === "1";
//        st.info1 = this._pwget( pre + "/info1");
//        st.info2 = this._pwget( pre + "/info2");

        DEBUG && fv.console.log( "st = ", st );

        this.emit( "g1df.changed", { name: id, state: st } );
        DEBUG && fv.console.groupEnd();
    };
    /*

     fv.GlobalStatePW.prototype._parseQUProfileState = function ( id) {
     var DEBUG = true;
     var prefix = "quprofile-" + id;
     DEBUG && fv.console.group("parsing " + prefix);
     var st = this.parsedState.quprofiles[id]; // shortcut
     if( st == null) {
     st = {};
     this.parsedState.quprofiles[id] = st;
     }
     //        st.showCursor1 = this._pwget( prefix + "/showCursor1") === "1";
     //        st.plotStyle = this._pwget( prefix + "/plotStyle");
     st.title = this._pwget( prefix + "/title");
     st.q = this._pwget( prefix + "/q");
     st.u = this._pwget( prefix + "/u");
     st.qmean = this._pwget( prefix + "/qmean");
     st.umean = this._pwget( prefix + "/umean");
     st.frame = this._pwget( prefix + "/frame");

     DEBUG && fv.console.log("st = ", st);

     this.emit("quprofile.changed", { name: id, state: st });
     DEBUG && fv.console.groupEnd();
     };

     */

    fv.GlobalStatePW.prototype._parseCachedColormap = function () {
        var st = this.parsedState.cachedColormap;
        var str = this._pwget( "/cachedColormap/data" );
        st.data = [];
        if ( str < 12 && str.length % 6 !== 0 ) {
            fv.console.warn( "Bad cached colormap length =", str.length );
        } else {
            for ( var i = 0 ; i + 6 <= str.length ; i += 6 ) {
                var s = str.slice( i, i + 6 );
                st.data.push( parseInt( s, 16 ) );
            }
        }
        st.histMin = parseFloat( this._pwget( "/cachedColormap/histMin" ) );
        st.histMax = parseFloat( this._pwget( "/cachedColormap/histMax" ) );
        this.emit( "cachedColormap.changed", st );
    };

// hub message handlers
    fv.GlobalStatePW.prototype._gridHandler = function ( val, pth ) {
        if ( pth === "togs.mainWindow.grid.visible" ) {
            this._sendCommand( "SetWCS", { gridVisible: val ? "ON" : "OFF" } );
        }
        else if ( pth === "togs.mainWindow.grid.lineColor" ) {
            this._sendCommand( "SetWCS", { lineColor: val } );
        }
        else if ( pth === "togs.mainWindow.grid.textColor" ) {
            this._sendCommand( "SetWCS", { textColor: val } );
        }
        else if ( pth === "togs.mainWindow.setWCS" ) {
            this._sendCommand( "SetWCS", { wcs: val } );
        }
        else {
            DEBUG && fv.console.warn( "GS::_gridHandler - unknown msg: ", pth );
        }
    };
    fv.GlobalStatePW.prototype._parsedStateRequestHandler = function () {
        this.emit( "parsedState", this.parsedState );
    };
    fv.GlobalStatePW.prototype._mainWindowMouseMoveHandler = function ( val ) {
        var pt = this._viewToServer( val.viewName, { x: val.x, y: val.y } );
        this._pwset( "/requests/mainwindow-cursor", pt.x + " " + pt.y );
    };
    fv.GlobalStatePW.prototype._mainWindowMouseClickHandler = function ( val ) {
        var pt = this._viewToServer( val.viewName, { x: val.x, y: val.y } );
        pt.button = val.button;
        this._sendCommand( "MainView-click", pt );
    };
    fv.GlobalStatePW.prototype._mainWindowMouseWheelHandler = function ( val ) {
        var pt = this._viewToServer( val.viewName, { x: val.x, y: val.y } );
        this._sendCommand( "MainView-wheel", {
            x: pt.x, y: pt.y, delta: val.delta
        } );
    };
    fv.GlobalStatePW.prototype._mainWindowZoomRectHandler = function ( val ) {
        var pt1 = this._viewToServer( val.viewName, { x: val.x1, y: val.y1 } );
        var pt2 = this._viewToServer( val.viewName, { x: val.x2, y: val.y2 } );
        this._sendCommand( "MainView-rectangle", {
            x1: pt1.x, y1: pt1.y, x2: pt2.x, y2: pt2.y
        } );
    };
    fv.GlobalStatePW.prototype._mainWindowZoomPresetHandler = function ( val ) {
        if ( val.zoom < 0 )
            this._sendCommand( "SetSliderZoom", { zoom: val.zoom } );
        else
            this._sendCommand( "SetPixelZoom", { zoom: val.zoom } );
    };
    fv.GlobalStatePW.prototype._mainWindowKeyDownHandler = function ( val ) {
        this._sendCommand( "MainView-keyDown", {
            mouseX: val.mouseX,
            mouseY: val.mouseY,
            keyCode: val.keyCode
        } );
    };
    fv.GlobalStatePW.prototype._setViewQuality = function ( viewName, val ) {
        var params = {};
        if ( ! pureweb.getClient().supportsBinary() ) {
            params = {'UseBase64': true};
        }
        var type = pureweb.SupportedEncoderMimeType.JPEG;
        if ( val === 0 ) type = pureweb.SupportedEncoderMimeType.PNG;
        var quality = null;
        switch ( val ) {
            case 1:
                quality = 100;
                break;
            case 2:
                quality = 95;
                break;
            case 3:
                quality = 90;
                break;
            case 4:
                quality = 80;
                break;
            case 5:
                quality = 70;
                break;
            case 6:
                quality = 60;
                break;
            case 7:
                quality = 50;
                break;
            case 8:
                quality = 40;
                break;
            case 9:
                quality = 30;
                break;
            case 10:
                quality = 20;
                break;
            case 11:
                quality = 10;
                break;
        }

        var interactiveQuality = new pureweb.client.EncoderFormat( type, quality, params );
        var fullQuality = new pureweb.client.EncoderFormat( type, quality, params );
        var pw = this.m_views[ viewName];
        // if view does not exist, just set the cache
        if ( pw == null || pw.view == null ) {
            this.m_views[ viewName] = {
                view: null,
                interactiveQuality: interactiveQuality,
                fullQuality: fullQuality
            }
        }
        else {
            // otherwise set the actual quality and refresh
            pw.view.setEncoderConfiguration( new pureweb.client.EncoderConfiguration( interactiveQuality, fullQuality ) );
            pw.view.refresh();
        }
    };
    fv.GlobalStatePW.prototype._regionSetHandler = function ( val ) {
        if ( val.regionId === "regionStats" ) {
            this._pwset( "/requests/set-rs-region",
                val.x1 + " " + val.y1 + " " + val.x2 + " " + val.y2 );
        }
        else if ( val.regionId === "g2dFit" ) {
            this._pwset( "/requests/set-g2dfit-region",
                val.ng + " " + val.x1 + " " + val.y1 + " " + val.x2 + " " + val.y2 );
        }
        /*
         else if( val.regionId === "threeDsurfacePlot") {
         this._pwset( "/requests/set-threeDsurfacePlot-region",
         val.x1 + " " + val.y1 + " " + val.x2 + " " + val.y2 );
         }
         */
        else {
            fv.console.warn( "setRegion with unknown id=", val.regionId );
        }
    };

    fv.GlobalStatePW.prototype._registerViewHandler = function ( val ) {
        var DEBUG = false;
        var pw = this.m_views[ val.viewName];
        if ( pw == null ) {
            this.m_views[ val.viewName] = {};
        } else {
            if ( pw.view != null ) {
                throw new Error( "PureWebView '" + val.viewName + "' is already defined" );
            }
        }
        // create a PureWeb view
        pw = new pureweb.client.View( {id: val.id, viewName: val.viewName } );
        // support for safari... (browsers that don't support binary format?)
        var params = {};
        if ( ! pureweb.getClient().supportsBinary() ) {
            window.console.warn( "Using BASE64 on this browser..." );
            params = {'UseBase64': true};
        }

        // set the view's encoding parameters
        var interactiveQuality, fullQuality;
        if ( this.m_views[ val.viewName] != null ) {
            interactiveQuality = this.m_views[ val.viewName].interactiveQuality;
            fullQuality = this.m_views[ val.viewName].fullQuality;
        }
        if ( interactiveQuality == null || fullQuality == null ) {
            if ( val.format === "JPEG" ) {
                interactiveQuality = new pureweb.client.EncoderFormat( pureweb.SupportedEncoderMimeType.JPEG, 90, params );
                fullQuality = new pureweb.client.EncoderFormat( pureweb.SupportedEncoderMimeType.JPEG, 90, params );
            }
            else {
                interactiveQuality = new pureweb.client.EncoderFormat( pureweb.SupportedEncoderMimeType.PNG, null, params );
                fullQuality = new pureweb.client.EncoderFormat( pureweb.SupportedEncoderMimeType.PNG, null, params );
            }
        }
        pw.setEncoderConfiguration( new pureweb.client.EncoderConfiguration( interactiveQuality, fullQuality ) );

        pw.resize();
        this.m_views[ val.viewName] = {
            view: pw,
            fullQuality: fullQuality,
            interactiveQuality: interactiveQuality
        };

        // tell the server we have rendered the view
        pureweb.listen( pw, pureweb.client.View.EventType.VIEW_UPDATED,
            function ( e ) {
                var params = e.args.getEncodingParameters();
                if ( params.updateID != null ) {
                    this._pwset( "/requests/viewUpdated/" + pw.getViewName(), params.updateID );
//                    fv.console.log("/requests/viewUpdated/" + pw.getViewName(), params.updateID);
                }
            }, false, this );

        // Special case for MainView
        // for MainView there are transformation parameters attached, save them and emit an
        // event
        if ( val.viewName === "MainView" ) {
            var mvu = 1;
            pureweb.listen( pw, pureweb.client.View.EventType.VIEW_UPDATED,
                function ( e ) {
                    DEBUG && fv.console.log( "this = %O", this );
                    DEBUG && fv.console.log( "e = %O", e );
                    var params = e.args.getEncodingParameters();
                    this.parsedState.mainWindow.tx.xa = parseFloat( params.txa );
                    this.parsedState.mainWindow.tx.xb = parseFloat( params.txb );
                    this.parsedState.mainWindow.tx.ya = parseFloat( params.tya );
                    this.parsedState.mainWindow.tx.yb = parseFloat( params.tyb );
                    this.emit( "mainWindow.txChanged", this.parsedState.mainWindow.tx.txObj );
                    fv.console.log( "mainview update ", mvu );
                    mvu ++;
                }, false, this );
        }

    };
    fv.GlobalStatePW.prototype._resizeViewHandler = function ( val ) {
        var pw = this.m_views[ val.viewName];
        if ( pw == null || pw.view == null ) {
            throw new Error( "PureWebView '" + val.id + "' is not yet defined" );
        }
        pw = pw.view;
        pw.resize();
    };

    fv.GlobalStatePW.prototype._colormapPresetHandler = function ( val ) {
        this._pwset( "/Colormap/Preset", val );
    };
    fv.GlobalStatePW.prototype._colormapReverseHandler = function ( val ) {
        this._pwset( "/Colormap/Reverse", val ? "1" : "0" );
    };
    fv.GlobalStatePW.prototype._colormapInvertHandler = function ( val ) {
        this._pwset( "/Colormap/Invert", val ? "1" : "0" );
    };
    fv.GlobalStatePW.prototype._colormapSlidersHandler = function ( val ) {
        this._pwset( "/Colormap/Red", val.red );
        this._pwset( "/Colormap/Green", val.green );
        this._pwset( "/Colormap/Blue", val.blue );
        this._pwset( "/Colormap/scale1", val.s1 );
        this._pwset( "/Colormap/scale2", val.s2 );
    };

    fv.GlobalStatePW.prototype._histogramPresetHandler = function ( val ) {
        this._sendCommand( "histogram",
            { cmd: "preset", index: val.index, zoom: val.zoom } );
    };
    fv.GlobalStatePW.prototype._histogramSetMarkersHandler = function ( val ) {
        this._sendCommand( "histogram",
            { cmd: "setMarkers", x1: val.marker1, x2: val.marker2 } );
    };
    fv.GlobalStatePW.prototype._histogramZoomHandler = function ( val ) {
        var cmd = val ? "zoom" : "unzoom";
        this._sendCommand( "histogram", { cmd: cmd } );
    };
    fv.GlobalStatePW.prototype._histogramLogScaleHandler = function ( val ) {
        this._sendCommand( "histogram", { cmd: "logScale", val: val } );
    };
    fv.GlobalStatePW.prototype._histogramSmoothGraphHandler = function ( val ) {
        this._sendCommand( "histogram", { cmd: "smoothGraph", val: val } );
    };
    fv.GlobalStatePW.prototype._histogramSetCursorHandler = function ( val ) {
        this._sendCommand( "histogram", { cmd: "setCursor", x: val } );
    };
    fv.GlobalStatePW.prototype._histogramSetSetVisibleHandler = function ( val ) {
        this._sendCommand( "histogram", { cmd: "setVisible", val: val } );
    };
    fv.GlobalStatePW.prototype._loadFileShortcutHandler = function ( index ) {
        this._sendCommand( "LoadFileShortcut", { index: index } );
    };
    fv.GlobalStatePW.prototype._movieHandler = function ( val ) {
        DEBUG && fv.console.log( "GS::_movieHandler - msg: %O", val );
        if ( val.cmd === "setFrame" ) {
            this._pwsetdc( "/requests/movie/frame", val.val );
        }
        else if ( val.cmd === "play" ) {
            this._sendCommand( "/movie/command", { cmd: "play"} );
        }
        else if ( val.cmd === "rplay" ) {
            this._sendCommand( "/movie/command", { cmd: "rplay"} );
        }
        else if ( val.cmd === "stop" ) {
            this._sendCommand( "/movie/command", { cmd: "stop"} );
        }
        else if ( val.cmd === "setFrameDelay" ) {
            this._sendCommand( "/movie/command", { cmd: "setFrameDelay", val: val.val } );
        }
        else if ( val.cmd === "setStartFrame" ) {
            this._sendCommand( "/movie/command", { cmd: "setStartFrame", val: val.val } );
        }
        else if ( val.cmd === "setEndFrame" ) {
            this._sendCommand( "/movie/command", { cmd: "setEndFrame", val: val.val } );
        }
        else if ( val.cmd === "setFrameSkip" ) {
            this._sendCommand( "/movie/command", { cmd: "setFrameSkip", val: val.val } );
        }
        else if ( val.cmd === "setBouncing" ) {
            this._sendCommand( "/movie/command", { cmd: "setBouncing", val: val.val } );
        }
        else {
            DEBUG && fv.console.warn( "GS::_movieHandler - unknown msg: %O", val );
        }
    };

    fv.GlobalStatePW.prototype._g1dfHandler = function ( val, pth ) {
        var a = pth.split( "." );
        fv.assert( a != null && a.length > 0 );
        var cmd = a[ a.length - 1];

        if ( cmd === "invokeG1Dfit" ) {
            this._sendCommand( "invokeG1Dfit", { id: val.id} );
        }
        else if ( cmd === "releaseG1Dfit" ) {
            this._sendCommand( "releaseG1Dfit", { id: val.id} );
        }
        else if ( cmd === "uiAction" ) {
            // special case for cursor (updates could be too frequent, so use xml state)
            if ( val.cmd === "cursor1mouse" ) {
                this._pwset( "/requests/g1df/" + val.id + "/mouseCursor1", "" + val.val1 );
            }
            else {
                this._sendCommand( "g1df-" + val.id, val );
            }
        }

        else {
            DEBUG && fv.console.warn( "GS::_g1dfHandler - unknown msg: ", pth, val );
        }
    };

    fv.GlobalStatePW.prototype._quProfileHandler = function ( val, pth ) {
        var a = pth.split( "." );
        fv.assert( a != null && a.length > 0 );
        // command is the last part of the message
        var cmd = a[ a.length - 1];

        if ( cmd === "uiAction" ) {
            // special case for cursor (updates could be too frequent, so use xml state)
            if ( val.cmd === "mouseHover" ) {
                var str = "" + val.val1 + " " + val.val2;
                this._pwset( "/requests/quprofile-" + val.id + "/mouseHover",
                    "" + val.val1 + " " + val.val2 );
//                fv.console.log("uiAction mouseHover", val);
//                fv.console.log("->", str);
            }
            else {
                this._sendCommand( "quprofile-" + val.id, val );
            }
        }
        else {
            DEBUG && fv.console.warn( "GS::_QUHandler - unknown msg: ", pth, val );
        }
    };

    fv.GlobalStatePW.prototype._plot3dHandler = function ( val, pth ) {
        var a = pth.split( "." );
        fv.assert( a != null && a.length > 0 );
        var cmd = a[ a.length - 1];

        if ( cmd === "setRegion" ) {
            this._pwset( "/requests/plot3d/setRegion",
                val.x1 + " " + val.y1 + " " + val.x2 + " " + val.y2 );
        }
        else {
            DEBUG && fv.console.warn( "GS::_plot3dHandler - unknown msg: ", pth, val );
        }
    };

    fv.GlobalStatePW.prototype._shareRequestHandler = function () {
        var webClient = pureweb.getFramework().getClient();
        var that = this;
        if ( this.m_shareUrl == null ) {
            webClient.getSessionShareUrlAsync(
                '', '', 1800000, '', function ( getUrl, exception ) {
                    fv.console.warn( 'shareurlcb', getUrl, exception, arguments );
                    if ( getUrl != null ) {
                        getUrl = getUrl + "?client=html5&share_password=";
                        that.m_shareUrl = getUrl;
                        window.prompt( "Here is your collaboration URL:", getUrl );
                        that.emit( "gs.shareUrl", getUrl );
                    } else {
                        alert( 'An error occurred creating the share URL: ' + exception.description );
                    }
                } );
        } else {
            //If a share URL already exists, we just want to invalidate it
            webClient.invalidateSessionShareUrlAsync( this.m_shareUrl, function ( exception ) {
                if ( exception != null ) {
                    alert( 'An error occurred invalidating the share URL: ' + exception );
                } else {
                    that.m_shareUrl = null;
                }
            } );
        }
    };

//    fv.GlobalStatePW.prototype._fractionalPolarizationHandler = function ( data ) {
//        this._pwset( "/fractionalPolarization", data.val );
//    };

    /**
     * Convert screen client coordinates to server screen coordinates
     * @param viewName
     * @param pt
     * @returns {*}
     * @private
     */
    fv.GlobalStatePW.prototype._viewToServer = function ( viewName, pt ) {
        var pw = this.m_views[ viewName];
        if ( pw == null || pw.view == null ) {
            return null;
        }
        pw = pw.view;
//        var res = pw.viewToImage({X: pt.x, Y: pt.y});
//        return { x: res.X, y: res.Y };
        var res = pw.sourceToLocal( pt );
        return { x: res.x, y: res.y };
    };

    /**
     * Convert from local mouse coordinates to server image coordinates (floating point)
     * @param mousePt
     * @private
     */
    fv.GlobalStatePW.prototype._mainViewMouse2serverImagef = function ( mousePt ) {
        var t = this.parsedState.mainWindow.tx;
        var serverPt = this._viewToServer( "MainView", mousePt );
        var serverImagePt = {
            x: (serverPt.x - t.xb) / t.xa,
            y: (serverPt.y - t.yb) / t.ya
        };
        return serverImagePt;
    };

    /**
     * Convert from server image coordinates to mouse coordinates.
     * For example, (0,0) will return the top left coordinates of the first pixel in the image.
     * @param mousePt
     * @private
     */
    fv.GlobalStatePW.prototype._mainViewServerImage2mousef = function ( serverImagePt ) {
        var t = this.parsedState.mainWindow.tx;
        var serverPt = {
            x: t.xa * serverImagePt.x + t.xb,
            y: t.ya * serverImagePt.y + t.yb
        };
        var mousePt = this._serverToView( "MainView", serverPt );
        return mousePt;
    };

    /**
     * Convert from server screen coordinates to client coordinates
     * @param viewName
     * @param pt
     * @returns {*}
     * @private
     */
    fv.GlobalStatePW.prototype._serverToView = function ( viewName, pt ) {
        var pw = this.m_views[ viewName];
        if ( pw == undefined ) {
            return null;
        }
        pw = pw.view;
        if ( pw == null ) {
            return null;
        }
        var res = pw.localToSource( pt );
        return { x: res.x, y: res.y };
    };

//Connected changed event handler - creates the ScribbleView View instance and initializes the
//diagnostics panel (if it is present).
    fv.GlobalStatePW.prototype.onConnectedChanged = function ( e ) {
        fv.console.log( "GlobalStatePW:onConnectedChanged", e.target.isConnected(), e );
        if ( e.target.isConnected() ) {
            //register event listeners for connection stalled and session state failed events
            var client = pureweb.getClient();
            pureweb.listen( client, pureweb.client.WebClient.EventType.STALLED_CHANGED,
                this.onStalledChanged, false, this );
            pureweb.listen( client, pureweb.client.WebClient.EventType.SESSION_STATE_CHANGED,
                this.onSessionStateChanged, false, this );
            this.emit( "connected" );
        }
    };

//Session state changed event handler - checks for the failed state.
    fv.GlobalStatePW.prototype.onSessionStateChanged = function ( event ) {
        fv.console.log( "GlobalStatePW:onSessionStateChanged", event );
        var sessionState = pureweb.getClient().getSessionState();
        fv.console.log( "onSessionChanged", event, sessionState );
        if ( sessionState === pureweb.client.SessionState.FAILED ) {
            if ( this.m_lastSessionState === pureweb.client.SessionState.CONNECTING ) {
                // we have never actually connected
                this.emit( "connectionFailed" );
                fv.console.warn( 'Unable to connect to the Scribble service application.' );
            }
            else {
                // we were connected, but now we are not
                this.emit( "connectionLost" );
                fv.console.warn( 'Connection to the server has been lost. Refresh the page to restart.' );
            }
        }
        this.m_lastSessionState = sessionState;
    };

//Stalled state changed event handler - logs a message indicating if the connection to the service
//application has entered the stalled state, or whether it has recovered.
    fv.GlobalStatePW.prototype.onStalledChanged = function ( event ) {
        if ( pureweb.getClient().isStalled() ) {
            this.emit( "connection.stalled" );
            fv.console.log( 'Connection to the Scribble service application has stalled and may have been lost.' );
        }
        else {
            this.emit( "connection.unstalled" );
            fv.console.log( 'Connection to the Scribble service application has recovered.' );
        }
    };


    fv.GlobalStatePW.prototype._directCommandHandler = function ( data ) {
        this._sendCommand( data.cmd, data );
    };

})();

