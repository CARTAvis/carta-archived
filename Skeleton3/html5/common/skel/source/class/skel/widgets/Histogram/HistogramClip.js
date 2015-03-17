/**
 * Controls for setting the clip for the histogram, either as a [min,max] range
 * or as a percentage.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramClip", {
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
        CMD_SET_CLIP_MIN : "setColorMin",
        CMD_SET_CLIP_MIN_PERCENT: "setColorMinPercent",
        CMD_SET_CLIP_MAX : "setColorMax",
        CMD_SET_CLIP_MAX_PERCENT: "setColorMaxPercent",
        CMD_APPLY_CLIP_IMAGE : "setClipToImage"
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
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.HBox(1);
            this._setLayout(widgetLayout);
            
            var overallContainer = new qx.ui.groupbox.GroupBox( "Linked Image Clip (shift + mouse left drag)", "");
            overallContainer.setLayout( new qx.ui.layout.VBox(1));
            overallContainer.setContentPadding(1,1,1,1);
            this._add( overallContainer );
            
            //Custom clip
            var rangeContainer = new qx.ui.container.Composite();
            this.m_customCheck = new qx.ui.form.CheckBox( "Custom Image Clip");
            this.m_customCheck.setValue( true );
            this.m_customCheck.addListener(skel.widgets.Path.CHANGE_VALUE, function(){
                var customValue = this.m_customCheck.getValue();
                rangeContainer.setEnabled( customValue );
                this._sendCustomClipCmd();
            }, this );
            var horContainer2 = new qx.ui.container.Composite();
            horContainer2.setLayout( new qx.ui.layout.HBox(1));
            horContainer2.add( new qx.ui.core.Spacer(), {flex:1} );
            horContainer2.add( this.m_customCheck );
            horContainer2.add( new qx.ui.core.Spacer(), {flex:1} );
            overallContainer.add(horContainer2 );
            
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
            this.m_minClipListenerId = this.m_minClipText.addListener( "textChanged", 
                    this._sendClipMinCmd,  this );
            
            var valueLabel = new qx.ui.basic.Label( "Value:");
            valueLabel.setTextAlign( "right");
            this.m_percentMinClipText = new skel.widgets.CustomUI.NumericTextField( 0, 100);
            this.m_percentMinClipText.setToolTipText( "Percentage to clip from the left on the horizontal axis; 0 is no clipping.");
            this.m_percentMinClipText.setIntegerOnly( false );
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
            this.m_maxClipText.setIntegerOnly( false );
            this.m_maxClipListenerId = this.m_maxClipText.addListener( "textChanged", 
                    this._sendClipMaxCmd, this );
            
            var percentLabel = new qx.ui.basic.Label( "Percent:");
            percentLabel.setTextAlign( "right");
            this.m_percentMaxClipText = new skel.widgets.CustomUI.NumericTextField( 0, 100);
            this.m_percentMaxClipText.setToolTipText( "Percentage to clip from the right on the horizontal axis; 0 is no clipping.");
            this.m_percentMaxClipText.setIntegerOnly( false );
            this.m_percentMaxClipListenerId = this.m_percentMaxClipText.addListener( "textChanged", 
                    this._sendClipMaxPercentCmd, this );
            
            rangeContainer.add( maxLabel, {row:0, column:2});
            rangeContainer.add( this.m_maxClipText, {row:1, column:2});
            rangeContainer.add( this.m_percentMaxClipText, {row:2, column:2});
            rangeContainer.add( percentLabel, {row:2, column:0});
            
            //Apply to image
            this.m_applyImageClip = new qx.ui.form.Button( "Apply to Image");
            this.m_applyImageClip.setToolTipText( "Clip all linked images by minimum and maximum range values.");
            this.m_applyImageClip.addListener( "execute", function(e){
                if ( this.m_connector !== null ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramClip.CMD_APPLY_CLIP_IMAGE;
                    var params = "";
                    this.m_connector.sendCommand( cmd, params, function(){});
                }
            }, this );
            var horContainer = new qx.ui.container.Composite();
            horContainer.setLayout( new qx.ui.layout.HBox(2));
            horContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            horContainer.add( this.m_applyImageClip );
            horContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            overallContainer.add( horContainer );
        },
        
        /**
         * Notify the server of the lower clip amount.
         */
        _sendClipMinCmd: function(){
            if( this.m_connector !== null ){
                var minClip = this.m_minClipText.getValue();
                if( !isNaN(minClip) ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramClip.CMD_SET_CLIP_MIN;
                    var params = "colorMin:"+minClip;
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
                    var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramClip.CMD_SET_CLIP_MAX;
                    var params = "colorMax:"+maxClip;
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
                     var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramClip.CMD_SET_CLIP_MIN_PERCENT;
                     var params = "colorMinPercent:"+minPercentClip;
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
                     var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramClip.CMD_SET_CLIP_MAX_PERCENT;
                     var params = "colorMaxPercent:"+maxPercentClip;
                     this.m_connector.sendCommand( cmd, params, this._errorClipMaxPercentCB( ));
                 }
             }
         },
         
         /**
          * Send a command to the server indicating whether or not to use clips
          * separate from zoom.
          */
         _sendCustomClipCmd : function(){
             if ( this.m_connector !== null ){
                 var customClip = this.m_customCheck.getValue();
                 var path = skel.widgets.Path.getInstance();
                 var cmd = this.m_id + path.SEP_COMMAND + "setCustomClip";
                 var params = "customClip:"+customClip;
                 this.m_connector.sendCommand( cmd, params, function(){});
             }
         },

        /**
         * Set upper and lower bounds for the histogram range.
         * @param min {Number} a lower (inclusive) bound.
         * @param max {Number} an upper (inclusive) bound.
         */
        setColorRange : function( min, max ){
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
        setColorRangePercent : function( min, max ){
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
         * Set whether or not to enable custom clip settings.
         * @param val {boolean} true to enable custom clip settings; false otherwise.
         */
        setCustomClip : function( val ){
            var oldValue = this.m_customCheck.getValue();
            if ( val != oldValue ){
                this.m_customCheck.setValue( val );
            }
        },
       
        
        /**
         * Set the server side id of this histogram.
         * @param id {String} the server side id of the object that produced this histogram.
         */
        setId : function( id ){
            this.m_id = id;
        },
        

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
        m_customCheck : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});
