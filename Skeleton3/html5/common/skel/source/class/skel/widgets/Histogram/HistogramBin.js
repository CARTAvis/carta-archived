/**
 * Controls for the histogram's bin count.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

//Event Path : TextField -> Slider -> Server -> TextField

qx.Class.define("skel.widgets.Histogram.HistogramBin", {
    extend : qx.ui.core.Widget,

    
    construct : function() {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init();
    },
    
    statics : {
        CMD_SET_BIN_COUNT : "setBinCount"
    },

    members : {


        /**
         * Initializes the UI.
         */
        _init : function() {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
            
            this.m_binCountText = new skel.widgets.CustomUI.NumericTextField(0,this.m_MAX_BINS);
            this.m_binCountText.addListener("textChanged",
                    function(ev) {
                        var binCountStr = ev.getData().val;
                        var binCount = parseInt( binCountStr );
                        if ( !isNaN( binCount ) ){
                            this.m_binCountSlider.setValue( binCount );
                        }
                }, this);
            skel.widgets.TestID.addTestId( this.m_binCountText, skel.widgets.TestID.HISTOGRAM_BIN_COUNT_INPUT);
            this._add( this.m_binCountText );
            
            this.m_binCountSlider = new qx.ui.form.Slider();
            this.m_binCountSlider.setMinimum( 0 );
            this.m_binCountSlider.setMaximum( this.m_MAX_BINS );
            this.m_binCountSlider.setValue( 25 );
            this.m_binCountSlider.addListener( "changeValue", function(){
                //Notify the server of the new value.
                var value = this.m_binCountSlider.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramBin.CMD_SET_BIN_COUNT;
                var params = "binCount:"+value;
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this);
            skel.widgets.TestID.addTestId( this.m_binCountSlider, skel.widgets.TestID.HISTOGRAM_BIN_COUNT_SLIDER);
            this._add( this.m_binCountSlider );
        },
        
        /**
         * Set a new histogram bin count.
         * @param binCount {Number} the new bin count.
         */
        setBinCount : function( binCount ){
            var oldBinCount = this.m_binCountText.getValue();
            if ( binCount != oldBinCount ){
                this.m_binCountText.setValue( binCount );
            }
        },
        
        /**
         * Set the server side id of this histogram.
         * @param id {String} the server side id of the object that produced this histogram.
         */
        setId : function( id ){
            this.m_id = id;
        },

        
        m_binCountText : null,
        m_binCountSlider : null,
        m_id : null,
        m_connector : null,
        m_MAX_BINS : 100
        
    }
});