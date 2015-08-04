/**
 * Displays controls for customizing colormap properties..
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.PageColorMap", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Color Map", "");
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
           
            this.m_scaleSettings = new skel.widgets.Colormap.ColorScale();
            this.m_colorMixSettings = new skel.widgets.Colormap.ColorMix();
            
            this.add( this.m_scaleSettings );
            this.add( this.m_colorMixSettings, {flex:1} );
        },
        

        
        /**
         * Update from the server when the color map settings have changed.
         * @param controls {Object} - information about the color map 
         *      from the server.
         */
        setControls : function( controls ){
            if ( this.m_scaleSettings !== null ){
                this.m_scaleSettings.setMapName( controls.colorMapName);
                this.m_scaleSettings.setReverse( controls.reverse );
                this.m_scaleSettings.setInvert( controls.invert );
            }
           
            if ( this.m_colorMixSettings !== null ){
                this.m_colorMixSettings.setMix( controls.colorMix.redPercent, 
                        controls.colorMix.greenPercent, controls.colorMix.bluePercent );
            }
        },
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_scaleSettings.setId( id );
            this.m_colorMixSettings.setId ( id );
        },
        
        m_id : null,
        
        m_scaleSettings : null,
        m_colorMixSettings : null
    }
});