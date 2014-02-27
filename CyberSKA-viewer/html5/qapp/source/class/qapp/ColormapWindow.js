/**
 * Colormap window for the CyberSKA image viewer.
 * User: pfederl
 */

/**

 @ignore(fv.console.log)
 @ignore(fv.assert)
 @ignore(fv.makeGlobalVariable)

 @asset(qapp/icons/colormap16.png)


 ************************************************************************ */

qx.Class.define("qapp.ColormapWindow",
    {
        extend: qapp.BetterWindow,

        construct: function (hub) {
            this.base(arguments, hub);

//            this.setWidth(390);
            this.setShowMinimize(false);
            this.setShowMaximize(true);
            this.setShowClose(true);
            this.setUseResizeFrame(false);
            this.setAlwaysOnTop(true);
            this.setCaption("Colormap");
            this.setMinWidth(342);
//            this.setHeight(317);
            this.setWidth(1);
            this.setContentPadding(0, 0, 0, 0);
            this.setIcon( "qapp/icons/colormap16.png");
            this.m_vars = {};

            this.setLayout(new qx.ui.layout.VBox(0));

            var pwview = new qapp.PureWebView("colormapview", "ColormapView", qapp.PureWebView.PNG);
            pwview.setHeight(100);
            pwview.setMinHeight( 50);
            pwview.setMinWidth( 250);
            this.add(pwview, { flex: 1});

            var lowerContainer = new qx.ui.container.Composite(new qx.ui.layout.HBox(5));
            lowerContainer.setPadding(5);
            this.add(lowerContainer, { flex: 1});

            var lowerLeftContainer = new qx.ui.container.Composite( new qx.ui.layout.VBox(5));
            lowerContainer.add( lowerLeftContainer, {flex: 1});

//            var buttonsContainer = new qx.ui.groupbox.GroupBox("Presets");
            var buttonsContainer = new qx.ui.container.Composite();
            buttonsContainer.setLayout(new qx.ui.layout.Flow(2, 2));
            buttonsContainer.setDecorator( "group");
            buttonsContainer.setPadding( 5);
//            buttonsContainer.setContentPadding(3);
//            buttonsContainer.setAllowShrinkY( false);
            var cmaps = [ "Aberration", "Sunbow", "Fire", "CubeHelix1", "CubeHelix2", "Gray", "Heat",
                "Mutant", "RGB", "Sea", "Spring", "Velocity"
//                , "Blargh", "foobar1", "foobar2", "foobar3", "foobar4", "foobar5", "foobar6"
            ];
            cmaps.forEach(function (e) {
                var button = new qx.ui.form.Button(e);
                button.addListener("execute", function () {
                    this._emit("ui.colormap.preset", e);
                }, this);
                button.setAppearance( "cmap-preset-button");
                button.setFocusable( false);
                buttonsContainer.add(button);
            }, this);
            lowerLeftContainer.add(buttonsContainer, {flex: 1});

            // add invert & reverse checkboxes
            var checkBoxContainer = new qx.ui.container.Composite(new qx.ui.layout.HBox(3));
            checkBoxContainer.add(
                new qapp.boundWidgets.ToggleButton("Reverse", "/Colormap/Reverse")
                    .set({ padding: [0, 3, 0, 3] })
            );
            checkBoxContainer.add(
                new qapp.boundWidgets.ToggleButton("Invert", "/Colormap/Invert")
                    .set({ padding: [0, 3, 0, 3] })
            );
            lowerLeftContainer.add(checkBoxContainer);

            // add 2D slider for gamma correction
//            var gammaContainer = new qx.ui.groupbox.GroupBox("Gamma");
//            gammaContainer.setContentPadding(0);
            var gammaContainer = new qx.ui.container.Composite();
            gammaContainer.setLayout( new qx.ui.layout.VBox(0));
            gammaContainer.add( new qapp.boundWidgets.RichLabel( "<b>Gamma:</b>&nbsp;", "", "/Colormap/gamma")
                .set({ padding: 0, paddingLeft: 5 })
            );
            this.m_twoDSlider = new qapp.widgets.TwoDSlider( this.m_hub).set({
                minWidth: 150, maxWidth: 150,
                minHeight: 150, maxHeight: 150
            });
            gammaContainer.add(this.m_twoDSlider);
            lowerContainer.add(gammaContainer);
//            this.m_vars.gamma = fv.makeGlobalVariable( "/Colormap/gamma", function(val){
//                gammaContainer.setLegend( "Gamma: " + val);
//            });


            var fixSlider = function( slider, color) {
                slider.addListener( "appear", function(){
                    slider.getContentElement().setStyle( "background-image", "none");
                    slider.getContentElement().setStyle( "background-color", color);
//                    slider.getContentElement().setStyle( "border", "1px solid rgba(0,0,0,0)");
                    slider.getContentElement().setStyle( "border", "1px solid rgba(0,0,0,0.3)");
                }, this);
            }.bind(this);

            // add the sliders
            var sliderContainer = new qx.ui.container.Composite(new qx.ui.layout.HBox(3));
            this.m_redSlider = new qx.ui.form.Slider("vertical").set({
                value: 0, minimum: 0, maximum: 100, singleStep: 1, pageStep: 10, focusable: false
            });
//            this.m_redSlider.getChildControl("knob").setDecorator( "red-knob");
//            this.m_redSlider.addListener( "appear", function(){
//                this.m_redSlider.getContentElement().setStyle( "background-image", "none");
//                this.m_redSlider.getContentElement().setStyle( "background-color", "#f88");
//                this.m_redSlider.getContentElement().setStyle( "border", "1px solid rgba(0,0,0,0)");
//            }, this);
            fixSlider( this.m_redSlider, "#C9564E");

            sliderContainer.add(this.m_redSlider);
            this.m_greenSlider = new qx.ui.form.Slider("vertical").set({
                value: 0, minimum: 0, maximum: 100, singleStep: 1, pageStep: 10, focusable: false
            });
//            this.m_greenSlider.getChildControl("knob").setDecorator( "green-knob");
            sliderContainer.add(this.m_greenSlider);
            fixSlider( this.m_greenSlider, "#64BE64");
            this.m_blueSlider = new qx.ui.form.Slider("vertical").set({
                value: 0, minimum: 0, maximum: 100, singleStep: 1, pageStep: 10, focusable: false
            });
//            this.m_blueSlider.getChildControl("knob").setDecorator( "blue-knob");
            fixSlider( this.m_blueSlider, "#4E86C9");
            sliderContainer.add(this.m_blueSlider);
            lowerContainer.add(sliderContainer);

            this.m_redSlider.addListener( "changeValue", this._sliderCB, this);
            this.m_greenSlider.addListener( "changeValue", this._sliderCB, this);
            this.m_blueSlider.addListener( "changeValue", this._sliderCB, this);
            this.m_twoDSlider.addListener( "changeValue", this._sliderCB, this);

            this.m_redSlider.setBackgroundColor( "rgba(128,0,0,0.3)");


/*
            // subscribe to both parsedState & setHeader
            this.m_hub.subscribe("toui.parsedState", function (val) {
                fv.console.log("parsedState");
                this.m_parsedStateRef = val;
                this.updateFromState();
            }, this);

            // let the controller know we can receive parsed state updates
            this._emit("ui.parsedStateRequest");
*/

            fv.console.log("ColormapWindow constructed.");
        },

        members: {
//            m_hub: null,
//            m_parsedStateRef: null,
//            m_invertCheckBox: null,
            m_reverseCheckBox: null,
            m_redSlider: null,
            m_greenSlider: null,
            m_blueSlider: null,
            m_twoDSlider: null,

/*
            updateFromState: function () {
                fv.assert(this.m_parsedStateRef !== null, "parsed state not set!");
            },
*/

            /**
             *
             * @private
             */
            _sliderCB: function() {
                this._emit( "ui.colormap.sliders", {
                    red: 100 - this.m_redSlider.getValue(),
                    green: 100 - this.m_greenSlider.getValue(),
                    blue: 100 - this.m_blueSlider.getValue(),
                    s1: this.m_twoDSlider.getValue().x,
                    s2: this.m_twoDSlider.getValue().y
                });
            }

        },

        properties: {

        }

    });

