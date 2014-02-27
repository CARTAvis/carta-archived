/* global qx, fv */
/* jshint expr: true */

/**

 @ignore(fv.console.*)
 @ignore(fv.assert)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(fv.lib.linMap)
 @ignore(fv.makeGlobalVariable)

 */

qx.Class.define("qapp.widgets.QUProfileWidget",
    {
        extend: qx.ui.container.Composite,

        /**
         * Creates a new profile widget
         */
        construct: function (hub, profileID) {
            this.base(arguments);
            this.m_hub = hub;
            this.m_lastMousePosition = null;
            this.m_profileID = profileID;
            this.m_mouseDownPosition = null;
            this.m_zoomType = "none"; // none | vertical | horizontal | deciding
            this.set({ backgroundColor: "rgba(193, 211, 195, 0.1)"});

            this.m_zoom_i1 = null;
            this.m_zoom_i2 = null;
            this.m_zoom_y1 = null;
            this.m_zoom_y2 = null;

            // our prefix
            var prefix = "/quprofile-" + this.m_profileID + "/";

            // setup defered call for updating everything
            this.m_updateDefer = new qx.util.DeferredCall(this._updateDeferCB, this);

            this.setLayout(new qx.ui.layout.VBox());

            var topGroup = new qx.ui.container.Composite(new qx.ui.layout.HBox(0));
            this.add(topGroup, { flex: 1});

            var canvasGroup = new qx.ui.container.Composite(new qx.ui.layout.Grow());
            this.m_pwView = new qapp.PureWebView(profileID, "quprofile-" + profileID, qapp.PureWebView.PNG);
            canvasGroup.add(this.m_pwView);

            this.m_canvasTop = new qx.ui.embed.Canvas().set({
                syncDimension: true
            });
            canvasGroup.add(this.m_canvasTop);
            this.m_canvasTop.addListener("resize", function () {
                this._redrawLater();
            }, this);
            this.m_canvasTop.addListener("redraw", function () {
                this._redrawLater();
            }, this);
            this.m_canvasTop.addListener("mousemove", this._canvasMouseMoveCB, this);
            this.m_canvasTop.addListener("mousedown", this._canvasMouseDownCB, this);
            this.m_canvasTop.addListener("mouseup", this._canvasMouseUpCB, this);

            // Options
            // ==========
            this.m_optionsGroup = new qx.ui.container.Composite(new qx.ui.layout.VBox(0));
            this.m_optionsGroup.set({
                padding: 5,
                backgroundColor: "rgb(193, 211, 195)"
            });
            this.m_optionsGroup.exclude();

            this.m_optionToggles = {};
            function tgl(label, id, that) {
                var t = new qapp.boundWidgets.CheckBox( label, prefix + id);
                that.m_optionsGroup.add(t);
                return t;
            }
            tgl("Cursor", "showCursor", this);
            tgl("Mean", "showMean", this);
            tgl("Grid", "showGrid", this);
            tgl("Sunbow", "showSunbow", this);
            tgl("Connect", "showConnect", this);

            // dotSize slider
            this.m_optionsGroup.add( new qx.ui.basic.Label( "Size:"));
            var slider = new qapp.boundWidgets.Slider( {
                path: prefix + "dotSize",
                min: 0.3,
                max: 9.999 });
            this.m_optionsGroup.add( slider);

            // transparency slider
            this.m_optionsGroup.add( new qx.ui.basic.Label( "Transparency:"));
            slider = new qapp.boundWidgets.Slider( {
                path: prefix + "transparency",
                min: 0.1,
                max: 1.0 });
            this.m_optionsGroup.add( slider);

            topGroup.add(canvasGroup, { flex: 1});
            topGroup.add(this.m_optionsGroup);

            // status bar
            var statusContainer = new qx.ui.container.Composite(new qx.ui.layout.Flow);
            statusContainer.set({
                paddingLeft: 2,
                paddingRight: 2
            });

            var colorManager = qx.theme.manager.Color.getInstance();
            var valueColor = colorManager.resolve("value");
            var valueLabelColor = colorManager.resolve("valueLabel");

            var qMeanStr = "<span style='text-decoration:overline;'>q</span>";
            var uMeanStr = "<span style='text-decoration:overline;'>u</span>";
            function lbl(s) {
                return "<span style='color:" + valueLabelColor + "'>" + s + "</span>";
            }
            function val(s) {
                return "<span style='color:" + valueColor + "'>" + s + "</span>";
            }
            function fmt( label, value) {
                return lbl( label) + ":" + val( value);
            }

            statusContainer.add( new qapp.boundWidgets.RichLabel(
                "", fmt.bind( this, "q"), prefix + "q"));
            statusContainer.add(new qx.ui.core.Spacer(5));

            statusContainer.add( new qapp.boundWidgets.RichLabel(
                "", fmt.bind( this, "u"), prefix + "u"));
            statusContainer.add(new qx.ui.core.Spacer(5));

            statusContainer.add( new qapp.boundWidgets.RichLabel(
                "", fmt.bind( this, qMeanStr), prefix + "qmean"));
            statusContainer.add(new qx.ui.core.Spacer(5));

            statusContainer.add( new qapp.boundWidgets.RichLabel(
                "", fmt.bind( this, uMeanStr), prefix + "umean"));
            statusContainer.add(new qx.ui.core.Spacer(5));

            statusContainer.add( new qapp.boundWidgets.RichLabel(
                "", fmt.bind( this, "Frame"), prefix + "frame"));

            this.add( new qx.ui.core.Widget().set({ height: 1, backgroundColor: "#aaa"}));
            this.add(statusContainer);

            // register event handlers
            this.addListener("keyup", this._keyDownCB, this, false);
            this.addListener("mouseover", this._mouseOverCB, this, false);
            this.addListener("mouseout", this._mouseOutCB, this, false);

            // monitor visibility
            this.m_visibilityVar = fv.makeGlobalVariable( prefix + "visibleOnClient", function(val){
                fv.console.log( "visibility " + this.m_profileID + " = " + val);
            });
            this.addListener( "appear", function() {
                this.m_visibilityVar.set( true);
            }, this);
            this.addListener( "disappear", function() {
                this.m_visibilityVar.set( false);
            }, this);

            this._redrawLater(); // update everything
        },

        members: {

            m_marginLeft: 5,
            m_marginRight: 20,
            m_marginTop: 10,
            m_marginBottom: 3,

            _emit: function (path, data) {
                fv.assert(this.m_hub !== null, "hub is NULL");
                this.m_hub.emit(path, data);
            },

            /**
             * Return the profile ID
             */
            getProfileId: function () {
                return this.m_profileID;
            },

            // this is called by the window (or any other surrounding container)
            toggleShowOptions: function () {
                if (this.m_optionsGroup.isVisible()) {
                    this.m_optionsGroup.exclude();
                } else {
                    this.m_optionsGroup.show();
                }
            },

            // rest zoom
            resetZoom: function() {
                this._uiAction( "zoomReset", true);
            },

            /**
             * returns mouse event's local position (with respect to this widget)
             * @param event {Event}
             */
            _localPos: function (event) {
                var box = this.m_canvasTop.getContentLocation("box");
                return {
                    x: event.getDocumentLeft() - box.left,
                    y: event.getDocumentTop() - box.top
                };
            },

            _drawTop: function () {
                var ctx = this.m_canvasTop.getContext2d();
                var width = this.m_canvasWidth;
                var height = this.m_canvasHeight;

                ctx.clearRect(0, 0, width, height);

                if( this.m_mouseDownPosition) {
                    // draw the zoom selection
                    ctx.fillStyle = "rgba(0,255,0,0.3)";
                    ctx.fillRect(this.m_mouseDownPosition.x, this.m_mouseDownPosition.y,
                        this.m_lastMousePosition.x - this.m_mouseDownPosition.x,
                        this.m_lastMousePosition.y - this.m_mouseDownPosition.y);
                    ctx.strokeStyle = "rgba(0,200,0,1)";
                    ctx.lineWidth = 2;
                    ctx.strokeRect(this.m_mouseDownPosition.x, this.m_mouseDownPosition.y,
                        this.m_lastMousePosition.x - this.m_mouseDownPosition.x,
                        this.m_lastMousePosition.y - this.m_mouseDownPosition.y);
                }
            },

            _updateHoverInfo: function() {
                this.m_hoverType = null;
                this.m_hoverVal = null;

                if( this.m_ics != null) {
                    for( var i = 0 ; i < this.m_ics.num ; i ++ ) {
                        var dx = this.m_lastMousePosition.x - this.m_ics.vals[i].centerx;
                        var dy = this.m_lastMousePosition.y - this.m_ics.vals[i].amply;
                        var d = dx * dx + dy * dy;
                        if( d < 15) {
                            this.m_hoverType = "ic";
                            this.m_hoverVal = i;
                        }
                    }
                }
            },

            _canvasMouseDownCB: function (event) {
                if( event.getButton() === "right") {
                    var pt = this._localPos(event);;
                    this._uiAction( "rightClick", pt.x, pt.y);
                }
                else {
                    this.m_canvasTop.capture();
                    this.m_mouseDownPosition = this._localPos(event);
                    this._redrawLater();
                }
            },

            _canvasMouseMoveCB: function (event) {
                var pt = this._localPos(event);
                this.m_lastMousePosition = pt;
                if( this.m_mouseDownPosition) {
                    this._redrawLater();
                } else {
                    this._uiAction( "mouseHover", pt.x, pt.y);
                }
            },

            _canvasMouseUpCB: function (/*event*/) {
                this.m_canvasTop.releaseCapture();
                // if we were in the zoom mode, do the zoom
                if( this.m_mouseDownPosition != null) {
                    this._uiAction( "zoomRect",
                        this.m_mouseDownPosition.x,
                        this.m_mouseDownPosition.y,
                        this.m_lastMousePosition.x,
                        this.m_lastMousePosition.y
                    );
                }
                this.m_mouseDownPosition = null;
                this._redrawLater();
            },

            /**
             * Schedules a redraw.
             */
            _redrawLater: function () {
                this.m_updateDefer.schedule();
            },

            /**
             * Callback for defered update.
             */
            _updateDeferCB: function () {
                var DEBUG = false && fv.GLOBAL_DEBUG;

                DEBUG && fv.console.group("updateDefer-" + this.m_profileID + ":");

                // save the canvas width/height for other members
                this.m_canvasWidth = this.m_canvasTop.getCanvasWidth();
                this.m_canvasHeight = this.m_canvasTop.getCanvasHeight();

                // redraw the top canvas if needed
                this._drawTop();

                DEBUG && fv.console.groupEnd();
            },

            _keyDownCB: function (event) {
                if (event.getKeyIdentifier() === "Escape") {
                    this.m_mouseDownPt = null;
                    this._redrawLater();
                }
            },

            _mouseOverCB: function (event) {
                this.activate();
            },

            _mouseOutCB: function (event) {
                this.deactivate();
                this.m_lastMousePosition = null;
                this._uiAction( "mouseHover", null);
            },

            _uiAction: function( cmd, val1) {
                var data = {
                    id: this.m_profileID,
                    cmd: cmd
                };
                for( var i = 1 ; i < arguments.length ; i ++ ) {
                    data[ "val" + i.toString()] = arguments[i];
                    this._emit( "ui.quprofile.uiAction", data);
                }
            }

        },

        properties: {

        }

    });

