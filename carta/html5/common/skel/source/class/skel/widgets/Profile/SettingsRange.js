/**
 * Displays controls for customizing profile range settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.SettingsRange", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Range", "");
        this._init( );
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this.setMargin( 1, 1, 1, 1 );
            this._setLayout(new qx.ui.layout.HBox(2));
           
            this.m_zoomSettings = new skel.widgets.CustomUI.ZoomControlsWidget("profile");
            
            this.add( this.m_zoomSettings );
        },
        
        
        /**
         * Update range data based on server-side values.
         * @param profileData {Object} - server-side information.
         */
        dataUpdate : function(profileData){
            if ( this.m_zoomSettings !== null ){
                this.m_zoomSettings.setClipBounds( profileData.clipMinClient, profileData.clipMaxClient );
                this.m_zoomSettings.setClipPercents( profileData.clipMinPercent, profileData.clipMaxPercent);
                this.m_zoomSettings.setBufferAmount( profileData.clipBuffer );
            }
        },
        
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_zoomSettings.setId( id );
        },
        
        m_id : null,
        m_zoomSettings : null
    }
});