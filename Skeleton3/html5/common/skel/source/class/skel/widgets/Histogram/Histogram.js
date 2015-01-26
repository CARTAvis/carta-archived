/**
 * Displays a histogram and controls for customizing it.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.Histogram", {
    extend : qx.ui.core.Widget,

    construct : function(  ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {
        /**
         * Callback for a change in histogram settings.
         */
        _histogramChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var hist = JSON.parse( val );
                    if ( this.m_binSettings !== null ){
                        this.m_binSettings.setBinCount(hist.binCount );
                    }
                  
                    if ( this.m_rangeSettings !== null ){
                        this.m_rangeSettings.setClipBounds( hist.clipMin, hist.clipMax );
                        this.m_rangeSettings.setClipIndex( hist.clipIndex );
                        this.m_rangeSettings.setApplyClipToImage( hist.applyClipToImage );
                    }
                    if ( this.m_cubeSettings !== null ){
                        this.m_cubeSettings.setPlaneMode( hist.planeMode );
                        this.m_cubeSettings.setSelectedPlane( hist.selectedPlane );
                        this.m_cubeSettings.setPlaneBounds( hist.planeMin, hist.planeMax );
                    }
                    if ( this.m_displaySettings !== null ){
                        this.m_displaySettings.setStyle(hist.graphStyle);
                        this.m_displaySettings.setLogCount( hist.logCount );
                        this.m_displaySettings.setColored( hist.colored );
                    }
                    if ( this.m_twoDSettings !== null ){
                        this.m_twoDSettings.setFootPrint( hist.twoDFootPrint );
                    }
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
                }
            }
        },
        
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
            this.setAllowGrowX( true );
            this.setAllowGrowY( true );
            this._initMain();
            this._initControls();
        },
       
        /**
         * Initializes the histogram settings.
         */
        _initControls : function(){
            this.m_controlComposite = new qx.ui.container.Composite();
            this.m_controlComposite.setLayout( new qx.ui.layout.HBox(2));
            this.m_rangeSettings = new skel.widgets.Histogram.HistogramRange();
            this.m_displaySettings = new skel.widgets.Histogram.HistogramDisplay();
            this.m_binSettings = new skel.widgets.Histogram.HistogramBin();
            this.m_zoomSettings = new skel.widgets.Histogram.HistogramZoom();
            this.m_cubeSettings = new skel.widgets.Histogram.HistogramCube();
            this.m_twoDSettings = new skel.widgets.Histogram.Histogram2D();
            this._layoutControls();
            this._add( this.m_controlComposite );
        },
        
        
        /**
         * Initializes the menu for setting the visibility of individual histogram
         * settings and the main graph.
         */
        _initMain : function(){
            this.m_mainComposite = new qx.ui.container.Composite();
            this.m_mainComposite.setLayout( new qx.ui.layout.VBox(2));
            this.m_mainComposite.set ({
                minWidth : 200,
                minHeight : 200,
                allowGrowX: true,
                allowGrowY: true
            });
            
            var settingsComposite = new qx.ui.container.Composite();
            var settingsLayout = new qx.ui.layout.HBox( 2 );
            settingsComposite.setLayout( new qx.ui.layout.HBox(2));
            
            this.m_rangeVisible = new qx.ui.form.CheckBox( "Clip...");
            this.m_rangeVisible.setValue( false );
            this.m_rangeVisible.addListener("execute", this._layoutControls, this);
            
            this.m_displayVisible = new qx.ui.form.CheckBox( "Display...");
            this.m_displayVisible.setValue( false );
            this.m_displayVisible.addListener("execute", this._layoutControls, this);
            this.m_binVisible = new qx.ui.form.CheckBox( "Bin...");
            this.m_binVisible.setValue( false );
            this.m_binVisible.addListener("execute", this._layoutControls, this);
            skel.widgets.TestID.addTestId( this.m_binVisible, skel.widgets.TestID.HISTOGRAM_BIN_COUNT_CHECK);

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
            
            this.m_mainComposite._add( settingsComposite );
            
            this._add(this.m_mainComposite);
        },
        
        /**
         * Initialize the histogram view.
         */
        _initView : function(){
            if (this.m_view === null) {
                this.m_view = new skel.boundWidgets.View(this.m_id);
                this.m_view.setAllowGrowX( false );
                this.m_view.setAllowGrowY( false );
                this.m_view.setHeight(335);
                this.m_view.setWidth(335);
                if ( this.m_mainComposite.indexOf( this.m_view) < 0 ){
                    this.m_mainComposite.add( this.m_view );

                }
            }
        },
        
        /**
         * Adds/removes histogram settings based on user preference.
         */
        _layoutControls : function(){
            this.m_controlComposite.removeAll();
            if ( this.m_rangeVisible.getValue()){
                this.m_controlComposite.add( this.m_rangeSettings );
            }
            if ( this.m_displayVisible.getValue() ){
                this.m_controlComposite.add( this.m_displaySettings );
            }
            if ( this.m_binVisible.getValue() ){
                this.m_controlComposite.add( this.m_binSettings );
            }
            if ( this.m_zoomVisible.getValue() ){
                this.m_controlComposite.add( this.m_zoomSettings );
            }
            if ( this.m_cubeVisible.getValue() ){
                this.m_controlComposite.add( this.m_cubeSettings );
            }
            if ( this.m_twoDVisible.getValue() ){
                this.m_controlComposite.add( this.m_twoDSettings );
            }
            
        },
        
        /**
         * Register to get updates on histogram settings from the server.
         */
        _registerHistogram : function(){
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id);
            this.m_sharedVar.addCB(this._histogramChangedCB.bind(this));
            this._initView();
            this._histogramChangedCB();
            
        },
        
        /**
         * Set the server side id of this histogram.
         * @param controlId {String} the server side id of the object that produced this histogram.
         */
        setId : function( controlId ){
            this.m_id = controlId;
            
            this.m_binSettings.setId( this.m_id );
            this.m_cubeSettings.setId( this.m_id );
            this.m_twoDSettings.setId( this.m_id );
            this.m_rangeSettings.setId( this.m_id );
            this.m_displaySettings.setId( this.m_id );
            this.m_zoomSettings.setId( this.m_id );
            this._registerHistogram();
        },
        
        m_controlComposite : null,
        m_mainComposite : null,
        
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
        m_zoomVisible : null,
        
        m_id : null,
        m_connector : null,
        m_sharedVar : null,
        
        m_view : null
    }


});