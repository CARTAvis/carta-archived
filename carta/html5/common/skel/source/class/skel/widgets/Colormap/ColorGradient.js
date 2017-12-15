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
            var HLine_y_coordinate = height/2;
            ctx.fillRect( 0, 0, width, HLine_y_coordinate );

            // start to draw labels
            ctx.beginPath();
            // draw horizontal lines
            ctx.moveTo(0, 0);
            ctx.lineTo(width, 0);
            ctx.moveTo(0, HLine_y_coordinate);
            ctx.lineTo(width, HLine_y_coordinate);
            // draw vertical lines
            var labels = 10;
            var label_height = height/10;
            for (var j = 0; j < labels+1; j++) {
                ctx.moveTo(width*j/labels, 0);
                ctx.lineTo(width*j/labels, label_height);
                ctx.moveTo(width*j/labels, HLine_y_coordinate - label_height);
                ctx.lineTo(width*j/labels, HLine_y_coordinate);
            }
            // draw tilted lines to indicate Min/Max intensity labels
            ctx.moveTo(0, HLine_y_coordinate);
            ctx.lineTo(width/labels/2, HLine_y_coordinate + label_height*2.5);
            ctx.moveTo(width, HLine_y_coordinate);
            ctx.lineTo(width - width/labels/2, HLine_y_coordinate + label_height*2.5);

            // finish drawing canvas and lines
            ctx.stroke();

            // label the grades of colormap with texts
            var gradeLength = this.m_colorGrades.length;
            if (gradeLength > 0) {
                var text_width = width/10;
                var text_y_coordinate = HLine_y_coordinate + label_height*2;
                var text_y_coordinate2 = HLine_y_coordinate + label_height*4;
                ctx.font="normal 20px Arial";
                for (var k = 0; k < gradeLength; k++) {
                    if (k === 0) {
                        ctx.textAlign = "left";
                        ctx.strokeText(this.m_colorGrades[k], width*k/gradeLength, text_y_coordinate2, text_width);
                    } else if ((k+1) === gradeLength) {
                        ctx.textAlign = "right";
                        ctx.strokeText(this.m_colorGrades[k], width*k/gradeLength, text_y_coordinate2, text_width);
                    } else if (k%20 === 0) {
                        ctx.textAlign = "center";
                        ctx.strokeText(this.m_colorGrades[k], width*k/gradeLength, text_y_coordinate, text_width);
                    }
                }
            }
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

        /**
         * Returns a callback for updating the display with new color grades.
         * @param anObject {skel.widgets.ColorMap.ColorGradient}.
         * @return {Function} the callback for updating the gradient color grades.
         */
        setColorGrades : function( colorGrades ){
            if ( colorGrades !== null  ){
                this.m_colorGrades = colorGrades.split(",");
                this.update();
            }
        },

        m_scaleRed : 1,
        m_scaleBlue : 1,
        m_scaleGreen : 1,
        m_name : null,
        m_invert : null,
        m_reverse : null,
        m_stops : null,
        m_colorGrades : null
    }
});
