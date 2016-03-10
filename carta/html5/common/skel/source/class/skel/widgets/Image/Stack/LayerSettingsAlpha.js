/**
 * Controls for setting the transparency of a layer.
 */

qx.Class.define("skel.widgets.Image.Stack.LayerSettingsAlpha", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this._init( );
    },
    
    statics : {
        TYPE : "alpha"
    },

    members : {
        
        /**
         * Return an identifier for the type of layer settings.
         * @return {String} - an identifier for the type of layer settings.
         */
        getType : function(){
            return skel.widgets.Image.Stack.LayerSettingsAlpha.TYPE;
        },
        
        
        /*
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout( new qx.ui.layout.VBox(2) );
            this._initOpacity();
        },
        
        /**
         * Initialize the opacity control.
         */
        _initOpacity : function(){
            var transContainer = new qx.ui.container.Composite();
            transContainer.setLayout( new qx.ui.layout.HBox(1));
            this.m_transparency = new skel.widgets.CustomUI.TextSlider("setMaskAlpha", "alpha",
                    0, skel.widgets.Path.MAX_RGB, 0, 
                    "Transparency", true, 
                    "Set the transparency.", "Slide to set the transparency.",
                    "maskAlphaTextField", "maskAlphaSlider", false);
            this.m_transparency.setNotify( true );
            transContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            transContainer.add( this.m_transparency );
            transContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            this._add( transContainer );
        },
       
 
        /**
         * Update the UI with mask information from the server.
         * @param mask {Object} - mask information from the server.
         */
        setControls : function( mask ){
            this.m_transparency.setValue( mask.alpha );
        },
        
        
        /**
         * Set the id of the server-side object that handles mask information.
         * @param id {String} - server-side identifier of object handling mask information.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_transparency.setId( id );
        },
        
        m_id : null,
        m_transparency : null
    }
});