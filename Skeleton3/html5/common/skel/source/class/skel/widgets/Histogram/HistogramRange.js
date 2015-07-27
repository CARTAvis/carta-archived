/**
 * Controls for setting the zoom for the histogram, either as a [min,max] range
 * or as a percentage.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramRange", {
    extend : qx.ui.core.Widget,

    construct : function( ) {
        this.base(arguments);
        this._init( );
        
        //Initiate connector.
         if ( typeof mImport !== "undefined"){
             this.m_connector = mImport("connector");
         }
    },
    
    statics : {
        CMD_SET_CLIP_MIN : "setClipMin",
        CMD_SET_CLIP_MIN_PERCENT: "setClipMinPercent",
        CMD_SET_CLIP_MAX : "setClipMax",
        CMD_SET_CLIP_MAX_PERCENT: "setClipMaxPercent",
        CMD_ZOOM_FULL : "zoomFull",
        CMD_ZOOM_RANGE : "zoomRange"
    },

    members : {
        
        /**
         * Callback for showing/clearing errors in a text field.
         * @param textWidget {skel.widgets.CustomUI.ErrorTextField} the text field.
         * @return {function} the callback which will show/clear the error based on the
         *      error message.
         */
        _errorCB : function( textWidget ){
            return function( msg ){
                if ( msg !== null && msg.length > 0 ){
                    textWidget.setError( true );
                }
                else {
                    var oldError = textWidget.isError();
                    if ( oldError ){
                        textWidget.setError( false );
                        var errorMan = skel.widgets.ErrorHandler.getInstance();
                        errorMan.clearErrors();
                    }
                }
            };
        },
        
        /**
         * Callback for a server error when setting the histogram minimum clip value.
         * @return {function} which displays/clears the error.
         */
        _errorClipMinCB : function(  ){
            return this._errorCB( this.m_minClipText );
        },
        
        /**
         * Callback for a server error when setting the histogram maximum clip value.
         * @return {function} which displays/clears the error.
         */
        _errorClipMaxCB : function(){
            return this._errorCB( this.m_maxClipText );
        },
        
        /**
         * Callback for a server error when setting the histogram minimum clip percent.
         * @return {function} which displays/clears the error.
         */
        _errorClipMinPercentCB : function( ){
            return this._errorCB(this.m_percentMinClipText );
        },
        
        /**
         * Callback for a server error when setting the histogram maximum clip percent.
         * @return {function} which displays/clears the error.
         */
        _errorClipMaxPercentCB : function( ){
            return this._errorCB( this.m_percentMaxClipText );
        },
        
        /**
         * Callback for a server error when setting the clip buffer size.
         * @return {function} which displays/clears the error.
         */
        _errorBufferCB : function(){
            return this._errorCB( this.m_bufferText );
        },

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.HBox(1);
            this._setLayout(widgetLayout);
            
            var overallContainer = new qx.ui.groupbox.GroupBox( "Zoom (graph mouse left drag)", "");
            overallContainer.setLayout( new qx.ui.layout.VBox(1));
            overallContainer.setContentPadding(1,1,1,1);
            this._add( overallContainer );
            
            var rangeContainer = new qx.ui.container.Composite();
            var gridLayout = new qx.ui.layout.Grid();
            gridLayout.setColumnAlign(0, "right","middle");
            gridLayout.setRowAlign( 0, "center", "middle");
            gridLayout.setSpacingX( 1 );
            gridLayout.setSpacingY( 1 );
            rangeContainer.setLayout( gridLayout);
            overallContainer.add( rangeContainer);
            
            //Minimum
            var minLabel = new qx.ui.basic.Label( "Min");
            minLabel.setTextAlign( "center");
            this.m_minClipText = new skel.widgets.CustomUI.NumericTextField( null, null);
            this.m_minClipText.setToolTipText( "Smallest value on the horizontal axis.");
            this.m_minClipText.setIntegerOnly( false );
            this.m_minClipText.setTextId( "histogramZoomMinValue");
            this.m_minClipListenerId = this.m_minClipText.addListener( "textChanged", 
                    this._sendClipMinCmd,  this );
            
            var valueLabel = new qx.ui.basic.Label( "Value:");
            valueLabel.setTextAlign( "right");
            this.m_percentMinClipText = new skel.widgets.CustomUI.NumericTextField( 0, 100);
            this.m_percentMinClipText.setToolTipText( "Percentage to zoom from the left on the horizontal axis; 0 is no left zoom.");
            this.m_percentMinClipText.setIntegerOnly( false );
            this.m_percentMinClipText.setTextId( "histogramZoomMinPercent");
            this.m_percentMinClipListenerId = this.m_percentMinClipText.addListener( "textChanged", 
                    this._sendClipMinPercentCmd, this );
            
            rangeContainer.add( minLabel, {row: 0, column:1});
            rangeContainer.add( this.m_minClipText, {row:1, column:1});
            rangeContainer.add( this.m_percentMinClipText, {row:2, column:1});
            rangeContainer.add( valueLabel, {row:1, column:0});
           
            //Maximum
            var maxLabel = new qx.ui.basic.Label( "Max");
            maxLabel.setTextAlign( "center");
            this.m_maxClipText = new skel.widgets.CustomUI.NumericTextField( null, null );
            this.m_maxClipText.setToolTipText( "Largest value on the horizontal axis");
            this.m_maxClipText.setTextId( "histogramZoomMaxValue");
            this.m_maxClipText.setIntegerOnly( false );
            this.m_maxClipListenerId = this.m_maxClipText.addListener( "textChanged", 
                    this._sendClipMaxCmd, this );
            
            var percentLabel = new qx.ui.basic.Label( "Percent:");
            percentLabel.setTextAlign( "right");
            this.m_percentMaxClipText = new skel.widgets.CustomUI.NumericTextField( 0, 100);
            this.m_percentMaxClipText.setToolTipText( "Percentage to zoom in from the right on the horizontal axis; 100 is no right zoom.");
            this.m_percentMaxClipText.setIntegerOnly( false );
            this.m_percentMaxClipText.setTextId( "histogramZoomMaxPercent");
            this.m_percentMaxClipListenerId = this.m_percentMaxClipText.addListener( "textChanged", 
                    this._sendClipMaxPercentCmd, this );
            
            rangeContainer.add( maxLabel, {row:0, column:2});
            rangeContainer.add( this.m_maxClipText, {row:1, column:2});
            rangeContainer.add( this.m_percentMaxClipText, {row:2, column:2});
            rangeContainer.add( percentLabel, {row:2, column:0});
            
            //Buffer
            this.m_buffer = new qx.ui.form.CheckBox( "Buffer");
            this.m_buffer.addListener( skel.widgets.Path.CHANGE_VALUE, function(e){
                var useBuffer = this.m_buffer.getValue();
                if ( this.m_connector !== null ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + "setUseClipBuffer";
                    var params = "useClipBuffer:"+useBuffer;
                    this.m_connector.sendCommand( cmd, params, function(){});
                }
                this.m_bufferText.setEnabled( useBuffer );
            }, this );
           
            this.m_bufferText = new skel.widgets.CustomUI.NumericTextField( 0, 100);
            this.m_bufferText.setIntegerOnly( true );
            this.m_bufferText.setToolTipText( "Provide extra space at each end of the histogram; specify as a percentage [0,100).");
            this.m_bufferText.setEnabled( false );
            this.m_bufferText.addListener( "textChanged", function(){
                if ( this.m_connector !== null ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + "setClipBuffer";
                    var params = "clipBuffer:"+this.m_bufferText.getValue();
                    this.m_connector.sendCommand( cmd, params, this._errorBufferCB());
                }
            }, this );
            var perLabel = new qx.ui.basic.Label( "%");
            
            var bufContainer = new qx.ui.container.Composite();
            bufContainer.setLayout( new qx.ui.layout.HBox(2));
            bufContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            bufContainer.add( this.m_buffer );
            bufContainer.add( this.m_bufferText );
            bufContainer.add( perLabel );
            bufContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            overallContainer.add( bufContainer );
            
            //Zoom and Selected Buttons
            this.m_fullRange = new qx.ui.form.Button( "Full" );
            this.m_fullRange.setToolTipText( "Zoom out to full histogram range.");
            this.m_fullRange.addListener( "execute", function(){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramRange.CMD_ZOOM_FULL;
                var params = "";
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this );
            this.m_selectedRange = new qx.ui.form.Button( "Selected");
            this.m_selectedRange.setToolTipText( "Zoom to the graphically selected range.");
            this.m_selectedRange.addListener( "execute", function(){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramRange.CMD_ZOOM_RANGE;
                var params = "";
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this );
            
            var butContainer = new qx.ui.container.Composite();
            butContainer.setLayout( new qx.ui.layout.HBox(2));
            butContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            butContainer.add( this.m_selectedRange );
            butContainer.add( this.m_fullRange );
            butContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            overallContainer.add( butContainer );
        },
        
        /**
         * Notify the server of the lower clip amount.
         */
        _sendClipMinCmd: function(){
            if( this.m_connector !== null ){
                var minClip = this.m_minClipText.getValue();
                if( !isNaN(minClip) ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_MIN;
                    var params = "clipMin:"+minClip;
                    this.m_connector.sendCommand( cmd, params, this._errorClipMinCB( ));
                }
            }

        },
        /**
         * Notify the server of the upper clip amount.
         */
        _sendClipMaxCmd: function(){
            if( this.m_connector !== null ){
                var maxClip = this.m_maxClipText.getValue();
                if( !isNaN(maxClip) ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_MAX;
                    var params = "clipMax:"+maxClip;
                    this.m_connector.sendCommand( cmd, params, this._errorClipMaxCB());
                }
            }
        },

        /**
         * Notify the server of the lower clip percentage.
         */
         _sendClipMinPercentCmd: function(){
             if( this.m_connector !== null ){
                 var minPercentClip = this.m_percentMinClipText.getValue();
                 if( !isNaN(minPercentClip) ){
                     var path = skel.widgets.Path.getInstance();
                     var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_MIN_PERCENT;
                     var params = "clipMinPercent:"+minPercentClip;
                     this.m_connector.sendCommand( cmd, params, this._errorClipMinPercentCB());
                 }
             }
         },

         /**
          * Notify the server of the upper clip percentage.
          */
         _sendClipMaxPercentCmd: function(){
             if( this.m_connector !== null ){
                 var maxPercentClip = this.m_percentMaxClipText.getValue();
                 if( !isNaN(maxPercentClip) ){
                     var path = skel.widgets.Path.getInstance();
                     var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_MAX_PERCENT;
                     var params = "clipMaxPercent:"+maxPercentClip;
                     this.m_connector.sendCommand( cmd, params, this._errorClipMaxPercentCB( ));
                 }
             }
         },
         
         /**
          * Set whether or not to buffer the clips.
          * @param val {boolean} true if the clip should be buffered; false otherwise.
          */
         setBuffer : function( val ){
             var oldBuffer = this.m_buffer.getValue();
             if ( oldBuffer != val ){
                 this.m_buffer.setValue( val );
             }
         },
         
         /**
          * Set the amount of the clip buffer.
          * @param amount {Number} an integer indicating the total amount of buffering.
          */
         setBufferAmount : function( amount ){
             var oldAmount = this.m_bufferText.getValue();
             if ( oldAmount === null || amount != oldAmount ){
                 this.m_bufferText.setValue( amount );
             }
         },

        /**
         * Set upper and lower bounds for the histogram range.
         * @param min {Number} a lower (inclusive) bound.
         * @param max {Number} an upper (inclusive) bound.
         */
        setClipBounds : function( min, max ){
            this.m_minClipText.removeListenerById( this.m_minClipListenerId );
            this.m_maxClipText.removeListenerById( this.m_maxClipListenerId );
            var oldClipMin = this.m_minClipText.getValue();
            if ( oldClipMin != min ){
                this.m_minClipText.setValue( min );
            }
            var oldClipMax = this.m_maxClipText.getValue();
            if ( oldClipMax != max ){
                this.m_maxClipText.setValue( max );
            }
            this.m_maxClipListenerId = this.m_maxClipText.addListener( "textChanged", this._sendClipMaxCmd, this );
            this.m_minClipListenerId = this.m_minClipText.addListener( "textChanged", this._sendClipMinCmd, this );
        },

        /**
         * Set the amount to clip at each end of the histogram based on percentiles.
         * @param min {Number} a decimal [0,1] representing the left amount to clip.
         * @param max {Number} a decimal [0,1] representing the right amount to clip.
         */
        setClipPercents : function( min, max ){
            this.m_percentMinClipText.removeListenerById( this.m_percentMinClipListenerId );
            this.m_percentMaxClipText.removeListenerById( this.m_percentMaxClipListenerId );
            var newMin = min;
            var newMax = max;
            var oldClipMinPercent = this.m_percentMinClipText.getValue();
            if ( oldClipMinPercent != newMin ){
                this.m_percentMinClipText.setValue( newMin );
            }
            var oldClipMax = this.m_percentMaxClipText.getValue();
            if ( oldClipMax != newMax ){
                this.m_percentMaxClipText.setValue( newMax );
            }
            this.m_percentMinClipListenerId = this.m_percentMinClipText.addListener( "textChanged", this._sendClipMinPercentCmd, this );
            this.m_percentMaxClipListenerId = this.m_percentMaxClipText.addListener( "textChanged", this._sendClipMaxPercentCmd, this );
        },
        

        
       
        
        /**
         * Set the server side id of this histogram.
         * @param id {String} the server side id of the object that produced this histogram.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        m_buffer : null,
        m_bufferText : null,
        m_id : null,
        m_connector : null,
        
        m_maxClipText : null,
        m_minClipText : null,
        m_maxClipListenerId : null,
        m_minClipListenerId : null,
        m_percentMinClipText : null,
        m_percentMaxClipText : null,
        m_percentMinClipListenerId : null,
        m_percentMaxClipListenerId : null,
        m_fullRange : null,
        m_selectedRange : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});
