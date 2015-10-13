/**
 * A passive canvas drawing a a black square with diagonal white lines to represent
 * a grid.
 */

qx.Class.define("skel.widgets.Image.Grid.Settings.DiagonalLineCanvas", {
    extend: qx.ui.embed.Canvas,
    
    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this._init( );
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
            //Background
            ctx.rect( 0, 0, width, height );
            ctx.fillStyle="black";
            ctx.fill();
            
            //Diagonal lines
            this._drawDiagonals( width, height, ctx );
        },
    
        /**
         * Draws diagonal lines on the canvas.
         * @param width {Integer} - canvas width.
         * @param height {Integer} - canvas height.
         * @param ctx {CanvasRenderingContext2D} - canvas rendering context.
         */
        _drawDiagonals : function( width, height, ctx ){
            ctx.strokeStyle="white";
            ctx.lineWidth = 2;
            var increment = 10;
            var slope = 0.8;
            var yIntercept = 5;
            var yInterceptNeg = 45;
            var maxI = this.X_MAX / increment;
            for ( var i = 0; i < maxI; i++ ){
                //First positive slope line
                var y1 = slope * this.X_MIN + yIntercept +( i*increment);
                var y2 = slope * this.X_MAX + yIntercept +( i*increment);
                this._drawLine( ctx, this.X_MIN, y1, this.X_MAX, y2 );
                
                //First negative slope line
                y1 = -1 * slope * this.X_MIN + yInterceptNeg + (i * increment );
                y2 = -1 * slope * this.X_MAX + yInterceptNeg + (i * increment );
                this._drawLine( ctx, this.X_MIN, y1, this.X_MAX, y2 );
                
                if ( i > 0 ){
                    //Second positive slope line
                    y1 = 0;
                    y2 = slope * this.X_MAX + yIntercept -( i*increment);
                    var xStart = ( i*increment - yIntercept ) / slope;
                    this._drawLine( ctx, xStart, y1, this.X_MAX, y2 );
                    
                    //Second negative slope line
                    y1 = -1 * slope * this.X_MIN + yInterceptNeg - ( i * increment );
                    y2 = 0;
                    xStart = ( i * increment - yInterceptNeg ) / (-1 * slope);
                    this._drawLine( ctx, this.X_MIN, y1, xStart, y2 );
                }
                
                
            }
        },
        
        /**
         * Draws a line from (x1,y1) to (x2,y2).
         * @param ctx {CanvasRenderingContext2D} - the canvas rendering context.
         * @param x1 {Number} - the x-coordinate of the line's first end point.
         * @param y1 {Number} - the y-coordinate of the line's first end point.
         * @param x2 {Number} - the x-coordinate of the line's second end point.
         * @param y2 {Number} - the y-coordinate of the line's second end point.
         */
        _drawLine : function( ctx, x1, y1, x2, y2 ){
            ctx.beginPath();
            ctx.moveTo( x1, y1 );
            ctx.lineTo( x2, y2 );
            ctx.stroke();
        },
        
        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            this.setAllowGrowX( false );
            this.setAllowGrowY( false );
            this.setCanvasWidth( this.X_MAX );
            this.setCanvasHeight(this.Y_MAX );
        },

        X_MIN : 0,
        X_MAX : 50,
        Y_MIN : 0,
        Y_MAX : 50
    }
});