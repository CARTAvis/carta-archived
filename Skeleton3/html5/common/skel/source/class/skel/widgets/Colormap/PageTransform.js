/**
 * Displays controls for color transform settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.PageTransform", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Transform", "");
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
           
            this.m_transformSettings = new skel.widgets.Colormap.ColorTransform();
            this.m_modelSettings = new skel.widgets.Colormap.ColorModel();
            this.m_preferences = new skel.widgets.CustomUI.SignificantDigits();
           
            this.add( this.m_transformSettings );
            this.add( this.m_modelSettings );
            this.add( this.m_preferences );
        },
        
                
        /**
         * Update from the server when the color transform settings have changed.
         * @param controls {Object} - information about the color transform 
         *      settings from the server.
         */
        setControls : function( controls ){
            if ( this.m_modelSettings !== null ){
                this.m_transformSettings.setControls( controls);
                this.m_modelSettings.setGamma( controls.gamma );
                this.m_preferences.setControls( controls );
            }
        },
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_transformSettings.setId( id );
            this.m_modelSettings.setId ( id );
            this.m_preferences.setId( id );
        },
        
        
        
        m_id : null,
        
        m_transformSettings : null,
        m_modelSettings : null,
        m_preferences : null
    }
});