/**
 * Colormap window for the CyberSKA image viewer.
 * User: pfederl
 */

/* global fv, fv.lib.now, qx */

/**

 @ignore(fv.console.log)
 @ignore(fv.assert)
 @ignore(fv.hub.emit)
 @ignore(fv.lib.now)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(fv.makeGlobalVariable)

 @asset(qapp/icons/histogram16.png)

 ************************************************************************ */

qx.Class.define("qapp.widgets.HistogramWindow",
    {
        extend: qapp.BetterWindow,

        construct: function (hub) {
            this.base(arguments, hub);

//            this.m_hub = hub;

            this.setWidth(390);

            this.setShowMinimize(false);
            this.setShowMaximize(true);
            this.setShowClose(true);
            this.setUseResizeFrame(false);
            this.setAlwaysOnTop(true);
//            this.setWidth( 520);
//            this.setHeight( 500);
            this.setCaption("Histogram");
            this.setMinWidth(334);
            this.setMinHeight(200);
            this.setContentPadding(0, 0, 0, 0);
            this.setIcon( "qapp/icons/histogram16.png");

            this.setLayout(new qx.ui.layout.VBox(0));

            // add preset buttons
            {
                var presetsContainer = new qx.ui.container.Composite(
                    new qx.ui.layout.HBox(5).set({
                        alignX: "center"}));
                var radioGroup = new qx.ui.form.RadioGroup();
                radioGroup.setAllowEmptySelection(true);
                var presets = [ "95%", "98%", "99%", "99.5%", "99.9%", "99.99%", "100%" ];
                this.m_presetButtons = [];
                presets.forEach(function (e, ind) {
                    var button = new qx.ui.toolbar.RadioButton(e).set({
                        toolTipText: "Set histogram to show " + e + " of the data.<br>" +
                            "Right click also automatically zooms in to the data."
                    });
                    button.addListener("execute", function () {
                        this._emit("ui.histogram.preset", { index: ind, zoom: false });
                        button.setValue(true);
                    }, this);
                    button.addListener("mouseup", function (event) {
                        if (event.getButton() == "right")
                            this._emit("ui.histogram.preset", { index: ind, zoom: true });
                        button.setValue(true);
                    }, this);
                    radioGroup.add(button);
                    button.setFocusable(false);
                    presetsContainer.add(button);
                    this.m_presetButtons.push(button);
                }, this);
                this.add(presetsContainer);
            }

            // thin separator
            {
                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});
            }

//            // add info area
//            {
//                var infoContainer = new qx.ui.container.Composite(new qx.ui.layout.HBox(5)).set({
////                    minHeight: 100,
//                    allowShrinkY: false,
////                    height: 100,
//                    padding: 2,
//                    backgroundColor: "rgba(255,255,0,0.1)"
//
//                });
//                this.m_textArea1 = new qx.ui.embed.Html().set({
//                    allowShrinkY: false,
////                    backgroundColor: "rgba(255,255,0,0.5)",
//                    height: 1,
//                    font: "monospace"
//                });
//                infoContainer.add(this.m_textArea1, { flex: 1});
//                this.m_textArea2 = new qx.ui.embed.Html().set({
//                    allowShrinkY: false,
////                    backgroundColor: "rgba(255,128,0,0.5)",
//                    height: 1,
//                    font: "monospace"
//                });
//                infoContainer.add(this.m_textArea2, { flex: 1});
////                this.add(infoContainer);
//            }

            // new info area
            {
                var lbl = function (txt) {
                    var lab = new qx.ui.basic.Label(txt);
//                    lab.setTextAlign( "right");
                    lab.setTextColor( "#0000aa");
                    lab.setPadding( 1);
//                    lab.setBackgroundColor( "#000000");
                    return lab;
                };
                var infoContainer2 = new qx.ui.container.Composite( new qx.ui.layout.Grid(5,2));
                infoContainer2.setPadding(3);
                infoContainer2.getLayout().setColumnAlign( 0, "right", "middle");
                infoContainer2.getLayout().setColumnAlign( 1, "left", "middle");
                infoContainer2.add(lbl("Lower clip:"), {row: 0, column: 0});
                infoContainer2.add(lbl("Upper clip:"), {row: 1, column: 0});
                infoContainer2.add(lbl("Frame min:"), {row: 2, column: 0});
                infoContainer2.add(lbl("Frame max:"), {row: 3, column: 0});

                var tf = new qapp.boundWidgets.TextField("/Histogram/LowerClip");
                tf.setAppearance("textfieldLight");
                tf.setToolTipText("Click to edit, enter to finish editing.");
                tf.addListener( "editValue", function(e){
                    this.m_gVars.preset.set(-1);
                }.bind(this));
                infoContainer2.add(tf, {row: 0, column: 1});
                var tf = new qapp.boundWidgets.TextField("/Histogram/UpperClip");
                tf.setAppearance("textfieldLight");
                tf.setToolTipText("Click to edit, enter to finish editing.");
                tf.addListener( "editValue", function(e){
                    this.m_gVars.preset.set(-1);
                }.bind(this));
                infoContainer2.add(tf, {row: 1, column: 1});
                infoContainer2.add(new qapp.boundWidgets.Label("","","/Histogram/GlobalMin"), {row: 2, column: 1});
                infoContainer2.add(new qapp.boundWidgets.Label("","","/Histogram/GlobalMax"), {row: 3, column: 1});

                infoContainer2.getLayout().setColumnFlex(2, 1);
                infoContainer2.getLayout().setColumnFlex(3, 1);
                infoContainer2.getLayout().setColumnAlign( 2, "right", "middle");
                infoContainer2.getLayout().setColumnAlign( 3, "left", "middle");
                infoContainer2.add(lbl("NaNs:"), {row: 0, column: 2});
                infoContainer2.add(lbl("Bin:"), {row: 1, column: 2});
                infoContainer2.add(lbl("Bin count:"), {row: 2, column: 2});
                infoContainer2.add(lbl("Values in range:"), {row: 3, column: 2});

                infoContainer2.add(new qapp.boundWidgets.Label("","","/Histogram/nNaNs"), {row: 0, column: 3});
                infoContainer2.add(new qapp.boundWidgets.Label("","","/Histogram/binValue"), {row: 1, column: 3});
                infoContainer2.add(new qapp.boundWidgets.Label("","","/Histogram/nValuesInBin"), {row: 2, column: 3});
                infoContainer2.add(new qapp.boundWidgets.Label("","","/Histogram/trueCount"), {row: 3, column: 3});

                this.add( infoContainer2);
            }

            // thin separator
            {
                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});
            }

            // add graphics widget
            {
                var graphicsContainer = new qx.ui.container.Composite(new qx.ui.layout.Grow());
                this.add(graphicsContainer, { flex: 1});

                var graphWidget = new qapp.PureWebView(
                    "histogramview", "HistogramView", qapp.PureWebView.PNG
                ).set({
                        minHeight: 30,
                        height: 100
                    });
//                this.add(graphWidget, { flex: 1});
                graphicsContainer.add(graphWidget);
                this.m_overlay = new qx.ui.embed.Canvas().set({
                    syncDimension: true
                });
                graphicsContainer.add(this.m_overlay);
                this.m_overlay.addListener("redraw", this._overlayRedrawCB, this);
                this.m_overlay.addListener("mousemove", this._overlayMouseMoveCB, this);
                this.m_overlay.addListener("mousedown", this._overlayMouseDownCB, this);
                this.m_overlay.addListener("mouseup", this._overlayMouseUpCB, this);

                this.m_overlay.addListener("touchmove", this._touchMoveCB, this);
                this.m_overlay.addListener("touchend", this._touchEndCB, this);
                this.m_overlay.addListener("touchcancel", this._touchCancelCB, this);
                this.m_overlay.addListener("touchstart", this._touchStartCB, this);
            }

            // thin separator
            {
                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});
            }

            // add bottom buttons
            {
                var bottomButtonsContainer = new qx.ui.container.Composite(
                    new qx.ui.layout.HBox(5).set({
                        alignX: "center"
                    })
                ).set({ padding: 5});
                this.add(bottomButtonsContainer);
                bottomButtonsContainer.add(new qx.ui.core.Spacer(1), {flex: 1});
                var zoomButton = new qx.ui.form.Button("Zoom to selection");
                zoomButton.addListener("execute", function () {
                    this._emit("ui.histogram.zoom", true);
                }, this);
                bottomButtonsContainer.add(zoomButton);
                var unzoomButton = new qx.ui.form.Button("Show all");
                unzoomButton.addListener("execute", function () {
                    this._emit("ui.histogram.zoom", false);
                }, this);
                bottomButtonsContainer.add(unzoomButton);
                bottomButtonsContainer.add(new qx.ui.core.Spacer(1), {flex: 1});
                this.m_logScaleToggle = new qx.ui.form.CheckBox("Log scale").set({
                    toolTipText: "Toggle logarithmic scale on/off",
                    value: true
                });
                bottomButtonsContainer.add(this.m_logScaleToggle);
                this.m_smoothToggle = new qx.ui.form.CheckBox("Bars").set({
                    toolTipText: "Toggles betwee smooth graph and bar graph.",
                    value: false
                });
                bottomButtonsContainer.add(this.m_smoothToggle);
                bottomButtonsContainer.add(new qx.ui.core.Spacer(1), {flex: 1});

                this.m_logScaleToggle.addListener("changeValue", function () {
                    this._emit("ui.histogram.logScale", this.m_logScaleToggle.getValue());
                }, this);
                this.m_smoothToggle.addListener("changeValue", function () {
                    this._emit("ui.histogram.smoothGraph", !this.m_smoothToggle.getValue());
                }, this);
            }

            // handle preset
            this.m_gVars = {};
            this.m_gVars.preset = fv.makeGlobalVariable( "/Histogram/Index", function( val) {
                fv.console.log( "preset = ", val);
                this.m_presetButtons.forEach(function (button, ind) {
                    button.setValue(ind == val);
                }, this);
            }.bind(this));

            // handle markers
            this.m_gVars.marker1 = fv.makeGlobalVariable( "/Histogram/marker1", function( val) {
                if( this.interactingWithMarkers()) return;
                this.m_marker1 = val;
                this.m_overlay.update();
            }.bind(this));
            this.m_gVars.marker2 = fv.makeGlobalVariable( "/Histogram/marker2", function( val) {
                if( this.interactingWithMarkers()) return;
                this.m_marker2 = val;
                this.m_overlay.update();
            }.bind(this));

            // let the controller know we can receive parsed state updates
