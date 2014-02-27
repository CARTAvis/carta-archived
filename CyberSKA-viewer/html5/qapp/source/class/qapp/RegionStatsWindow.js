/**

 @ignore(fv.console.*)
 @ignore(fv.assert)
 @ignore(fv.hub.emit)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(fv.lib.closure)


 @asset(qapp/icons/graph.png)

 ************************************************************************ */

/* global fv, qx */

qx.Class.define("qapp.RegionStatsWindow",
    {
        extend: qapp.BetterWindow,

        construct: function (hub) {
            this.base(arguments, hub);

//            this.m_hub = hub;

            this.setShowMinimize(false);
            this.setShowMaximize(false);
            this.setShowClose(true);
            this.setUseResizeFrame(false);
            this.setAlwaysOnTop(true);
            this.setWidth(200);
            this.setLayout(new qx.ui.layout.VBox(3));
            this.setCaption("Region statistics");
            this.setContentPadding(3);

            this._constructChildren();

            // we are using this flag to ignore update events from some widgets when we are
            // setting their values ourselves. Qooxdoo will fire events on (some?) widgets
            // even during setValue()... and there does not seem to be a way to disable it.
            this.m_inUpdateState = false;

            this.m_hub.subscribe("toui.regionStats.setState", this._updateState, this);
//            this.m_hub.subscribe("toui.regionStats.hasDepth", this._hasDepthCB, this);

            // if we become active, ask whoever is in charge of the region rectangle
            // to move it to the top
            this.addListener( "changeActive", function() {
                this._emit( "ui.regionStats.windowStatus",
                    { active: this.getActive(), visible: this.isVisible() });
            }, this);
            this.addListener( "changeVisibility", function() {
                this._emit( "ui.regionStats.windowStatus",
                    { active: this.getActive(), visible: this.isVisible() });
            }, this);

            // let the controller know we can receive parsed state updates
            this._emit("ui.regionStats.isReady");

            fv.GLOBAL_DEBUG && fv.console.log("Region Stats window constructed.");
        },

        members: {
/*
            m_hub: null,

            _emit: function (path, data) {
                this.m_hub.emit(path, data);
            },
*/

            _graphButtonCB: function( id, ev) {
                fv.console.log( "Graph button ", id, "clicked");
                this._emit( "toui.windowManager.toggle", "rs-" + id);
            },

            _constructChildren: function() {
                var that = this;
                this.m_graphButtons = [];
                function vl(label) {
                    return new qx.ui.basic.Label(label).set({ appearance: "valueLabel"});
                }
                function vl2(label) {
                    return new qx.ui.basic.Label(label).set({ appearance: "valueLabel2"});
                }
                function vll( con, label, defaultValue, graphButtonId) {
                    graphButtonId = graphButtonId || null;
                    var row = con.getLayout().getRowCount();
                    var dv = defaultValue || "n/a";
                    con.add( vl( label), { row: row, column: 0});
                    var res = vl2( dv);
                    con.add( res, { row: row, column: 1 });
                    if( graphButtonId != null) {
                        var gb = new qx.ui.form.Button("graph", "qapp/icons/graph.png");
                        gb.set({ show: "icon", focusable: false, padding: 1 });
                        gb.addListener( "execute", that._graphButtonCB.bind( that, graphButtonId));
                        con.add( gb, { row: row, column: 2});
                        that.m_graphButtons.push( gb);
                    }
                    return res;
                }
                this.m_labels = {};
                fv.lib.closure( function() {
                    var con = new qx.ui.container.Composite( new qx.ui.layout.Grid(5,1));
                    con.setAllowGrowX( true);
                    this.m_labels.regionSize = vll( con, "Region size:");
                    this.m_labels.pixels = vll( con, "Pixels:");
                    this.m_labels.nans = vll( con, "NaN pixels:", null, "nan");
                    this.m_labels.minValue = vll( con, "MinValue:", null, "min");
                    this.m_labels.maxValue = vll( con, "Max value:", null, "max");
                    this.m_labels.average = vll( con, "Average:", null, "avg");
                    this.m_labels.sum = vll( con, "Sum:", null, "sum");
                    this.m_labels.std = vll( con, "Std. dev.:", null, "std");

                    con.getLayout().setColumnAlign( 0, "right", "bottom");
                    con.getLayout().setColumnAlign( 1, "left", "bottom");
                    con.getLayout().setColumnAlign( 2, "right", "bottom");
                    con.getLayout().setColumnFlex( 1, 1);
                    this.add( con);
                }, this);

                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});

                fv.lib.closure( function(){
                    var con = new qx.ui.container.Composite( new qx.ui.layout.Grid(5,1));
                    this.m_labels.bkgLevel = vll( con, "Background level:", null, "bkg");
                    this.m_labels.sumMinusBkg = vll( con, "Sum - background:", null, "sumbkg");
                    this.m_labels.maxMinuxBkg = vll( con, "Max - background:", null, "maxbkg");
                    con.getLayout().setColumnAlign( 0, "right", "middle");
                    con.getLayout().setColumnFlex( 1, 1);
                    this.add( con);
                }, this);

                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});

                fv.lib.closure( function(){
                    var con = new qx.ui.container.Composite( new qx.ui.layout.Grid(5,1));
                    this.m_labels.totalFluxDensity = vll( con, "Total flux density:");
                    this.m_labels.aboveBackground = vll( con, "Above background:");
                    this.m_labels.beamArea = vll( con, "Beam area:");
                    con.getLayout().setColumnAlign( 0, "right", "middle");
                    con.getLayout().setColumnFlex( 1, 1);
                    this.add( con);
                }, this);

                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});

                fv.lib.closure( function(){
                    var con = new qx.ui.container.Composite( new qx.ui.layout.Grid(5,1));
                    this.m_labels.intensityOfExtremum = vll( con, "Intensity of extremum:");
                    con.add( vl("Position of extremum:"), { row: 1, column: 0});
                    this.m_labels.positionOfExtremum = vl2( "n/a").set({rich:true});
                    con.add( this.m_labels.positionOfExtremum, { row: 2, column: 0, colSpan: 2});
                    this.m_labels.positionOfExtremum2 = vl2( "n/a").set({rich:true});
                    con.add( this.m_labels.positionOfExtremum2, { row: 3, column: 0, colSpan: 2});
                    con.getLayout().setColumnAlign( 0, "right", "middle");
                    con.getLayout().setColumnFlex( 1, 1);
                    con.getLayout().setRowAlign( 2, "left", "middle");
                    con.getLayout().setRowAlign( 3, "left", "middle");
                    this.add( con);
                }, this);


                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});
                this.add( new qx.ui.basic.Label("Hold down SHIFT to pan/zoom").set({
                    textAlign: "center",
                    allowGrowX: true,
                    textColor: "#555555",
                    font : "faint"
                }));

                this.m_progressBar = new qx.ui.indicator.ProgressBar(0, 100);
                this.m_progressBar.setHeight( 10);
                this.add( this.m_progressBar);
            },

