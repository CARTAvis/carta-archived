/**
 * Controls for customize the red, blue, and green settings in an individual color map.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColorMix", {
    extend : qx.ui.core.Widget,
    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init( );
    },
    
    statics : {
        SLIDER_MIN : 0,
        SLIDER_MAX : 100,
        CMD_SET_COLORMIX : "setColorMix"
    },

    members : {
        
        
        /**
         * Callback for a server error when setting the color mix.
         * @param anObject {skel.widgets.ColorMap.ColorMix}.
         */
        _errorColorsCB : function( anObject ){
            return function( percents ){
                if ( percents ){
                    var percentArray = percents.split( ",");
                    var red = parseInt(percentArray[0]);
                    var green = parseInt( percentArray[1]);
                    var blue = parseInt( percentArray[2]);
                    anObject.setMix( red, green, blue );
                }
            };
        },
        
        /**
         * Return the percentage of a color in the map.
         * @param value {Number} the amount of color.
         * @return {Number} the color percentage.
         */
        _getColorPercent : function( value ){
            var sliderMin = skel.widgets.Colormap.ColorMix.SLIDER_MIN;
            var range = this._getRange();
            var percent = (value - sliderMin) / range;
            return percent;
        },
        
        /**
         * Return the GUI color map range.
         * @return {Number} the GUI color range.
         */
        _getRange : function(){
            return skel.widgets.Colormap.ColorMix.SLIDER_MAX - skel.widgets.Colormap.ColorMix.SLIDER_MIN;
        },

        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            var widgetLayout = new qx.ui.layout.VBox(0);
            this._setLayout(widgetLayout);
            this.setAllowGrowX( true );
            
            var sliderContainer = new qx.ui.container.Composite();
            sliderContainer.setHeight( 50 );
            sliderContainer.setAllowGrowX( true );
            sliderContainer.setLayout( new qx.ui.layout.VBox(2));
            this.m_colorCanvas = new skel.widgets.Colormap.ColorMixCanvas();
            sliderContainer.add( this.m_colorCanvas );
            this.m_redSlider = new qx.ui.form.Slider();
            this.m_redSlider.setDecorator( "slider-red");
            this.m_redSliderListenerId = this._initSlider( this.m_redSlider, sliderContainer );
            this.m_greenSlider = new qx.ui.form.Slider();
            this.m_greenSlider.setDecorator( "slider-green");
            this.m_greenSliderListenerId = this._initSlider( this.m_greenSlider, sliderContainer );
            this.m_blueSlider = new qx.ui.form.Slider();
            this.m_blueSliderListenerId = this._initSlider( this.m_blueSlider, sliderContainer );
            this.m_blueSlider.setDecorator( "slider-blue" );
            
            this._add( sliderContainer );
            
            var syncContainer = new qx.ui.container.Composite();
            syncContainer.setAllowGrowX( true );
            syncContainer.setLayout( new qx.ui.layout.HBox(1));
            this.m_synchronizeCheck = new qx.ui.form.CheckBox( "Synchronize Colors");
            this.m_synchronizeCheck.setValue( true );
            syncContainer.add( new qx.ui.core.Spacer(), {flex:1});
            syncContainer.add( this.m_synchronizeCheck );
            syncContainer.add( new qx.ui.core.Spacer(), {flex:1});
            this._add( syncContainer );
        },
        
        /**
         * Initialize an color slider.
         * @param slider {qx.ui.form.Slider} the slider to customize.
         * @param sliderGroup {qx.ui.groupbox.GroupBox} the container for the slider.
         */
        _initSlider : function( slider, sliderGroup ){
            slider.setMinimum( skel.widgets.Colormap.ColorMix.SLIDER_MIN );
            slider.setMaximum( skel.widgets.Colormap.ColorMix.SLIDER_MAX );
            var sliderId = slider.addListener( skel.widgets.Path.CHANGE_VALUE,this._sliderChangedValue, this );
            slider.setValue( 0 );
            slider.setSingleStep( 1 );
            slider.setPageStep( 10 );
            slider.setFocusable( false );
            slider.setOrientation( "horizontal");
            sliderGroup.add( slider );
            return sliderId;
        },
        
        /**
         * Return the amount of a color in the map.
         * @param percent {Number} the percentage of the color in the map.
         * @return {Number} the amount of the color in the map.
         */
        _percentToValue : function( percent ){
            var minSlider = skel.widgets.Colormap.ColorMix.SLIDER_MIN;
            var range = this._getRange();
            var value = percent * range + minSlider;
            value = Math.floor( value );
            return value;
        },
        
        
        /**
         * Callback for a slider changing its value.
         * @param e {qx.ui.event.Type}.
         */
        _sliderChangedValue : function( e ){
            if ( this.m_synchronizeCheck !== null && this.m_synchronizeCheck.getValue()){
                var newValue = e.getData();
                this.m_redSlider.removeListenerById( this.m_redSliderListenerId );
                this.m_greenSlider.removeListenerById( this.m_greenSliderListenerId );
                this.m_blueSlider.removeListenerById( this.m_blueSliderListenerId );
                if ( this.m_redSlider.getValue() != newValue ){
                    this.m_redSlider.setValue( newValue );
                }
                if ( this.m_greenSlider.getValue() != newValue ){
                    this.m_greenSlider.setValue( newValue );
                }
                if ( this.m_blueSlider.getValue() != newValue ){
                    this.m_blueSlider.setValue( newValue );
                }
                this.m_redSliderListenerId = this.m_redSlider.addListener( skel.widgets.Path.CHANGE_VALUE,this._sliderChangedValue, this );
                this.m_greenSliderListenerId = this.m_greenSlider.addListener( skel.widgets.Path.CHANGE_VALUE,this._sliderChangedValue, this );
                this.m_blueSliderListenerId = this.m_blueSlider.addListener( skel.widgets.Path.CHANGE_VALUE,this._sliderChangedValue, this );
                this._sendColorMixChangedCommand();
            }
            else {
                this._sendColorMixChangedCommand();
            }
        },
        

        
        /**
         * Notify the server that the color mix has changed.
         */
        _sendColorMixChangedCommand : function(){
            if ( this.m_id !== null ){
                if ( this.m_redSlider !== null && this.m_greenSlider !==null && this.m_blueSlider !== null){
                    var redPercent = this._getColorPercent( this.m_redSlider.getValue());
                    var greenPercent = this._getColorPercent( this.m_greenSlider.getValue());
                    var bluePercent = this._getColorPercent( this.m_blueSlider.getValue());
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Colormap.ColorMix.CMD_SET_COLORMIX;
                    var params = "red:"+redPercent+",green:"+greenPercent+",blue:"+bluePercent;
                    this.m_connector.sendCommand( cmd, params, this._errorColorsCB( this ));
                }
            }
        },
        
        /**
         * Set the server side id of the color map.
         * @param id {String} the unique server side id of this color map.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        /**
         * Update the GUI with the new color mix percents.
         * @param redPercent {Number} a decimal in [0,1] indicating the red percentage.
         * @param greenPercent {Number} a decimal in [0,1] indicating the green percentage.
         * @param bluePercent {Number} a decimal in [0,1] indicating the blue percentage.
         */
        setMix : function( redPercent, greenPercent, bluePercent ){
            this.m_colorCanvas.setScales( redPercent, greenPercent, bluePercent );
            this.m_redSlider.removeListenerById( this.m_redSliderListenerId );
            this.m_greenSlider.removeListenerById( this.m_greenSliderListenerId );
            this.m_blueSlider.removeListenerById( this.m_blueSliderListenerId );
            var red = this._percentToValue( redPercent );
            var green = this._percentToValue( greenPercent );
            var blue = this._percentToValue( bluePercent );
            if ( this.m_redSlider.getValue() != red ){
                this.m_redSlider.setValue( red );
            }
            if ( this.m_blueSlider.getValue() != blue ){
                this.m_blueSlider.setValue( blue );
            }
            if ( this.m_greenSlider.getValue() != green ){
                this.m_greenSlider.setValue( green );
            }
            this.m_redSliderListenerId = this.m_redSlider.addListener( skel.widgets.Path.CHANGE_VALUE,this._sliderChangedValue, this );
            this.m_greenSliderListenerId = this.m_greenSlider.addListener( skel.widgets.Path.CHANGE_VALUE,this._sliderChangedValue, this );
            this.m_blueSliderListenerId = this.m_blueSlider.addListener( skel.widgets.Path.CHANGE_VALUE,this._sliderChangedValue, this );
        },
        
        /**
         * Invert the colors in the mix.
         * @param invert {boolean} - invert the colors in the mix.
         */
        setInvert : function( invert ){
            this.m_colorCanvas.setInvert( invert );
        },
        
        /**
         * Reverse the colors in the mix.
         * @param reverse {boolean} - reverse the colors in the mix.
         */
        setReverse : function( reverse ){
            this.m_colorCanvas.setReverse( reverse );
        },
        
        /**
         * Set the name of the color map being used.
         * @param name {String} - the name of the color map in use.
         */
        setColorMapName : function( name ){
            this.m_colorCanvas.setColorName( name );
        },
        
        m_id : null,
        m_colorCanvas : null,
        m_connector : null,
        m_redSlider : null,
        m_redSliderListenerId : null,
        m_greenSlider : null,
        m_greenSliderListenerId : null,
        m_colorGraph : null,
        m_blueSlider : null,
        m_blueSliderListenerId : null,
        m_synchronizeCheck : null
    }
});