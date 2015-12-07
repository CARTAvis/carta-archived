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
        
        /**
         * Callback for a change in the available composition modes on the server.
         */
        _compModesChangedCB : function(){
            if ( this.m_sharedVarCompModes ){
                var val = this.m_sharedVarCompModes.get();
                if ( val ){
                    try {
                        var oldName = this.m_compModeCombo.getValue();
                        var modeNames = JSON.parse( val );
                        this.m_compModeCombo.setSelectItems( modeNames.modes );
                       
                        //Try to reset the old selection
                        if ( oldName !== null ){
                            this.m_compModeCombo.setSelectValue( oldName );
                        }
                    }
                    catch( err ){
                        console.log( "Could not layer composition modes: "+val );
                        console.log( "Err="+err );
                    }
                }
            }
        },
        
        /*
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout( new qx.ui.layout.VBox(1) );
            this.m_content = new qx.ui.groupbox.GroupBox( "");
            this.m_content.setLayout( new qx.ui.layout.VBox(1) );
            this._add( this.m_content );
            this._initCompositeModes();
            this._initMask();
        },
        
        /**
         * Initialize UI as to whether or not to apply a mask.
         */
        _initCompositeModes : function(){
            var hContainer = new qx.ui.container.Composite();
            hContainer.setLayout( new qx.ui.layout.HBox(1) );
            var label = new qx.ui.basic.Label( "Composer:" );
            this.m_compModeCombo = new skel.widgets.CustomUI.SelectBox( 
                    "setCompositionMode", "mode");
            skel.widgets.TestID.addTestId( this.m_compModeCombo, "layerCompositionMode");
            this.m_compModeCombo.setToolTipText( "Select a layer composition mode.");
            
            //this.m_applyId = this.m_applyCheck.addListener( "changeValue", this._sendApplyCmd, this );
            hContainer.add( new qx.ui.core.Spacer(1), {flex:1} );
            hContainer.add( label );
            hContainer.add( this.m_compModeCombo );
            hContainer.add( new qx.ui.core.Spacer(1), {flex:1} );
            this.m_content.add( hContainer );
        },
        
        
        /**
         * Initialize the mask color controls.
         */
        _initMask : function(){
            this.m_maskColor = new skel.widgets.Image.Stack.MaskControlsColor();
            this.m_content.add( this.m_maskColor );
        },
        
        
        /**
         * Update the UI with server information.
         * @param mask {Object} - information from server about the mask.
         */
        setControls : function(mask){
            this.m_compModeCombo.setSelectValue( mask.mode );
            this.m_maskColor.setControls( mask );
        },
        
        /**
         * Set the server-side id of the object that handles masking of
         * images.
         * @param id {String} - server-side id for mask controls.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_maskColor.setId( id );
            this.m_compModeCombo.setId( id );
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVarCompModes = this.m_connector.getSharedVar(path.LAYER_COMPOSITION_MODES);
            this.m_sharedVarCompModes.addCB( this._compModesChangedCB.bind( this));
            this._compModesChangedCB();
        },
        
        m_compModeCombo : null,
        m_connector : null,
        m_content : null,
        m_id : null,
        m_sharedVarCompModes : null,
        m_maskColor : null
    }
});