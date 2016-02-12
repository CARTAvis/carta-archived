/**
 * Displays controls for the color/transparency on the border of a plot.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.PageBorderBackground", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Border Background", "");
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
            
            var controlContainer = new qx.ui.container.Composite();
            controlContainer.setLayout( new qx.ui.layout.VBox(2) );
            
            //Default check
            var checkContainer = new qx.ui.container.Composite();
            checkContainer.setLayout( new qx.ui.layout.HBox(2));
            this.m_borderDefaultCheck = new qx.ui.form.CheckBox( "Default");
            this.m_borderDefaultCheck.setToolTipText( "The plot border background color & transparency will be at default values when 'Default' is selected.")
            this.m_borderId = this.m_borderDefaultCheck.addListener( "changeValue", this._borderDefaultChanged, this );
            checkContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            checkContainer.add( this.m_borderDefaultCheck );
            checkContainer.add( new qx.ui.core.Spacer(2), {flex:1});
            controlContainer.add( checkContainer );
            
            //Transparency
            this.m_transparency = new skel.widgets.CustomUI.TextSlider("setBorderAlpha", "alpha",
                    0, skel.widgets.Path.MAX_RGB, 25, "Opacity", false, "Set the border opacity.", "Slide to set the plot border opacity.",
                    "borderTransparencyTextField", "borderTransparencySlider", false);
            controlContainer.add( this.m_transparency );
            
            //Color selector
            this.m_colorSelector = new skel.widgets.CustomUI.ColorSelector();
            this.m_colorId = this.m_colorSelector.addListener( 'appear', function(){
                this.m_colorAppeared = true;
                this._setColor( this.m_red, this.m_green, this.m_blue);
            }, this );
            this._add( controlContainer );
            this._add( this.m_colorSelector );
        },
        
        /**
         * Update the enabled status & notify the server when the user
         * has changed whether or not to use the default border settings.
         */
        _borderDefaultChanged : function(){
            this._updateEnabledStatus();
            this._sendDefaultBorderCmd();
        },
        
        
        /**
         * Notify the parent that the nan color has changed.
         */
        _sendColorCmd : function(){
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setBorderColor";
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
        _sendDefaultBorderCmd : function(){
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setDefaultBorder";
                var val = this.m_borderDefaultCheck.getValue();
                var param = "borderDefault:"+val;
                this.m_connector.sendCommand( cmd, param, function(){} );
            }
        },
        
        
        /**
         * Update the UI with whether or to use the nan default color or
         * not.
         */
        _setBorderDefault : function( borderDefault ){
            this.m_borderDefaultCheck.removeListenerById( this.m_borderId );
            this.m_borderDefaultCheck.setValue( borderDefault );
            this._updateEnabledStatus();
            this.m_borderId = this.m_borderDefaultCheck.addListener( "changeValue", this._borderDefaultChanged, this );
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
         * Update from the server when the nan color settings have changed.
         * @param controls {Object} - information about the nan color 
         *      settings from the server.
         */
        setControls : function( controls ){
            var red = controls.borderColor.red;
            var green = controls.borderColor.green;
            var blue = controls.borderColor.blue;
            this._setColor( red, green, blue );
            this._setBorderDefault( controls.borderDefault );
            var alpha = controls.borderColor.alpha;
            this._setTransparency( alpha );
        },
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_transparency.setId( id );
        },
        
        /**
         * Update the UI with a server-side opacity level.
         * @param alpha {Number} - a transparency level in [0,255].
         */
        _setTransparency : function( alpha ){
            this.m_transparency.setValue( alpha );
        },
        
        /**
         * Updates the enabled status of the color picker based on whether
         * a default nan is being used.
         */
        _updateEnabledStatus : function(){
            var defaultNan = this.m_borderDefaultCheck.getValue();
            this.m_colorSelector.setEnabled( !defaultNan );
            this.m_transparency.setEnabled( !defaultNan );
        },
        
        m_colorAppeared : false,
        m_colorId : null,
        m_colorSelector : null,
        m_connector : null,
        m_red : null,
        m_green : null,
        m_blue : null,
        m_id : null,
        m_borderDefaultCheck : null,
        m_borderId : null,
        m_transparency : null
       
    }
});