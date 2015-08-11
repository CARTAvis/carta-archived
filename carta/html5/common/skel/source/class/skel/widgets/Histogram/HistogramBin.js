/**
 * Controls for the histogram's bin count.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/


qx.Class.define("skel.widgets.Histogram.HistogramBin", {
    extend : qx.ui.core.Widget, 
    
    construct : function() {
        this.base(arguments);
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
        }
        this._init();
    },


    members : {
        
        
        /**
         * Callback for a server error when setting the bin width.
         * @param anObject {skel.widgets.Histogram.HistogramBin}.
         */
        _errorBinWidthCB : function( anObject ){
            return function( msg ){
                if ( msg !== null && msg.length > 0 ){
                    anObject.m_widthText.setError( true );
                }
                else {
                    var oldError = anObject.m_widthText.isError();
                    if ( oldError ){
                        anObject.m_widthText.setError( false );
                        var errorMan = skel.widgets.ErrorHandler.getInstance();
                        errorMan.clearErrors();
                    }
                }
            };
        },

        /**
         * Initializes the UI.
             
         */
        _init : function() {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);

            var binContainer = new qx.ui.groupbox.GroupBox("Bins", "");
            binContainer.setContentPadding(1,1,1,1);
            binContainer.setLayout( new qx.ui.layout.VBox(1));
            this._add( binContainer);
            
            //Bin Count
            this.m_binCountTextSlider = new skel.widgets.CustomUI.TextSlider("setBinCount", "binCount",
                    1,10000, 25, "Count", true, "Set the number of histogram bins.", "Slide to set the number of histogram bins.",
                    "histogramBinCountTextField", "histogramBinCountSlider", false);
            binContainer.add( this.m_binCountTextSlider );
            
            //Bin Width
            var binWidthContainer = new qx.ui.container.Composite();
            binWidthContainer.setLayout( new qx.ui.layout.HBox(1));
            var widthLabel = new qx.ui.basic.Label( "Width:");
            binWidthContainer.add( widthLabel);
            this.m_widthText = new skel.widgets.CustomUI.NumericTextField(0,null);
            this.m_widthText.setIntegerOnly( false );
            this.m_widthListenerId = this.m_widthText.addListener( "textChanged", this._sendWidthCmd, this._errorBinWidthCB(this) );
            this.m_widthText.setToolTipText( "Specify the width of the histogram bins.");
            binWidthContainer.add( this.m_widthText );
            binContainer.add( binWidthContainer );
        },
        
        
        /**
         * Sends a bin width change to the server.
         */
        _sendWidthCmd : function(){
            if ( this.m_connector !== null ){
                var value = this.m_widthText.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND +"setBinWidth";
                var params = "binWidth:"+value;
                this.m_connector.sendCommand( cmd, params, this._errorBinWidthCB( this));
            }
        },
        
        /**
         * Set a new histogram bin count.
         * @param binCount {Number} the new bin count.
         */
        setBinCount : function( binCount ){
            this.m_binCountTextSlider.setValue( binCount );
        },
        
        /**
         * Set the maximum allowed value of the bin count.
         * @param binCountMax {Number} the maximum bin count allowed.
         */
        setBinCountMax : function( binCountMax ){
            this.m_binCountTextSlider.setMax( binCountMax );
        },
        
        /**
         * Sets a new histogram bin width.
         * @param binWidth {Number} the new bin width.
         */
        setBinWidth : function( binWidth ){
            if ( binWidth !== undefined ){
                var oldBinWidth = this.m_widthText.getValue();
                if ( oldBinWidth !== null || Math.abs( binWidth - oldBinWidth > 0.000001 )){
                    this.m_widthText.removeListenerById( this.m_widthListenerId );
                    this.m_widthText.setValue( binWidth );
                    this.m_widthListenerId = this.m_widthText.addListener( "textChanged", this._sendWidthCmd, this );
                }
            }
        },
        
        /**
         * Set the server side id of this histogram.
         * @param id {String} the server side id of the object that produced this histogram.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_binCountTextSlider.setId( id );
        },

       
        m_binCountTextSlider : null, 
        m_id : null,
        m_connector : null,
        m_widthListenerId : null,
        m_widthText: null
        
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});