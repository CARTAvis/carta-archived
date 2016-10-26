/**
 * Displays controls for customizing the profile.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.Settings", {
    extend : qx.ui.core.Widget,
    include : skel.widgets.MTabMixin,

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
            this.m_tabListenId = this.m_tabView.addListener( "changeSelection", this._sendTabIndex, this );
            this._add( this.m_tabView );
            
            this.m_pages = [];
            this.m_pages[this.m_INDEX_PLOT] = new skel.widgets.Profile.SettingsDisplay();
            this.m_pages[this.m_INDEX_PROFILES] = new skel.widgets.Profile.SettingsProfiles();
            this.m_pages[this.m_INDEX_CURVES] = new skel.widgets.Profile.SettingsCurves();
            this.m_pages[this.m_INDEX_RANGE] = new skel.widgets.Profile.SettingsRange();
            this.m_pages[this.m_INDEX_FIT] = new skel.widgets.Profile.SettingsFit();
            for ( var i = 0; i < this.m_pages.length; i++ ){
                this.m_tabView.add( this.m_pages[i] );
            }
        },
        
        /**
         * Update the UI based on server-side values.
         * @param profilePrefs {Object} - server-side fit parameters.
         */
        prefUpdate : function( profilePrefs ){
            this.m_pages[this.m_INDEX_PLOT].prefUpdate( profilePrefs );
            this.m_pages[this.m_INDEX_FIT].prefUpdate( profilePrefs );
            this.m_pages[this.m_INDEX_PROFILES].prefUpdate( profilePrefs );
            var tabIndex = profilePrefs.tabIndex;
            this._selectTab( tabIndex );
        },
        
        /**
         * Callback for when profile data state changes on the server.
         */
        _profileDataCB : function(){
            var val = this.m_sharedVarData.get();
            if ( val ){
                try {
                    var profileData = JSON.parse( val );
                    this.m_pages[this.m_INDEX_CURVES].dataUpdate( profileData );
                    this.m_pages[this.m_INDEX_PROFILES].dataUpdate( profileData );
                    this.m_pages[this.m_INDEX_RANGE].dataUpdate( profileData );
                    this.m_pages[this.m_INDEX_FIT].dataUpdate( profileData );
                }
                catch( err ){
                    console.log( "Settings Could not parse: "+val+" error: "+err );
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
            for ( var i = 0; i < this.m_pages.length; i++ ){
                this.m_pages[i].setId( id );
            }
            this._register();
        },
        
        /**
         * Set whether or not manual fit guesses will be used.
         * @param manual {boolean} - true if manual fit guesses will be used;
         *      false otherwise.
         */
        setManualFitGuesses : function( manual ){
            this.m_pages[this.m_INDEX_FIT].setManual( manual );
        },
      
        m_sharedVarData : null,
        m_INDEX_RANGE : 1,
        m_INDEX_CURVES : 4,
        m_INDEX_PROFILES : 2,
        m_INDEX_FIT : 3,
        m_INDEX_PLOT : 0
    }
});