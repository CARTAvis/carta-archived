/**
 * Setting controls for a group of layers.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/


qx.Class.define("skel.widgets.Image.Stack.LayerSettingsGroup", {
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
        TYPE : "group"
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
        
        /**
         * Return an identifier for the type of layer settings.
         * @return {String} - an identifier for the type of layer settings.
         */
        getType : function(){
            return skel.widgets.Image.Stack.LayerSettingsGroup.TYPE;
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
            hContainer.add( new qx.ui.core.Spacer(1), {flex:1} );
            hContainer.add( label );
            hContainer.add( this.m_compModeCombo );
            hContainer.add( new qx.ui.core.Spacer(1), {flex:1} );
            this.m_content.add( hContainer );
        },
        
        

        /**
         * Update the UI with server information.
         * @param mode {String} - information from server about the layer composition mode.
         */
        setControls : function( mode){
            this.m_compModeCombo.setSelectValue( mode );
        },
        
        /**
         * Set the server-side id of the object that handles masking of
         * images.
         * @param id {String} - server-side id for mask controls.
         */
        setId : function( id ){
            this.m_id = id;
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
        m_sharedVarCompModes : null
    }
});