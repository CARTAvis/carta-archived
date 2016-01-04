/**
 * Displays image and region statistics.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Statistics.Statistics", {
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
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout(new qx.ui.layout.Grow());
            this.m_statContainer = new qx.ui.container.Composite();
            this.m_statContainer.setLayout( new qx.ui.layout.VBox(2) );
            
            //Image Statistics
            this.m_statsImage = new skel.widgets.Statistics.StatisticsImage();
            this.m_statsImage.addListener( "imageChanged", function(evt){
                var data = evt.getData();
                if ( this.m_selectIndex != data.index ){
                    this.m_selectIndex = data.index;
                    this._statsChanged();
                }
            }, this );
            
            //Divider
            this.m_divWidget = new qx.ui.core.Widget();
            this.m_divWidget.setHeight( 2 );
            this.m_divWidget.setBackgroundColor( skel.theme.Color.colors.selection );
            
            //RegionStatistics
            this.m_statsRegions = new skel.widgets.Statistics.StatisticsRegion();
            this._add( this.m_statContainer );
        },
       
       
        /**
         * Register the shared statistics variable in order to receive updates
         * from the server.
         */
        _registerStatistics : function(){
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id);
            this.m_sharedVar.addCB(this._statisticsChangedCB.bind(this));
            this._statisticsChangedCB();
        },
        
        /**
         * Register the shared statistics variable in order to receive updates
         * from the server.
         */
        _registerStatisticsData : function(){
            var path = skel.widgets.Path.getInstance();
            var dataPath = this.m_id + path.SEP + path.DATA;
            this.m_sharedVarData = this.m_connector.getSharedVar( dataPath );
            this.m_sharedVarData.addCB(this._statisticsChangedDataCB.bind(this));
            this._statisticsChangedDataCB();
        },
        
        
        /**
         * Set the server side id of statistics.
         * @param controlId {String} the server side id of the object that produced statistics.
         */
        setId : function( controlId ){
            this.m_id = controlId;
            this._registerStatistics();
            this._registerStatisticsData();
        },
        
        /**
         * Update the UI based on new statistics from the server.
         * @param firstInit {boolean} - true if this is the first data update
         *      from the server; false if other data updates have previously happened.
         */
        _statsChanged : function( firstInit ){
            this.m_statsImage.updateImages( this.m_stats );
            if ( 0 <= this.m_selectIndex && this.m_selectIndex < this.m_stats.length ){
                //Image stats are always the first.
                var stats = this.m_stats[this.m_selectIndex];
                this.m_statsImage.updateStats( stats[0] );
                //Region stats are the remainder
                var oldRegionStats = this.m_statsRegions.isStats();
                this.m_statsRegions.updateStats( stats.slice(1, stats.length) );
                var newRegionStats = this.m_statsRegions.isStats();
                if ( oldRegionStats != newRegionStats || firstInit ){
                    this._layout();
                }
            }
        },
        
        /**
         * Show image and region statistics based on what is available.
         */
        _layout : function(){
            this.m_statContainer.removeAll();
            if ( this.m_showImageStats && this.m_stats !== null ){
                this.m_statContainer.add( this.m_statsImage );
            }
            var regionStats = this.m_statsRegions.isStats();
            if ( regionStats ){
                if ( this.m_showImageStats && this.m_showRegionStats ){
                    this.m_statContainer.add( this.m_divWidget );
                }
                if ( this.m_showRegionStats ){
                    this.m_statContainer.add( this.m_statsRegions );
                }
            }
        },
        
        /**
         * Callback for a change in statistics on the server.
         */
        _statisticsChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var statPrefs = JSON.parse( val );
                    this.m_showImageStats = statPrefs.showStatsImage;
                    this.m_showRegionStats = statPrefs.showStatsRegion;
                    
                    this._layout();
                   
                }
                catch ( err ){
                    console.log( "Problem updating statistics: "+val );
                    console.log( "Error: "+err);
                }
            }
        },
        
        /**
         * Callback for a change in statistics on the server.
         */
        _statisticsChangedDataCB : function(){
            var val = this.m_sharedVarData.get();
            if ( val ){
                try {
                    var firstInit = false;
                    if ( this.m_stats == null || this.m_stats.length == 0 ){
                        firstInit = true;
                    }
                    var statistics = JSON.parse( val );
                    this.m_selectIndex = statistics.selectedIndex;
                    this.m_stats = statistics.stats;
                    this._statsChanged( firstInit );
                }
                catch( err ){
                    console.log( "Problem updating statistics data: "+val );
                    console.log( "Error: " + err );
                }
            }
        },
        
        m_connector : null,
        m_sharedVar : null,
        m_sharedVarData : null,
        m_selectIndex : 0,
        m_showImageStats : false,
        m_showRegionStats : false,
      
        m_stats : null,
        
        m_statContainer : null,
        m_statsImage : null,
        m_statsRegions : null,
        m_divWidget : null,
        
        m_id : null
    }


});