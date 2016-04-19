/**
 * Controls for setting the zoom for the profiler, either as a [min,max] range
 * or as a percentage.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.SettingsZoom", {
    extend : qx.ui.core.Widget,

    construct : function( ) {
        this.base(arguments);
        this._init( );
        
        //Initiate connector.
         if ( typeof mImport !== "undefined"){
             this.m_connector = mImport("connector");
         }
    },
    
   

    members : {
        

        
        /**
         * Callback for a server error when setting the histogram minimum clip value.
         * @return {function} which displays/clears the error.
         */
        _errorRangeCB : function( msg ){
            if ( msg == null || msg.length == 0 ){
                var errorMan = skel.widgets.ErrorHandler.getInstance();
                errorMan.clearErrors();
            }
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
                    this._sendRangeCmd,  this );
      
            var valueLabel = new qx.ui.basic.Label( "Value:");
            valueLabel.setTextAlign( "right");
            this.m_percentMinClipText = new skel.widgets.CustomUI.NumericTextField( 0, 100);
            this.m_percentMinClipText.setToolTipText( "Percentage to zoom from the left on the horizontal axis; 0 is no left zoom.");
            this.m_percentMinClipText.setIntegerOnly( false );
            this.m_percentMinClipText.setTextId( "histogramZoomMinPercent");
            this.m_percentMinClipListenerId = this.m_percentMinClipText.addListener( "textChanged", 
                    this._sendRangePercentCmd, this );
            
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
                    this._sendRangeCmd, this );
            
            var percentLabel = new qx.ui.basic.Label( "Percent:");
            percentLabel.setTextAlign( "right");
            this.m_percentMaxClipText = new skel.widgets.CustomUI.NumericTextField( 0, 100);
            this.m_percentMaxClipText.setToolTipText( "Percentage to zoom in from the right on the horizontal axis; 100 is no right zoom.");
            this.m_percentMaxClipText.setIntegerOnly( false );
            this.m_percentMaxClipText.setTextId( "histogramZoomMaxPercent");
            this.m_percentMaxClipListenerId = this.m_percentMaxClipText.addListener( "textChanged", 
                    this._sendRangePercentCmd, this );
            
            rangeContainer.add( maxLabel, {row:0, column:2});
            rangeContainer.add( this.m_maxClipText, {row:1, column:2});
            rangeContainer.add( this.m_percentMaxClipText, {row:2, column:2});
            rangeContainer.add( percentLabel, {row:2, column:0});
            
            //Buffer
            this.m_buffer = new qx.ui.form.CheckBox( "Buffer");
            /*this.m_buffer.addListener( skel.widgets.Path.CHANGE_VALUE, function(e){
                var useBuffer = this.m_buffer.getValue();
                if ( this.m_connector !== null ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + "setUseClipBuffer";
                    var params = "useClipBuffer:"+useBuffer;
                    this.m_connector.sendCommand( cmd, params, function(){});
                }
                this.m_bufferText.setEnabled( useBuffer );
            }, this );*/
           
            this.m_bufferText = new skel.widgets.CustomUI.NumericTextField( 0, 100);
            this.m_bufferText.setIntegerOnly( true );
            this.m_bufferText.setToolTipText( "Provide extra space at each end of the histogram; specify as a percentage [0,100).");
            this.m_bufferText.setEnabled( false );
            /*this.m_bufferText.addListener( "textChanged", function(){
                if ( this.m_connector !== null ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + "setClipBuffer";
                    var params = "clipBuffer:"+this.m_bufferText.getValue();
                    this.m_connector.sendCommand( cmd, params, this._errorBufferCB());
                }
            }, this );*/
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
                var cmd = this.m_id + path.SEP_COMMAND + "zoomFull";
                var params = "";
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this );
            this.m_selectedRange = new qx.ui.form.Button( "Selected");
            this.m_selectedRange.setToolTipText( "Zoom to the graphically selected range.");
            this.m_selectedRange.addListener( "execute", function(){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "zoomSelected";
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
        
       
        
        _sendRangeCmd: function(){
            if( this.m_connector !== null ){
                var minZoom = this.m_minClipText.getValue();
                var maxZoom = this.m_maxClipText.getValue();
                console.log( "_sendRangeCmd minZoom="+minZoom+" maxZoom="+maxZoom);
                if( !isNaN(minZoom) && !isNaN(maxZoom) ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id+path.SEP_COMMAND + "setZoomRange";
                    var params = "zoomMin:"+minZoom + ",zoomMax:"+maxZoom;
                    this.m_connector.sendCommand( cmd, params, this._errorRangeCB( ));
                }
            }
        },
        
        _sendRangePercentCmd: function(){
            if( this.m_connector !== null ){
                var minZoomPercent = this.m_percentMinClipText.getValue();
                var maxZoomPercent = this.m_percentMaxClipText.getValue();
                console.log( "_sendRangePercentCmd min="+minZoomPercent+" max="+maxZoomPercent);
                if( !isNaN(minZoomPercent) && !isNaN(maxZoomPercent) ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id+path.SEP_COMMAND + "setZoomRangePercent";
                    var params = "zoomMinPercent:"+minZoomPercent + ",zoomMaxPercent:"+maxZoomPercent;
                    this.m_connector.sendCommand( cmd, params, this._errorRangeCB( ));
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
         
         dataUpdate : function( data ){
             console.log( "Settings zoom data Update");
             this.setZoomBounds( data.zoomMin, data.zoomMax );
             this.setZoomPercents( data.zoomMinPercent, data.zoomMaxPercent );
         },

        /**
         * Set upper and lower bounds for the profile zoom range.
         * @param min {Number} a lower (inclusive) bound.
         * @param max {Number} an upper (inclusive) bound.
         */
        setZoomBounds : function( min, max ){
            if ( this.m_minClipListenerId !== null ){
                this.m_minClipText.removeListenerById( this.m_minClipListenerId );
            }
            if ( this.m_maxClipListenerId !== null ){
                this.m_maxClipText.removeListenerById( this.m_maxClipListenerId );
            }
            var oldClipMin = this.m_minClipText.getValue();
            if ( oldClipMin != min ){
                this.m_minClipText.setValue( min );
            }
            var oldClipMax = this.m_maxClipText.getValue();
            if ( oldClipMax != max ){
                this.m_maxClipText.setValue( max );
            }
            this.m_maxClipListenerId = this.m_maxClipText.addListener( "textChanged", this._sendRangeCmd, this );
            this.m_minClipListenerId = this.m_minClipText.addListener( "textChanged", this._sendRangeCmd, this );
        },

        /**
         * Set the amount to clip at each end of the histogram based on percentiles.
         * @param min {Number} a decimal [0,1] representing the left amount to clip.
         * @param max {Number} a decimal [0,1] representing the right amount to clip.
         */
        setZoomPercents : function( min, max ){
            if ( this.m_percentMinClipListenerId !== null ){
                this.m_percentMinClipText.removeListenerById( this.m_percentMinClipListenerId );
            }
            if ( this.m_percentMaxClipListenerId !== null ){
                this.m_percentMaxClipText.removeListenerById( this.m_percentMaxClipListenerId );
            }
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
            this.m_percentMinClipListenerId = this.m_percentMinClipText.addListener( "textChanged", this._sendRangePercentCmd, this );
            this.m_percentMaxClipListenerId = this.m_percentMaxClipText.addListener( "textChanged", this._sendRangePercentCmd, this );
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
