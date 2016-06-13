/**
 * Draws a pixel box on the zoom view.
 */

qx.Class.define("skel.widgets.Image.Zoom.ZoomCanvas",{
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
                    var lineLength = Math.min( width, height ) * .25;
                    var vertX = centerX;
                    var vertY = centerY - lineLength;
                    var hortX = centerX + lineLength;
                    var hortY = centerY;
                    this._drawBox( ctx );
                }
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
             * Set the color for the pixel box.
             * @param red {Number} - the amount of red, [0,255].
             * @param green {Number} - the amount of green, [0,255].
             * @param blue {Number} - the amount of blue, [0,255].
             */
            setBoxColor : function( red, green, blue ){
                this.m_boxColor = this._toHex( red, green, blue );
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
                this.setShowBox( controls.boxVisible );
                this.update();
            },
            
            
            /**
             * Store the location of the box representing the main image.
             * @param x0 {Number} - the x-coordinate of one box corner.
             * @param y0 {Number} - the y-coordinate of one box corner.
             * @param x1 {Number} - the x-coordinate of the opposite box corner.
             * @param y1 {Number} - the y-coordinate of the opposite box corner.
             */
            setImageCorners : function( x0, y0, x1, y1 ){
                this.m_corner0X = x0;
                this.m_corner0Y = y0;
                this.m_corner1X = x1;
                this.m_corner1Y = y1;
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
           
            m_showBox : true,
            m_boxLineWidth : 2
        }
    });