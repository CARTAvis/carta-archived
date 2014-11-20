/**
 * Displays a file browser.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColorMix", {
    extend : qx.ui.core.Widget,

    construct : function(  ) {
        this.base(arguments);
        this._init( );
    },

    members : {


        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
            
            
            var sliderGroup = new qx.ui.groupbox.GroupBox( "Color Values");
            sliderGroup.setHeight( 200 );
            sliderGroup.setLayout( new qx.ui.layout.HBox(2));
            this.m_redSlider = new qx.ui.form.Slider();
            this.m_redSlider.setDecorator( "slider-red");
            this._initSlider( this.m_redSlider, sliderGroup );
            this.m_greenSlider = new qx.ui.form.Slider();
            this.m_greenSlider.setDecorator( "slider-green");
            this._initSlider( this.m_greenSlider, sliderGroup );
            this.m_blueSlider = new qx.ui.form.Slider();
            this._initSlider( this.m_blueSlider, sliderGroup );
            this.m_blueSlider.setDecorator( "slider-blue" );
            this._add( sliderGroup );
            
            this.m_synchronizeCheck = new qx.ui.form.CheckBox( "Synchronize");
            this.m_synchronizeCheck.setValue( true );
            this._add( this.m_synchronizeCheck );
        },
        
        _initSlider : function( slider, sliderGroup ){
            slider.setMinimum( 0 );
            slider.setMaximum( 100 );
            slider.addListener( "changeValue",function(e){
                if ( this.m_synchronizeCheck != null && this.m_synchronizeCheck.getValue()){
                    var newValue = e.getData();
                    if ( this.m_redSlider.getValue() != newValue ){
                        this.m_redSlider.setValue( newValue );
                    }
                    if ( this.m_greenSlider.getValue() != newValue ){
                        this.m_greenSlider.setValue( newValue );
                    }
                    if ( this.m_blueSlider.getValue() != newValue ){
                        this.m_blueSlider.setValue( newValue );
                    }
                }
            }, this );
            slider.setValue( 0 );
            slider.setSingleStep( 1 );
            slider.setPageStep( 10 );
            slider.setFocusable( false );
            slider.setOrientation( "vertical");
            sliderGroup.add( slider );
        },
        
        m_redSlider : null,
        m_greenSlider : null,
        m_blueSlider : null,
        m_synchronizeCheck : null
    }
});