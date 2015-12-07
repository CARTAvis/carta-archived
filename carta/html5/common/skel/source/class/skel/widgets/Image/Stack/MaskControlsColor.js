/**
 * Controls for setting the mask color and transparency.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Stack.MaskControlsColor", {
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
        
        /*
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout( new qx.ui.layout.VBox(2) );
            this._initOpacity();
            this._initPresets();
            this._add( new qx.ui.core.Spacer(1), {flex:1} );
            this._initRgbs();
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
            this.m_transparency.addListener( "textSliderChanged", this._setPreviewColor, this );
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
         * Initialize controls for setting RGB.
         */
        _initRgbs : function(){
            var hContainer = new qx.ui.container.Composite();
            hContainer.setLayout( new qx.ui.layout.HBox(1));
            hContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            var rgbContainer = new qx.ui.container.Composite();
            rgbContainer.setLayout( new qx.ui.layout.Grid(2,2));
            this.m_spinRed = new qx.ui.form.Spinner( 0, 0, 255 );
            this.m_spinRed.setToolTipText( "Set the red mask color amount in the selected layer(s).");
            this.m_spinRedId = this.m_spinRed.addListener("changeValue", this._primaryColorChanged, this );
            skel.widgets.TestID.addTestId( this.m_spinRed, "filterRGBSpinRed");
            this.m_spinBlue = new qx.ui.form.Spinner( 0, 0, 255 );
            this.m_spinBlue.setToolTipText( "Set the blue mask color amount in the selected layer(s).");
            this.m_spinBlueId = this.m_spinBlue.addListener( "changeValue", this._primaryColorChanged, this );
            skel.widgets.TestID.addTestId( this.m_spinBlue, "filterRGBSpinBlue");
            this.m_spinGreen = new qx.ui.form.Spinner( 0, 0, 255 );
            this.m_spinGreen.setToolTipText( "Set the green mask color amount in the selected layer(s).");
            this.m_spinGreenId = this.m_spinGreen.addListener( "changeValue", this._primaryColorChanged, this );
            skel.widgets.TestID.addTestId( this.m_spinGreen, "filterRGBSpinGreen");
            this._addColorRgb( rgbContainer, this.m_spinRed, "Red:", 0);
            this._addColorRgb( rgbContainer, this.m_spinGreen, "Green:", 1);
            this._addColorRgb( rgbContainer, this.m_spinBlue, "Blue:", 2);
            hContainer.add( rgbContainer );
            hContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            this._add( hContainer );
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
            this._setRgbColors( 255, 0, 0 );
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
            this._setRgbColors( 0, 255, 0 );
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
            this._setRgbColors( 0, 0, 255 );
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
            this._setRgbColors( 255, 255, 255 );
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
         * Update the selected states of the presets based on the
         * RGB color values.
         */
        _updatePresets : function(){
            var red = this.m_spinRed.getValue();
            var green = this.m_spinGreen.getValue();
            var blue = this.m_spinBlue.getValue();
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
            if ( this.m_id !== null ){
                var red = this.m_spinRed.getValue();
                var green = this.m_spinGreen.getValue();
                var blue = this.m_spinBlue.getValue();
                var params = "red:"+red+",green:"+green+",blue:"+blue;
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setMaskColor";
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Update the UI with mask information from the server.
         * @param mask {Object} - mask information from the server.
         */
        setControls : function( mask ){
            this._setRgbColors( mask.red, mask.green, mask.blue );
            this._updatePresets();
            this.m_transparency.setValue( mask.alpha );
            this._setControlsEnabled( mask.alphaSupport, mask.colorSupport );
        },
        
        /**
         * Enable/disable mask controls based on the type of filter being applied.
         * @param enabledTransparency {boolean} - true if a filter supporting transparency
         *      is being applied; false, otherwise.
         * @param enabledColor {boolean} - true if a filter supporting color is being
         *      applied; false, otherwise.
         */
        _setControlsEnabled : function( enabledTransparency, enabledColor ){
            this.m_presetRed.setEnabled( enabledColor );
            this.m_presetGreen.setEnabled( enabledColor );
            this.m_presetBlue.setEnabled( enabledColor );
            this.m_presetNone.setEnabled( enabledColor );
            this.m_spinRed.setEnabled( enabledColor );
            this.m_spinBlue.setEnabled( enabledColor );
            this.m_spinGreen.setEnabled( enabledColor );
            this.m_transparency.setEnabled( enabledTransparency );
        },
        
        /**
         * Set the id of the server-side object that handles mask information.
         * @param id {String} - server-side identifier of object handling mask information.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_transparency.setId( id );
        },

        
        /**
         * Update the preview panel with the latest color information.
         */
        _setPreviewColor : function(){
            var red = this.m_spinRed.getValue();
            var green = this.m_spinGreen.getValue();
            var blue = this.m_spinBlue.getValue();
            var alpha = this.m_transparency.getValue();
            var alphaNorm = alpha / 255;
            var rgbArray = [red, green, blue];
            var hexStr = qx.util.ColorUtil.rgbToHexString(rgbArray );
            this.m_preview.setBackgroundColor( hexStr );
            this.m_preview.setOpacity( alphaNorm );
        },
        
        /**
         * Update the rgb controls with new color information.
         * @param redAmount {Number} - the amount of red.
         * @param greenAmount {Number} - the amount of green.
         * @param blueAmount {Number} - the amount of blue.
         */
        _setRgbColors : function( redAmount, greenAmount, blueAmount ){
            this.m_spinRed.removeListenerById( this.m_spinRedId );
            this.m_spinGreen.removeListenerById( this.m_spinGreenId );
            this.m_spinBlue.removeListenerById( this.m_spinBlueId );
            if ( redAmount != this.m_spinRed.getValue() ){
                this.m_spinRed.setValue( redAmount );
            }
            if ( greenAmount != this.m_spinGreen.getValue() ){
                this.m_spinGreen.setValue( greenAmount );
            }
            if ( blueAmount != this.m_spinBlue.getValue() ){
                this.m_spinBlue.setValue( blueAmount );
            }
            this.m_spinRedId = this.m_spinRed.addListener( "changeValue", this._primaryColorChanged, this );
            this.m_spinGreenId = this.m_spinGreen.addListener( "changeValue", this._primaryColorChanged, this );
            this.m_spinBlueId = this.m_spinBlue.addListener( "changeValue", this._primaryColorChanged, this );
        },
        
        m_connector : null,
        m_id : null,
        m_presetRed : null,
        m_presetGreen : null,
        m_presetBlue : null,
        m_presetNone : null,
        m_preview : null,
        m_spinRed : null,
        m_spinGreen : null,
        m_spinBlue : null,
        m_spinRedId : null,
        m_spinGreenId : null,
        m_spinBlueId : null,
        m_transparency : null,
        
        m_BORDER_NONE : "no-border",
        m_BORDER_LINE : "line-border"

    }
});