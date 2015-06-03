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
                    if ( this.m_binSettings !== null ){
                        this.m_binSettings.setBinCount( hist.binCount );
                        this.m_binSettings.setBinWidth( hist.binWidth );
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
            this._setLayout(new qx.ui.layout.Grow());
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content );
            this.m_content.setLayout(new qx.ui.layout.VBox());
            
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
            
            this.m_content.add( this.m_mainComposite, {flex:1});
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
        layoutControls : function( widget, visible ){
            if(visible){
                //Add the widget to the settings container.
                if ( this.m_settingsContainer.indexOf( widget ) < 0 ){
                    this.m_settingsContainer.add( widget );
                }
                //Make sure the settings container is visible.
                if ( this.m_content.indexOf( this.m_settingsContainer ) < 0 ){
                    this.m_content.add( this.m_settingsContainer );
                }
            }
            else {
                //Remove the widget from the settings container.
                if ( this.m_settingsContainer.indexOf( widget ) >= 0 ){
                    this.m_settingsContainer.remove( widget );
                }
                //If this is the last widget in the container, then remove the container.
                var layoutItems = this.m_settingsContainer.getChildren();
                var childCount = layoutItems.length;
                if ( childCount === 0 ){
                    if ( this.m_content.indexOf( this.m_settingsContainer ) >= 0 ){
                        this.m_content.remove( this.m_settingsContainer );
                    }
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
         * Show or hide the bin count settings.
         * @param visible {boolean} if the settings should be shown; false otherwise.
         */
        showHideBinCount : function( visible ){
            this.layoutControls( this.m_binSettings, visible );
        },
        
        /**
         * Show or hide the clip settings.
         * @param visible {boolean} if the settings should be shown; false otherwise.
         */
        showHideClips : function( visible ){
            this.layoutControls( this.m_clipSettings, visible );
        },
        
        /**
         * Show or hide the cube settings.
         * @param visible {boolean} if the settings should be shown; false otherwise.
         */
        showHideCube : function( visible ){
            this.layoutControls( this.m_cubeSettings, visible );
        },
        
        /**
         * Show or hide the 2D settings.
         * @param visible {boolean} if the settings should be shown; false otherwise.
         */
        showHide2D : function( visible ){
            this.layoutControls( this.m_twoDSettings, visible );
        },
        
        /**
         * Show or hide the range settings.
         * @param visible {boolean} if the settings should be shown; false otherwise.
         */
        showHideRange : function( visible ){
            this.layoutControls( this.m_rangeSettings, visible );
        },
        
        /**
         * Show or hide the display settings.
         * @param visible {boolean} if the settings should be shown; false otherwise.
         */
        showHideDisplay : function( visible ){
            this.layoutControls( this.m_displaySettings, visible );
        },
        
        m_content : null,
        m_mainComposite : null,
        m_settingsContainer : null,
        
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