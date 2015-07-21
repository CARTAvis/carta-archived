/**
 * A rectangular shape.
 */

/* global qx, skel, console */

qx.Class.define("skel.widgets.Draw.Rectangle", {

    extend : skel.widgets.Draw.Shape,

    construct : function( winId ){
        this.base(arguments, winId, "RegionRectangle" );
        this.m_topLeft = {
            x : 0,
            y : 0
        };
        this.m_bottomRight = {
            x : 0,
            y : 0
        };
    },
    
    members : {
        /**
         * Updates the top-left and bottom-right corners of this rectangle.
         * @param topLeftX {Number} the x-coordinate of the top left corner.
         * @param topLeftY {Number} the y-coordinate of the top left corner.
         * @param bottomRightX {Number} the x-coordinate of the bottom right corner.
         * @param bottomRightY {Number} the y-coordinate of the bottom right corner.
         */
        _assignCoords : function( topLeftX, topLeftY, bottomRightX, bottomRightY ){
            var changed = false;
            if ( this.m_topLeft.x != topLeftX ){
                this.m_topLeft.x = topLeftX;
                changed = true;
            }
            if ( this.m_topLeft.y != topLeftY ){
                this.m_topLeft.y = topLeftY;
                changed = true;
            }
            if ( this.m_bottomRight.x != bottomRightX ){
                this.m_bottomRight.x = bottomRightX;
                changed = true;
            }
            if ( this.m_bottomRight.y != bottomRightY ){
                this.m_bottomRight.y = bottomRightY;
                changed = true;
            }
            return changed;
        },
        
        /**
         * Returns information describing this rectangle.
         */
        getShapeInfo: function () {
            return this.m_topLeft.x+" "+this.m_topLeft.y+
            " "+this.m_bottomRight.x+" "+this.m_bottomRight.y;
        },
        
        /**
         * Returns the width of this rectangle.
         */
        _getWidth : function(){
            return Math.abs( this.m_topLeft.x - this.m_bottomRight.x );
        },
        
        /**
         * Returns the height of this rectangle.
         */
        _getHeight : function(){
            return Math.abs( this.m_topLeft.y - this.m_bottomRight.y );
        },
        
        /**
         * Adjust one of the sides of the rectangle as appropriate.
         * @param mouseMovePt {Object} the amount the side should move in x-,y-coordinates.
         */
        doResize : function( mouseMovePt){
            var c = qx.lang.Array.contains;
            var resized = false;
            if( c( [skel.widgets.Draw.Shape.CURSOR_RESIZE_E,
                    skel.widgets.Draw.Shape.CURSOR_RESIZE_NE,
                    skel.widgets.Draw.Shape.CURSOR_RESIZE_SE], this.m_hoverInfo)) {
                this.resizeEast( mouseMovePt );
                resized = true;
            }
            if( c( [skel.widgets.Draw.Shape.CURSOR_RESIZE_W,
                    skel.widgets.Draw.Shape.CURSOR_RESIZE_NW,
                    skel.widgets.Draw.Shape.CURSOR_RESIZE_SW], this.m_hoverInfo)) {
                this.resizeWest( mouseMovePt );
                resized = true;
            }
            if( c( [skel.widgets.Draw.Shape.CURSOR_RESIZE_N,
                    skel.widgets.Draw.Shape.CURSOR_RESIZE_NE,
                    skel.widgets.Draw.Shape.CURSOR_RESIZE_NW], this.m_hoverInfo)) {
                this.resizeNorth( mouseMovePt );
                resized = true;
            }
            if( c( [skel.widgets.Draw.Shape.CURSOR_RESIZE_S,
                    skel.widgets.Draw.Shape.CURSOR_RESIZE_SE,
                    skel.widgets.Draw.Shape.CURSOR_RESIZE_SW], this.m_hoverInfo)) {
                this.resizeSouth( mouseMovePt );
                resized = true;
            }
            return resized;
        },
        
        /**
         * Draws this rectangle.
         * @param ctx {Object} the painting context.
         */
        draw : function( ctx ){
            ctx.fillStyle = this.m_fillStr;
            var width = this._getWidth();
            var height = this._getHeight();
            ctx.fillRect(this.m_topLeft.x, this.m_topLeft.y, width, height);

            ctx.strokeStyle = this.m_strokeStr;
            ctx.lineWidth = this.m_lineWidth;
            ctx.strokeRect(this.m_topLeft.x, this.m_topLeft.y, width, height);
        },
        
        /**
         * Draws this rectangle with decorators to reflect various states.
         * @param ctx {Object} the painting context.
         * @param active {boolean} whether or not this rectangle is selected.
         * @param hovered {boolean} whether or not the mouse is hovered over this rectangle.
         * @param resizing {boolean} whether or not the mouse is around the edge of this rectangle for resizing.
         * @param translator {skel.widgets.Draw.ImageMouseTranslator} used to translate image/mouse coordinates.
         */
        _drawDecorators : function( ctx, active, hovered, resizing, translator ){
            // don't draw invisible rectangles
            if( ! this.m_visible) return;
            var topLeftTrans = translator.serverImage2mouse( this.m_topLeft );
            var bottomRightTrans = translator.serverImage2mouse( this.m_bottomRight );
            var x1 = topLeftTrans.x;
            var x2 = bottomRightTrans.x;
            var y1 = topLeftTrans.y;
            var y2 = bottomRightTrans.y;
            var w = x2 - x1;
            var h = y2 - y1;

            // don't draw null rectangles
            if( w < 2 && h < 2) return;

            ctx.save();

            var fontHeight = Math.round(Math.min( w, h));
            if( fontHeight < 6) fontHeight = 6;
            if( fontHeight > 10) fontHeight = 10;

            // draw the caption at the bottom
            if ( this.m_label ){
                ctx.save();
//                    ctx.font = "6pt Arial";
                ctx.font = fontHeight.toString() + "pt Arial";
                var metrics = ctx.measureText(this.m_label);
                metrics.height = fontHeight;
                var margin = 4;
                var tx = x2 - metrics.width - margin;
                var ty = y2 + metrics.height + 2;
                var tw = metrics.width + margin * 2;
                var th = metrics.height + margin * 2;
                ctx.shadowColor = "black";
                ctx.shadowBlur = 3;
                ctx.fillStyle = "white";
                ctx.fillText(this.m_label, tx, ty);
                ctx.restore();
            }

            if ( hovered) {
                ctx.fillStyle = this.m_fillStr;
                ctx.fillRect(x1, y1, w, h);
            }

            if (active) {
                ctx.strokeStyle = "rgb(0,255,0)";
            }
            else {
                ctx.strokeStyle = "rgb(255,255,255)";
            }

            ctx.lineWidth = this.m_lineWidth;
            ctx.strokeRect(x1, y1, w, h);

            // exagerate corners if hovered
            var cornerSize = Math.min(w / 2 - 2, h / 2 - 2, 7);
            if (hovered && cornerSize > 3) {
                ctx.lineWidth = 4;
//                    ctx.strokeStyle = "red";
                ctx.beginPath();
                // top left corner
                ctx.moveTo(x1, y1 + cornerSize);
                ctx.lineTo(x1, y1);
                ctx.lineTo(x1 + cornerSize, y1);
                // top right corner
                ctx.moveTo(x2, y1 + cornerSize);
                ctx.lineTo(x2, y1);
                ctx.lineTo(x2 - cornerSize, y1);
                // bottom left corner
                ctx.moveTo(x1, y2 - cornerSize);
                ctx.lineTo(x1, y2);
                ctx.lineTo(x1 + cornerSize, y2);
                // bottom right corner
                ctx.moveTo(x2, y2 - cornerSize);
                ctx.lineTo(x2, y2);
                ctx.lineTo(x2 - cornerSize, y2);
                ctx.stroke();
            }
            ctx.restore();
        },
        
        /**
         * Move this rectangle.
         * @param mouseDownPt {Object} the (x,y) coordinate of the last mouse down.
         * @param growPt {Object} the (x,y) coordinate of the current location of the mouse.
         */
        doMove : function( mouseDownPt, growPt ){
            var moved = false;
            if ( mouseDownPt.x < growPt.x ){
                moved = this._assignCoords( mouseDownPt.x, mouseDownPt.y, growPt.x, growPt.y );
            }
            else if ( mouseDownPt.x == growPt.x ){
                if ( mouseDownPt.y <= growPt.y ){
                    moved = this._assignCoords( mouseDownPt.x, mouseDownPt.y, growPt.x, growPt.y );
                }
                else {
                    moved = this._assignCoords( growPt.x, growPt.y, mouseDownPt.x, mouseDownPt.y );
                }
            }
            else {
                moved = this._assignCoords( growPt.x, growPt.y, mouseDownPt.x, mouseDownPt.y );
            }
            
            return moved;
        },
        
        /**
         * Returns true if mx lies within the x-coordinate range of this rectangle.
         * @param mx {Number} an x-coordinate.
         * @param errorMargin {Number} an error margin.
         */
        isMouseInXRange : function( mx, errorMargin ){
            var inXRange = false;
            if ( mx > this.m_topLeft.x - errorMargin ){
                if ( mx < this.m_bottomRight.x + errorMargin ){
                    inXRange = true;
                }
            }
            return inXRange;
        },
        
        /**
         * Returns true if my lies within the y-coordinate range of this rectangle.
         * @param my {Number} an y-coordinate.
         * @param errorMargin {Number} an error margin.
         */
        isMouseInYRange : function( my, errorMargin ){
            var inYRange = false;
            if ( my > this.m_topLeft.y - errorMargin ){
                if ( my < this.m_bottomRight.y + errorMargin ){
                    inYRange = true;
                }
            }
            return inYRange;
        },
        
        /**
         * Returns true if the point is located near the x-location of this rectangle's left side.
         * @param mx {Number} the x-coordinate of the point.
         * @param margin {Number} an error margin for what "near" means.
         */
        _isMouseLeftSide : function( mx, margin ){
            var leftSide = false;
            if ( Math.abs( this.m_topLeft.x - mx ) < margin ){
                leftSide = true;
            }
            return leftSide;
        },
        
        /**
         * Returns true if the point is located near the x-location of this rectangle's right side.
         * @param mx {Number} the x-coordinate of the point.
         * @param margin {Number} an error margin for what "near" means.
         */
        _isMouseRightSide : function( mx, margin ){
            var rightSide = false;
            if ( Math.abs( this.m_bottomRight.x - mx) < margin ){
                rightSide = true;
            }
            return rightSide;
        },
        
        /**
         * Returns true if the point is located near the y-location of this rectangle's top side.
         * @param mx {Number} the y-coordinate of the point.
         * @param margin {Number} an error margin for what "near" means.
         */
        _isMouseTopSide : function( my, margin ){
            var topSide = false;
            if ( Math.abs( this.m_topLeft.y - my ) < margin ){
                topSide = true;
            }
            return topSide;
        },
        
        /**
         * Returns true if the point is located near the y-location of this rectangle's bottom side.
         * @param mx {Number} the y-coordinate of the point.
         * @param margin {Number} an error margin for what "near" means.
         */
        _isMouseBottomSide : function( my, margin ){
            var bottomSide = false;
            if ( Math.abs( this.m_bottomRight.y - my ) < margin ){
                bottomSide = true;
            }
            return bottomSide;
        },
        
        /**
         * Returns whether or not this rectangle is visible.
         */
        isVisible : function(){
            return this.m_visible;
        },
        
        /**
         * Move the right-side of this rectangle by a given amount.
         * @param mouseMovePt {Object} the (x,y) side move amount.
         */
        resizeEast : function( mouseMovePt ){
            this.m_bottomRight.x = Math.max( this.m_bottomRight.x + mouseMovePt.x, this.m_topLeft.x + 1);
        },
        
        /**
         * Move the left-side of this rectangle by a given amount.
         * @param mouseMovePt {Object} the (x,y) side move amount.
         */
        resizeWest : function( mouseMovePt ){
            this.m_topLeft.x = Math.min( this.m_topLeft.x + mouseMovePt.x, this.m_bottomRight.x - 1);
        },
        
        /**
         * Move the top-side of this rectangle by a given amount.
         * @param mouseMovePt {Object} the (x,y) side move amount.
         */
        resizeNorth : function( mouseMovePt ){
            this.m_topLeft.y = Math.min( this.m_topLeft.y + mouseMovePt.y, this.m_bottomRight.y - 1);
        },
        
        /**
         * Move the bottom-side of this rectangle by a given amount.
         * @param mouseMovePt {Object} the (x,y) side move amount.
         */
        resizeSouth : function( mouseMovePt ){
            this.m_bottomRight.y = Math.max( this.m_bottomRight.y + mouseMovePt.y, this.m_topLeft.y + 1);
        },
        
        /**
         * Set the initial point of this rectangle.
         * @param pt {Object} the (x,y) coordinate of a corner of this rectangle.
         */
        setInitialPt : function ( pt ){
            this._assignCoords( pt.x, pt.y, pt.x, pt.y );
        },
        

        
        /**
         * Copy the location of the new rectangle into this one.
         * @param newShape {skel.widgets.Draw.Rectangle} the new rectangle to copy.
         */
        updateShape : function( newShape ){
            var updated = false;
            if ( this.m_topLeft.x != newShape.topLeftX ){
                this.m_topLeft.x = newShape.topLeftX;
                updated = true;
            }
            if ( this.m_topLeft.y != newShape.topLeftY ){
                this.m_topLeft.y = newShape.topLeftY;
                updated = true;
            }
            if ( this.m_bottomRight.x != newShape.bottomRightX ){
                this.m_bottomRight.x = newShape.bottomRightX;
                updated = true;
            }
            if ( this.m_bottomRight.y != newShape.bottomRightY ){
                this.m_bottomRight.y = newShape.bottomRightY;
                updated = true;
            }
            return updated;
        },
        
        /**
         * Determine the hover status of the mouse with regard to this rectangle.
         * @param lastMouse{Object} the (x,y) coordinates of the mouse.
         * @param margin {Number} an error margin for determining closeness.
         */
        updateHoverStatus : function( lastMouse, margin ){
            var mx = lastMouse.x;
            var my = lastMouse.y;
            var inXrange = this.isMouseInXRange( mx, margin );
            var inYrange = this.isMouseInYRange( my, margin );
            this.m_resize = false;
            this.m_hover = false;
            this.m_hoverInfo = skel.widgets.Draw.Shape.CURSOR_DEFAULT;
            if ( inXrange && inYrange ){
               var left = this._isMouseLeftSide( mx, margin);
               var right = this._isMouseRightSide( mx, margin );
               var top = this._isMouseTopSide( my, margin);
               var bottom = this._isMouseBottomSide(my, margin );
                
               if (left && top) {
                  this.m_hoverInfo = skel.widgets.Draw.Shape.CURSOR_RESIZE_NW;
                  this.m_resize = true;
               }
               else if (left && bottom) {
                  this.m_hoverInfo = skel.widgets.Draw.Shape.CURSOR_RESIZE_SW;
                  this.m_resize = true;
               }
               else if (right && top) {
                  this.m_hoverInfo = skel.widgets.Draw.Shape.CURSOR_RESIZE_NE;
                  this.m_resize = true;
               }
               else if (right && bottom) {
                  this.m_hoverInfo = skel.widgets.Draw.Shape.CURSOR_RESIZE_SE;
                  this.m_resize = true;
               }
               else if (left) {
                  this.m_hoverInfo = skel.widgets.Draw.Shape.CURSOR_RESIZE_W;
                  this.m_resize = true;
               }
               else if (right) {
                  this.m_hoverInfo = skel.widgets.Draw.Shape.CURSOR_RESIZE_E;
                  this.m_resize = true;
               }
               else if (top) {
                  this.m_hoverInfo = skel.widgets.Draw.Shape.CURSOR_RESIZE_N;
                  this.m_resize = true;
               }
               else if (bottom) {
                  this.m_hoverInfo = skel.widgets.Draw.Shape.CURSOR_RESIZE_S;
                  this.m_resize = true;
               }
               else {
                  this.m_hoverInfo = skel.widgets.Draw.Shape.CURSOR_MOVE;
               }
               this.m_hover = true;
            }
          
            return this.m_hoverInfo;
        },

        m_topLeft : null,
        m_bottomRight : null
    }
});