//            this._emit("ui.parsedStateRequest");

            // subscribe to both parsedState & histogramInfo
//            this.m_hub.subscribe("toui.parsedState", function (val) {
//                this.m_parsedStateRef = val;
//                this._updateFromState();
//            }, this);
//            this.m_hub.subscribe("toui.histogram.sync", function (val) {
//                this._updateFromState();
//            }, this);

            this.m_gVars.cursor = fv.makeGlobalVariable( "/Histogram/cursor", function(val){
                this.m_cursorX = parseInt(val);
                this.m_overlay.update();
            }.bind(this));

            this.addListener( "appear", function() {
                this._emit( "ui.histogram.setVisible", true);
            }, this);
            this.addListener( "disappear", function() {
                this._emit( "ui.histogram.setVisible", false);
            }, this);
        },

        members: {
//            m_hub: null,
            m_parsedStateRef: null,
//            m_textArea1: null,
//            m_textArea2: null,
            m_presetButtons: null,
            m_logScaleToggle: null,
            m_overlay: null,
            m_marker1: 40, // interactive left marker
            m_marker2: 140, // interactive right marker
            m_lastMarkerInteraction: null,
            m_cursorX: null,
            m_drag: 0,

            interactingWithMarkers: function() {
                if( this.m_lastMarkerInteraction == null) return false;
                return fv.lib.now() - this.m_lastMarkerInteraction < 500;
            },

            /*
            _updateFromState: function () {
                fv.assert(this.m_parsedStateRef !== null, "parsed state not set!");

                var h = this.m_parsedStateRef.histogram;

//                function blue(x) {
//                    return "<span style='color:darkblue;'>" + x + "</span>"
//                }

//                var html = "Lower clip: " + blue(h.lowerClip) + "<br>" +
//                    "Upper clip: " + blue(h.upperClip) + "<br>" +
//                    " Frame min: " + blue(h.frameMin) + "<br>" +
//                    " Frame max: " + blue(h.frameMax);
//                html = html.replace(/ /g, '&nbsp;');
//                this.m_textArea1.setHtml(html);

//                html = "         NaNs: " + blue(h.nNaNs) + "<br>" +
//                    "    Bin value: " + blue(h.binValue) + "<br>" +
//                    "Values in bin: " + blue(h.nValuesInBin) + "<br>" +
//                    "       True %: " + blue(h.trueCount);
//                html = html.replace(/ /g, '&nbsp;');
//                this.m_textArea2.setHtml(html);

//                // adjust the height of the text area to match the content
//                var div = this.m_textArea1.getContentElement().getDomElement();
//                if (div) {
//                    qx.ui.core.queue.Manager.flush();
////                    fv.GLOBAL_DEBUG && fv.console.log("content height after = %O", div.scrollHeight);
//                    this.m_textArea1.setHeight(div.scrollHeight);
//                }

//                this.m_presetButtons.forEach(function (button, ind) {
//                    button.setValue(ind == h.preset);
//                }, this);

                // set the markers
//                if (this.m_lastMarkerInteraction === null ||
//                    (fv.lib.now() - this.m_lastMarkerInteraction > 500)) {
//                    this.m_marker1 = h.marker1;
//                    this.m_marker2 = h.marker2;
//                }
//                else {
//                    // TODO: we missed an update because of interaction, we should schedule one
//                    // just in case someone/something else caused a change in state...
//                }
//                this.m_overlay.update();
            },
            */

            // callback to redraw the overlay
            _overlayRedrawCB: function (e) {
                var data = e.getData();
                var width = data.width;
                var height = data.height;
                var ctx = data.context;

                // clear area (transparent color)
                ctx.clearRect(0, 0, width, height);

                var x1 = this.m_marker1 * width;
                var x2 = this.m_marker2 * width;

                // draw the left / right shades
                ctx.fillStyle = "rgba(255,255,255,0.3)";
                ctx.fillRect(0, 0, x1, height);
                ctx.fillRect(x2, 0, width, height);

                // draw top bar
                ctx.fillStyle = "#008800";
                ctx.fillRect(x1, 0, x2 - x1, 5);

                // draw vertical markers
                ctx.strokeStyle = "#008800";
                ctx.lineWidth = 1;
                ctx.beginPath();
                ctx.moveTo(x1, 0);
                ctx.lineTo(x1, height-20);
                ctx.moveTo(x2, 0);
                ctx.lineTo(x2, height-20);
//                ctx.moveTo(0, height/2);
//                ctx.lineTo(x1, height/2);
//                ctx.moveTo(x2, height/2);
//                ctx.lineTo(width, height/2);
                ctx.stroke();
                ctx.closePath();

                if (this.m_cursorX !== null) {
                    ctx.strokeStyle = "rgba(255,0,0,0.5)";
                    ctx.lineWidth = 1;
                    ctx.beginPath();
                    ctx.moveTo(this.m_cursorX + 0.5, 0);
                    ctx.lineTo(this.m_cursorX + 0.5, height);
                    ctx.stroke();
                    ctx.closePath();
                }
            },

            _overlayMouseMoveCB: function (event) {
                var box = this.m_overlay.getContentLocation("box");
                var pt = {
                    x: event.getDocumentLeft() - box.left,
                    y: event.getDocumentTop() - box.top
                };

                if (this.m_drag === 1) {
                    this.m_drag = 1;
                    this.m_marker1 = pt.x / this.m_overlay.getCanvasWidth();
                    if (this.m_marker1 > this.m_marker2) {
                        this.m_marker1 = this.m_marker2;
                    }
                    this.m_overlay.update();
                    this._emit("ui.histogram.setMarkers",
                        { marker1: this.m_marker1, marker2: this.m_marker2 });
                    this.m_lastMarkerInteraction = fv.lib.now();
                }
                else if (this.m_drag === 2) {
                    this.m_drag = 2;
                    this.m_marker2 = pt.x / this.m_overlay.getCanvasWidth();
                    if (this.m_marker1 > this.m_marker2) {
                        this.m_marker2 = this.m_marker1;
                    }
                    this.m_overlay.update();
                    this._emit("ui.histogram.setMarkers",
                        { marker1: this.m_marker1, marker2: this.m_marker2 });
                    this.m_lastMarkerInteraction = fv.lib.now();
                }
                else {
                    this.m_cursorX = pt.x;
                    this.m_overlay.update();
//                    this._emit("ui.histogram.setCursor", pt.x / this.m_overlay.getCanvasWidth());
//                    this._emit("ui.histogram.setCursor", this.m_cursorX);
                    this.m_gVars.cursor.set( this.m_cursorX);
                }

            },

            _overlayMouseDownCB: function (event) {
                this.m_overlay.capture();
                var box = this.m_overlay.getContentLocation("box");
                var pt = {
                    x: event.getDocumentLeft() - box.left,
                    y: event.getDocumentTop() - box.top
                };
                if (event.getButton() === "left") {
                    this.m_drag = 1;
                    this.m_marker1 = pt.x / this.m_overlay.getCanvasWidth();
                    if (this.m_marker1 > this.m_marker2) this.m_marker1 = this.m_marker2;
                    this.m_lastMarkerInteraction = fv.lib.now();
                }
                else if (event.getButton() === "right") {
                    this.m_drag = 2;
                    this.m_marker2 = pt.x / this.m_overlay.getCanvasWidth();
                    if (this.m_marker1 > this.m_marker2) this.m_marker2 = this.m_marker1;
                    this.m_lastMarkerInteraction = fv.lib.now();
                }
                this.m_overlay.update();
                this._emit("ui.histogram.setMarkers",
                    { marker1: this.m_marker1, marker2: this.m_marker2 });
            },

            _overlayMouseUpCB: function (event) {
                this.m_overlay.releaseCapture();
                var box = this.m_overlay.getContentLocation("box");
                var pt = {
                    x: event.getDocumentLeft() - box.left,
                    y: event.getDocumentTop() - box.top
                };
                this.m_drag = 0;
            },

            _touchBoiler: function(e) {
                this.m_touchDevice = true;
                e.preventDefault();
                this.m_overlay.update();

                // make a copy of all touches
                var i;
                var box = this.getContentLocation("box");
                this.m_touches = [];
                var a = e.getAllTouches();
                for( i = 0 ; i < a.length ; i ++ ) {
                    this.m_touches[i] = {
                        x: e.getDocumentLeft(i) - box.left,
                        y: e.getDocumentTop(i) - box.top,
                        id: e.getIdentifier(i)
                    };
                }

                // return relative horizontal touches in an array
                var res = [];
                var val;
                var cw = this.m_overlay.getCanvasWidth();
                for( i = 0 ; i < a.length ; i ++ ) {
                    val = this.m_touches[i].x / cw;
                    if( val < 0) val = 0;
                    if( val > 1) val = 1;
                    res.push( val);
                }
                res.sort();
                return res;
            },

            _touchStartCB: function(e) {
                var m = this._touchBoiler(e);

                if( m.length === 1) {
                    // figure out which marker the touch was closer to and set m_drag to 1 or 2
                    var d1 = m[0] - this.m_marker1;
                    var d2 = m[0] - this.m_marker2;
                    this.m_drag = ( Math.abs(d1) < Math.abs(d2)) ? 1 : 2;
                    if( this.m_drag === 1)
                        this.m_marker1 = Math.min( m[0], this.m_marker2);
                    else if( this.m_drag === 2)
                        this.m_marker2 = Math.max( m[0], this.m_marker1);
                }
                else if( m.length === 2) {
                    this.m_drag = 3; // indicating two finger touch
                    this.m_marker1 = m[0];
                    this.m_marker2 = m[1];
                }
                else {
                    this.m_drag = 0;
                    return;
                }
                this._emit("ui.histogram.setMarkers",
                    { marker1: this.m_marker1, marker2: this.m_marker2 });
                this.m_lastMarkerInteraction = fv.lib.now();
            },

            _touchMoveCB: function(e) {
                var m = this._touchBoiler(e);

                if( m.length === 1) {
                    if( this.m_drag === 1)
                        this.m_marker1 = Math.min( m[0], this.m_marker2);
                    else if( this.m_drag === 2)
                        this.m_marker2 = Math.max( m[0], this.m_marker1);
                }
                else if( m.length === 2) {
                    this.m_marker1 = m[0];
                    this.m_marker2 = m[1];
                }
                else {
                    this.m_drag = 0;
                    return;
                }
                this._emit("ui.histogram.setMarkers",
                    { marker1: this.m_marker1, marker2: this.m_marker2 });
                this.m_lastMarkerInteraction = fv.lib.now();
            },

            _touchEndCB: function(e) {
                this._touchBoiler(e);
            },

            _touchCancelCB: function(e) {
                this._touchBoiler(e);
            }
        },

        properties: {

        }

    });

