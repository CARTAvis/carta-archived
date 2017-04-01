/**
 * Displays a color map as a gradient.
 */


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
            if ( this.m_stops[stopIndex].indexOf("#") >= 0 ){
                gradient.addColorStop( floatStop, this.m_stops[stopIndex] );
            }
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
                    this._addStop( grd, i, i );
                }
            }
            ctx.fillStyle = grd;
            ctx.fillRect( 0, 0, width, height );
        },
        
        
        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            this.setAllowGrowX( true );
            this.setAllowGrowY( true );
            this.setMinWidth( 100 );
            this.setMinHeight( 25 );
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
         * Returns a callback for updating the display with new color stops.
         * @param anObject {skel.widgets.ColorMap.ColorGradient}.
         * @return {Function} the callback for updating the gradient color stops.
         */
        setStops : function( colorStops ){
            if ( colorStops !== null  ){
                this.m_stops = colorStops.split(",");
                this.update();
            }
        },
        
        m_scaleRed : 1,
        m_scaleBlue : 1,
        m_scaleGreen : 1,
        m_name : null,
        m_invert : null,
        m_reverse : null,
        m_stops : null
    }
});
