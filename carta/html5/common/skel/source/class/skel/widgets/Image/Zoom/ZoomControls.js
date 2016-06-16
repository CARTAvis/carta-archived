/**
 * Color, line width, and visibility settings for a draw element of the zoom view.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Zoom.ZoomControls", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 5, 5, 5, 5 );
            this._setLayout(new qx.ui.layout.VBox(2));
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content, {flex:1} );
            this.m_content.setLayout(new qx.ui.layout.HBox(5));
            this._initProps();
            this._initColor();
        },
        
        
        /**
         * Initialize the color selector UI element.
         */
        _initColor : function(){
            this.m_colorSelector = new skel.widgets.CustomUI.ColorSelector();
            this.m_colorId = this.m_colorSelector.addListener( 'appear', function(){
                this.m_colorAppeared = true;
                if ( this.m_red !== null && this.m_green !== null && this.m_blue !== null ){
                    this._setColor( this.m_red, this.m_green, this.m_blue);
                }
            }, this );
            this.m_content.add( this.m_colorSelector );
        },
        
        
        /**
         * Initializes the sliders.
         */
        _initProps : function(){
            var vContainer = new qx.ui.container.Composite();
            vContainer.setLayout( new qx.ui.layout.VBox(5));
            this.m_thickness = new skel.widgets.CustomUI.TextSlider("setLineWidth", "width",
                    1, 10, 1, "Box Line Width", false, "Set pixel box line width.", "Slide to set pixel box line width.",
                    "zoomlineWidthTextField", "zoomlineWidthSlider", false);
            this.m_show = new qx.ui.form.CheckBox( "Box Visible");
            this.m_show.setValue( true );
            this.m_show.setToolTipText( "Show/hide pixel box");
            this.m_showListenId = this.m_show.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendShowCmd, this );
            var label = new qx.ui.basic.Label( "Zoom Factor:")
            this.m_zoomText = new skel.widgets.CustomUI.NumericTextField( 1, null );
            this.m_zoomText.setIntegerOnly( true );
            this.m_zoomText.setToolTipText( "Set the factor by which pixels should be enlarged." );
            this.m_zoomText.addListener( "textChanged", function(){
                this._sendZoomCmd();
            }, this );
            vContainer.add( this.m_show );
            vContainer.add( this.m_thickness );
            vContainer.add( label );
            vContainer.add( this.m_zoomText );
            vContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            this.m_content.add( vContainer );
        },

        /**
         * Send a command to the server to change the visibility of the
         * draw element.
         */
        _sendShowCmd : function(){
            if ( this.m_connector !== null && this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setVisible";
                var visible = this.m_show.getValue();
                var param = "visible:"+visible;
                this.m_connector.sendCommand( cmd, param, function(){} );
            }
        },
        
        /**
         * Notify the server that the color of the draw element has changed.
         */
        _sendColorCmd : function(){
            if ( this.m_connector !== null && this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setColor";
                var red = this.m_colorSelector.getRed();
                var green = this.m_colorSelector.getGreen();
                var blue = this.m_colorSelector.getBlue();
                var param = "red:"+red+",green:"+green+",blue:"+blue;
                this.m_connector.sendCommand( cmd, param, function(){} );
            }
        },
        
        /**
         * Send the zoom factor to the server.
         */
        _sendZoomCmd : function(){
            if ( this.m_connector !== null && this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setZoomFactor";
                var factor = this.m_zoomText.getValue();
                var param = "zoom:" + factor;
                this.m_connector.sendCommand( cmd, param, function(){} );
            }
        },
        
        /**
         * Update the color picker with the new  color.
         * @param red {Number} - the amount of red [0,255].
         * @param green {Number} - the amount of green [0,255].
         * @param blue {Number} - the amount of blue [0,255].
         */
        _setColor : function( red, green, blue ){
            if ( this.m_colorAppeared ){
                if ( this.m_colorId != null ){
                    this.m_colorSelector.removeListenerById( this.m_colorId );
                }
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
         * Update the UI based on server-side grid settings.
         * @param controls {Object} - server side grid settings.
         */
        setControls : function( controls ){
            this._setZoomFactor( controls.zoom );
            this._setShow( controls.boxVisible );
            this._setThickness( controls.box.width );
            var red = controls.box.red;
            var green = controls.box.green;
            var blue = controls.box.blue;
            this._setColor( red, green, blue );
            this.m_thickness.setMax( controls.penWidthMax );
        },
        
        /**
         * Set the server side id of this control UI.
         * @param id {String} - the server side id of the object that contains data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_thickness.setId( id );
        },

        
        /**
         * Set whether or not the draw element is visible based on server-side
         * values.
         * @param visible {boolean} - true if the draw element is visible; false,
         *      otherwise.
         */
        _setShow : function( visible ){
            var oldVisible = this.m_show.getValue();
            if ( oldVisible != visible ){
                this.m_show.removeListenerById( this.m_showListenId );
                this.m_show.setValue( visible );
                this.m_showListenId = this.m_show.addListener( skel.widgets.Path.CHANGE_VALUE, 
                        this._sendShowCmd, this );
            }
        },

        
        /**
         * Set the thickness of the draw element.
         * @param thickness {Number} - the thickness of the draw element.
         */
        _setThickness : function( thickness ){
            if ( typeof thickness !=="undefined" && 
                    this.m_thickness.getValue() != thickness ){
                this.m_thickness.setValue( thickness );
            }
        },
        
        /**
         * Set the zoom factor from server-side values.
         * @param zoom {Number} - the zoom factor.
         */
        _setZoomFactor : function( zoom ){
            this.m_zoomText.setValue( zoom );
        },
        
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_colorId : null,
        m_colorSelector : null,
        m_show : null,
        m_showListenId : null,
        m_thickness : null,
       
        m_red : null,
        m_green : null,
        m_blue : null,
        m_colorAppeared : false,
      
        m_zoomText : null
    }
});