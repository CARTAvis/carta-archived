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
        
        //Initiate connector.
         if ( typeof mImport !== "undefined"){
             this.m_connector = mImport("connector");
         }
    },
    
    statics : {
        CMD_SET_CLIP_PERCENT : "setClipPercent",
        CMD_SET_CLIP_MIN : "setClipMin",
        // CMD_SET_CLIP_MIN_PERCENT: "setClipMinPercent",
        CMD_SET_CLIP_MAX : "setClipMax",
        // CMD_SET_CLIP_MAX_PERCENT: "setClipMaxPercent",
        CMD_APPLY_CLIP_IMAGE : "setClipToImage",
        CHANGE_VALUE : "changeValue"
    },

    members : {
        
        /**
         * Callback for a server error when setting the histogram range.
         * @param anObject {skel.widgets.Histogram.HistogramRange}.
         */
        _errorRangeCB : function( anObject ){
            return function( range ){
                // if ( range ){
                //     var rangeArray = range.split( ",");
                //     var min = parseInt(rangeArray[0]);
                //     var max = parseInt( rangeArray[1]);
                //     anObject.setClipBounds( min, max );
                // }
            };
        },
        _errorRangePercentCB : function( anObject ){
            return function( rangePercent ){
                // if ( rangePercent ){
                //     var rangeArray = range.split( ",");
                //     var min = parseInt(rangeArray[0]);
                //     var max = parseInt( rangeArray[1]);
                // }
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
                this._sendClipMinCmd();
            }, this );

            var percentMinLabel = new qx.ui.basic.Label( "Min Percent:");
            this.m_percentMinClipText = new skel.widgets.CustomUI.NumericTextField( 0, 100);
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
                this._sendClipMaxCmd();
            }, this );

            var percentMaxLabel = new qx.ui.basic.Label( "Max Percent:");
            this.m_percentMaxClipText = new skel.widgets.CustomUI.NumericTextField( 0, 100);
            this.m_percentMaxClipText.setIntegerOnly( false );
            this.m_percentMaxClipText.addListener( "textChanged", function(){
                this._sendClipRangePercentCmd();
            }, this );

            maxComposite.add( maxLabel );
            maxComposite.add( this.m_maxClipText );
            maxComposite.add(percentMaxLabel);
            maxComposite.add(this.m_percentMaxClipText);
            
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
        },
        
        /**
         * Notify the server that the clip range has changed.
         */

        _sendClipMinCmd: function(){
            if( this.m_connector !== null ){
                var minClip = this.m_minClipText.getValue();
                var links = this.m_links;
                if( !isNaN(minClip) ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_MIN;
                    var params = "clipMin:"+minClip+",links:"+links;
                    this.m_connector.sendCommand( cmd, params, this._errorRangeCB( this ));
                }
            }

        },

        _sendClipMaxCmd: function(){
            if( this.m_connector !== null ){
                var maxClip = this.m_maxClipText.getValue();
                var links = this.m_links;
                if( !isNaN(maxClip) ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_MAX;
                    var params = "clipMax:"+maxClip+",links:"+links;
                    this.m_connector.sendCommand( cmd, params, this._errorRangeCB( this ));

                }
            }

        },

        // _sendClipMinPercentCmd: function(){
        //     if( this.m_connector !== null ){
        //         var minPercentClip = this.m_percentMinClipText.getValue();
        //         var links = this.m_links;
        //         if( !isNaN(minPercentClip) ){
        //             var path = skel.widgets.Path.getInstance();
        //             var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_MIN_PERCENT;
        //             var params = "clipMinPercent:"+minPercentClip+",links:"+links;
        //             this.m_connector.sendCommand( cmd, params, this._errorRangeCB( this ));
        //         }
        //     }

        // },

        // _sendClipMaxPercentCmd: function(){
        //     if( this.m_connector !== null ){
        //         var maxPercentClip = this.m_percentMaxClipText.getValue();
        //         var links = this.m_links;
        //         if( !isNaN(maxPercentClip) ){
        //             var path = skel.widgets.Path.getInstance();
        //             var cmd = this.m_id+path.SEP_COMMAND+skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_MAX_PERCENT;
        //             var params = "clipMaxPercent:"+maxPercentClip+",links:"+links;
        //             this.m_connector.sendCommand( cmd, params, this._errorRangeCB( this ));
        //         }
        //     }

        // },

        /**
         * Notify the server that the clip range has changed.
         */
        _sendClipRangePercentCmd : function(){
            if ( this.m_connector !== null ){
                var minClipPercent = this.m_percentMinClipText.getValue();
                var maxClipPercent = this.m_percentMaxClipText.getValue();
                var links = this.m_links;
                if ( !isNaN(minClipPercent) && !isNaN(maxClipPercent) ){
                    minClipPercent = minClipPercent/100;
                    maxClipPercent = maxClipPercent/100;
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramRange.CMD_SET_CLIP_PERCENT;
                    var params = "clipMinPercent:"+minClipPercent+",clipMaxPercent:"+maxClipPercent+",links:"+links;
                    this.m_connector.sendCommand( cmd, params, this._errorRangeCB( this));

                }
            }
        },

        /**
         * Set upper and lower bounds for the histogram range.
         * @param min {Number} a lower (inclusive) bound.
         * @param max {Number} an upper (inclusive) bound.
         */
        setClipBounds : function( min, max ){
            var oldClipMin = this.m_minClipText.getValue();
            if ( oldClipMin != min ){
                this.m_minClipText.setValue( min );
            }
            var oldClipMax = this.m_maxClipText.getValue();
            if ( oldClipMax != max ){
                console.log("maxclip: "+max);
                this.m_maxClipText.setValue( max );
            }
        },

        setClipPercents : function( min, max ){
            min = min*100;
            max = max*100;
            var oldClipMinPercent = this.m_percentMinClipText.getValue();
            if ( oldClipMinPercent != min ){
                this.m_percentMinClipText.setValue( min );
            }
            var oldClipMax = this.m_percentMaxClipText.getValue();
            if ( oldClipMax != max ){
                this.m_percentMaxClipText.setValue( max );
            }
        },
        
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

        setLinks : function( links ){
            this.m_links = links;
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

        m_links : null
    }
});
