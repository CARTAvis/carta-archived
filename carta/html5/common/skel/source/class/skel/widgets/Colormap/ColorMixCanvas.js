/**
 * Displays lines showing the amount of red, blue and green in the color map.
 */

qx.Class.define("skel.widgets.Colormap.ColorMixCanvas", {
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
            this._drawColorGraph( width, height, ctx );
        },

        
        /**
         * Draws a colored line indicating the amount of that color in the map.
         * @param width {Integer} New canvas width
         * @param height {Integer} New canvas height
         * @param ctx {CanvasRenderingContext2D} The rendering ctx to draw to.
         * @param colorStr {String} a string naming a color.
         * @param hexIndex {Number} the location in the hex representation where the color can be found.
         */
        _drawColor : function( width, height, ctx, colorStr, hexIndex ){
            if ( this.m_stops !== null ){
                ctx.beginPath();
                var drawHeight = height;
                var drawStart = 0;
               
                for ( var i = 0; i < this.m_stops.length; i++ ){
                    var colorIndex = i;
                    var hexColor = this.m_stops[colorIndex].substring( hexIndex, hexIndex + 2);
                    var intColor = parseInt( hexColor, 16 );
                    var percentColor = intColor / 255;
                    var lineY = drawHeight - drawHeight * percentColor;
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
            this._drawColor( width, height, ctx, "red", 1 );
            this._drawColor( width, height, ctx, "green", 3 );
            this._drawColor( width, height, ctx, "blue", 5 );
        },
        
        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            this.setAllowGrowX( true );
            this.setAllowGrowY( true );
            this.setMinWidth( 100 );
            this.setHeight( 25 );
        },
        
        
        /**
         * Sets the name of the color map to display in the gradient.
         * @param colorMapName {String} the name of a colormap.
         */
        setColorName : function( colorMapName ){
            if ( this.m_name !== colorMapName ){
                this.m_name = colorMapName;
            }
        },
        
        
        /**
         * Returns a callback for updating the display with new color stops.
         * @param anObject {skel.widgets.ColorMap.ColorGradient}.
         * @return {Function} the callback for updating the gradient color stops.
         */
        setStops : function( colorStops ){
            if ( colorStops !==null ){
                this.m_stops = colorStops.split(",");
                this.update();
            }
        },

        m_name : null,
        m_stops : null
    }
});