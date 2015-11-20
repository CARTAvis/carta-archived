/**
 * Controls for setting an image mask.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/


qx.Class.define("skel.widgets.Image.Stack.MaskControls", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init( );
    },
    

    members : {
        
        /*
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout( new qx.ui.layout.VBox(1) );
            this.m_content = new qx.ui.groupbox.GroupBox( /*"Mask"*/);
            this.m_content.setLayout( new qx.ui.layout.VBox(1) );
            this._add( this.m_content );
            this._initCheck();
            this._initMask();
        },
        
        /**
         * Initialize UI as to whether or not to apply a mask.
         */
        _initCheck : function(){
            var checkContainer = new qx.ui.container.Composite();
            checkContainer.setLayout( new qx.ui.layout.HBox(1) );
            this.m_applyCheck = new qx.ui.form.CheckBox( "Apply Mask" );
            this.m_applyCheck.setToolTipText( "Apply a color/transparency mask to the selected layer(s)." );
            this.m_applyId = this.m_applyCheck.addListener( "changeValue", this._sendApplyCmd, this );
            checkContainer.add( new qx.ui.core.Spacer(1), {flex:1} );
            checkContainer.add( this.m_applyCheck );
            checkContainer.add( new qx.ui.core.Spacer(1), {flex:1} );
            this.m_content.add( checkContainer );
        },
        
        
        /**
         * Initialize the mask color controls.
         */
        _initMask : function(){
            this.m_maskColor = new skel.widgets.Image.Stack.MaskControlsColor();
            this.m_content.add( this.m_maskColor );
        },
        
        /**
         * Notify the server as to whether a mask should be applied to the selected
         * layers.
         */
        _sendApplyCmd : function(){
            if ( this.m_id !== null ){
                var apply = this.m_applyCheck.getValue();
                var params = "apply:"+apply;
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setUseMask";
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Update the UI with server information.
         * @param mask {Object} - information from server about the mask.
         */
        setControls : function(mask){
            this.m_applyCheck.removeListenerById( this.m_applyId );
            this.m_applyCheck.setValue( mask.apply );
            this.m_applyId = this.m_applyCheck.addListener( "changeValue", this._sendApplyCmd, this );
            
            this.m_maskColor.setControls( mask );
            this.m_maskColor.setControlsEnabled( mask.apply );
        },
        
        /**
         * Set the server-side id of the object that handles masking of
         * images.
         * @param id {String} - server-side id for mask controls.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_maskColor.setId( id );
        },
        
        m_applyCheck : null,
        m_applyId : null,
        m_connector : null,
        m_content : null,
        m_id : null,
        
        m_maskColor : null

    }
});