/**
 * Displays controls for customizing the profile.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.Settings", {
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
            this.setPadding( 0, 0, 0, 0 );

            this._setLayout( new qx.ui.layout.VBox(1));

            this.m_tabView = new qx.ui.tabview.TabView();
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this._add( this.m_tabView );
            
            this.m_rangeSettings = new skel.widgets.Profile.SettingsRange();
            this.m_tabView.add( this.m_rangeSettings );
        },
        
        /**
         * Callback for when profile data state changes on the server.
         */
        _profileDataCB : function(){
            var val = this.m_sharedVarData.get();
            if ( val ){
                try {
                    var profileData = JSON.parse( val );
                    if ( this.m_rangeSettings !== null ){
                        this.m_rangeSettings.dataUpdate( profileData );
                    }
                }
                catch( err ){
                    console.log( "Could not parse: "+val+" error: "+err );
                }
            }
        },
        
        /**
         * Register to get updates from the server.
         */
        _register : function(){
            var path = skel.widgets.Path.getInstance();
            var dataPath = this.m_id + path.SEP + path.DATA;
            this.m_sharedVarData = this.m_connector.getSharedVar( dataPath );
            this.m_sharedVarData.addCB( this._profileDataCB.bind( this));
            this._profileDataCB();
        },

        
        /**
         * Store the server-side id of the server profile settings object.
         * @param id {String} the server-side id of the profile settings object.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_rangeSettings.setId( id );
            this._register();
        },
        
        m_id : null,
        m_connector : null,
        
        m_tabView : null,
        m_rangeSettings : null,
        m_sharedVarData : null
    }
});