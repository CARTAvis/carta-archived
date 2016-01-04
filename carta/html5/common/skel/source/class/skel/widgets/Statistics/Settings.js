/**
 * Statistics settings (controls).
 */


/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Statistics.Settings", {
    extend : qx.ui.core.Widget,

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
            this._setLayout(new qx.ui.layout.HBox(2));
            
            this._add( new qx.ui.core.Spacer(2), {flex:1} );
            var content = new qx.ui.container.Composite();
            content.setLayout( new qx.ui.layout.Grid() );
            this._add( content );
            this._add( new qx.ui.core.Spacer(2), {flex:1} );
            
            //Show Images Checkbox
            var showImageLabel = new qx.ui.basic.Label( "Image:");
            this.m_showImageCheck = new qx.ui.form.CheckBox();
            this.m_showImageId = this.m_showImageCheck.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendShowImageStatsCmd, this );
            content.add( showImageLabel, {row:0, column:0});
            content.add( this.m_showImageCheck, {row:0, column:1});
            
            //Show Region Statistics CheckBox
            var showRegionLabel = new qx.ui.basic.Label( "Region");
            this.m_showRegionCheck = new qx.ui.form.CheckBox();
            this.m_showRegionId = this.m_showRegionCheck.addListener( skel.widgets.Path.CHANGE_VALUE,
                    this._sendShowRegionStatsCmd, this );
            content.add( showRegionLabel, {row:1, column:0});
            content.add( this.m_showRegionCheck, {row:1, column:1});
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
         * Send a command to the server to show/hide image statistics.
         */
        _sendShowImageStatsCmd : function(){
            if ( this.m_id !== null ){
                var showImageStats = this.m_showImageCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setShowStatsImage";
                var params = "visible:"+showImageStats;
                this.m_connector.sendCommand( cmd, params, null);
            }
        },
        
        /**
         * Send a command to the server to show/hide region statistics.
         */
        _sendShowRegionStatsCmd : function(){
            if ( this.m_id !== null ){
                var showRegionStats = this.m_showRegionCheck.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setShowStatsRegion";
                var params = "visible:"+showRegionStats;
                this.m_connector.sendCommand( cmd, params, null);
            }
        },
        
        /**
         * Set the server-side id for the statistics settings.
         */
        setId : function( id ){
            this.m_id = id;
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
                    
                    //Update show image stats
                    var showImageStats = statPrefs.showStatsImage;
                    if ( this.m_showImageId !== null ){
                        this.m_showImageCheck.removeListenerById( this.m_showImageId );
                        this.m_showImageCheck.setValue( showImageStats );
                        this.m_showImageId = this.m_showImageCheck.addListener( skel.widgets.Path.CHANGE_VALUE, 
                                this._sendShowImageStatsCmd, this );
                    }
                    
                    //Update show region stats
                    var showRegionStats = statPrefs.showStatsRegion;
                    if ( this.m_showRegionId !== null ){
                        this.m_showRegionCheck.removeListenerById( this.m_showRegionId );
                        this.m_showRegionCheck.setValue( showRegionStats );
                        this.m_showRegionId = this.m_showRegionCheck.addListener( skel.widgets.Path.CHANGE_VALUE, 
                                this._sendShowRegionStatsCmd, this );
                    }
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
        m_showRegionCheck : null,
        m_showImageCheck : null,
        m_showRegionId : null,
        m_showImageId : null

    }
});