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
                        this.m_binSettings.setBinWidth(hist.binWidth );
                    }
                    
                    if ( this.m_rangeSettings !== null ){
                        this.m_rangeSettings.setClipBounds( hist.clipMin, hist.clipMax );
                        this.m_rangeSettings.setApplyClipToImage( hist.applyClipToImage );
                        this.m_rangeSettings.setClipPercents( hist.clipMinPercent, hist.clipMaxPercent);
                        this.m_rangeSettings.setBuffer( hist.useClipBuffer );
                        this.m_rangeSettings.setBufferAmount( hist.clipBuffer );
                    }
                    if ( this.m_cubeSettings !== null ){
                        this.m_cubeSettings.setPlaneMode( hist.planeMode );
                        this.m_cubeSettings.setPlaneRangeMax( hist.planeRangeMax );
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
                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.clearErrors();

                }
                catch( err ){
                    console.log( "Could not parse: "+val+" error: "+err );
                }
            }
        },
        
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.Grow();
            this._setLayout(widgetLayout);
            this.setAllowGrowX( true );
            this.setAllowGrowY( true );
            this.m_settingsVisible = false;
            this._initMain();
            this._initControls();
            
        },
       
        /**
         * Initializes the histogram settings.
         */
        _initControls : function(){
            var layout = new qx.ui.layout.Flow();
            layout.setAlignX( "left" );
            layout.setSpacingX(1);
            layout.setSpacingY(1);

            this.m_settingsContainer = new qx.ui.container.Composite(layout);
            this.m_settingsContainer.setPadding(1);
            this.m_rangeSettings = new skel.widgets.Histogram.HistogramRange();
            this.m_binSettings = new skel.widgets.Histogram.HistogramBin();
            this.m_displaySettings = new skel.widgets.Histogram.HistogramDisplay();
            this.m_zoomSettings = new skel.widgets.Histogram.HistogramZoom();
            this.m_cubeSettings = new skel.widgets.Histogram.HistogramCube();
            this.m_twoDSettings = new skel.widgets.Histogram.Histogram2D();

            this.m_settingsContainer.add(this.m_rangeSettings);
            this.m_settingsContainer.add(this.m_binSettings);
            this.m_settingsContainer.add(this.m_displaySettings);
            this.m_settingsContainer.add(this.m_twoDSettings);
            this.m_settingsContainer.add(this.m_cubeSettings);
            this.m_settingsContainer.add(this.m_zoomSettings);
        },
        
        
        /**
         * Initializes the menu for setting the visibility of individual histogram
         * settings and the main graph.
         */
        _initMain : function(){
            this.m_mainComposite = new qx.ui.container.Composite();
            this.m_mainComposite.setLayout( new qx.ui.layout.VBox(2));
            this.m_mainComposite.set ({
                minWidth : this.m_MIN_DIM,
                minHeight : this.m_MIN_DIM,
                allowGrowX: true,
                allowGrowY: true
            });
            
            this._add(this.m_mainComposite);
        },
        
        /**
         * Initialize the histogram view.
         */
        _initView : function(){
            if (this.m_view === null) {
                this.m_view = new skel.boundWidgets.View.DragView(this.m_id);
                this.m_view.setAllowGrowX( true );
                this.m_view.setAllowGrowY( true );
                this.m_view.setMinHeight(this.m_MIN_DIM);
                this.m_view.setMinWidth(this.m_MIN_DIM);
                if ( this.m_mainComposite.indexOf( this.m_view) < 0 ){
                    this.m_mainComposite.add( this.m_view, {flex:1} );

                }
            }
        },
        
        layoutControls : function(){
            if(this.m_settingsVisible){
                if ( this.m_mainComposite.indexOf( this.m_settingsContainer) < 0 ){
                    this.m_mainComposite.add( this.m_settingsContainer );
                }
            }
            else{
                if ( this.m_mainComposite.indexOf( this.m_settingsContainer) >= 0 ){
                    this.m_mainComposite.remove( this.m_settingsContainer );
                }
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
        
        /**
         * Set whether or not the user settings should be visible.
         * @param visible {boolean} true if the settings should be visible; false otherwise.
         */
        toggleSettings : function( ){
            this.m_settingsVisible = !this.m_settingsVisible;
            this.layoutControls();
        },
        
        m_mainComposite : null,
        m_settingsContainer : null,
        m_settingsVisible : null,
        
        m_binSettings : null,
        m_cubeSettings : null,
        m_twoDSettings : null,
        m_rangeSettings : null,
        m_displaySettings : null,
        m_zoomSettings : null,
        m_MIN_DIM : 150,
      
        m_id : null,
        m_connector : null,
        m_sharedVar : null,
        
        m_view : null
    }


});