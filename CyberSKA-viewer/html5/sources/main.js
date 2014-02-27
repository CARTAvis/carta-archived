"use strict";

/* global fv, goog */

(function () {

    /** @const */
    var DEBUG = true && fv.GLOBAL_DEBUG;

    /**
     * @constructor
     */
    fv.MainController = function () {
        DEBUG && fv.console.info("Creating main controller");
        this.pwViews = {};

        // we need to cache the region & number of gaussians since they are being
        // set via two different UI modules, and the server currently accepts them
        // in a single message
        this.m_g2d = {
            x1: 0, y1: 0, x2: 0, y2: 0,
            ng: 0, regionId: "g2dFit"
        };
    };

    /**
     *
     */
    fv.MainController.prototype.run = function () {

        // create a central hub
        DEBUG && fv.console.group("Main controller running");
        fv.hub = new fv.lib.Hub("main-fv-hub");

        // create the global state module and ask it to load
        fv.gs = new fv.GlobalStatePW(fv.hub);
        fv.gs.run();

        // listen for UI bootstrap event
        fv.hub.subscribe("ui.bootstrapped", function (ui) {
            DEBUG && fv.console.info("GUI has been bootstrapped", arguments);

            fv.hub.emit("toui.run");
//            // TODO: do we need this? remember the ui pointer
//            this.ui = ui;
        }, this);

        // listen for ui created event
        fv.hub.subscribe("ui.created", function () {
            DEBUG && fv.console.info("GUI has been created", arguments);
        }, this);

        {
            fv.hub.forward("ui.pureWebView.created", "togs.view.register");
            fv.hub.forward("ui.pureWebView.resized", "togs.view.resize");
//            fv.hub.forward("gs.header", "toui.setHeader");

            // TODO: this is not the way to do things... remove & replace with targeted functionality
            fv.hub.forward("ui.parsedStateRequest", "togs.parsedStateRequest");
            fv.hub.forward("gs.parsedState", "toui.parsedState");

//            fv.hub.forward("gs.mainWindow.cursor", "toui.mainWindow.cursor");
            fv.hub.forward("gs.mainWindow.txChanged", "toui.mainWindow.setTx");
            fv.hub.forward("ui.mainWindow.grid.visible", "togs.mainWindow.grid.visible");
            fv.hub.forward("ui.mainWindow.grid.lineColor", "togs.mainWindow.grid.lineColor");
            fv.hub.forward("ui.mainWindow.grid.textColor", "togs.mainWindow.grid.textColor");
            fv.hub.forward("ui.mainWindow.keyDown", "togs.mainWindow.keyDown");
            fv.hub.forward("ui.mainWindow.mouseMove", "togs.mainWindow.mouseMove");
            fv.hub.forward("ui.mainWindow.mouseClick", "togs.mainWindow.mouseClick");
            fv.hub.forward("ui.mainWindow.mouseWheel", "togs.mainWindow.mouseWheel");
            fv.hub.forward("ui.mainWindow.zoomRect", "togs.mainWindow.zoomRect");
            fv.hub.forward("ui.mainWindow.zoomPreset", "togs.mainWindow.zoomPreset");
            fv.hub.forward("ui.mainWindow.setWCS", "togs.mainWindow.setWCS");
//            fv.hub.forward("gs.mainWindow.visibleImageRect", "ui.mainWindow.visibleImageRect");
            fv.hub.forward("ui.mainWindow.setQuality", "togs.mainWindow.setQuality");
            fv.hub.forward("ui.mainSideBar.shareRequest", "togs.shareRequest");
//            fv.hub.forward("ui.mainSideBar.fractionalPolarization", "togs.fractionalPolarization");

            //            fv.hub.forward("ui.mainWindow.regionSet", "togs.mainWindow.regionSet");
            fv.hub.subscribe("ui.mainWindow.regionSet", function (val) {
                if (val.regionId == this.m_g2d.regionId) {
                    this.m_g2d.x1 = val.x1;
                    this.m_g2d.x2 = val.x2;
                    this.m_g2d.y1 = val.y1;
                    this.m_g2d.y2 = val.y2;
                    fv.hub.emit("togs.mainWindow.regionSet", this.m_g2d);
                }
                else if (val.regionId == "regionStats") {
//                else {
                    fv.hub.emit("togs.mainWindow.regionSet", val);
                }
//                else if (val.regionId == "threeDsurfacePlot") {
//                    fv.hub.emit("toui.plot3d.regionSet", val);
//                }
                else {
                    // ignore the rest
                }
            }, this);

            // TODO: this should be refactored
            // g2d should listen to window.regionSet messages instead, instead of this
            // cumbersome way of storing state info by the main controller...
            fv.hub.subscribe("ui.g2d.setNG", function (val) {
                this.m_g2d.ng = val;
                fv.hub.emit("togs.mainWindow.regionSet", this.m_g2d);
            }, this);
        }

        {
            fv.hub.forward("ui.colormap.preset", "togs.colormap.preset");
            fv.hub.forward("ui.colormap.invert", "togs.colormap.invert");
            fv.hub.forward("ui.colormap.reverse", "togs.colormap.reverse");
            fv.hub.forward("ui.colormap.sliders", "togs.colormap.sliders");
        }

        {
            fv.hub.forward("ui.histogram.preset", "togs.histogram.preset");
            fv.hub.forward("ui.histogram.setMarkers", "togs.histogram.setMarkers");
            fv.hub.forward("ui.histogram.setCursor", "togs.histogram.setCursor");
            fv.hub.forward("ui.histogram.zoom", "togs.histogram.zoom");
            fv.hub.forward("ui.histogram.logScale", "togs.histogram.logScale");
            fv.hub.forward("ui.histogram.smoothGraph", "togs.histogram.smoothGraph");
            fv.hub.forward("ui.histogram.setVisible", "togs.histogram.setVisible");
//            fv.hub.forward("gs.histogram.changed", "toui.histogram.sync");
        }

        {
            fv.hub.forward("ui.loadFileShortcut", "togs.loadFileShortcut");
        }

//        fv.hub.forward("ui.moviePlayer.playForward", "togs.moviePlayer.playForward");
//        fv.hub.forward("ui.moviePlayer.playBackwards", "togs.moviePlayer.playBackwards");
//        fv.hub.forward("ui.moviePlayer.stopPlaying", "togs.moviePlayer.stopPlaying");

        // movie control messages
        // ======================
        {
            // when movie player is ready, send it the current state (if we have it)
            fv.hub.subscribe("ui.moviePlayer.isReady", function () {
                if (this.m_parsedState !== null)
                    fv.hub.emit("toui.moviePlayer.setState", this.m_parsedState.movie);
            }, this);
            fv.hub.forward("ui.moviePlayer.request", "togs.movie.request");
            fv.hub.forward("gs.movie.changed", "toui.moviePlayer.setState");
        }

        // region stats messages
        // ======================
        {
            fv.hub.subscribe("ui.regionStats.isReady", function () {
                if (this.m_parsedState !== null)
                    fv.hub.emit("toui.regionStats.setState", this.m_parsedState.regionStats);
            }, this);
//            fv.hub.forward("ui.regionStats.request", "togs.regionStats.request");
            fv.hub.forward("gs.regionStats.changed", "toui.regionStats.setState");
        }

        // gauss 2d fit message
        // ======================
        {
            fv.hub.subscribe("ui.g2dWindow.isReady", function () {
                if (this.m_parsedState !== null)
                    fv.hub.emit("toui.g2dWindow.setState", this.m_parsedState.gauss2dFit);
            }, this);
            fv.hub.forward("gs.gauss2dFit.changed", "toui.g2dWindow.setState");
        }

        // 3d plot message
        // ======================
        {
            // cache some state info coming from gs in case we receive it before plot3d is ready
            this.m_plot3d = {
                isReady: false,
                setState: null,
                regionSet: null,
                cachedColormap: null
            };
            fv.hub.subscribe( "gs.plot3d.changed", function(val) {
                this.m_plot3d.setState = val;
                if( this.m_plot3d.isReady) {
                    fv.hub.emit( "toui.plot3d.setState", this.m_plot3d.setState);
                }
            }, this);
            fv.hub.subscribe( "ui.mainWindow.regionSet", function (val) {
                if (val.regionId !== "threeDsurfacePlot") return;
                this.m_plot3d.regionSet = val;
                if( this.m_plot3d.isReady) {
                    fv.hub.emit( "toui.plot3d.regionSet", this.m_plot3d.regionSet);
                }
            }, this);

            fv.hub.subscribe( "gs.cachedColormap.changed", function(val) {
                this.m_plot3d.cachedColormap = val;
                if( this.m_plot3d.isReady) {
                    fv.hub.emit( "toui.plot3d.setCachedColormap", this.m_plot3d.cachedColormap);
                }
            }, this);

            fv.hub.subscribe("ui.plot3d.isReady", function () {
                this.m_plot3d.isReady = true;
                // send whatever cached info we have
                if( this.m_plot3d.setState != null) {
                    fv.hub.emit( "toui.plot3d.setState", this.m_plot3d.setState);
                }
                if( this.m_plot3d.regionSet != null) {
                    fv.hub.emit( "toui.plot3d.regionSet", this.m_plot3d.regionSet);
                }
                if( this.m_plot3d.cachedColormap != null) {
                    fv.hub.emit( "toui.plot3d.setCachedColormap", this.m_plot3d.cachedColormap);
                }
            }, this);
//            fv.hub.forward("gs.plot3d.changed", "toui.plot3d.setState");
//            fv.hub.forward("gs.cachedColormap.changed", "toui.plot3d.setCachedColormap");

            fv.hub.forward("ui.plot3d.setRegion", "togs.plot3d.setRegion");
        }

        // profile/g1df updates
        // ======================
        {
            fv.hub.forward("gs.g1df.changed", "toui.g1df.changed");
            fv.hub.forwardMulti("ui.g1df.*", "togs.g1df.*");
            fv.hub.forward("gs.g1dFittersList.changed", "toui.g1dFittersList.changed");

            fv.hub.forwardMulti("ui.quprofile.*", "togs.guprofile.*");
//            fv.hub.forward("gs.quprofile.changed", "toui.quprofile.changed");
        }

        // listen for parsed state messages from the global state and send it to everyone
        // that is known to want it
        // TODO: this should not be necessary
        fv.hub.subscribe("gs.parsedState", function (parsedState) {
            this.m_parsedState = parsedState;
            fv.hub.emit("toui.moviePlayer.setState", this.m_parsedState.movie);
        }, this);

        // tell global state to send us the parsed state
        fv.hub.emit("togs.parsedStateRequest");

        DEBUG && fv.console.groupEnd();
    }

    // startup the application
    fv.mc = new fv.MainController();
    fv.mc.run();


    DEBUG && fv.console.log("releasing hub buffer");
    fv.hub.releaseBuffer();

})();

