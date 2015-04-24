/**
 * Displays a histogram and controls for customizing it.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.Histogram", {
    extend : qx.ui.core.Widget, 

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {
        
        /**
         * Return the number of significant digits used in the histogram display fields.
         * @return {Number} the significant digits to display.
         */
        getSignificantDigits : function(){
            return this.m_significantDigits;
        },
        
        /**
         * Callback for a change in histogram preference settings.
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
                    if ( this.m_clipSettings !== null ){
                        this.m_clipSettings.setCustomClip( hist.customClip );
                    }
                    
                    if ( this.m_rangeSettings !== null ){
                        this.m_rangeSettings.setBuffer( hist.useClipBuffer );
                    }
                    if ( this.m_cubeSettings !== null ){
                        this.m_cubeSettings.setPlaneMode( hist.planeMode );
                        this.m_cubeSettings.setUnit( hist.rangeUnit );
                    }
                   
                    if ( this.m_displaySettings !== null ){
                        this.m_displaySettings.setStyle(hist.graphStyle);
                        this.m_displaySettings.setLogCount( hist.logCount );
                        this.m_displaySettings.setColored( hist.colored );
                    }
                    if ( this.m_twoDSettings !== null ){
                        this.m_twoDSettings.setFootPrint( hist.twoDFootPrint );
                    }
                    this.m_significantDigits = hist.significantDigits;
                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.clearErrors();

                }
                catch( err ){
                    console.log( "Could not parse: "+val+" error: "+err );
                }
            }
        },
        
        /**
         * Callback for a change in histogram data dependent settings.
         */
        _histogramDataCB : function(){
            var val = this.m_sharedVarData.get();
            if ( val ){
                try {
                    var hist = JSON.parse( val );
                    if ( this.m_clipSettings !== null ){
                        this.m_clipSettings.setColorRange( hist.colorMin, hist.colorMax );
                        this.m_clipSettings.setColorRangePercent( hist.colorMinPercent, hist.colorMaxPercent);
                    }
                    
                    if ( this.m_rangeSettings !== null ){
                        this.m_rangeSettings.setClipBounds( hist.clipMin, hist.clipMax );
                        this.m_rangeSettings.setClipPercents( hist.clipMinPercent, hist.clipMaxPercent);
                        this.m_rangeSettings.setBufferAmount( hist.clipBuffer );
                    }
                    if ( this.m_cubeSettings !== null ){
                        this.m_cubeSettings.setPlaneBounds( hist.planeMin, hist.planeMax );
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
            this.m_clipSettings = new skel.widgets.Histogram.HistogramClip();
            this.m_displaySettings = new skel.widgets.Histogram.HistogramDisplay();
            this.m_cubeSettings = new skel.widgets.Histogram.HistogramCube();
            this.m_twoDSettings = new skel.widgets.Histogram.Histogram2D();

            this.m_settingsContainer.add(this.m_rangeSettings);
            this.m_settingsContainer.add(this.m_binSettings);
            this.m_settingsContainer.add(this.m_displaySettings);
            this.m_settingsContainer.add(this.m_cubeSettings);
            this.m_settingsContainer.add( this.m_clipSettings);
            this.m_settingsContainer.add(this.m_twoDSettings);
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
        
        /**
         * Add or remove the control settings.
         */
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
            var dataPath = this.m_id + path.SEP + "data";
            this.m_sharedVarData = this.m_connector.getSharedVar( dataPath );
            this.m_sharedVarData.addCB( this._histogramDataCB.bind( this));
            this._initView();
            this._histogramChangedCB();
            this._histogramDataCB();
            
        },
        
        /**
         * Set the server side id of this histogram.
         * @param controlId {String} the server side id of the object that produced this histogram.
         */
        setId : function( controlId ){
            this.m_id = controlId;
            
            this.m_binSettings.setId( this.m_id );
            this.m_clipSettings.setId( this.m_id );
            this.m_cubeSettings.setId( this.m_id );
            this.m_twoDSettings.setId( this.m_id );
            this.m_rangeSettings.setId( this.m_id );
            this.m_displaySettings.setId( this.m_id );
            this._registerHistogram();
        },
        
        /**
         * Set the number of significant digits to use for display purposes.
         * @param digits {Number} a positive integer indicating significant digits.
         */
        setSignificantDigits : function( digits ){
            if ( this.m_connector !== null ){
                //Notify the server of the new value.
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setSignificantDigits";
                var params = "significantDigits:"+digits;
                this.m_connector.sendCommand( cmd, params, null);
            }
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
        m_clipSettings : null,
        m_cubeSettings : null,
        m_twoDSettings : null,
        m_rangeSettings : null,
        m_displaySettings : null,
        m_significantDigits : null,
        m_MIN_DIM : 150,
      
        m_id : null,
        m_connector : null,
        m_sharedVar : null,
        m_sharedVarData : null,
        
        m_view : null
    }


});