/**
 * Displays controls for customizing profile range settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.SettingsDisplay", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Display", "");
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
           
            this.m_axesSettings = new skel.widgets.Profile.SettingsAxis();
            this.add( this.m_axesSettings );
            
            this.m_zoomSettings = new skel.widgets.Profile.SettingsZoom();
            this.add( this.m_zoomSettings );
            
            this.m_legendSettings = new skel.widgets.Profile.SettingsLegend();
            this.add( this.m_legendSettings );
        },
        
        
        /**
         * Update range data based on server-side values.
         * @param profilePrefs {Object} - server-side information.
         */
        prefUpdate : function(profilePrefs){
            if ( this.m_axisSettings !== null ){
                this.m_axesSettings.setAxisBottomUnits( profilePrefs.axisUnitsBottom );
                this.m_axesSettings.setAxisLeftUnits( profilePrefs.axisUnitsLeft );
                
            }
            if ( this.m_legendSettings !== null ){
                this.m_legendSettings.prefUpdate( profilePrefs );
            }
        },
        
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_axesSettings.setId( id );
            this.m_legendSettings.setId( id );
            this.m_zoomSettings.setId( id );
        },
        
        m_id : null,
        m_axesSettings : null,
        m_legendSettings : null,
        m_zoomSettings : null
    }
});