/**

 @ignore(fv.console.*)
 @ignore(fv.assert)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(fv.lib.linMap)
 @ignore(fv.makeGlobalVariable)

 ************************************************************************ */


qx.Class.define("qapp.widgets.ProfileWidget",
    {
        extend: qx.ui.container.Composite,

        /**
         * Creates a new profile widget
         */
        construct: function (hub, profileID) {
            var that = this;
            this.base(arguments);
            this.m_hub = hub;
            this.m_lastMousePosition = null;
            this.m_profileID = profileID;
            // prefix for global variables
            var prefix = "/g1dfs/" + this.m_profileID + "/";
            this.m_vars = {};
            this.m_mouseDownPosition = null;
            this.m_zoomType = "none"; // none | vertical | horizontal | deciding
            this.set({ backgroundColor: "rgba(193, 211, 195, 0.1)"});

            // setup defered call for updating everything
            this.m_updateDefer = new qx.util.DeferredCall(this._updateDeferCB, this);

            this.setLayout(new qx.ui.layout.VBox());

            var topGroup = new qx.ui.container.Composite(new qx.ui.layout.HBox(0));
            this.add(topGroup, { flex: 1});

            var canvasGroup = new qx.ui.container.Composite(new qx.ui.layout.Grow());
            this.m_pwView = new qapp.PureWebView(profileID, "g1dview-" + profileID, qapp.PureWebView.PNG);
            canvasGroup.add(this.m_pwView);

            this.m_canvasTop = new qx.ui.embed.Canvas().set({
                syncDimension: true
            });
            canvasGroup.add(this.m_canvasTop);
            this.m_canvasTop.addListener("resize", function () {
                this._setDirty("canvasDimensions");
            }, this);
            this.m_canvasTop.addListener("redraw", function () {
                this._setDirty("topCanvas");
            }, this);
            this.m_canvasTop.addListener("mousemove", this._canvasMouseMoveCB, this);
            this.m_canvasTop.addListener("mousedown", this._canvasMouseDownCB, this);
            this.m_canvasTop.addListener("mouseup", this._canvasMouseUpCB, this);

            // fitting options
            this.m_fittingOptionsWidgets = {};
            this.m_fittingOptionsWidgets.group = new qx.ui.container.Composite(new qx.ui.layout.VBox(3));
            this.m_fittingOptionsWidgets.group.set({ padding: 5});
            var grp;
            grp = new qx.ui.container.Composite(new qx.ui.layout.HBox(0));
            this.m_fittingOptionsWidgets.group.add( grp);
            grp.add( new qx.ui.basic.Label( "Gaussians:"));
            this.m_fittingOptionsWidgets.nGaussSpinner = new qx.ui.form.Spinner( 0, 1, 9);
            this.m_fittingOptionsWidgets.nGaussSpinner.addListener( "changeValue", function(e) {
                this._uiAction( "nGauss", e.getData());
            }, this);
            grp.add( this.m_fittingOptionsWidgets.nGaussSpinner);
            grp = new qx.ui.container.Composite(new qx.ui.layout.HBox(0));
            this.m_fittingOptionsWidgets.group.add( grp);
            grp.add( new qx.ui.basic.Label( "Poly.terms:"));
            this.m_fittingOptionsWidgets.nPolySpinner = new qx.ui.form.Spinner( 0, 1, 9);
            this.m_fittingOptionsWidgets.nPolySpinner.addListener( "changeValue", function(e) {
                this._uiAction( "nPoly", e.getData());
            }, this);
            grp.add( this.m_fittingOptionsWidgets.nPolySpinner);
            this.m_fittingOptionsWidgets.group.add( new qapp.boundWidgets.CheckBox(
                "Random heuristics", prefix + "randomHeuristics"));
            this.m_fittingOptionsWidgets.manualToggle = new qapp.boundWidgets.CheckBox(
                "Manual initial conditions", prefix + "manualInitialConditions");
            this.m_fittingOptionsWidgets.group.add( this.m_fittingOptionsWidgets.manualToggle);
            this.m_fittingOptionsWidgets.showBarsToggle = new qapp.boundWidgets.CheckBox(
                "Show manipulation bars", prefix + "showManipBars");
            this.m_fittingOptionsWidgets.showBarsToggle.addListener( "changeValue", function() {
                this._setDirty();
            }, this);
            this.m_fittingOptionsWidgets.group.add( this.m_fittingOptionsWidgets.showBarsToggle);
            this.m_fittingOptionsWidgets.manualToggle.getSharedVar().addCallback( function(val) {
                that.m_fittingOptionsWidgets.showBarsToggle.setEnabled( val === "1");
            });
            this.m_fittingOptionsWidgets.resetButton = new qx.ui.form.Button( "Reset initial conditions");
            this.m_fittingOptionsWidgets.resetButton.addListener( "execute", function() {
                this._uiAction( "resetInitialConditions");
            }, this);
            this.m_fittingOptionsWidgets.group.add( this.m_fittingOptionsWidgets.resetButton);

            this.m_fittingOptionsWidgets.group.add( new qapp.boundWidgets.CheckBox(
                "Display residual", prefix + "showResidual"));
            this.m_fittingOptionsWidgets.group.add( new qapp.boundWidgets.CheckBox(
                "Display peak labels", prefix + "showPeakLabels"));
            this.m_fittingOptionsWidgets.group.add( new qapp.boundWidgets.CheckBox(
                "Display values in WCS", prefix + "useWCS"));

            // Options
            // ==========
            this.m_optionsGroup = new qx.ui.container.Composite(new qx.ui.layout.VBox(3));
            this.m_optionsGroup.set({ padding: 5});
            this.m_optionsGroup.exclude();

            function tgl2(label, id, cb) {
                var t = new qapp.boundWidgets.CheckBox(label, prefix + id);
                that.m_optionsGroup.add(t);
                if( cb != null) {
                    t.getSharedVar().addCallback( cb);
                }
                that.m_optionToggles[ id] = t;
                return t;
            }

            this.m_optionToggles = {};
            tgl2("Visual mean and rms", "showRms");
            tgl2("Show cursor", "showCursor1", this._setDirty.bind(this));

            tgl2("Automatic horizontal range", "autoX");
            tgl2("Automatic vertical range", "autoY");
            // buttons to set the plot type
            // --------------------------------------
//            this.m_optionsGroup.add(new qx.ui.core.Spacer(10, 10));
            var radioContainer = new qx.ui.container.Composite(new qx.ui.layout.HBox(3)).set({
                allowStretchX: true,
                allowGrowX: true,
                alignX: "center"
            });
            radioContainer.add(new qx.ui.core.Spacer(), {flex: 1});
            var radioGroup = new qx.ui.form.RadioGroup();

            function rdio(label, id, container, group, that) {
                var radioButton = new qapp.widgets.MyToggleButton(label).set({padding: 2, focusable: false});
                radioButton.setUserData("val", id);
                container.add(radioButton);
                group.add(radioButton);
                return radioButton;
            }

            this.m_plotStyleRadioButtons = {};
            this.m_plotStyleRadioButtons.join = rdio("Join", "join", radioContainer, radioGroup, this);
            this.m_plotStyleRadioButtons.hist = rdio("Hist", "hist", radioContainer, radioGroup, this);
            this.m_plotStyleRadioButtons.cross = rdio("Cross", "cross", radioContainer, radioGroup, this);
            radioContainer.add(new qx.ui.core.Spacer(), {flex: 1});
            this.m_optionsGroup.add(radioContainer);
            radioGroup.setAllowEmptySelection(false);
            radioGroup.addListener("changeSelection", function (e) {
                var id = e.getData()[0].getUserData("val");
                this._setPlotType(id);
            }, this);

//            topGroup.add(this.m_fittingOptionsWidgets.group);
//            topGroup.add(canvasGroup, { flex: 1});
//            topGroup.add(this.m_optionsGroup);

            var rightPane = new qx.ui.container.Composite( new qx.ui.layout.VBox(3));
            rightPane.set({ backgroundColor: "rgb(193, 211, 195)"});
            rightPane.add( this.m_fittingOptionsWidgets.group);
            rightPane.add( this.m_optionsGroup);
            topGroup.add(canvasGroup, { flex: 1});
            topGroup.add( rightPane);

            // fitting results status
            this.m_resultsStatusBar = new qx.ui.container.Composite(new qx.ui.layout.VBox(0));
            this.m_resultsStatusBar.add( new qx.ui.core.Widget().set({ height: 1, backgroundColor: "#aaa"}));
            this.m_resultsWidgets = {};
            this.m_resultsWidgets.group =  new qx.ui.container.Composite(new qx.ui.layout.HBox);
//            this.add( this.m_resultsWidgets.group);
            this.m_resultsStatusBar.add( this.m_resultsWidgets.group);
            var preFn = function( val) {
                return "<pre style='margin:0px;font-size:9px;'>" + val + "</pre>";
            };
            this.m_resultsWidgets.fitResults = new qapp.boundWidgets.RichLabel(
                "", preFn, prefix + "fitResults")
                .set({
                    allowStretchX: true,
                    allowGrowX: true,
                    font: "monospace"
                });
            this.m_resultsWidgets.group.add( this.m_resultsWidgets.fitResults, { flex: 1});
//            this.m_resultsWidgets.fitInitConds = new qapp.boundWidgets.RichLabel(
//                "", preFn, prefix + "fitInitConds")
//                .set({
//                    allowStretchX: true,
//                    allowGrowX: true,
//                    font: "monospace"
//                });
//            this.m_resultsWidgets.group.add( this.m_resultsWidgets.fitInitConds, { flex: 1});
            this.add( this.m_resultsStatusBar);

            // light status
            this.add( new qx.ui.core.Widget().set({ height: 1, backgroundColor: "#aaa"}));
            this.m_lightStatusBar = new qx.ui.container.Composite(new qx.ui.layout.VBox(0));
            var statusContainer1 = new qx.ui.container.Composite(new qx.ui.layout.HBox);
            statusContainer1.set({
                paddingLeft: 2,
                paddingRight: 2
            });
            statusContainer1.add(new qx.ui.basic.Label("Cursor:").set({textColor: "valueLabel"}));
            statusContainer1.add( new qapp.boundWidgets.RichLabel("","",prefix + "cursor1/pos").set({textColor: "value"}));
            statusContainer1.add(new qx.ui.core.Spacer, {flex: 1});
            statusContainer1.add(new qx.ui.basic.Label("Value:").set({textColor: "valueLabel"}));
            statusContainer1.add( new qapp.boundWidgets.RichLabel("","",prefix + "cursor1/val").set({textColor: "value"}));
            this.m_lightStatusBar.add( statusContainer1);
            var statusContainer2 = new qx.ui.container.Composite(new qx.ui.layout.HBox);
            statusContainer2.set({
//                toolTipText: "Click to toggle visual Mean & Rms",
                paddingLeft: 2,
                paddingRight: 2
            });
//            statusContainer2.addListener("click", this.toggleDrawRms, this);
            statusContainer2.add(new qx.ui.basic.Label("Mean:").set({textColor: "valueLabel"}));
            statusContainer2.add( new qapp.boundWidgets.RichLabel("","",prefix + "mean").set({textColor: "value"}));

            statusContainer2.add(new qx.ui.core.Spacer, {flex: 1});
            statusContainer2.add(new qx.ui.basic.Label("Rms:").set({textColor: "valueLabel"}));
            statusContainer2.add( new qapp.boundWidgets.RichLabel("","",prefix + "rms").set({textColor: "value"}));
            this.m_lightStatusBar.add( statusContainer2);
            this.add( this.m_lightStatusBar);

            // keyboard/mouse events
            this.addListener("keyup", this._keyDownCB, this, false);
            this.addListener("mouseover", this._mouseOverCB, this, false);
            this.addListener("mouseout", this._mouseOutCB, this, false);

            // monitor visibility
            this.addListener( "appear", function() {
                this._uiAction( "setVisible", true);
            }, this);
            this.addListener( "disappear", function() {
                this._uiAction( "setVisible", false);
            }, this);

            // make the fitting portions of the widgets invisible by default
            this.m_fittingOptionsWidgets.group.exclude();
            this.m_resultsStatusBar.exclude();

            // other shared variables
            this.m_vars.initialGuess = fv.makeGlobalVariable(
                prefix + "initialGuess", this._initialGuessCB.bind(this), false);
            this.m_vars.isFitter = fv.makeGlobalVariable(
                prefix + "isFitter", this._isFitterCB.bind(this), false);
            this.m_vars.mPos = fv.makeGlobalVariable(
                prefix + "cursor1/mpos", function(val){
                    that.m_cursor1mpos = parseFloat(val);
                    that._setDirty();
                });
            this.m_vars.mVal = fv.makeGlobalVariable(
                prefix + "cursor1/mval", function(val){
                    that.m_cursor1mval = parseFloat(val);
                    that._setDirty();
                });
            this.m_vars.plotArea = fv.makeGlobalVariable(
                prefix + "plotArea", function(val) {
                    fv.console.log( "plotArea ", val);
                    if( val == null) val = "1 10 1 10";
                    val = val.split(" ");
                    if( val.length < 4) val = [ "1", "10", "1", "10"];
                    for( var i = 0 ; i < val.length ; i ++ ) { val[i] = parseInt( val[i]); }
                    that.m_x1 = val[0];
                    that.m_x2 = val[1];
                    that.m_y1 = val[2];
                    that.m_y2 = val[3];
                    that._setDirty();
                });

            this._setDirty(); // update everything
        },

        members: {

            m_x1: 1,
            m_x2: 10,
            m_y1: 1,
            m_y2: 10,
            // list of movable objects
            m_movables: [],

            /**
             * Return the profile ID
             */
            getProfileId: function () {
                return this.m_profileID;
            },

            _initialGuessCB: function( val) {
                fv.console.group( "initiaGuess changed = " + val);
                val = val || "";
                this.m_ics = { num: 0, vals: []};
                val = val.split(" ");
                this.m_ics.num = Math.floor( val.length / 3);
                this.m_ics.vals = [];
                for( var i = 0 ; i < this.m_ics.num ; i ++ ) {
                    this.m_ics.vals[i] = {
                        centerx: parseFloat( val[i*3+0]),
                        fwhmdx:  parseFloat( val[i*3+1]),
                        amply:   parseFloat( val[i*3+2])
                    };
                }
                function makeTri( x, y, psize, col1, col2) {
                    var cx = x;
                    var cy = y;
                    var size = psize;
                    var color1 = col1;
                    var color2 = col2;
                    var hover = false;
                    return {
                        getdsq: function( pt) {
                            var dx = pt.x - cx;
                            var dy = pt.y - cy;
                            dx /= psize;
                            dy /= psize;
                            if( Math.abs(dx) > 1 || Math.abs(dy) > 1) return 1e9;
                            return dx * dx + dy * dy;
                        },
                        draw: function( ctx) {
                            ctx.fillStyle = hover ? color1 : color2;
                            ctx.beginPath();
                            ctx.moveTo( cx, cy - size);
                            ctx.lineTo( cx + size, cy + size);
                            ctx.lineTo( cx - size, cy + size);
                            ctx.lineTo( cx, cy - size);
                            ctx.fill();
                            ctx.closePath();
                        },
                        setHover: function( val) {
                            hover = val;
                        }
                    };
                }
                function makeRectH( x, y, pwidth, pheight, col1, col2) {
                    var cx = x;
                    var cy = y;
                    var width = pwidth;
                    var height = pheight;
                    var color1 = col1;
                    var color2 = col2;
                    var hover = false;
                    return {
                        getdsq: function( pt) {
                            if( pt.x >= cx && pt.x <= cx + width && pt.y >= cy && pt.y <= cy + height) {
                                var dx = pt.x - cx - width/2;
                                var dy = pt.y - cy - height/2;
                                dx /= width;
                                dy /= height;
                                return (dx * dx + dy * dy);
                            }
                            else {
                                return 1e9;
                            }
                        },
                        draw: function( ctx) {
                            ctx.fillStyle = hover ? color1 : color2;
                            ctx.fillRect( cx, cy, width, height);
                        },
                        setHover: function( val) {
                            hover = val;
                        }
                    };
                }
                function makeSq( x, y, psize, col1, col2) {
                    return makeRectH( x - psize, y - psize, psize * 2, psize * 2, col1, col2);
                }

                // regenerate movables
                this.m_movables = [];
                for( var i = 0 ; i < this.m_ics.num ; i ++ ) {
                    var ic = this.m_ics.vals[i];
                    var obj;
                    obj = makeRectH( ic.centerx - ic.fwhmdx, ic.amply, ic.fwhmdx * 2, this.m_y2 + 10 - ic.amply,
                        "rgba(255,255,0,0.4)", "rgba(255,255,0,0.2)");
                    obj.ic = ic; obj.ict = "bar";
                    this.m_movables.push( obj);
                    obj = makeRectH( ic.centerx - ic.fwhmdx, this.m_y2 + 10, ic.fwhmdx * 2, 2,
                        "#f00", "#f88");
                    obj.ic = ic; obj.ict = "none";
                    obj.getdsq = function() { return 1e9; };
                    this.m_movables.push( obj);

                    obj = makeSq( ic.centerx, ic.amply, 5, "#f00", "rgba(255,0,0,0.5)");
                    obj.ic = ic; obj.ict = "top";
                    this.m_movables.push( obj);
                    obj = makeTri( ic.centerx + ic.fwhmdx, this.m_y2 + 7, 5, "#f00", "#f88");
                    obj.ic = ic; obj.ict = "right";
                    this.m_movables.push( obj);
                    obj = makeTri( ic.centerx - ic.fwhmdx, this.m_y2 + 7, 5, "#f00", "#f88");
                    obj.ic = ic; obj.ict = "left";
                    this.m_movables.push( obj);
                }
                this._setDirty();
                fv.console.groupEnd();
            },

            _isFitterCB: function(val) {
                val = val === "1";
                if( val) {
                    this.m_fittingOptionsWidgets.group.show();
//                    this.m_resultsWidgets.group.show();
                    this.m_resultsStatusBar.show();
                    this.m_optionsGroup.show();
//                    this.m_lightStatusBar.exclude();
                }
                else {
                    this.m_fittingOptionsWidgets.group.exclude();
//                    this.m_resultsWidgets.group.exclude();
                    this.m_resultsStatusBar.exclude();
//                    this.m_lightStatusBar.show();
                }
            },

            toggleShowOptions: function () {
                if (this.m_optionsGroup.isVisible()) {
                    this.m_optionsGroup.exclude();
                } else {
                    this.m_optionsGroup.show();
                }
            },

            resetZoom: function() {
                this.m_optionToggles.autoX.getSharedVar().set( "1");
                this.m_optionToggles.autoY.getSharedVar().set( "1");
            },

            _setPlotType: function (val) {
                this._uiAction("plotType", val);
            },

            toggleDrawRms: function (val) {
                var sv = this.m_optionToggles.showRms.getSharedVar();
                sv.set( sv.get() === "0");
            },

            /**
             * Set extra render info
             *
             */
            setState: function (info) {
                if( info.plotStyle === "0")
                    this.m_plotStyleRadioButtons.join.setValue(true);
                else if( info.plotStyle === "1")
                    this.m_plotStyleRadioButtons.hist.setValue(true);
                else
                    this.m_plotStyleRadioButtons.cross.setValue(true);

                this.m_fittingOptionsWidgets.nGaussSpinner.setValue( info.nGauss);
                this.m_fittingOptionsWidgets.nPolySpinner.setValue( info.nPoly);

                this._setDirty();
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

                // draw the zoom selection
                if (this.m_zoomType === "deciding") {
                    // do nothing
                }
                else if (this.m_zoomType === "horizontal") {
                    ctx.fillStyle = "rgba(0,255,0,0.3)";
                    ctx.fillRect(this.m_mouseDownPosition.x, 0,
                        this.m_lastMousePosition.x - this.m_mouseDownPosition.x, height);
                    ctx.strokeStyle = "rgba(0,200,0,0.5)";
                    ctx.lineWidth = 2;
                    ctx.beginPath();
                    ctx.moveTo(this.m_mouseDownPosition.x, 0);
                    ctx.lineTo(this.m_mouseDownPosition.x, height);
                    ctx.moveTo(this.m_lastMousePosition.x, 0);
                    ctx.lineTo(this.m_lastMousePosition.x, height);
                    ctx.stroke();
                }
                else if (this.m_zoomType === "vertical") {
                    ctx.fillStyle = "rgba(0,255,0,0.3)";
                    ctx.fillRect(0, this.m_mouseDownPosition.y,
                        width, this.m_lastMousePosition.y - this.m_mouseDownPosition.y);
                    ctx.strokeStyle = "rgba(0,200,0,0.5)";
                    ctx.lineWidth = 2;
                    ctx.beginPath();
                    ctx.moveTo(0, this.m_mouseDownPosition.y);
                    ctx.lineTo(width, this.m_mouseDownPosition.y);
                    ctx.moveTo(0, this.m_lastMousePosition.y);
                    ctx.lineTo(width, this.m_lastMousePosition.y);
                    ctx.stroke();
                }

                // draw the cursor
                if( this.m_optionToggles.showCursor1.getValue() && this.m_mouseDownPosition == null) {
                    if( isFinite( this.m_cursor1mpos)) {
                        ctx.beginPath();
                        ctx.moveTo( this.m_cursor1mpos, 0);
                        ctx.lineTo( this.m_cursor1mpos, height);
                        ctx.strokeStyle = "rgba(0,0,255,0.25)";
                        ctx.lineWidth = 2;
                        ctx.stroke();

                        if( isFinite(this.m_cursor1mval)) {
                            ctx.beginPath();
                            ctx.arc( this.m_cursor1mpos, this.m_cursor1mval, 3, 0, 2 * Math.PI, false);
                            ctx.fillStyle = "rgba(255,0,0,1)";
                            ctx.fill();
                        }
                    }
                }

                // draw the manipulation bars
                if( false && this.m_fittingOptionsWidgets.showBarsToggle.getValue()) {
                    for( var i = 0 ; i < this.m_ics.num ; i ++ ) {
                        var ic = this.m_ics.vals[i];
                        if( this.m_hoverType === "ic" && this.m_hoverVal === i )
                            ctx.fillStyle = "rgba(255,0,0,1.0)";
                        else
                            ctx.fillStyle = "rgba(255,0,0,0.4)";
                        ctx.fillRect( ic.centerx - ic.fwhmdx, ic.amply - 1, 2 * ic.fwhmdx, 3);
                        ctx.fillRect( ic.centerx - 5, ic.amply - 5, 10, 10);

                        ctx.fillStyle = "rgba(255,0,0,0.1)";
                        ctx.fillRect( ic.centerx - ic.fwhmdx, ic.amply - 1,
                            ic.fwhmdx * 2, this.m_y2 + 10 - (ic.amply - 1));
                    }
                }

                // debug: draw the plot area
//                ctx.fillStyle = "rgba(255,255,0,0.1)";
//                ctx.fillRect( this.m_x1, this.m_y1, this.m_x2-this.m_x1, this.m_y2-this.m_y1);

                // draw all movable objects
                if( this.m_fittingOptionsWidgets.showBarsToggle.getValue()) {
                    for( var i = 0 ; i < this.m_movables.length ; i ++ ) {
                        this.m_movables[i].draw( ctx);
                    }
                }

            },

            /**
             * Updates m_hoverObjectIndex based on the contents of m_movables[]
             *
             * @private
             */
            _updateHoverInfo: function() {
                this.m_hoverObjectIndex = null;

                var minD = Number.NaN;
                for( var i = 0 ; i < this.m_movables.length ; i ++ ) {
                    var d = this.m_movables[i].getdsq( this.m_lastMousePosition);
                    if( d > 15) continue; // at least min. distance
                    if( d < minD || this.m_hoverObjectIndex === null) {
                        this.m_hoverObjectIndex = i;
                        minD = d;
                    }
                }
                // set hover state for all objects
                for( var i = 0 ; i < this.m_movables.length ; i ++ ) {
                    this.m_movables[i].setHover( i === this.m_hoverObjectIndex);
                }
            },

            _canvasMouseMoveCB: function (event) {
                var pt = this._localPos(event);
                this.m_lastMousePosition = pt;
                if (this.m_mouseDownPosition == null) {
                    this._updateHoverInfo();
                    this._uiAction( "cursor1mouse", pt.x );
                    // call redraw anyways because the cursor probably changed y position
                    this._setDirty();
                    return;
                }
                // if there are no movables on the screen, we are in zoom mode
                if( this.m_movables.length < 1) {
                    // We are zooming. If we are not decided yet which zoom we're doing, we need to
                    // monitor how far the mouse has moved since being clicked and decide based on that
                    if (this.m_zoomType === "deciding") {
                        var dx = Math.abs(this.m_mouseDownPosition.x - this.m_lastMousePosition.x);
                        var dy = Math.abs(this.m_mouseDownPosition.y - this.m_lastMousePosition.y);
                        if (Math.max(dx, dy) < 10) {
                            // do nothing, keep the state in deciding mode
                        }
                        else if (dx > dy && dx > 5) {
                            this.m_zoomType = "horizontal";
                        }
                        else {
                            this.m_zoomType = "vertical";
                        }
                    } /*else {
                        // do nothing, just redraw, we are decided
                    }*/
                }
                else if( this.m_hoverObjectIndex !== null) {
                    var obj = this.m_movables[ this.m_hoverObjectIndex];
//                    obj.move( this.m_lastMousePosition);
                    // update the appropriate ics
                    if( obj.ict === "top") {
                        obj.ic.centerx = this.m_lastMousePosition.x;
                        obj.ic.amply = this.m_lastMousePosition.y;
                    }
                    else if( obj.ict === "right") {
                        obj.ic.fwhmdx = this.m_lastMousePosition.x - obj.ic.centerx;
                    }
                    else if( obj.ict === "left") {
                        obj.ic.fwhmdx = obj.ic.centerx - this.m_lastMousePosition.x;
                    }
                    else if( obj.ict === "bar") {
                        obj.ic.centerx = this.m_lastMousePosition.x;
                    }
                    else {
                        fv.console.warn( "Not sure what to do with " + obj.ict);
                    }
                    if( obj.ic.fwhmdx < 1) obj.ic.fwhmdx = 1;

                    // resend the state of initial conditions
                    var str = "";
                    for( var i = 0 ; i < this.m_ics.vals.length ; i ++ ) {
                        if( i > 0) str += " ";
                        str += this.m_ics.vals[i].centerx + " "
                          + this.m_ics.vals[i].fwhmdx + " "
                          + this.m_ics.vals[i].amply;
                    }
                    fv.console.log( "setting ic=", this.m_profileID, str);
                    this.m_vars.initialGuess.set( str);
                    this._initialGuessCB( str);
                }

                this._setDirty();
            },

            _canvasMouseDownCB: function (event) {
                this.m_canvasTop.capture();
                var pt = this._localPos(event);
                this.m_mouseDownPosition = pt;
                if( event.getButton() === "left") {
                    this.m_zoomType = "deciding";
                } else {
                    this.m_zoomType = "none";
                }
                this._setDirty();
            },

            _canvasMouseUpCB: function (event) {
                this.m_canvasTop.releaseCapture();
                // if we were in the zoom mode, do the zoom
                if (this.m_zoomType === "horizontal") {
                    this._uiAction( "horizontalZoom",
                        this.m_mouseDownPosition.x,
                        this.m_lastMousePosition.x
                    );
                }
                else if (this.m_zoomType === "vertical") {
                    this._uiAction( "verticalZoom",
                        this.m_mouseDownPosition.y,
                        this.m_lastMousePosition.y
                    );
                }
                else if( event.getButton() === "right") {
                    this._uiAction( "rightClick",
                        this.m_mouseDownPosition.x,
                        this.m_mouseDownPosition.y
                    );
                }

                this.m_mouseDownPosition = null;
                this.m_zoomType = "none";
                this._setDirty("hzoom", "vzoom");
            },


            /**
             * Arranges to have everything recalculated and repainted.
             */
            _setDirty: function () {
                this.m_updateDefer.schedule();
            },

            /**
             * Callback for deferred update.
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
                    this.m_mouseDownPosition = null;
                    this.m_zoomType = "none";
                    this.resetZoom();
                    this._setDirty();
                }
            },

            _mouseOverCB: function (event) {
                this.activate();
            },

            _mouseOutCB: function (event) {
                this.deactivate();
                this.m_lastMousePosition = null;
                this._uiAction( "cursor1mouse", null);
            },

            _uiAction: function( cmd /* , val1, val2, val3, ... */ ) {
                var data = {
                    id: this.m_profileID,
                    cmd: cmd
                };
                for( var i = 1 ; i < arguments.length ; i ++ ) {
                    data[ "val" + i.toString()] = arguments[i];
                }
                this.fireDataEvent( "uiAction", data);
            }

        },

        properties: {

        }

    });

