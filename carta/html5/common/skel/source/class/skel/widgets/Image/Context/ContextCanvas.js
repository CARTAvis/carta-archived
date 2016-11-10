/**
 * Draws directional arrows on the context image, as well as a rectangle indicating
 * the location of the current image view.
 */

qx.Class.define("skel.widgets.Image.Context.ContextCanvas",{
        extend : qx.ui.embed.Canvas,
        
        /**
         * Constructor.
         */
        construct : function() {
            this.base(arguments);
            this.setSyncDimension(true);
        },

        members : {

            /**
             * Template method, which can be used by derived classes
             * to redraw the content. It is called each time the
             * canvas dimension change and the canvas needs to be
             * updated.
             * 
             * @param width {Integer} New canvas width
             * @param height {Integer} New canvas height
             * @param ctx {CanvasRenderingContext2D} - the
             *                rendering ctx to draw to
             */
            _draw : function(width, height, ctx) {
                if ( this.isVisible() ){
                    this.base(arguments);
                    ctx.clearRect( 0, 0, width, height );
                    var loc = this.getContentLocation();
                    var left = 0;
                    var top = 0;
                    var centerX = left + width / 2;
                    var centerY = top + height / 2;
                    var lineLength = Math.min( this.m_imageWidth, this.m_imageHeight ) * .25;
                    if ( lineLength > 0 ){
                        var vertX = centerX;
                        var vertY = centerY - lineLength;
                        var hortX = centerX + lineLength;
                        var hortY = centerY;
                        this._drawXYCompass( ctx, centerX, centerY, vertX, vertY, hortX, hortY );
                        this._drawNECompass( ctx, centerX, centerY, vertX, vertY, centerX - lineLength, hortY );
                        this._drawBox( ctx );
                    }
                }
            },
           
            
            /**
             * Draw an arrow toward the positive x-axis.
             * @param baseX {Number} - the starting x-coordinate for the arrow.
             * @param baseY {Number} - the starting y-coordinate for the arrow.
             * @param ctx {CanvasRenderingContext2D} - the
             *                rendering ctx to draw to
             */
            _drawArrowX : function( baseX, baseY, ctx ){
                ctx.beginPath();
                var startY = baseY - this.m_ARROW_SIDE / 2;
                ctx.moveTo( baseX, startY );
                var secondY = baseY + this.m_ARROW_SIDE / 2;
                ctx.lineTo( baseX, secondY );
                var width = Math.sqrt(3) * this.m_ARROW_SIDE / 2;
                ctx.lineTo( baseX+ width, baseY );
                ctx.closePath();
                ctx.fill();
            },
            
            /**
             * Draw an east direction arrow.
             * @param baseX {Number} - the starting x-coordinate for the arrow.
             * @param baseY {Number} - the starting y-coordinate for the arrow.
             * @param ctx {CanvasRenderingContext2D} - the
             *                rendering ctx to draw to
             */
            _drawArrowE : function( baseX, baseY, ctx ){
                ctx.beginPath();
                var rotateY = this._rotateY( 0, -1 * this.m_ARROW_SIDE / 2 );
                var rotateX = this._rotateX( 0, -1 * this.m_ARROW_SIDE / 2 );
                var startY = baseY + rotateY;
                var startX = baseX + rotateX;
                ctx.moveTo( startX, startY );
                var rotateY2 = this._rotateY( 0, this.m_ARROW_SIDE / 2 );
                var rotateX2 = this._rotateX( 0, this.m_ARROW_SIDE / 2 );
                var secondY = baseY + rotateY2;
                var secondX = baseX + rotateX2;
                ctx.lineTo( secondX, secondY );
                var width = -1 * Math.sqrt(3) * this.m_ARROW_SIDE / 2;
                var rotateY3 = this._rotateY( width, 0 );
                var rotateX3 = this._rotateX( width, 0 );
                var thirdY = baseY + rotateY3;
                var thirdX = baseX + rotateX3;
                ctx.lineTo( thirdX, thirdY );
                ctx.closePath();
                ctx.fill();
            },
            
            
            /**
             * Draw an arrow in the positive y-direction.
             * @param baseX {Number} - the starting x-coordinate for the arrow.
             * @param baseY {Number} - the starting y-coordinate for the arrow.
             * @param ctx {CanvasRenderingContext2D} - the
             *                rendering ctx to draw to
             */
            _drawArrowY: function( baseX, baseY, ctx ){
                ctx.beginPath()
                var startX = baseX - this.m_ARROW_SIDE / 2;
                ctx.moveTo( startX, baseY );
                var secondX = baseX + this.m_ARROW_SIDE / 2;
                ctx.lineTo( secondX, baseY );
                var height = Math.sqrt(3) * this.m_ARROW_SIDE / 2;
                ctx.lineTo( baseX, baseY - height );
                ctx.closePath();
                ctx.fill();
            },
            
            /**
             * Draw a north pointing arrow.
             * @param baseX {Number} - the starting x-coordinate for the arrow.
             * @param baseY {Number} - the starting y-coordinate for the arrow.
             * @param ctx {CanvasRenderingContext2D} - the
             *                rendering ctx to draw to
             */
            _drawArrowN: function( baseX, baseY, ctx ){
                ctx.beginPath()
                var rotateX = this._rotateX( -this.m_ARROW_SIDE/ 2, 0 );
                var rotateY = this._rotateY( -this.m_ARROW_SIDE/ 2, 0 );
                ctx.moveTo( baseX + rotateX, baseY + rotateY );
                
                var rotateX2 = this._rotateX( this.m_ARROW_SIDE/2, 0 );
                var rotateY2 = this._rotateY( this.m_ARROW_SIDE/2, 0 );
                ctx.lineTo( baseX + rotateX2, baseY + rotateY2 );
                
                var height = -1 * Math.sqrt(3) * this.m_ARROW_SIDE / 2;
                var rotateX3 = this._rotateX( 0, height );
                var rotateY3 = this._rotateY( 0, height );
                ctx.lineTo( baseX + rotateX3, baseY + rotateY3 );
                ctx.closePath();
                ctx.fill();
            },
            
            /**
             * Draw a box representing the location of the main image
             * view.
             * @param ctx {CanvasRenderingContext2D} - the
             *                rendering ctx to draw to
             */
            _drawBox : function( ctx ){
                if ( this.m_showBox ){
                    if ( this.m_corner0X != this.m_corner1X ||
                            this.m_corner0Y != this.m_corner1Y ){
                        if ( this.m_boxColor !== null ){
                            ctx.strokeStyle = this.m_boxColor;
                        }
                        ctx.lineWidth = this.m_boxLineWidth;
                        if ( this.m_selected ){
                            ctx.lineWidth = this.m_boxLineWidth + 2;
                        }
                        ctx.beginPath();
                        ctx.moveTo( this.m_corner0X, this.m_corner0Y );
                        ctx.lineTo( this.m_corner1X, this.m_corner0Y );
                        ctx.lineTo( this.m_corner1X, this.m_corner1Y );
                        ctx.lineTo( this.m_corner0X, this.m_corner1Y );
                        ctx.lineTo( this.m_corner0X, this.m_corner0Y );
                        ctx.stroke();
                        ctx.restore();
                    }
                }
            },
           
            /**
             * Draw a line segment.
             * @param startX {Number} - the x-coordinate of the segment start.
             * @param startY {Number} - the y-coordinate of the segment start.
             * @param endX {Number} - the x-coordinate of the segment end.
             * @param endY {Number} - the y-coordinate of the segment end.
             * @param ctx {CanvasRenderingContext2D} - the
             *                rendering ctx to draw to
             */
            _drawLine : function(startX, startY, endX, endY, ctx) {
                ctx.beginPath();
                ctx.moveTo(startX, startY);
                ctx.lineTo(endX, endY);
                ctx.stroke();
            },
            
            /**
             * Draw a compass showing the north and east on the canvas.
             * @param ctx {CanvasRenderingContext2D} - the rendering ctx to draw to
             * @param centerX {Number} - the x-pixel coordinate of the compass center.
             * @param centerY {Number} - the y-pixel coordinate of the compass center.
             * @param vertX {Number} - the x-pixel coordinate of an end of a compass line.
             * @param vertY {Number} - the y-pixel coordinate of an end of a compass line.
             * @param hortX {Number} - the x-pixel coordinate of an end of a compass line.
             * @param hortY {Number} - the y-pixel coordinate of an end of a compass line.
             */
            _drawNECompass : function( ctx, centerX, centerY, vertX, vertY, hortX, hortY){
                if ( this.m_showCompassNE ){
                    var SHORT_FACTOR = 0.5;
                    var transVertX = SHORT_FACTOR * (vertX - centerX);
                    var transVertY = SHORT_FACTOR * (vertY - centerY);
                    var transHortX = SHORT_FACTOR * (hortX - centerX);
                    var transHortY = SHORT_FACTOR * (hortY - centerY);
                 
                    var vertXRotate = this._rotateX( transVertX, transVertY ) + centerX;
                    var vertYRotate = this._rotateY( transVertX, transVertY ) + centerY;
                    var hortXRotate = this._rotateX( transHortX, transHortY ) + centerX;
                    var hortYRotate = this._rotateY( transHortX, transHortY ) + centerY;
                    ctx.lineWidth = this.m_compassNELineWidth;
                    ctx.strokeStyle = this.m_compassNEColor;
                    ctx.fillStyle = this.m_compassNEColor;
                    
                    this._drawLine( centerX, centerY, vertXRotate, vertYRotate, ctx );
                    this._drawLine( centerX, centerY, hortXRotate, hortYRotate, ctx );
                   
                    this._drawArrowE( hortXRotate, hortYRotate, ctx );
                    this._drawArrowN(vertXRotate, vertYRotate, ctx );
                    ctx.lineWidth = this.m_textLineWidth;
                    this._drawTextE( hortXRotate, hortYRotate, "E", ctx );
                    this._drawTextN(vertXRotate, vertYRotate, "N", ctx );
                    
                    ctx.restore();
                }
            },
            
            /**
             * Draw text to the right of a specified location.
             * @param baseX {Number} - the starting x-coordinate for the text.
             * @param baseY {Number} - the starting y-coordinate for the text.
             * @param text {String} - the text to draw.
             * @param ctx {CanvasRenderingContext2D} - the
             *                rendering ctx to draw to
             */
            _drawTextX : function( baseX, baseY, text, ctx ){
                ctx.textBaseline = "middle";
                ctx.strokeText( text, baseX + this.m_ARROW_SIDE, baseY );
            },
            
            /**
             * Draw text to the east (rotated) of a specified location.
             * @param baseX {Number} - the starting x-coordinate for the text.
             * @param baseY {Number} - the starting y-coordinate for the text.
             * @param text {String} - the text to draw.
             * @param ctx {CanvasRenderingContext2D} - the
             *                rendering ctx to draw to
             */
            _drawTextE : function( baseX, baseY, text, ctx ){
                ctx.textBaseline = "middle";
                var rotateX = this._rotateX( -2 * this.m_ARROW_SIDE, 0 );
                var rotateY = this._rotateY( -2 * this.m_ARROW_SIDE, 0 );
                ctx.strokeText( text, baseX + rotateX, baseY + rotateY );
            },
            
            /**
             * Draw text to the north (rotated) of a specified location.
             * @param baseX {Number} - the starting x-coordinate for the text.
             * @param baseY {Number} - the starting y-coordinate for the text.
             * @param text {String} - the text to draw.
             * @param ctx {CanvasRenderingContext2D} - the
             *                rendering ctx to draw to
             */
            _drawTextN: function( baseX, baseY, text, ctx ){
                var metrics = ctx.measureText( text );
                var width = metrics.width;
                var rotateX = this._rotateX( -1 * width/2, -1.5*this.m_ARROW_SIDE );
                var rotateY = this._rotateY( -1 * width/2, -1.5*this.m_ARROW_SIDE );
                ctx.strokeText( text, baseX  + rotateX, baseY + rotateY );
            },
            
            
            /**
             * Draw text above the specified location.
             * @param baseX {Number} - the starting x-coordinate for the text.
             * @param baseY {Number} - the starting y-coordinate for the text.
             * @param text {String} - the text to draw.
             * @param ctx {CanvasRenderingContext2D} - the
             *                rendering ctx to draw to
             */
            _drawTextY: function( baseX, baseY, text, ctx ){
                var metrics = ctx.measureText( text );
                var width = metrics.width;
                ctx.strokeText( text, baseX - width/2, baseY - 1.5*this.m_ARROW_SIDE );
            },
            
            
            /**
             * Draw a compass showing the direction of the coordinate axes on the canvas.
             * @param ctx {CanvasRenderingContext2D} - the rendering ctx to draw to
             * @param centerX {Number} - the x-pixel coordinate of the compass center.
             * @param centerY {Number} - the y-pixel coordinate of the compass center.
             * @param vertX {Number} - the x-pixel coordinate of an end of a compass line.
             * @param vertY {Number} - the y-pixel coordinate of an end of a compass line.
             * @param hortX {Number} - the x-pixel coordinate of an end of a compass line.
             * @param hortY {Number} - the y-pixel coordinate of an end of a compass line.
             */
            _drawXYCompass : function( ctx, centerX, centerY, vertX, vertY, hortX, hortY){
                if ( this.m_showCompassXY ){
                    ctx.lineWidth = this.m_compassXYLineWidth;
                    ctx.strokeStyle = this.m_compassXYColor;
                    ctx.fillStyle = this.m_compassXYColor;
                    
                    this._drawLine( centerX, centerY, vertX, vertY, ctx );
                    this._drawLine( centerX, centerY, hortX, hortY, ctx );
                   
                    this._drawArrowX( hortX, hortY, ctx );
                    this._drawArrowY(vertX, vertY, ctx );
                    ctx.lineWidth = this.m_textLineWidth;
                    this._drawTextX( hortX, hortY, "X", ctx );
                    this._drawTextY(vertX, vertY, "Y", ctx );
                    
                    ctx.restore();
                }
            },
            
            /**
             * Calculate a new x-coordinate based on the rotation.
             * @param x {Number} - the non-rotated x-coordinate.
             * @param y {Number} - the non-rotated y-coordinate.
             */
            _rotateX : function(  x, y ){
                var cosAngle = Math.cos( this.m_rotate );
                var sinAngle = Math.sin( this.m_rotate );
                var rotateX = x * cosAngle + y * sinAngle;
                return rotateX
            },
            
            /**
             * Calculate a new y-coordinate based on the rotation.
             * @param x {Number} - the non-rotated x-coordinate.
             * @param y {Number} - the non-rotated y-coordinate.
             */
            _rotateY : function( x, y ){
                var cosAngle = Math.cos( this.m_rotate );
                var sinAngle = Math.sin( this.m_rotate );
                var rotateY = -1 * x * sinAngle + y * cosAngle;
                return rotateY;
            },
            
            
            
            /**
             * Set the color for the image box.
             * @param red {Number} - the amount of red, [0,255].
             * @param green {Number} - the amount of green, [0,255].
             * @param blue {Number} - the amount of blue, [0,255].
             */
            setBoxColor : function( red, green, blue ){
                this.m_boxColor = this._toHex( red, green, blue );
            },
            
            /**
             * Set the color for the coordinate axis compass.
             * @param red {Number} - the amount of red, [0,255].
             * @param green {Number} - the amount of green, [0,255].
             * @param blue {Number} - the amount of blue, [0,255].
             */
            setCompassColorXY : function( red, green, blue ){
                this.m_compassXYColor = this._toHex( red, green, blue );
            },
            
            
            /**
             * Set the color for the direction (North/East) compass.
             * @param red {Number} - the amount of red, [0,255].
             * @param green {Number} - the amount of green, [0,255].
             * @param blue {Number} - the amount of blue, [0,255].
             */
            setCompassColorNE : function( red, green, blue ){
                this.m_compassNEColor = this._toHex( red, green, blue );
            },
            
            
            /**
             * Set UI values based on server-side values.
             * @param controls {Object} - server side state of the draw controls.
             */
            setControls : function( controls ){
                if ( controls.box != "undefined" ){
                    this.setBoxColor( controls.box.red, controls.box.green, controls.box.blue );
                    this.m_boxLineWidth = controls.box.width;
                }
                if ( controls.compassXY != "undefined" ){
                    this.setCompassColorXY( controls.compassXY.red, controls.compassXY.green, controls.compassXY.blue );
                    this.m_compassXYLineWidth = controls.compassXY.width;
                }
                if ( controls.compassNE != "undefined" ){
                    this.setCompassColorNE( controls.compassNE.red, controls.compassNE.green, controls.compassNE.blue );
                    this.m_compassNELineWidth = controls.compassNE.width;
                }
                this.setShowBox( controls.boxVisible );
                this.setShowCompassXY( controls.compassVisibleXY );
                this.setShowCompassNE( controls.compassVisibleNE );
                this.update();
            },
            
            
            /**
             * Store the location of the box representing the main image.
             * @param x0 {Number} - the x-coordinate of one box corner.
             * @param y0 {Number} - the y-coordinate of one box corner.
             * @param x1 {Number} - the x-coordinate of the opposite box corner.
             * @param y1 {Number} - the y-coordinate of the opposite box corner.
             * @param selected {boolean} - true if the box is selected; false otherwise.
             */
            setImageCorners : function( x0, y0, x1, y1, selected, width, height, rotate ){
                this.m_corner0X = x0;
                this.m_corner0Y = y0;
                this.m_corner1X = x1;
                this.m_corner1Y = y1;
                this.m_imageWidth = width;
                this.m_imageHeight = height;
                this.m_selected = selected;
                this.m_rotate = rotate;
                this.update();
            },
            
            /**
             * Set whether or not the image box should be drawn.
             * @param show {boolean} - true if the image box should be drawn; 
             *          false otherwise.
             */
            setShowBox : function( show ){
                this.m_showBox = show;
            },
            
            /**
             * Set whether or not the north east compass should be drawn.
             * @param show {boolean} - true if the NE compass should be drawn; 
             *          false otherwise.
             */
            setShowCompassNE : function( show ){
                this.m_showCompassNE = show;
            },
            
            /**
             * Set whether or not the xy compass should be drawn.
             * @param show {boolean} - true if the xy compass should be drawn; 
             *          false otherwise.
             */
            setShowCompassXY : function( show ){
                this.m_showCompassXY = show;
            },
            
            /**
             * Helper function to convert an RGB color value to a hexidecimal
             * representation.
             * @param red {Number} - the amount of red [0,255].
             * @param green {Number} - the amount of green [0,255].
             * @param blue {Number} - the amount of blue [0,255].
             * @return {String} - the hex representation of the color.
             */
            _toHex : function( red, green, blue ){
                var rgbArray = [red,green,blue];
                var hexStr = qx.util.ColorUtil.rgbToHexString(rgbArray );
                return hexStr;
            },
            
            m_boxColor : null,
            m_compassXYColor : null,
            m_compassNEColor : null,
            
            m_corner0X : 0,
            m_corner0Y : 0,
            m_corner1X : 0,
            m_corner1Y : 0,
            
            m_imageWidth : 0,
            m_imageHeight : 0,
           
            m_ARROW_SIDE : 8,
            
            m_rotate : 0,
            m_selected : false,
            
            m_showBox : true,
            m_showCompassXY : true,
            m_showCompassNE : true,
            
            m_boxLineWidth : 2,
            m_compassXYLineWidth : 2,
            m_compassNELineWidth : 2,
            m_textLineWidth : 1
        }
    });