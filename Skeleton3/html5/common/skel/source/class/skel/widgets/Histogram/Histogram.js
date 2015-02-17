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
                        //this.m_rangeSettings.setClipIndex( hist.clipIndex );
                        this.m_rangeSettings.setApplyClipToImage( hist.applyClipToImage );
                        this.m_rangeSettings.setClipPercents( hist.clipMinPercent, hist.clipMaxPercent);
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
                    console.log( "Could not parse: "+val+" error: "+err );
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
            this.m_settingsVisible = false;
        },
       
        /**
         * Initializes the histogram settings.
    
         */
        _initControls : function(){
            //this.m_controlComposite = new qx.ui.container.Composite();
            //this.m_controlComposite.setLayout( new qx.ui.layout.HBox(10));

            var layout = new qx.ui.layout.Flow();
            layout.setAlignX( "left" );
            layout.setSpacingX(10);
            layout.setSpacingY(10);

            this.m_settingsContainer = new qx.ui.container.Composite(layout);
            this.m_settingsContainer.setPadding(10);

            this.m_rangeContainer = new qx.ui.groupbox.GroupBox("Range Settings", "");
            this.m_rangeContainer.setLayout( new qx.ui.layout.HBox(10));
            this.m_rangeSettings = new skel.widgets.Histogram.HistogramRange();
            this.m_rangeContainer.add( this.m_rangeSettings );
            //this.m_controlComposite.add(m_rangeContainer);
            //this._add(m_rangeContainer);
            // m_settingsContainer.add(m_rangeContainer);

            this.m_binContainer = new qx.ui.groupbox.GroupBox("Bin Count Settings", "");
            this.m_binContainer.setLayout( new qx.ui.layout.HBox(5));
            this.m_binSettings = new skel.widgets.Histogram.HistogramBin();
            this.m_binContainer.add( this.m_binSettings );
            //this._add(m_binContainer);
            // m_settingsContainer.add(m_binContainer);
            
            this.m_displayContainer = new qx.ui.groupbox.GroupBox("Display Settings", "");
            this.m_displayContainer.setLayout( new qx.ui.layout.HBox(10));
            this.m_displaySettings = new skel.widgets.Histogram.HistogramDisplay();
            this.m_displayContainer.add( this.m_displaySettings );
            //this._add(m_displayContainer);
            // m_settingsContainer.add(m_displayContainer);
            
            this.m_zoomContainer = new qx.ui.groupbox.GroupBox("Zoom Settings", "");
            this.m_zoomContainer.setLayout( new qx.ui.layout.HBox(10));
            this.m_zoomSettings = new skel.widgets.Histogram.HistogramZoom();
            this.m_zoomContainer.add( this.m_zoomSettings );
            // this._add(m_zoomContainer);
            // m_settingsContainer.add(m_zoomContainer);

            this.m_cubeContainer = new qx.ui.groupbox.GroupBox("Cube Settings", "");
            this.m_cubeContainer.setLayout( new qx.ui.layout.HBox(10));
            this.m_cubeSettings = new skel.widgets.Histogram.HistogramCube();
            this.m_cubeContainer.add( this.m_cubeSettings );
            // this._add(m_cubeContainer);
            // m_settingsContainer.add(m_cubeContainer);

            this.m_twoDContainer = new qx.ui.groupbox.GroupBox("2D Settings", "");
            this.m_twoDContainer.setLayout( new qx.ui.layout.HBox(10));
            this.m_twoDSettings = new skel.widgets.Histogram.Histogram2D();
            this.m_twoDContainer.add( this.m_twoDSettings );
            // this._add(m_twoDContainer);
            // m_settingsContainer.add(m_twoDContainer);

            this._add(this.m_settingsContainer);
            //this._layoutControls();
            //this._add( this.m_controlComposite );
            
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
            
           
            
            this._add(this.m_mainComposite);
        },
        
        /**
         * Initialize the histogram view.
         */
        _initView : function(){
            if (this.m_view === null) {
                this.m_view = new skel.boundWidgets.View.SuffixedView(this.m_id);
                this.m_view.setAllowGrowX( false );
                this.m_view.setAllowGrowY( false );
                this.m_view.setHeight(335);
                this.m_view.setWidth(335);
                if ( this.m_mainComposite.indexOf( this.m_view) < 0 ){
                    this.m_mainComposite.add( this.m_view );

                }
            }
        },
        
        layoutControls : function(){
            if(this.m_settingsVisible){
                this.m_settingsContainer.add(this.m_rangeContainer);
                this.m_settingsContainer.add(this.m_binContainer);
                this.m_settingsContainer.add(this.m_displayContainer);
                this.m_settingsContainer.add(this.m_twoDContainer);
                this.m_settingsContainer.add(this.m_cubeContainer);
                this.m_settingsContainer.add(this.m_zoomContainer);
            }
            else{
                this.m_settingsContainer.removeAll();
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
      

        m_rangeContainer : null,
        m_binContainer : null,
        m_displayContainer : null,
        m_zoomContainer : null,
        m_cubeContainer : null,
        m_twoDContainer : null, 

        m_id : null,
        m_connector : null,
        m_sharedVar : null,
        
        m_view : null
    }


});