//            _hasDepthCB: function(val) {
//                var vis = val ? "visible" : "excluded";
//                for( var b in this.m_graphButtons) {
//                    b.setVisibility( vis);
//                }
//            },

            _updateState: function (state) {
                this.m_inUpdateState = true;
//                fv.GLOBAL_DEBUG && fv.console.group("region stats is receiving state update %O", state);

                this.setCaption("Region statistics (frame " + state.currentFrame.toString() + ")");

                this.m_labels.regionSize.setValue( state.width + " x " + state.height);
                this.m_labels.pixels.setValue( state.totalPixels);
                this.m_labels.nans.setValue( state.nanPixels);
                this.m_labels.minValue.setValue( state.min);
                this.m_labels.maxValue.setValue( state.max);
                this.m_labels.average.setValue( state.average);
                this.m_labels.sum.setValue( state.sum);
                this.m_labels.std.setValue( state.rms);

                this.m_labels.bkgLevel.setValue( state.bkgLevel);
                this.m_labels.sumMinusBkg.setValue( state.sumMinusBkg);
                this.m_labels.maxMinuxBkg.setValue( state.maxMinuxBkg);

                this.m_labels.totalFluxDensity.setValue( state.totalFluxDensity);
                this.m_labels.aboveBackground.setValue( state.aboveBackground);
                this.m_labels.beamArea.setValue( state.beamArea);

                this.m_labels.intensityOfExtremum.setValue( state.maxMinuxBkg);
/*
                this.m_labels.positionOfExtremum.setValue( state.maxPosX + " , " + state.maxPosX);
                this.m_labels.positionOfExtremum2.setValue( state.maxPosExtra);
*/
                this.m_labels.positionOfExtremum.setValue( state.maxPos1);
                this.m_labels.positionOfExtremum2.setValue( state.maxPos2);

                // toggle showing of graph buttons based on depth
                var vis = ( state.depth > 1 ) ? "visible" : "excluded";
                this.m_graphButtons.forEach( function( b) {
                    b.setVisibility( vis);
                }, this);

                fv.console.log( "rs progress: " + state.nFramesComputed + " / " + state.depth);
                this.m_progressBar.setMaximum( state.depth);
                this.m_progressBar.setValue( state.nFramesComputed);
                this.m_progressBar.setVisibility( state.depth === state.nFramesComputed ? "excluded" : "visible");
                fv.GLOBAL_DEBUG && fv.console.groupEnd();
                this.m_inUpdateState = false;


            }


        },

        properties: {

        }

    });

