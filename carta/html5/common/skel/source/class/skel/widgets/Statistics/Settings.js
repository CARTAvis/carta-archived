/**
 * Statistics settings (controls).
 */


/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Statistics.Settings", {
    extend : qx.ui.tabview.TabView,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init( );
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.m_settingsImage = new skel.widgets.Statistics.SettingsPage( "Image", "setShowStatsImage" );
            this.add( this.m_settingsImage );
            
            this.m_settingsRegion = new skel.widgets.Statistics.SettingsPage( "Region", "setShowStatsRegion");
            this.add( this.m_settingsRegion );
        },
        
        /**
         * Register for a callback for when statistics change on the server.
         */
        _register : function(){
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id );
            this.m_sharedVar.addCB(this._settingsChangedCB.bind(this));
            this._settingsChangedCB();
        },
        
        
        /**
         * Set the server-side id for the statistics settings.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_settingsImage.setId( this.m_id );
            this.m_settingsRegion.setId( this.m_id );
            this._register();
        },
        
        /**
         * Callback when statistics settings change on the server.
         */
        _settingsChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var statPrefs = JSON.parse( val );
                    this.m_settingsImage.setPrefs( statPrefs.showStatsImage, statPrefs.image );
                    
                    //Update show region stats
                    this.m_settingsRegion.setPrefs( statPrefs.showStatsRegion, statPrefs.region );
                }
                catch ( err ){
                    console.log( "Problem updating statistic settings: "+val );
                    console.log( "Error: "+err);
                }
            }
        },
        
        m_connector : null,
        m_id : null,
        m_sharedVar : null,
        
        m_settingImage : null,
        m_settingsRegion : null
    }
});