/**
 * Controls for setting the clip for the histogram, either as a [min,max] range
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
        
        //Initiate a request for the shared variable containing a list of clip percentages.
         if ( typeof mImport !== "undefined"){
             this.m_connector = mImport("connector");
             var pathDict = skel.widgets.Path.getInstance();
        //     this.m_sharedVarClipPercents = this.m_connector.getSharedVar(pathDict.CLIPS);
        //     this.m_sharedVarClipPercents.addCB(this._clipPercentsChangedCB.bind(this));
        //     this._clipPercentsChangedCB();
         }
    },
    
    statics : {
        CMD_SET_CLIP_PERCENT : "setClipPercent",
        CMD_SET_CLIP_RANGE : "setClipRange",
        CMD_APPLY_CLIP_IMAGE : "setClipToImage",
        CHANGE_VALUE : "changeValue"
    },

    members : {
        /**
         * Callback for a change in the available clip percentages on the server.
         */
        // _clipPercentsChangedCB : function(){
        //     var val = this.m_sharedVarClipPercents.get();
        //     if ( val ){
        //         try {
        //             var clips = JSON.parse( val );
        //             var clipCount = clips.clipCount;
        //             this.m_percentCombo.removeAll();
        //             for ( var i = 0; i < clipCount; i++ ){
        //                 var clipAmount = clips.clipList[i];
        //                 var tempItem = new qx.ui.form.ListItem( clipAmount.toString() );
        //                 this.m_percentCombo.add( tempItem );
        //             }
        //         }
        //         catch( err ){
        //             console.log( "Could not parse: "+val );
        //         }
        //     }
        // },
        
        /**
         * Callback for a server error when setting the histogram range.
         * @param anObject {skel.widgets.Histogram.HistogramRange}.
         */
        _errorRangeCB : function( anObject ){
            return function( range ){
                if ( range ){
                    var rangeArray = range.split( ",");
                    var min = parseInt(rangeArray[0]);
                    var max = parseInt( rangeArray[1]);
                    anObject.setClipBounds( min, max );
                }
            };
        },
        
        /**
         * Callback for a server error when setting the apply clip to image flag.
         * @param anObject {skel.widgets.Histogram.HistogramRange}.
         */
        _errorClipToImageCB : function( anObject ){
            return function( clipToImage ){
                if ( clipToImage ){
                    var clipToImageBool = skel.widgets.Util.toBool( clipToImage );
                    anObject.setApplyClipToImage( clipToImageBool );
                }
            };
        },
        
        /**
         * Callback for a server error when setting the clip percent.
         * @param anObject {skel.widgets.Histogram.HistogramRange}.
         */
        _errorClipIndexCB :function( anObject ){
            return function( clipPercent ){
                if ( clipPercent ){
                    var clipIndex = parseInt( clipPercent );
                    anObject.setClipIndex( clipIndex );
                }
            };
        },

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.HBox(2);
            this._setLayout(widgetLayout);

            //Minimum
            var minComposite = new qx.ui.container.Composite();
            minComposite.setLayout(new qx.ui.layout.VBox(2));

            var minLabel = new qx.ui.basic.Label( "Min:");
            this.m_minClipText = new skel.widgets.CustomUI.NumericTextField( null, null);
            this.m_minClipText.setIntegerOnly( false );
            this.m_minClipText.addListener( "textChanged", function(){
                var lowerBound = this.m_minClipText.getValue();
                this.m_maxClipText.setLowerBound( lowerBound );
                this._sendClipRangeCmd();
            }, this );

            var percentMinLabel = new qx.ui.basic.Label( "Min Percent:");
            this.m_percentMinClipText = new skel.widgets.CustomUI.NumericTextField( null, null);
            this.m_percentMinClipText.setIntegerOnly( false );

            this.m_percentMinClipText.addListener( "textChanged", function(){
                this._sendClipRangePercentCmd();                
            }, this );


            minComposite.add( minLabel );
            minComposite.add( this.m_minClipText );
            minComposite.add(percentMinLabel);
            minComposite.add(this.m_percentMinClipText);
            
            //Maximum
            var maxComposite = new qx.ui.container.Composite();
            maxComposite.setLayout(new qx.ui.layout.VBox(2));
            var maxLabel = new qx.ui.basic.Label( "Max:");
            this.m_maxClipText = new skel.widgets.CustomUI.NumericTextField( null, null );
            this.m_maxClipText.setIntegerOnly( false );
            this.m_maxClipText.addListener( "textChanged", function(){
                var upperBound = this.m_maxClipText.getValue();
                this.m_minClipText.setUpperBound( upperBound);
                this._sendClipRangeCmd();
            }, this );


            var percentMaxLabel = new qx.ui.basic.Label( "Max Percent:");
            this.m_percentMaxClipText = new skel.widgets.CustomUI.NumericTextField( null, null);
            this.m_percentMaxClipText.setIntegerOnly( false );
            this.m_percentMaxClipText.addListener( "textChanged", function(){
                this._sendClipRangePercentCmd();                
            }, this );



            maxComposite.add( maxLabel );
            maxComposite.add( this.m_maxClipText );
            maxComposite.add(percentMaxLabel);
            maxComposite.add(this.m_percentMaxClipText);
            
            
            //Percentile
            // var percentComposite = new qx.ui.container.Composite();
            // percentComposite.setLayout( new qx.ui.layout.HBox(2) );
            // var percentLabel = new qx.ui.basic.Label( "Percent:" );
            // this.m_percentCombo = new qx.ui.form.ComboBox();
            // this.m_percentCombo.addListener( skel.widgets.Histogram.HistogramRange.CHANGE_VALUE, function(e){
            //     if ( this.m_connector !== null ){
            //         var clipValue = e.getData();
            //         //Send a command to the server to let them know the clip percentage changed.
            //         var path = skel.widgets.Path.getInstance();
            //         var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_PERCENT;
            //         var params = "clipPercent:"+clipValue;
            //         this.m_connector.sendCommand( cmd, params, this._errorClipIndexCB(this));
            //     }
            // },this);
            // percentComposite.add( percentLabel );
            // percentComposite.add( this.m_percentCombo );
            
            //Apply to image
            this.m_applyImageClip = new qx.ui.form.CheckBox( "Apply to Image(s)");
            this.m_applyImageClip.addListener( skel.widgets.Histogram.HistogramRange.CHANGE_VALUE, function(e){
                if ( this.m_connector !== null ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramRange.CMD_APPLY_CLIP_IMAGE;
                    var params = "applyClipToImage:"+this.m_applyImageClip.getValue();
                    this.m_connector.sendCommand( cmd, params, this._errorClipToImageCB(this));
                }
            }, this );
            this._add( minComposite );
            this._add( maxComposite );
            // this._add( percentComposite );
            // this._add( this.m_applyImageClip );
        },
        
        /**
         * Notify the server that the clip range has changed.
         */
        _sendClipRangeCmd : function(){
            if ( this.m_connector !== null ){
                var minClip = this.m_minClipText.getValue();
                var maxClip = this.m_maxClipText.getValue();
                if ( !isNaN(minClip) && !isNaN(maxClip) ){
                    if ( minClip <= maxClip ){
                        var path = skel.widgets.Path.getInstance();
                        var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_RANGE;
                        var params = "clipMin:"+minClip+",clipMax:"+maxClip;
                        this.m_connector.sendCommand( cmd, params, this._errorRangeCB( this));
                    }
                }
            }
        },


        /**
         * Notify the server that the clip range has changed.
         */
        _sendClipRangePercentCmd : function(){
            if ( this.m_connector !== null ){
                var minClipPercent = this.m_percentMinClipText.getValue();
                var maxClipPercent = this.m_percentMaxClipText.getValue();
                if ( !isNaN(minClipPercent) && !isNaN(maxClipPercent) ){
                    // if ( minClipPercent <= maxClipPercent ){
                        var path = skel.widgets.Path.getInstance();
                        var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_PERCENT;
                        var params = "clipMinPercent:"+minClipPercent+",clipMaxPercent:"+maxClipPercent;
                        this.m_connector.sendCommand( cmd, params, this._errorRangeCB( this));
                    // }
                }
            }
        },
        
        /**
         * Set upper and lower bounds for the histogram range.
         * @param min {Number} a lower (inclusive) bound.
         * @param max {Number} an upper (inclusive) bound.
         */
        setClipBounds : function( min, max ){
            if ( min <= max ){
                var oldClipMin = this.m_minClipText.getValue();
                if ( oldClipMin != min ){
                    this.m_minClipText.setValue( min );
                }
                var oldClipMax = this.m_maxClipText.getValue();
                if ( oldClipMax != max ){
                    this.m_maxClipText.setValue( max );
                }
            }
        },

        setClipPercents : function( min, max ){
            if ( min <= max ){
                var oldClipMinPercent = this.m_percentMinClipText.getValue();
                if ( oldClipMinPercent != min ){
                    this.m_percentMinClipText.setValue( min );
                }
                var oldClipMax = this.m_percentMaxClipText.getValue();
                if ( oldClipMax != max ){
                    this.m_percentMaxClipText.setValue( max );
                }
            }
        },
        
        /**
         * Set the clip index of the GUI based on the server settings.
         * @param index {Number} the clip index.
         */
        // setClipIndex : function( index ){
        //     var selectables = this.m_percentCombo.getChildrenContainer().getSelectables();
        //     if ( 0 <= index && index < selectables.length ){
        //         var clipItem = selectables[index];
        //         var currValue = this.m_percentCombo.getValue();
        //         var comboValue = clipItem.getLabel();
        //         if ( currValue != comboValue ){
        //             this.m_percentCombo.setValue( comboValue );
        //         }
        //     }
        // },
        
        /**
         * Set whether or not to apply the clip to the image as well
         * as the histogram based on server settings.
         * @param apply {boolean} true if the clip should be applied to both
         *      the histogram and the image; false if it applies only to the histogram.
         */
        setApplyClipToImage : function( apply ){
            if ( this.m_applyImageClip.getValue() !== apply ){
                this.m_applyImageClip.setValue( apply );
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
        m_sharedVarClipPercents : null,
        
        m_maxClipText : null,
        m_minClipText : null,
        m_percentMinClipText : null,
        m_percentMaxClipText : null,
        m_applyImageClip : null,
        // m_percentCombo : null
    }
});