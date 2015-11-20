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
            parent.add( label, {row:rowIndex, column:0} );
            parent.add( colorRgb, {row:rowIndex, column:1} );
        },
        
        /*
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout( new qx.ui.layout.VBox(1) );
            this._initPresets();
            this._add( new qx.ui.core.Spacer(1), {flex:1} );
            this._initRgbs();
            this._initOpacity();
            this._initPreview();
        },
        
        /**
         * Initialize the opacity control.
         */
        _initOpacity : function(){
            var transContainer = new qx.ui.container.Composite();
            transContainer.setLayout( new qx.ui.layout.HBox(1));
            this.m_transparency = new skel.widgets.CustomUI.TextSlider("setMaskOpacity", "alpha",
                    0, skel.widgets.Path.MAX_RGB, 0, 
                    "Opacity", false, 
                    "Set the mask opacity.", "Slide to set the grid opacity.",
                    "maskOpacityTextField", "maskOpacitySlider", false);
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
            this.m_presetRed = this._makePreset( "#ff0000" );
            this.m_presetRed.setToolTipText( "Apply a red color mask to the selected layer(s).");
            this.m_presetGreen = this._makePreset( "#00ff00");
            this.m_presetGreen.setToolTipText( "Apply a green color mask to the selected layer(s).");
            this.m_presetBlue = this._makePreset( "#0000ff");
            this.m_presetBlue.setToolTipText( "Apply a blue color mask to the selected layer(s).");
            this.m_presetYellow = this._makePreset( "#ffff00");
            this.m_presetYellow.setToolTipText( "Apply a yellow color mask to the selected layer(s).");
            this.m_presetRed.addListener( "mousedown", this._presetRedSelected, this );
            this.m_presetGreen.addListener( "mousedown", this._presetGreenSelected, this );
            this.m_presetBlue.addListener( "mousedown", this._presetBlueSelected, this );
            this.m_presetYellow.addListener( "mousedown", this._presetYellowSelected, this );
            presetContainer.add( this.m_presetRed );
            presetContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            presetContainer.add( this.m_presetGreen );
            presetContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            presetContainer.add( this.m_presetBlue );
            presetContainer.add( new qx.ui.core.Spacer(2), {flex:1} );
            presetContainer.add( this.m_presetYellow );
            
            this._add( presetContainer );
        },
        
        /**
         * Initialize controls for setting RGB.
         */
        _initRgbs : function(){
            var rgbContainer = new qx.ui.container.Composite();
            rgbContainer.setLayout( new qx.ui.layout.Grid());
            this.m_spinRed = new qx.ui.form.Spinner( 0, 0, 255 );
            this.m_spinRed.setToolTipText( "Set the red mask color amount in the selected layer(s).");
            this.m_spinRedId = this.m_spinRed.addListener("changeValue", this._primaryColorChanged, this );
            this.m_spinBlue = new qx.ui.form.Spinner( 0, 0, 255 );
            this.m_spinBlue.setToolTipText( "Set the blue mask color amount in the selected layer(s).");
            this.m_spinBlueId = this.m_spinBlue.addListener( "changeValue", this._primaryColorChanged, this );
            this.m_spinGreen = new qx.ui.form.Spinner( 0, 0, 255 );
            this.m_spinGreen.setToolTipText( "Set the green mask color amount in the selected layer(s).");
            this.m_spinGreenId = this.m_spinGreen.addListener( "changeValue", this._primaryColorChanged, this );
            this._addColorRgb( rgbContainer, this.m_spinRed, "Red:", 0);
            this._addColorRgb( rgbContainer, this.m_spinGreen, "Green:", 1);
            this._addColorRgb( rgbContainer, this.m_spinBlue, "Blue:", 2);
            this._add( rgbContainer );
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
         * Construct a square for a preset color.
         * @param colorParam {String} - the preset color.
         */
        _makePreset : function( colorParam ){
            var preset = new qx.ui.core.Widget();
            preset.setBackgroundColor( colorParam );
            preset.setWidth( 15 );
            preset.setHeight( 15 );
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
            this.m_presetYellow.setDecorator( this.m_BORDER_NONE );
        },
        
        /**
         * Update the UI based on a preset color of green.
         */
        _presetGreenSelected : function(){
            this._setRgbColors( 0, 255, 0 );
            this.m_presetRed.setDecorator( this.m_BORDER_NONE );
            this.m_presetGreen.setDecorator( this.m_BORDER_LINE );
            this.m_presetBlue.setDecorator( this.m_BORDER_NONE );
            this.m_presetYellow.setDecorator( this.m_BORDER_NONE );
        },
        
        /**
         * Update the UI based on a preset color of blue.
         */
        _presetBlueSelected : function(){
            this._setRgbColors( 0, 0, 255 );
            this.m_presetRed.setDecorator( this.m_BORDER_NONE );
            this.m_presetGreen.setDecorator( this.m_BORDER_NONE );
            this.m_presetBlue.setDecorator( this.m_BORDER_LINE );
            this.m_presetYellow.setDecorator( this.m_BORDER_NONE );
        },
        
        /**
         * Update the UI based on a preset color of yellow.
         */
        _presetYellowSelected : function(){
            this._setRgbColors( 255, 255, 0 );
            this.m_presetRed.setDecorator( this.m_BORDER_NONE );
            this.m_presetGreen.setDecorator( this.m_BORDER_NONE );
            this.m_presetBlue.setDecorator( this.m_BORDER_NONE );
            this.m_presetYellow.setDecorator( this.m_BORDER_LINE );
        },
        
        /**
         * Update the UI based on a custom color.
         */
        _presetsNotSelected : function(){
            this.m_presetRed.setDecorator( this.m_BORDER_NONE );
            this.m_presetGreen.setDecorator( this.m_BORDER_NONE );
            this.m_presetBlue.setDecorator( this.m_BORDER_NONE );
            this.m_presetYellow.setDecorator( this.m_BORDER_NONE );
        },
        
        /**
         * Notification that the RGB value of the color mask
         * has changed.
         */
        _primaryColorChanged : function(){
            this._presetsNotSelected();
            this._setPreviewColor();
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
            this.m_transparency.setValue( mask.alpha );
        },
        
        /**
         * Enable/disable mask controls based on whether a mask is being
         * applied.
         * @param enabled {boolean} - true if a mask is being applied; false otherwise.
         */
        setControlsEnabled : function( enabled ){
            this.m_presetRed.setEnabled( enabled );
            this.m_presetGreen.setEnabled( enabled );
            this.m_presetBlue.setEnabled( enabled );
            this.m_presetYellow.setEnabled( enabled );
            this.m_spinRed.setEnabled( enabled );
            this.m_spinBlue.setEnabled( enabled );
            this.m_spinGreen.setEnabled( enabled );
            this.m_transparency.setEnabled( enabled );
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
            var rgbChanged = false;
            if ( redAmount != this.m_spinRed.getValue() ){
                this.m_spinRed.setValue( redAmount );
                rgbChanged = true;
            }
            if ( greenAmount != this.m_spinGreen.getValue() ){
                this.m_spinGreen.setValue( greenAmount );
                rgbChanged = true;
            }
            if ( blueAmount != this.m_spinBlue.getValue() ){
                this.m_spinBlue.setValue( blueAmount );
                rgbChanged = true;
            }
            if ( rgbChanged ){
                this._primaryColorChanged();
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
        m_presetYellow : null,
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