/**

 @ignore(fv.console.*)
 @ignore(fv.assert)
 @ignore(fv.hub.emit)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(fv.lib)


 ************************************************************************ */

/* global fv, qx */

qx.Class.define("qapp.G2dFitWindow",
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
            this.setCaption("2D gaussian fit");

            this._constructChildren();

            // we are using this flag to ignore update events from some widgets when we are
            // setting their values ourselves. Qooxdoo will fire events on (some?) widgets
            // even during setValue()... and there does not seem to be a way to disable it.
            this.m_inUpdateState = false;

            this.m_hub.subscribe("toui.g2dWindow.setState", this._updateState, this);

            // if we become active, ask whoever is in charge of the region rectangle
            // to move it to the top
            this.addListener( "changeActive", function() {
                this._emit( "ui.g2dfit.windowStatus",
                    { active: this.getActive(), visible: this.isVisible() });
            }, this);
            this.addListener( "changeVisibility", function() {
                this._emit( "ui.g2dfit.windowStatus",
                    { active: this.getActive(), visible: this.isVisible() });
            }, this);

            // let the controller know we can receive parsed state updates
            this._emit("ui.g2dWindow.isReady");

            fv.GLOBAL_DEBUG && fv.console.log("2d gaussian fit window constructed.");
        },

        members: {
/*
            m_hub: null,

            _emit: function (path, data) {
                this.m_hub.emit(path, data);
            },
*/

            _constructChildren: function() {

                this.setLayout(new qx.ui.layout.VBox(3));
                this.setContentPadding(3);

                // buttons to set the number of gaussians
                // --------------------------------------
                this.add( new qx.ui.basic.Label("Number of gaussians to fit:"));
                var radioContainer = new qx.ui.container.Composite( new qx.ui.layout.HBox(3)).set({
                    allowStretchX: true,
                    allowGrowX: true,
                    alignX: "center",
                    height: 10
                });
//                radioContainer.add( new qx.ui.core.Spacer(), {flex: 1});
                var radioGroup = new qx.ui.form.RadioGroup();
                for( var i = 0 ; i < 5 ; i ++ ) {
//                    var radioButton = new qx.ui.toolbar.RadioButton(i.toString());
//                    var radioButton = new qx.ui.form.ToggleButton(i.toString()).set({
                    var radioButton = new qapp.widgets.MyToggleButton(i.toString()).set({
                        padding: [2,5,2,5]});
                    radioButton.getChildControl("label").set({textAlign: "center"});
                    radioButton.setUserData( "val", i);
                    radioContainer.add(radioButton);
                    radioGroup.add(radioButton);
                }
                radioContainer.add( new qx.ui.core.Spacer(), {flex: 1});
                this.add(radioContainer);
                radioGroup.setAllowEmptySelection(false);
                radioGroup.addListener( "changeSelection", function(e){
//                    fv.console.log( "g2d num changed ", e.getData()[0].getUserData("val"));
                    var num = e.getData()[0].getUserData("val");
                    this._emit( "ui.g2d.setNG", num);
                }, this);

                // results area
                // ------------
                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});

                this.m_resultsWidget = new qx.ui.basic.Label("results").set({
                    rich: true,
                    allowGrowX: true,
                    allowShrinkX: true
                });
                this.add( this.m_resultsWidget, {flex: 1});

                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});
                this.add( new qx.ui.basic.Label("Hold down SHIFT to pan/zoom").set({
                    textAlign: "center",
                    allowGrowX: true,
                    textColor: "#555555",
                    font : "faint"
                }));


            },

            _updateState: function (state) {
                this.m_inUpdateState = true;
                fv.GLOBAL_DEBUG && fv.console.group("g2d is receiving update %O", state);

                function lbl(t) {
                    return "<span style='color:blue'>" + t + "</span> ";
                };

                this.setCaption("2D gaussian fit (frame " + state.frame.toString() + ")");

                var text = "";
//                text += lbl("isNull:") + state.isNull + "<br/>";
                text += lbl("Status:") + state.status + "<br/>";
                text += lbl("Rms:") + state.rms + "<br/>";
                text += lbl("ChiSq:") + state.chisq + "<br/>";
                text += lbl("Const. term:") + state.constantTerm + "<br/>";
                var sp = "&nbsp;&nbsp;&nbsp;"
                for( var i = 0 ; i < state.ng ; i ++ ) {
                    text += "<b>Gaussian #" + (i+1).toString() + ":</b><br/>";
                    text += sp + lbl("Peak:") + state.params[i].peak + "<br/>";
                    text += sp + lbl("Center:") + state.params[i].centerPix + "<br/>";
                    text += sp + sp + state.params[i].centerWCS + "<br/>";
                    text += sp + lbl("Size:") + state.params[i].sizePix + "<br/>";
                    text += sp + lbl("Angle:") + state.params[i].angle + "<br/>";

                }
                this.m_resultsWidget.setValue(text);

                fv.GLOBAL_DEBUG && fv.console.groupEnd();
                this.m_inUpdateState = false;

            }


        },

        properties: {

        }

    });

