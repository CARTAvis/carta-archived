/**
 * Displays a color map as a gradient.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColorGradient", {
    extend: qx.ui.embed.Canvas,
    
    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this._init( );
        this.update();
    },

    members : {
        
        /**
         * Adds a color to the gradient.
         * @param gradient {Object} a canvas gradient.
         * @param stopIndex {Number} the index of the color to add.
         * @param posIndex {Number} the location in the gradient for the color.
         */
        _addStop : function( gradient, stopIndex, posIndex ){
            var floatStop = posIndex / this.m_stops.length;
            var redHex = this._processColor( this.m_stops[stopIndex], 1, this.m_scaleRed );
            var greenHex = this._processColor( this.m_stops[stopIndex], 3, this.m_scaleGreen );
            var blueHex = this._processColor( this.m_stops[stopIndex], 5, this.m_scaleBlue );
            var stop = "#"+redHex+greenHex+blueHex;
            gradient.addColorStop( floatStop, stop );
        },
        
        /**
         * Template method, which can be used by derived classes to redraw the
         * content. It is called each time the canvas dimension change and the
         * canvas needs to be updated.
         *
         * @param width {Integer} New canvas width
         * @param height {Integer} New canvas height
         * @param ctx {CanvasRenderingContext2D} The rendering ctx to draw to
         */
        _draw: function (width, height, ctx) {
            this.base(arguments);
            ctx.clearRect( 0, 0, width, height);
            this._drawGradient( width, height, ctx );
            if ( ! this.m_gradientOnly ){
                this._drawColorGraph( width, height, ctx );
            }
        },
        
        /**
         * Draws a color gradient.
         * @param width {Integer} New canvas width
         * @param height {Integer} New canvas height
         * @param ctx {CanvasRenderingContext2D} The rendering ctx to draw to
         */
        _drawGradient : function( width, height, ctx ){
            var grd = ctx.createLinearGradient( 0, 0, width, 0);
            if ( this.m_stops !== null ){
                for ( var i = 0; i < this.m_stops.length; i++ ){
                    if ( ! this.m_reverse ){
                        this._addStop( grd, i, i );
                    }
                    else {
                        this._addStop( grd, this.m_stops.length -i - 1, i );
                    }
                }
            }
            ctx.fillStyle = grd;
            if ( this.m_gradientOnly ){
                ctx.fillRect( 0, 0, width, height );
            }
            else {
                ctx.fillRect( 0, height/2, width, height/2 );
            }
        },
        
        /**
         * Draws a colored line indicating the amount of that color in the map.
         * @param width {Integer} New canvas width
         * @param height {Integer} New canvas height
         * @param ctx {CanvasRenderingContext2D} The rendering ctx to draw to.
         * @param colorStr {String} a string naming a color.
         * @param hexIndex {Number} the location in the hex representation where the color can be found.
         * @param scaleFactor {Number} a multiplier to adjust the color strength.
         */
        _drawColor : function( width, height, ctx, colorStr, hexIndex, scaleFactor ){
            if ( this.m_stops !== null ){
                ctx.beginPath();
                var drawHeight = height / 2;
                var drawStart = 0;
               
                for ( var i = 0; i < this.m_stops.length; i++ ){
                    var colorIndex = i;
                    if ( this.m_reverse ){
                        colorIndex = this.m_stops.length - i - 1;
                    }
                    var hexColor = this.m_stops[colorIndex].substring( hexIndex, hexIndex + 2);
                    var intColor = parseInt( hexColor, 16 );
                    if ( this.m_invert ){
                        intColor = 255 - intColor;
                    }
                    var percentColor = intColor / 255;
                    var lineY = drawHeight - drawHeight * percentColor * scaleFactor;
                    var lineX = i / this.m_stops.length * width;
                    ctx.lineTo( lineX, lineY );
                }
                ctx.strokeStyle = colorStr;
                ctx.lineWidth = 5;
                ctx.stroke();
            }
        },
        
        /**
         * Draws three colored lines indicating the amount of blue, red, and green in the map.
         * @param width {Integer} New canvas width
         * @param height {Integer} New canvas height
         * @param ctx {CanvasRenderingContext2D} The rendering ctx to draw to.
         */
        _drawColorGraph : function( width, height, ctx ){
            this._drawColor( width, height, ctx, "red", 1, this.m_scaleRed);
            this._drawColor( width, height, ctx, "green", 3, this.m_scaleGreen );
            this._drawColor( width, height, ctx, "blue", 5, this.m_scaleBlue );
        },
        
        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            this.setAllowGrowX( true );
            this.setAllowGrowY( true );
            this.setMinWidth( 100 );
            this.m_connector = mImport("connector");
        },
        
        /**
         * Reads a color from the hex representation and applies inversion and scaling,
         * if appropriate.
         * @param stop {String} the hex representation of a color.
         * @param hexIndex {Number} the index of the color in the hex representation.
         * @param scale {Number} a scaling factor for the color.
         */
        _processColor : function ( stop, hexIndex, scale ){
            var start = hexIndex;
            var end = hexIndex + 2;
            if ( stop.substring(0,1) === "0"){
                start = 1;
            }
            var colorHex = stop.substring( start, end );
            var rgbColor = parseInt( colorHex, 16 );
            if ( this.m_invert ){
                rgbColor = 255 - rgbColor;
            }
            rgbColor = Math.round(rgbColor * scale);
            var processedHex = rgbColor.toString( 16 );
            if ( processedHex.length == 1 ){
                processedHex = "0"+processedHex;
            }
            return processedHex;
        },
        
        /**
         * Requests information from the server about the colors comprising the 
         *      current color gradient.
         */
        _sendUpdateStopsCmd : function(){
            if ( this.m_connector !== null && this.m_name !== null ){
                var params = "name:"+this.m_name;
                var path = skel.widgets.Path.getInstance();
                var cmd = path.COLORMAPS + path.SEP_COMMAND + "getColorStops";
                this.m_connector.sendCommand( cmd, params, this._updateColorStops(this));
            }
        },
        
        /**
         * Sets the name of the color map to display in the gradient.
         * @param colorMapName {String} the name of a colormap.
         */
        setColorName : function( colorMapName ){
            if ( this.m_name !== colorMapName ){
                this.m_name = colorMapName;
                this._sendUpdateStopsCmd();
            }
        },
        
        /**
         * Set whether or not to display both the color gradient and color mix or just
         * the gradient.
         * @param gradientOnly {bool} true to draw only the gradient; false othewise.
         */
        setGradientOnly : function( gradientOnly ){
            if ( this.m_gradientOnly !== gradientOnly ){
                this.m_gradientOnly = gradientOnly;
                this.update();
            }
        },
        
        /**
         * Set whether or not to invert the color map.
         * @param invertMap {boolean} true to invert the map; false, otherwise.
         */
        setInvert : function( invertMap ){
           if ( invertMap != this.m_invert ){
               this.m_invert = invertMap;
               this.update();
           }
        },
        
        /**
         * Set whether or not to reverse the color map.
         * @param reverseMap {boolean} true to reverse the colors in the map; false, otherwise.
         */
        setReverse : function( reverseMap ){
            if ( reverseMap != this.m_reverse ){
                this.m_reverse = reverseMap;
                this.update();
            }
        },
        
        /**
         * Set scale values for the colors in the map.
         * @param redValue {Number} the amount of red scaling, range [0,1].
         * @param greenValue {Number} the amount of green scaling, range [0,1].
         * @param blueValue {Number} the amount of blue scaling, range [0,1].
         */
        setScales : function( redValue, greenValue, blueValue ){
            var scalesChanged = false;
            if ( this.m_scaleRed != redValue ){
                this.m_scaleRed = redValue;
                scalesChanged = true;
            }
            if ( this.m_scaleGreen != greenValue ){
                this.m_scaleGreen = greenValue;
                scalesChanged = true;
            }
            if ( this.m_scaleBlue != blueValue ){
                this.m_scaleBlue = blueValue;
                scalesChanged = true;
            }
            if ( scalesChanged ){
                this.update();
            }
        },
        
        /**
         * Returns a callback for updating the display with new color stops.
         * @param anObject {skel.widgets.ColorMap.ColorGradient}.
         * @return {Function} the callback for updating the gradient color stops.
         */
        _updateColorStops : function( anObject ){
            return function( colorStops ){
                if ( colorStops !==null && colorStops != anObject.m_stops ){
                    anObject.m_stops = colorStops.split(",");
                    anObject.update();
                }
            };
        },
        
        m_connector : null,
        m_gradientOnly : false,
        m_scaleRed : 1,
        m_scaleBlue : 1,
        m_scaleGreen : 1,
        m_name : null,
        m_invert : null,
        m_reverse : null,
        m_stops : null
    }
});