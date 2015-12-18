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
         * Add a page for displaying region or image statistics.
         */
        _addStatsPage : function( title ){
            var statsPage = new skel.widgets.Statistics.StatisticsPage( title );
            this.m_tabView.add( statsPage );
            return statsPage;
        },
        
        /**
         * Remove the statistics tabs.
         */
        _clear : function(){
            var pages = this.m_tabView.getChildren();
            for ( var i = 0; i < pages.length; i++ ){
                this.m_tabView.remove( pages[i] );
            }
        },
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout(new qx.ui.layout.Grow());
            
            this.m_tabView = new qx.ui.tabview.TabView();
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this._add( this.m_tabView );
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
         * Set the server side id of statistics.
         * @param controlId {String} the server side id of the object that produced statistics.
         */
        setId : function( controlId ){
            this.m_id = controlId;
            this._registerStatistics();
        },
        
        /**
         * Callback for a change in statistics on the server.
         */
        _statisticsChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    this._clear();
                    var statistics = JSON.parse( val );
                    var statCount = statistics.stats.length;
                    for ( var i = 0; i < statCount; i++ ){
                        var page = this._addStatsPage( statistics.stats[i].name );
                        page.updateStats( statistics.stats[i] );
                    }
                   
                }
                catch ( err ){
                    console.log( "Problem updating statistics: "+val );
                    console.log( "Error: "+err);
                }
            }
        },
        
        m_connector : null,
        m_tabView : null,
        m_sharedVar : null,
        
        m_id : null
    }


});