( function() {

    THREE.ParametricGeometry2 = function ( func, nCols, nRows ) {

        THREE.Geometry.call( this );

        var verts = this.vertices;
        var faces = this.faces;
        var colors = this.colors;
        var uvs = this.faceVertexUvs[ 0 ];

        var row, col, p, x, y;
        var size = 100 / Math.max( nRows, nCols);
        var sliceCount = nCols + 1;

        for ( row = 0; row <= nRows; row ++ ) {
            for ( col = 0; col <= nCols; col ++ ) {
                p = func( row, col );
                x = (col - nCols/2) * size;
                y = (row - nRows/2) * size;
                verts.push( new THREE.Vector3( x, y, p.z));
                colors.push( new THREE.Color( p.color));
            }
        }

        var a, b, c, d;
        var uva, uvb, uvc, uvd;

        for ( row = 0; row < nRows; row ++ ) {

            for ( col = 0; col < nCols; col ++ ) {

                a = row * sliceCount + col;
                b = row * sliceCount + col + 1;
                c = (row + 1) * sliceCount + col + 1;
                d = (row + 1) * sliceCount + col;

                uva = new THREE.Vector2( col / nCols, row / nRows );
                uvb = new THREE.Vector2( ( col + 1 ) / nCols, row / nRows );
                uvc = new THREE.Vector2( ( col + 1 ) / nCols, ( row + 1 ) / nRows );
                uvd = new THREE.Vector2( col / nCols, ( row + 1 ) / nRows );

                faces.push( new THREE.Face3( a, b, d ) );
                uvs.push( [ uva, uvb, uvd ] );

                faces.push( new THREE.Face3( b, c, d ) );
                uvs.push( [ uvb.clone(), uvc, uvd.clone() ] );

            }

        }

        this.computeCentroids();
        this.computeFaceNormals();
        this.computeVertexNormals();

    };

    THREE.ParametricGeometry2.prototype = Object.create( THREE.Geometry.prototype );

})();

