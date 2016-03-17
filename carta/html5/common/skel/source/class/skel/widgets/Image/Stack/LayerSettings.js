/**
 * Controls for a layer in a stack.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/


qx.Class.define("skel.widgets.Image.Stack.LayerSettings", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this._init( );
    },
    

    members : {
        
        /*
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout( new qx.ui.layout.VBox(1) );
            this.m_content = new qx.ui.groupbox.GroupBox( "");
            this.m_content.setLayout( new qx.ui.layout.VBox(1) );
            this._add( this.m_content );
            
            //Everything will have a user settable name.
            this.m_nameText = new qx.ui.form.TextField();
            var nameLabel = new qx.ui.basic.Label( "Name:");
            var nameContainer = new qx.ui.container.Composite();
            nameContainer.setLayout( new qx.ui.layout.HBox(2) );
            nameContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            nameContainer.add( nameLabel );
            nameContainer.add( this.m_nameText, {flex:1} );
            nameContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            this.m_content.add( nameContainer );
            
            this.m_layerSettings = new skel.widgets.Image.Stack.LayerSettingsColor();
            this.m_content.add( this.m_layerSettings );
        },
        
        /**
         * Returns whether or not the settings support transparency.
         * @return {boolean} - true if the settings support transparency; false, otherwise.
         */
        _isModeAlpha : function(){
            var alphaMode = true;
            var layerType = this.m_layerSettings.getType();
            if ( layerType != skel.widgets.Image.Stack.LayerSettingsAlpha.TYPE ){
                alphaMode = false;
            }
            return alphaMode;
        },
        
        /**
         * Returns whether or not the settings support RGB.
         * @return {boolean} - true if the settings support RGB; false, otherwise.
         */
        _isModeColor : function(){
            var colorMode = true;
            if ( this.m_layerSettings.getType() != skel.widgets.Image.Stack.LayerSettingsColor.TYPE ){
                colorMode = false;
            }
            return colorMode;
        },
        
        /**
         * Returns whether or not the settings are for a group of layers.
         * @return {boolean} - true if the settings are for a group of layers; false, otherwise.
         */
        _isModeGroup : function(){
            var groupMode = true;
            var layerType = this.m_layerSettings.getType();
            if ( layerType != skel.widgets.Image.Stack.LayerSettingsGroup.TYPE ){
                groupMode = false;
            }
            return groupMode;
        },
        
        /**
         * Remove mode specific settings.
         */
        _removeLayerSettings : function(){
            var settingsIndex = this.m_content.indexOf( this.m_layerSettings );
            if ( settingsIndex >= 0 ){
                this.m_content.remove( this.m_layerSettings );
            }
        },
        
        /**
         * Complete setting the mode in a generic way (regardless of the layer type).
         */
        setGeneric : function(){
            this.m_layerSettings.setId( this.m_id );
            this.m_layerSettings.setLayerId ( this.m_layerId );
            this.m_content.add( this.m_layerSettings );
        },
       
        
        /**
         * Display settings appropriate to a group of layers.
         */
        setModeGroup : function(){
            var groupMode = this._isModeGroup();
            console.log( "setModeGroup groupMode="+groupMode);
            if ( !groupMode ){
                this._removeLayerSettings();
                this.m_layerSettings = new skel.widgets.Image.Stack.LayerSettingsGroup();
                this.setGeneric();
            }
        },
        
        /**
         * Display settings apppropriate for setting RGB.
         */
        setModeColor : function(){
            if ( !this._isModeColor() ){
                this._removeLayerSettings();
                this.m_layerSettings = new skel.widgets.Image.Stack.LayerSettingsColor();
                this.setGeneric();
            }
        },
        
        /**
         * Display settings appropriate for setting transparency.
         */
        setModeAlpha : function(){
            var alphaMode = this._isModeAlpha();
            if ( !alphaMode ){
                this._removeLayerSettings();
                this.m_layerSettings = new skel.widgets.Image.Stack.LayerSettingsAlpha();
                this.setGeneric();
            }
        },
      
        
        /**
         * Set the server-side id of the object that handles layer settings.
         * @param id {String} - server-side id for layer settings.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        /**
         * Set an identifier for the layer.
         */
        setLayerId : function( layerId ){
            this.m_layerId = layerId;
            this.m_layerSettings.setLayerId( layerId );
        },
        
        /**
         * Update the UI with a user set name for the layer.
         * @param name {String} - a user set name for the layer.
         */
        setName : function( name ){
            this.m_nameText.setValue( name );
        },
        
        /**
         * Update the UI with server specific setting information.
         * @param settings {Object} - server specific setting information.
         */
        setSettings : function( settings){
            this.m_layerSettings.setControls( settings );
        },
        
       
        m_content : null,
        m_id : null,
        m_layerId : null,
        m_mask : null,
        m_mode : null,
        m_layerSettings : null,
        m_nameText : null
    }
});