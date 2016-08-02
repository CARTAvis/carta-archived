/**
 * Displays controls for a North-East compass on the image.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Context.SettingsCompassNE", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "NE", "");
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this._setLayout(new qx.ui.layout.VBox(2));
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content, {flex:1} );
            this.m_content.setLayout(new qx.ui.layout.HBox(1));
            this.m_contextSettings = new skel.widgets.Image.Context.SettingsContext(1);
            this.m_content.add( this.m_contextSettings );
        },
      

        /**
         * Update the UI based on server-side context image settings.
         * @param controls {Object} - server side context image settings.
         */
        setControls : function( controls ){
            this.m_contextSettings.setControls( controls.compassNE, controls.compassVisibleNE, controls.penWidthMax );
        },

        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_contextSettings.setId( id );
           
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_contextSettings : null
      
    }
});