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
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
        }
        this._init();
    },
    
    statics : {
        CMD_SET_BIN_COUNT : "setBinCount"
    },

    members : {
        
        /**
         * Callback for a server error when setting the bin count.
         * @param anObject {skel.widgets.Histogram.HistogramBin}.
         */
        _errorBinCountCB : function( anObject ){
            return function( binCount ){
                if ( binCount ){
                    var binCountInt = parseInt( binCount );
                    anObject.setBinCount( binCountInt );
                }
            };
        },
        


        /**
         * Initializes the UI.
             
         */
        _init : function() {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);

            //var binContainer =  new qx.ui.groupbox.GroupBox("Code Assist", "");
            //binContainer.setLayout(new qx.ui.layout.VBox(2));

            this.m_binCountText = new skel.widgets.CustomUI.NumericTextField(0,this.m_MAX_BINS);
            this.m_binCountText.addListener("textChanged",
                    function(ev) {
                        var binCount = this.m_binCountText.getValue();
                        if ( !isNaN( binCount ) ){
                            this.m_binCountSlider.setValue( binCount );
                        }
                }, this);
            this.m_binCountText.setTextId( skel.widgets.TestID.HISTOGRAM_BIN_COUNT_INPUT );
            this._add( this.m_binCountText );
            
            this.m_binCountSlider = new qx.ui.form.Slider();
            this.m_binCountSlider.setMinimum( 0 );
            this.m_binCountSlider.setMaximum( this.m_MAX_BINS );
            this.m_binCountSlider.setValue( 25 );
            this.m_binCountSlider.addListener( "changeValue", function(){
                if ( this.m_connector !== null ){
                    //Notify the server of the new value.
                    var value = this.m_binCountSlider.getValue();
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramBin.CMD_SET_BIN_COUNT;
                    var params = "binCount:"+value;
                    this.m_connector.sendCommand( cmd, params, this._errorBinCountCB(this));
                }
            }, this);
            skel.widgets.TestID.addTestId( this.m_binCountSlider, skel.widgets.TestID.HISTOGRAM_BIN_COUNT_SLIDER);
            this._add( this.m_binCountSlider );
            //binContainer._add(this.m_binCountText);
            //binContainer._add(this.m_binCountSlider);

            //this._add(binContainer);

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
            //Someone may have moved the slider and got an error on the 
            //server so we may have to reset the slider.
            else {
                if ( binCount != this.m_binCountSlider.getValue()){
                    this.m_binCountSlider.setValue( binCount );
                }
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