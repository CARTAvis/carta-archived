/**
 * A display window for a generic plug-in.
 */

/*******************************************************************************
 * 
 * 
 * 
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColormapDialog",
{
    extend : qx.ui.container.Resizer,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        this._init();
    },

    members : {
        /**
         * Implemented to initialize a context menu.
         */
        _init : function() {
            console.log( "DisplayWindowColorMap windowIdInitialized");
            
            var widgetLayout = new qx.ui.layout.HBox(2);
            this._setLayout(widgetLayout);
            console.log( "Colormap Making histogram");
            this.m_histogram = new skel.widgets.Histogram.Histogram();
            this._add( this.m_histogram );
            console.log( "Colormap initMain");
            this._initMain();
            console.log( "ColorMap initControls");
            this._initControls();
            console.log( "Colormap constructed");
            
            this._initClose();
        },
        
        _initClose : function(){
            var buttonComposite = new qx.ui.container.Composite();
            var buttonLayout = new qx.ui.layout.HBox(2);
            buttonComposite.setLayout(buttonLayout);
            buttonComposite.add(new qx.ui.core.Spacer(), {
                flex : 1
            });
            var closeButton = new qx.ui.form.Button( "Close");
            closeButton.addListener("execute", function() {
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "closeColormap", ""));
            });
            buttonComposite.add(closeButton);
            this.m_mainComposite.add( buttonComposite);
        },
        
        _initControls : function(){
            this.m_controlComposite = new qx.ui.container.Composite();
            this.m_controlComposite.setLayout( new qx.ui.layout.HBox(2));
            this._layoutControls();
            this.m_mainComposite.add( this.m_controlComposite );
        },
        
        _initMain : function(){
            this.m_mainComposite = new qx.ui.container.Composite();
            this.m_mainComposite.setLayout( new qx.ui.layout.VBox(2));
            
            var settingsComposite = new qx.ui.container.Composite();
            var settingsLayout = new qx.ui.layout.HBox( 2 );
            settingsComposite.setLayout( new qx.ui.layout.HBox(2));
            
            this.m_scaleVisible = new qx.ui.form.CheckBox( "Scale...");
            this.m_scaleVisible.setValue( true );
            this.m_scaleVisible.addListener("execute", this._layoutControls, this);
            this.m_modelVisible = new qx.ui.form.CheckBox( "Model...");
            this.m_modelVisible.setValue( true );
            this.m_modelVisible.addListener("execute", this._layoutControls, this);
            this.m_paramsVisible = new qx.ui.form.CheckBox( "Parameters...");
            this.m_paramsVisible.setValue( true );
            this.m_paramsVisible.addListener("execute", this._layoutControls, this);
            this.m_colorMixVisible = new qx.ui.form.CheckBox( "Color Values...");
            this.m_colorMixVisible.setValue( true );
            this.m_colorMixVisible.addListener("execute", this._layoutControls, this);

            
            settingsComposite.add( this.m_scaleVisible );
            settingsComposite.add( this.m_modelVisible );
            settingsComposite.add( this.m_paramsVisible );
            settingsComposite.add( this.m_colorMixVisible );
            
            this.m_graph = new qx.ui.core.Widget();
            this.setBackgroundColor( "#00ff00");
            this.m_graph.setWidth( 200 );
            this.m_graph.setHeight( 100 );
            this.m_mainComposite._add( settingsComposite );
            this.m_mainComposite._add( this.m_graph );
            this._add(this.m_mainComposite);
        },
        
        _layoutControls : function(){
            this.m_controlComposite.removeAll();
            if ( this.m_scaleVisible.getValue()){
                if ( this.m_scaleSettings == null ){
                    console.log( "Colormap making scale");
                    this.m_scaleSettings = new skel.widgets.Colormap.ColorScale();
                }
                console.log( "Colormap adding scale");
                this.m_controlComposite.add( this.m_scaleSettings );
            }
            if ( this.m_modelVisible.getValue()){
                if ( this.m_modelSettings == null ){
                    console.log( "Colormap making model");
                    this.m_modelSettings = new skel.widgets.Colormap.ColorModel();
                }
                console.log( "Colormap adding model");
                this.m_controlComposite.add( this.m_modelSettings );
            }
            if ( this.m_paramsVisible.getValue()){
                if ( this.m_paramsSettings == null ){
                    console.log( "Color map making params");
                    this.m_paramsSettings = new skel.widgets.Colormap.ColorModelParameters();
                }
                console.log( "Colormap adding params");
                this.m_controlComposite.add( this.m_paramsSettings );
            }
            if ( this.m_colorMixVisible.getValue()){
                if ( this.m_colorMixSettings == null ){
                    console.log( "Colormap making mix");
                    this.m_colorMixSettings = new skel.widgets.Colormap.ColorMix();
                }
                console.log( "Colormap adding mix");
                this.m_controlComposite.add( this.m_colorMixSettings );
            }
        },
        m_controlComposite : null,
        m_mainComposite : null,
        m_graph : null,
        m_histogram : null,
        m_colorMixVisible : null,
        m_colorMixSettings : null,
        m_modelVisible : null,
        m_modelSettings : null,
        m_paramsVisible : null,
        m_paramSettings : null,
        m_scaleVisible : null,
        m_scaleSettings : null

    },
    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }
    }
});
