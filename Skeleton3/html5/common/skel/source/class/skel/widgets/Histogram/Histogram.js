/**
 * Displays a file browser.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.Histogram", {
    extend : qx.ui.core.Widget,

    construct : function() {
        this.base(arguments);

        this._init();

    },

    members : {


        /**
         * Initializes the UI.
         */
        _init : function() {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
            this._initMain();
            this._initControls();

        },
        
        _initControls : function(){
            this.m_controlComposite = new qx.ui.container.Composite();
            this.m_controlComposite.setLayout( new qx.ui.layout.HBox(2));
            this._layoutControls();
        },
        
        _layoutControls : function(){
            this.m_controlComposite.removeAll();
            if ( this.m_rangeVisible.getValue()){
                if ( this.m_rangeSettings == null ){
                    this.m_rangeSettings = new skel.widgets.Histogram.HistogramRange();
                }
                this.m_controlComposite.add( this.m_rangeSettings );
            }
            if ( this.m_displayVisible.getValue() ){
                if ( this.m_displaySettings == null ){
                    this.m_displaySettings = new skel.widgets.Histogram.HistogramDisplay();
                }
                this.m_controlComposite.add( this.m_displaySettings );
            }
            if ( this.m_binVisible.getValue() ){
                if ( this.m_binSettings == null ){
                    this.m_binSettings = new skel.widgets.Histogram.HistogramBin();
                }
                this.m_controlComposite.add( this.m_binSettings );
            }
            if ( this.m_zoomVisible.getValue() ){
                if ( this.m_zoomSettings == null ){
                    this.m_zoomSettings = new skel.widgets.Histogram.HistogramZoom();
                }
                this.m_controlComposite.add( this.m_zoomSettings );
            }
            if ( this.m_cubeVisible.getValue() ){
                if ( this.m_cubeSettings == null ){
                    this.m_cubeSettings = new skel.widgets.Histogram.HistogramCube();
                }
                this.m_controlComposite.add( this.m_cubeSettings );
            }
            if ( this.m_twoDVisible.getValue() ){
                if ( this.m_twoDSettings == null ){
                    this.m_twoDSettings = new skel.widgets.Histogram.Histogram2D();
                }
                this.m_controlComposite.add( this.m_twoDSettings );
            }
            this._add( this.m_controlComposite );
        },
    
        _initMain : function(){
            var mainComposite = new qx.ui.container.Composite();
            mainComposite.setLayout( new qx.ui.layout.VBox(2));
            
            var settingsComposite = new qx.ui.container.Composite();
            var settingsLayout = new qx.ui.layout.HBox( 2 );
            settingsComposite.setLayout( new qx.ui.layout.HBox(2));
            
            this.m_rangeVisible = new qx.ui.form.CheckBox( "Clip...");
            this.m_rangeVisible.setValue( false );
            this.m_rangeVisible.addListener("execute", this._layoutControls, this);
            this.m_displayVisible = new qx.ui.form.CheckBox( "Graph...");
            this.m_displayVisible.setValue( false );
            this.m_displayVisible.addListener("execute", this._layoutControls, this);
            this.m_binVisible = new qx.ui.form.CheckBox( "Bin...");
            this.m_binVisible.setValue( false );
            this.m_binVisible.addListener("execute", this._layoutControls, this);
            this.m_zoomVisible = new qx.ui.form.CheckBox( "Zoom...");
            this.m_zoomVisible.setValue( false );
            this.m_zoomVisible.addListener( "execute", this._layoutControls, this );
            this.m_cubeVisible = new qx.ui.form.CheckBox( "Cube...");
            this.m_cubeVisible.setValue( false );
            this.m_cubeVisible.addListener( "execute", this._layoutControls, this );
            this.m_twoDVisible = new qx.ui.form.CheckBox( "2D...");
            this.m_twoDVisible.setValue( false );
            this.m_twoDVisible.addListener( "execute", this._layoutControls, this );
            
            settingsComposite.add( this.m_rangeVisible);
            settingsComposite.add( this.m_displayVisible );
            settingsComposite.add( this.m_binVisible );
            settingsComposite.add( this.m_zoomVisible );
            settingsComposite.add( this.m_cubeVisible );
            settingsComposite.add( this.m_twoDVisible );
            
            this.m_graph = new qx.ui.core.Widget();
            this.setBackgroundColor( "#00ff00");
            this.m_graph.setWidth( 200 );
            this.m_graph.setHeight( 100 );
            mainComposite._add( settingsComposite );
            mainComposite._add( this.m_graph );
            this._add(mainComposite);
        },
        
        m_controlComposite : null,
        m_graph : null,
        m_binVisible : null,
        m_binSettings : null,
        m_cubeSettings : null,
        m_cubeVisible : null,
        m_twoDVisible : null,
        m_twoDSettings : null,
        m_rangeSettings : null,
        m_displaySettings : null,
        m_zoomSettings : null,
        m_rangeVisible : null,
        m_displayVisible : null,
        m_zoomVisible : null
    }


});