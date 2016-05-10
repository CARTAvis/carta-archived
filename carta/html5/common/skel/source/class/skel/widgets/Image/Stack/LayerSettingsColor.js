/**
 * Controls for setting the mask color and transparency.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Stack.LayerSettingsColor", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this.m_serverUpdate = false;
        this._init( );
    },
    
    statics : {
        TYPE : "color"
    },

    members : {
        
        /**
         * Internal method for adding a control that allows setting of a primary
         * color to the UI.
         * @param parent {qx.ui.container.Composite} - the container for the control.
         * @param colorRgb {qx.ui.form.Spinner} - the control to add.
         * @param labelText {String} - descriptive text for the control.
         * @param rowIndex {Number} - layout row index.
         */
        _addColorRgb : function( parent, colorRgb, labelText, rowIndex ){
            var label = new qx.ui.basic.Label( labelText );
            label.setTextAlign( "right");
            parent.add( label, {row:rowIndex, column:0} );
            parent.add( colorRgb, {row:rowIndex, column:1} );
        },
        
        /**
         * Return the selected UI color as an RGB array.
         * @return {Array} - the color as an RGB array.
         */
        _getColorAsRGB : function(){
            var red = 0;
            var green = 0;
            var blue = 0;
            if ( this._isRedSelected() ){
                red = 255;
            }
            else if ( this._isGreenSelected() ){
                green = 255;
            }
            else if ( this._isBlueSelected() ){
                blue = 255;
            }
            else {
                red = 255;
                blue = 255;
                green = 255;
            }
          
            var colorArray = [];
            colorArray[0] = red;
            colorArray[1] = green;
            colorArray[2] = blue;
            return colorArray;
        },
        
        /**
         * Return an identifier for the type of layer settings.
         * @return {String} - an identifier for the type of layer settings.
         */
        getType : function(){
            return skel.widgets.Image.Stack.LayerSettingsColor.TYPE;
        },
        
        
        /*
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout( new qx.ui.layout.VBox(2) );
            this._initOpacity();
            this._initPresets();
            this._add( new qx.ui.core.Spacer(1), {flex:1} );
            this._initPreview();
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
            this.m_transparency.addListener( "textSliderChanged", function(msg){
                this._setPreviewColor;
                this._sendTransparency(msg);
            }, this );
            
            transContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            transContainer.add( this.m_transparency );
            transContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            this._add( transContainer );
        },
        
        
        /**
         * Initialize the preset colors.
         */
        _initPresets : function(){
            var presetContainer = new qx.ui.container.Composite();
            presetContainer.setLayout( new qx.ui.layout.HBox(1));
            this.m_presetRed = this._makePreset( "#ff0000", "filterRedBox" );
            this.m_presetRed.setToolTipText( "Apply a red filter to the selected layer(s).");
            this.m_presetGreen = this._makePreset( "#00ff00", "filterGreenBox");
            this.m_presetGreen.setToolTipText( "Apply a green filter to the selected layer(s).");
            this.m_presetBlue = this._makePreset( "#0000ff", "filterBlueBox");
            this.m_presetBlue.setToolTipText( "Apply a blue filter to the selected layer(s).");
            this.m_presetNone = this._makePreset( "#ffffff", "filterNoneBox");
            this.m_presetNone.setToolTipText( "No color filter should be applied to the selected layer(s).");
            this.m_presetRed.addListener( "mousedown", this._presetRedSelected, this );
            this.m_presetGreen.addListener( "mousedown", this._presetGreenSelected, this );
            this.m_presetBlue.addListener( "mousedown", this._presetBlueSelected, this );
            this.m_presetNone.addListener( "mousedown", this._presetNoneSelected, this );
            presetContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            presetContainer.add( this.m_presetRed );
            presetContainer.add( new qx.ui.core.Spacer(2), {flex:0.5} );
            presetContainer.add( this.m_presetGreen );
            presetContainer.add( new qx.ui.core.Spacer(2), {flex:0.5} );
            presetContainer.add( this.m_presetBlue );
            presetContainer.add( new qx.ui.core.Spacer(2), {flex:0.5} );
            presetContainer.add( this.m_presetNone );
            presetContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            
            this._add( presetContainer );
        },
        
        
        /**
         * Initialize the color preview.
         */
        _initPreview : function(){
            this.m_preview = new qx.ui.core.Widget();
            this.m_preview.setDecorator( this.m_BORDER_LINE );
            this.m_preview.setBackgroundColor( "#000000");
            this.m_preview.setWidth( 100 );
            this.m_preview.setHeight( 20 );
            this._add( this.m_preview );
        },
        
        
        /**
         * Returns true if the user has selected blue.
         * @return {boolean} - true if the user has selected blue; false otherwise.
         */
        _isBlueSelected : function(){
            var blueSelected = false;
            if ( this.m_presetBlue.getDecorator() == this.m_BORDER_LINE ){
                blueSelected = true;
            }
            return blueSelected;
        },
        
        
        /**
         * Returns true if the user has selected green.
         * @return {boolean} - true if the user has selected green; false otherwise.
         */
        _isGreenSelected : function(){
            var greenSelected = false;
            if ( this.m_presetGreen.getDecorator() == this.m_BORDER_LINE ){
                greenSelected = true;
            }
            return greenSelected;
        },
        
        
        /**
         * Returns true if the user has selected red.
         * @return {boolean} - true if the user has selected red; false otherwise.
         */
        _isRedSelected : function(){
            var redSelected = false;
            if ( this.m_presetRed.getDecorator() == this.m_BORDER_LINE ){
                redSelected = true;
            }
            return redSelected;
        },
        
        
        /**
         * Return true if the user has selected white.
         * @return {boolean} - true if the color is white; false otherwise.
         */
        _isNoneSelected : function(){
            var noneSelected = false;
            if ( this.m_presetNone.getDecorator() == this.m_BORDER_LINE ){
                noneSelected = true;
            }
            return noneSelected;
        },
        
        /**
         * Construct a square for a preset color.
         * @param colorParam {String} - the preset color.
         * @param testId {String} - unique locator for testing
         */
        _makePreset : function( colorParam, testId ){
            var preset = new qx.ui.core.Widget();
            preset.setBackgroundColor( colorParam );
            preset.setWidth( 15 );
            preset.setHeight( 15 );
            skel.widgets.TestID.addTestId( preset, testId );
            return preset;
        },
        
        /**
         * Update the UI based on a preset color of red.
         */
        _presetRedSelected : function(){
            this.m_presetRed.setDecorator( this.m_BORDER_LINE );
            this.m_presetGreen.setDecorator( this.m_BORDER_NONE );
            this.m_presetBlue.setDecorator( this.m_BORDER_NONE );
            this.m_presetNone.setDecorator( this.m_BORDER_NONE );
            this._setPreviewColor();
            this._sendMaskColorCmd();
        },
        
        /**
         * Update the UI based on a preset color of green.
         */
        _presetGreenSelected : function(){
            this.m_presetRed.setDecorator( this.m_BORDER_NONE );
            this.m_presetGreen.setDecorator( this.m_BORDER_LINE );
            this.m_presetBlue.setDecorator( this.m_BORDER_NONE );
            this.m_presetNone.setDecorator( this.m_BORDER_NONE );
            this._setPreviewColor();
            this._sendMaskColorCmd();
        },
        
        /**
         * Update the UI based on a preset color of blue.
         */
        _presetBlueSelected : function(){
            this.m_presetRed.setDecorator( this.m_BORDER_NONE );
            this.m_presetGreen.setDecorator( this.m_BORDER_NONE );
            this.m_presetBlue.setDecorator( this.m_BORDER_LINE );
            this.m_presetNone.setDecorator( this.m_BORDER_NONE );
            this._setPreviewColor();
            this._sendMaskColorCmd();
        },
        
       
        /**
         * Update the UI based on no color filter.
         */
        _presetNoneSelected : function(){
            this.m_presetRed.setDecorator( this.m_BORDER_NONE );
            this.m_presetGreen.setDecorator( this.m_BORDER_NONE );
            this.m_presetBlue.setDecorator( this.m_BORDER_NONE );
            this.m_presetNone.setDecorator( this.m_BORDER_LINE );
            this._setPreviewColor();
            this._sendMaskColorCmd();
        },
        
        /**
         * Update the UI based on a custom color.
         */
        _presetsNotSelected : function(){
            this.m_presetRed.setDecorator( this.m_BORDER_NONE );
            this.m_presetGreen.setDecorator( this.m_BORDER_NONE );
            this.m_presetBlue.setDecorator( this.m_BORDER_NONE );
            this.m_presetNone.setDecorator( this.m_BORDER_NONE );
        },
        

        /**
         * Notification that the RGB value of the color mask
         * has changed.
         */
        _primaryColorChanged : function(){
            this._updatePresets();
            this._sendMaskColorCmd();
        },
        
        /**
         * Notify the server that the mask color has changed.
         */
        _sendMaskColorCmd : function(){
            if ( this.m_id !== null && !this.m_serverUpdate ){
                var colorArray = this._getColorAsRGB();
                var params = "id:"+ this.m_layerId+",red:"+colorArray[0]+",green:"+colorArray[1]+",blue:"+colorArray[2];
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setMaskColor";
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Send a command to the server to change the transparency.
         * @param msg {Object} - information about the transparency to send.
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
            this.m_serverUpdate = true;
            if ( mask.red == 255 && mask.blue == 0 && mask.green == 0){
                this._presetRedSelected();
            }
            else if ( mask.red == 0 && mask.blue == 255 && mask.green == 0 ){
                this._presetBlueSelected();
            }
            else if ( mask.red == 0 && mask.blue == 0 && mask.green == 255 ){
                this._presetGreenSelected();
            }
            else {
                this._presetNoneSelected();
            }
            this.m_serverUpdate = false;
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
         * @param id {String} - an identifier for the layer.
         */
        setLayerId : function( id ){
            this.m_layerId = id;
        },

        
        /**
         * Update the preview panel with the latest color information.
         */
        _setPreviewColor : function(){
            var rgbArray = this._getColorAsRGB();
            var alpha = this.m_transparency.getValue();
            var alphaNorm = alpha / 255;
            var hexStr = qx.util.ColorUtil.rgbToHexString(rgbArray );
            this.m_preview.setBackgroundColor( hexStr );
            this.m_preview.setOpacity( alphaNorm );
        },
        
        /**
         * Update the selected states of the presets based on the
         * RGB color values.
         */
        _updatePresets : function(){
            var colorArray = this._getColorAsRGB();
            var red = colorArray[0];
            var green = colorArray[1];
            var blue = colorArray[2];
            if ( red == 255 && green == 0 && blue == 0 ){
                this._presetRedSelected();
            }
            else if ( red == 0 && green == 255 && blue == 0 ){
                this._presetGreenSelected();
            }
            else if ( red == 0 && green == 0 && blue == 255 ){
                this._presetBlueSelected();
            }
            else if ( red == 255 && green == 255 && blue == 255 ){
                this._presetNoneSelected();
            }
            else {
                this._presetsNotSelected();
            }
            this._setPreviewColor();
        },
        
        
        
        m_connector : null,
        m_id : null,
        m_layerId : null,
        m_presetRed : null,
        m_presetGreen : null,
        m_presetBlue : null,
        m_presetNone : null,
        m_preview : null,
        m_serverUpdate : null,
        
        m_transparency : null,
        
        m_BORDER_NONE : "no-border",
        m_BORDER_LINE : "line-border"

    }
});