(function() {
    fv.supports = fv.supports || {};
    fv.supports.webgl = false;
    var canvas, ctx;
    try {
        canvas = document.createElement('canvas');
        ctx = canvas.getContext('webgl') || canvas.getContext('experimental-webgl');
    }
    catch (e) {
        ctx = null;
    }
    fv.supports.webgl = ( ctx != null);
    fv.console.log( "Webgl support:", fv.supports.webgl ? "Yes" : "No");
    if( false && fv.supports.webgl) {
        var extensions = ctx.getSupportedExtensions();
        for (var i = 0; i < extensions.length; i ++ ) {
            fv.console.log( "Extension ", i, extensions[i]);
        }
    }

    // help out garbage collector
    canvas = undefined;
    ctx = undefined;
})();

(function(){
    fv.images = fv.images || {};
    fv.images.checkerboard = "data:image/jpg;base64,/9j/4AAQSkZJRgABAQAAAQABAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRofHh0aHBwgJC4nICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/2wBDAQkJCQwLDBgNDRgyIRwhMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjL/wAARCAEAAQADASIAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwD3+iiigAooooAKKKKACiiigD4AooooAKKKKACiiigAooooA+/6KKKACiiigAooooAKKKKAPgCiiigAooooAKKKKACiiigD7/ooooAKKKKACiiigAooooA+AKKKKACiiigAooooAKKKKAPv+iiigAooooAKKKKACiiigD4AooooAKKKKACiiigAooooA+/6KKKACiiigAooooAKKKKAPgCiiigAooooAKKKKACiiigD7/ooooAKKKKACiiigAooooA+AKKKKACiiigAooooAKKKKAPv+iiigAooooAKKKKACiiigD4AooooAKKKKACiiigAooooA+/6KKKACiiigAooooAKKKKAPgCiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooA+/6KKKACiiigAooooAKKKKAPgCiiigAooooAKKKKACiiigD7/ooooAKKKKACiiigAooooA+AKKKKACiiigAooooAKKKKAPv+iiigAooooAKKKKACiiigD4AooooAKKKKACiiigAooooA+/6KKKACiiigAooooAKKKKAPgCiiigAooooAKKKKACiiigD7/ooooAKKKKACiiigAooooA+AKKKKACiiigAooooAKKKKAPv+iiigAooooAKKKKACiiigD4AooooAKKKKACiiigAooooA+/6KKKACiiigAooooAKKKKAPgCiiigAooooAKKKKACiiigD7/ooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKAPgCiiigAooooAKKKKACiiigD7/ooooAKKKKACiiigAooooA+AKKKKACiiigAooooAKKKKAPv+iiigAooooAKKKKACiiigD4AooooAKKKKACiiigAooooA+/6KKKACiiigAooooAKKKKAPgCiiigAooooAKKKKACiiigD7/ooooAKKKKACiiigAooooA+AKKKKACiiigAooooAKKKKAPv+iiigAooooAKKKKACiiigD4AooooAKKKKACiiigAooooA+/6KKKACiiigAooooAKKKKAPgCiiigAooooAKKKKACiiigD7/ooooAKKKKACiiigAooooA+AKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigD7/ooooAKKKKACiiigAooooA+AKKKKACiiigAooooAKKKKAPv+iiigAooooAKKKKACiiigD4AooooAKKKKACiiigAooooA+/6KKKACiiigAooooAKKKKAPgCiiigAooooAKKKKACiiigD7/ooooAKKKKACiiigAooooA+AKKKKACiiigAooooAKKKKAPv+iiigAooooAKKKKACiiigD4AooooAKKKKACiiigAooooA+/6KKKACiiigAooooAKKKKAPgCiiigAooooAKKKKACiiigD7/ooooAKKKKACiiigAooooA+AKKKKACiiigAooooAKKKKAPv+iiigAooooAKKKKACiiigD//2Q==";

})();

