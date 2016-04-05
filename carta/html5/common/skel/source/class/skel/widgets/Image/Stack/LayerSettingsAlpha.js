/**
 * Controls for setting the transparency of a layer.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Stack.LayerSettingsAlpha", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
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
            this.m_transparency.addListener( "textSliderChanged", this._sendTransparency, this );
            transContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            transContainer.add( this.m_transparency );
            transContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            this._add( transContainer );
        },
        
        /**
         * Send a command to the server to change the transparency.
         */
        _sendTransparency : function( msg ){
            var transp = msg.getData().value;
            var params = "id:"+ this.m_layerId+",alpha:"+transp;
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setMaskAlpha";
            this.m_connector.sendCommand( cmd, params, function(){});
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
        },
        
        /**
         * Set an identifier for the layer.
         * @param layerId {String} - an identifier for the layer.
         */
        setLayerId : function( layerId ){
            this.m_layerId = layerId;
        },
        m_connector : null,
        m_id : null,
        m_layerId : null,
        m_transparency : null
    }
});