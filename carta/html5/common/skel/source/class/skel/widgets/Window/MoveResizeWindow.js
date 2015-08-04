/**
 * A window that can either be shown in-line or as a stand-alone pop-up dialog.
 */


qx.Class.define( "skel.widgets.Window.MoveResizeWindow",
    {
        extend : qx.ui.window.Window,
        
        /**
         * Constructor.
         * @param detached {boolean} true if this is a standalone (Dialog) window;
         *      false if this is an inline, semi-permanent window.
         */
        construct: function ( detached ) {
            this.base(arguments);
            this._setDetached( detached );
        },
        
        statics : {
            CURSOR_RESIZE_NW : "nw-resize",
            CURSOR_RESIZE_SW : "sw-resize",
            CURSOR_RESIZE_NE : "ne-resize",
            CURSOR_RESIZE_SE : "se-resize",
            CURSOR_RESIZE_W : "w-resize",
            CURSOR_RESIZE_E : "e-resize",
            CURSOR_RESIZE_N : "n-resize",
            CURSOR_RESIZE_S : "s-resize",
            //CURSOR_MOVE : "move",
            //CURSOR_CROSSHAIR : "crosshair",
            CURSOR_DEFAULT : "default"
        },

        members: {

            /**
             * Determine the appropriate cursor to show (for example, resize)
             * based on the location of the mouse.
             * @param mousePt {Object} the local (x,y) position of the mouse.
             */
            _checkCursor : function( mousePt ){
                var box = this.getContentLocation("box");
                
                var leftEdge = this._isMatch( mousePt.x);
                var boxWidth = box.right - box.left;
                var rightEdge = this._isMatch( mousePt.x - boxWidth);
                var topEdge = this._isMatch( mousePt.y);
                var boxHeight = box.bottom - box.top;
                var bottomEdge = this._isMatch( mousePt.y - boxHeight);
                //Left Right
                if ( leftEdge && bottomEdge ){
                    this.setCursor( skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_SW);
                }
                else if ( bottomEdge && rightEdge ){
                    this.setCursor( skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_SE);
                }
                else if ( rightEdge && topEdge ){
                    this.setCursor( skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_NE);
                }
                else if ( topEdge && leftEdge ){
                    this.setCursor( skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_NW);
                }
                else if ( rightEdge ){
                    this.setCursor( skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_E );
                }
                else if ( topEdge ){
                    this.setCursor( skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_N );
                }
                else if ( bottomEdge ){
                    this.setCursor( skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_S );
                }
                else if ( leftEdge ){
                    this.setCursor( skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_W );
                }
                else {
                    this.setCursor( skel.widgets.Window.MoveResizeWindow.CURSOR_DEFAULT );
                }
              
            },
            
            /**
             * Respond to an event which has triggered the context menu to
             * show.
             */
            //Written because when the context menu is opened the mouse
            //up event was not received so mouse moves were being interpreted
            //as window moves after selected from the context menu.
            _contextMenuEvent : function(){
                this.m_mouseDown = null;
                this.m_mouseClick = true;
            },
            
            /**
             * Move this window.
             * @param amountX {Number} the pixel amount in the x-direction.
             * @param amountY {Number} the pixel amount in the y-direction.
             */
            _doMove : function( amountX, amountY ){
                var box = this.getContentLocation("box");
                var left = box.left + amountX;
                var top = box.top + amountY;
                var width = box.right - box.left;
                var height = box.bottom - box.top;
                this.setUserBounds( left, top, width, height);
            },
            
            /**
             * Resize this window.
             * @param amountX {Number} the change in width.
             * @param amountY {Number} the change in height.
             */
            _doResize : function( amountX, amountY ){
                var cursor = this.getCursor();
                var box = this.getContentLocation( "box");
                var newLeft = box.left;
                var newTop = box.top;
                var width = box.right - box.left;
                var height = box.bottom - box.top;
                var resize = false;
                if ( this._isResizeNorth(cursor)){
                    newTop = newTop + amountY;
                    height = height - amountY;
                    resize = true;
                }
                if ( this._isResizeSouth( cursor)){
                    height = height + amountY;
                    resize = true;
                }
                if ( this._isResizeEast( cursor) ){
                    newLeft = newLeft + amountX;
                    width = width - amountX;
                    resize = true;
                }
                if ( this._isResizeWest( cursor)){
                    width = width + amountX;
                    resize = true;
                }
                if ( resize ){
                    this.setUserBounds( newLeft, newTop, width, height );
                }
            },
            
            /**
             * Returns whether or not the number is close to zero.
             * @param mouseLoc {Number} a number.
             * @return {boolean} true if the number is 'close to' zero; false otherwise.
             */
            _isMatch : function( mouseLoc ){
                var ERROR_MARGIN = 15;
                var match = false;
                if ( Math.abs( mouseLoc) < ERROR_MARGIN ){
                    match = true;
                }
                return match;
            },
            
            /**
             * Returns true if the point is not near the margins of the
             * window; false otherwise.
             * @param mouseLoc {Object} the local (x,y) coordinate of the mouse.
             * @return {boolean} true if the mouse is not near one of the edges of the window;
             *  false if the mouse is near a border.
             */
            _isCursorCenter: function( mouseLoc ){
                var cursorCenter = false;
                var MARGIN = 10;
                var box = this.getContentLocation( "box");
                if ( MARGIN <= mouseLoc.x && mouseLoc.x <= box.right - box.left - MARGIN ){
                    if ( MARGIN <= mouseLoc.y && mouseLoc.y <= box.bottom - box.top - MARGIN ){
                        cursorCenter = true;
                    }
                }
                return cursorCenter;
            },
            
            /**
             * Returns true if this window is acting as a pop-up dialog; false
             * if this window is being displayed in-line.
             * @return {boolean} true if this is a detached window; false otherwise.
             */
            isDetached : function(){
                return this.m_detached;
            },
            
            /**
             * Returns true if the cursor is indicating the bottom of the
             * window will be resized; false otherwise.
             * @param cursor {String} a cursor identifier.
             * @return {boolean} true for a bottom resize; false otherwise.
             */
            _isResizeSouth : function( cursor ){
                var resizeSouth = false;
                if (cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_SE  ||
                        cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_S ||
                        cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_SW ){
                    resizeSouth = true;
                }
                return resizeSouth;
            },
            
            /**
             * Returns true if the cursor is indicating the top of the
             * window will be resized; false otherwise.
             * @param cursor {String} a cursor identifier.
             * @return {boolean} true for a top resize; false otherwise.
             */
            _isResizeNorth : function( cursor ){
                var resizeNorth = false;
                if (cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_NE  ||
                        cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_N ||
                        cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_NW){
                    resizeNorth = true;
                }
                return resizeNorth;
            },
            
            /**
             * Returns true if the cursor is indicating the right of the
             * window will be resized; false otherwise.
             * @param cursor {String} a cursor identifier.
             * @return {boolean} true for a right resize; false otherwise.
             */
            _isResizeWest : function( cursor ){
                var resizeEast = false;
                if (cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_NE  ||
                    cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_E ||
                    cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_SE ){
                    resizeEast = true;
                }
                return resizeEast;
            },
            
            /**
             * Returns true if the cursor is indicating the left of the
             * window will be resized; false otherwise.
             * @param cursor {String} a cursor identifier.
             */
            _isResizeEast : function( cursor ){
                var resizeWest = false;
                if (cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_NW  ||
                    cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_W ||
                    cursor == skel.widgets.Window.MoveResizeWindow.CURSOR_RESIZE_NW ){
                    resizeWest = true;
                }
                return resizeWest;
            },
            
            /**
             * Returns true if the cursor indicates a window resize operation; false otherwise.
             * @return {boolean} true if a window resize operation is indicated; false otherwise.
             */
            _isResize : function(){
                var resize = false;
                var cursor = this.getCursor();
                if ( this._isResizeNorth( cursor) || this._isResizeSouth( cursor) ||
                        this._isResizeWest( cursor) || this._isResizeEast( cursor)){
                    resize = true;
                }
                return resize;
            },
            
            /**
             * Callback for a mouse move event.
             * @param event {MouseEvent}.
             */
            _mouseMoveCB : function( event ){
                var mouseMove = skel.widgets.Util.localPos( this, event );
                this._updateClickStatus( mouseMove );
                if ( this.m_mouseDown ){
                    var moveX = mouseMove.x - this.m_mouseMove.x;
                    var moveY = mouseMove.y - this.m_mouseMove.y;
                    event.preventDefault();
                    var resizing = this._isResize();
                    if ( resizing ){
                        this._doResize( moveX, moveY );
                    }
                    else if ( this._isCursorCenter( mouseMove) ){
                        if ( !this.m_mouseClick ){
                            this._doMove( moveX, moveY );
                        }
                    }
                }
                else {
                    this._checkCursor( mouseMove );
                }
                this.m_mouseMove = mouseMove;
            },
            
            /**
             * Callback for a mouse press event.
             * @param event {MouseEvent}.
             */
            _mouseDownCB : function( event){
                this.m_mouseDown = skel.widgets.Util.localPos(this, event);
                this.m_mouseMove = this.m_mouseDown;
                this.capture();
            },
            
            /**
             * Callback for a mouse release event.
             */
            _mouseUpCB : function(){
                this.m_mouseDown = null;
                this.releaseCapture();
            },
            
            /**
             * Set whether this window will be a pop-up dialog or be shown
             * as a semipermanent in-line window.
             * @param detached {boolean} true for a detached window; false for
             *          an in-line window.
             */
            _setDetached : function( detached ){
                this.m_detached = detached;
                if ( detached ){
                    this.addListener("mousemove", this._mouseMoveCB, this);
                    this.addListener("mousedown", this._mouseDownCB, this );
                    this.addListener("mouseup", this._mouseUpCB, this);
                }
              
            },
            
            /**
             * Update whether the mouse event represents a mouse move or a
             * mouse click.
             * @param mouseMove {Object} the (x,y) coordinates of the mouse.
             */
            _updateClickStatus : function( mouseMove ){
                if ( this.m_mouseDown ){
                    // if mouse is down, we check how far the user dragged it, and if it's more
                    // than some threshold, we know it's not a click, but a rectangle select
                    var dx = this.m_mouseDown.x - mouseMove.x;
                    var dy = this.m_mouseDown.y - mouseMove.y;
                    var distSq = dx * dx + dy * dy;
                    if (distSq > 13) {
                        this.m_mouseClick = false;
                    }
                    else {
                        this.m_mouseClick = true;
                    }
                }
                else {
                    this.m_mouseClick = false;
                }
            },
            
            m_mouseDown : null,
            m_mouseMove : null,
            m_mouseClick : true,
            m_detached : false
        }

    } );

