/**
 * Displays controls for nan colors.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.PageNan", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "NaN", "");
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
        }
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
            this.m_nanDefaultCheck = new qx.ui.form.CheckBox( "Default");
            this.m_nanDefaultCheck.setToolTipText( "The bottom color map value will be used for the nan color when 'Default' is selected.")
            this.m_nanId = this.m_nanDefaultCheck.addListener( "changeValue", this._nanDefaultChanged, this );
            this.m_colorSelector = new skel.widgets.CustomUI.ColorSelector();
            this.m_colorId = this.m_colorSelector.addListener( 'appear', function(){
                this.m_colorAppeared = true;
                this._setColor( this.m_red, this.m_green, this.m_blue);
            }, this );
            this._add( this.m_nanDefaultCheck );
            this._add( this.m_colorSelector );
        },
        
        /**
         * Update the enabled status & notify the server when the user
         * has changed whether or not to use the default nan.
         */
        _nanDefaultChanged : function(){
            this._updateColorPickerEnabledStatus();
            this._sendDefaultNanCmd();
        },
        
        /**
         * Updates the enabled status of the color picker based on whether
         * a default nan is being used.
         */
        _updateColorPickerEnabledStatus : function(){
            var defaultNan = this.m_nanDefaultCheck.getValue();
            this.m_colorSelector.setEnabled( !defaultNan );
        },
        
        /**
         * Notify the parent that the nan color has changed.
         */
        _sendColorCmd : function(){
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setNanColor";
                var red = this.m_colorSelector.getRed();
                var green = this.m_colorSelector.getGreen();
                var blue = this.m_colorSelector.getBlue();
                var param = "red:"+red+",green:"+green+",blue:"+blue;
                this.m_connector.sendCommand( cmd, param, function(){} );
            }
        },
        
        /**
         * Notify the server that whether or not to use the nan default color
         * has changed.
         */
        _sendDefaultNanCmd : function(){
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setDefaultNan";
                var val = this.m_nanDefaultCheck.getValue();
                var param = "nanDefault:"+val;
                this.m_connector.sendCommand( cmd, param, function(){} );
            }
        },
        
                
        /**
         * Update from the server when the nan color settings have changed.
         * @param controls {Object} - information about the nan color 
         *      settings from the server.
         */
        setControls : function( controls ){
            var red = controls.nanColor.red;
            var green = controls.nanColor.green;
            var blue = controls.nanColor.blue;
            this._setColor( red, green, blue );
            this._setNanDefault( controls.nanDefault );
        },
        
        /**
         * Update the color picker with the new nan color.
         * @param red {Number} - the amount of red [0,255].
         * @param green {Number} - the amount of green [0,255].
         * @param blue {Number} - the amount of blue [0,255].
         */
        _setColor : function( red, green, blue ){
            if ( this.m_colorAppeared ){
                this.m_colorSelector.removeListenerById( this.m_colorId );
                this.m_colorSelector.setRed( red );
                this.m_colorSelector.setGreen( green );
                this.m_colorSelector.setBlue( blue );
                this.m_colorId = this.m_colorSelector.addListener( "changeValue", this._sendColorCmd, this );
            }
            else {
                this.m_red = red;
                this.m_green = green;
                this.m_blue = blue;
            }
        },
        
        /**
         * Update the UI with whether or to use the nan default color or
         * not.
         */
        _setNanDefault : function( nanDefault ){
            this.m_nanDefaultCheck.removeListenerById( this.m_nanId );
            this.m_nanDefaultCheck.setValue( nanDefault );
            this._updateColorPickerEnabledStatus();
            this.m_nanId = this.m_nanDefaultCheck.addListener( "changeValue", this._nanDefaultChanged, this );
        },
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
           
        },
        
        m_colorAppeared : false,
        m_colorId : null,
        m_colorSelector : null,
        m_connector : null,
        m_red : null,
        m_green : null,
        m_blue : null,
        m_id : null,
        m_nanDefaultCheck : null,
        m_nanId : null
       
    }
});