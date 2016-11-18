/**
 * Draws widgets for setting up manual guesses for fits.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/
qx.Class.define("skel.widgets.Profile.FitOverlay", {
    extend: qx.ui.embed.Canvas,

    /**
     * Constructor
     */
    construct: function () {
        this.base(arguments);
        this.setSyncDimension( true );
        if ( typeof mImport !== "undefined"){
        	this.m_connector = mImport("connector");
        }

        // setup defered call for updating everything
        this.m_updateDefer = new qx.util.DeferredCall(this.update, this);
        
        this.addListener("mousemove", this._canvasMouseMoveCB, this);
        this.addListener("mousedown", this._canvasMouseDownCB, this);
        this.addListener("mouseup", this._canvasMouseUpCB, this);
        this.addListener("mouseout", this._mouseOutCB, this );
        this._setDirty();
    },

    members: {
        

        /**
         * Callback for a mouse down event.
         * @param event {qx.event.type.Mouse} - the event.
         */
        _canvasMouseDownCB: function (event) {
            var pt = this._localPos(event);
            this.m_mouseDownPosition = pt;
        },

        
        /**
         * Callback for a mouse move event.
         * @param event {qx.event.type.Mouse} - the event.
         */
        _canvasMouseMoveCB: function (event) {
            var pt = this._localPos(event);
            this.m_lastMousePosition = pt;
            if (this.m_mouseDownPosition == null) {
                this._updateHoverInfo();
                // call redraw because the cursor position changed and we
                //may need to update hover information.
                this._setDirty();
                return;
            }
            if ( event.isCtrlPressed() ){
                if( this.m_hoverIndex !== null) {
                    var movableChanged = false;
                    var obj = this.m_movables[ this.m_hoverIndex];
                    var index = obj.guess;
                    // update the appropriate ics
                    if( obj.type === "top") {
                        if ( this.m_guesses[index].centerPixel != this.m_lastMousePosition.x ){
                            this.m_guesses[index].centerPixel = this.m_lastMousePosition.x;
                            movableChanged = true;
                        }
                        if ( this.m_guesses[index].peakPixel != this.m_lastMousePosition.y ){
                            this.m_guesses[index].peakPixel = this.m_lastMousePosition.y;
                            movableChanged = true;
                        }
                    }
                    else if( obj.type === "right") {
                        var fbhw = this.m_lastMousePosition.x - this.m_guesses[index].centerPixel;
                        if ( fbhw != this.m_guesses[index].fbhwPixel ){
                            this.m_guesses[index].fbhwPixel = fbhw;
                            movableChanged = true;
                        }
                    }
                    else if( obj.type === "left") {
                        var fbhw = this.m_guesses[index].centerPixel - this.m_lastMousePosition.x;
                        if ( fbhw != this.m_guesses[index].fbhwPixel ){
                            this.m_guesses[index].fbhwPixel = fbhw;
                            movableChanged = true;
                        }
                    }
                    else if( obj.type === "bar") {
                        if ( this.m_guesses[index].centerPixel != this.m_lastMousePosition.x ){
                            this.m_guesses[index].centerPixel = this.m_lastMousePosition.x;
                            movableChanged = true;
                        }
                    }
                    else {
                        console.log( "Not sure what to do with " + obj.type);
                    }
                    if( this.m_guesses[index].fbhwPixel < 1){
                        this.m_guesses[index].fbhwPixel = 1;
                        movableChanged = true;
                    }
                    if ( movableChanged ){
                        //Update the UI, triggering a draw
                        this._makeMovables();
                        //Notify the server
                        this._sendInitialFitGuessesCmd();
                    }
                }
                
            }
        },
        
        
        /**
         * Callback for a mouse up event.
         * @param event {qx.event.type.Mouse} - the event.
         */
        _canvasMouseUpCB: function (event) {
            //this.releaseCapture();
            this.m_mouseDownPosition = null;
        },
        

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
            ctx.clearRect(0, 0, width, height);
            if ( this.isVisible() && this.m_manual && this.m_manualShow){
                this._drawFitBox( ctx );
            }
        },
        
        
        /**
         * Draws the manual fit controls
         */
        _drawFitBox: function ( ctx ) {
            
            // draw all movable objects
            if( this.m_manual ) {
                for( var i = 0 ; i < this.m_movables.length ; i ++ ) {
                    this.m_movables[i].draw( ctx);
                }
            }
        },
        
        /**
         * Returns whether or not the pixel coordinates of the fit guesses have
         * changed with respect to a new set of server side guesses.
         * @param newGuesses {Object} - new initial fit guesses.
         */
        _isGuessesChanged : function( newGuesses ){
            var changed = false;
            if ( this.m_guesses === null && newGuesses !== null ){
                changed = true;
            }
            else if ( this.m_guesses.length != newGuesses.length ){
                changed = true;
            }
            else {
                var errorMargin = 1;
                for ( var i = 0; i < this.m_guesses.length; i++ ){
                    if ( Math.abs(this.m_guesses[i].centerPixel - newGuesses[i].centerPixel) > errorMargin ){
                        changed = true;
                        break
                    }
                    else if ( Math.abs( this.m_guesses[i].peakPixel - newGuesses[i].peakPixel) > errorMargin ){
                        changed = true;
                        break
                    }
                    else if ( Math.abs( this.m_guesses[i].fbhwPixel - newGuesses[i].fbhwPixel) > errorMargin ){
                        changed = true;
                        break;
                    }
                }
            }
            return changed;
        },
        
        /**
         * Returns mouse event's local position (with respect to this widget)
         * @param event {Event}
         */
        _localPos: function (event) {
            var box = this.getContentLocation("box");
            return {
                x: event.getDocumentLeft() - box.left,
                y: event.getDocumentTop() - box.top
            };
        },

        
        /**
         * Make a curve that mimics a Gaussian.
         * @param x {Number} - the left corner of the bounding box.
         * @param y {Number} - the upper corner of the bounding box.
         * @param pwidth {Number} - the width of the bounding box.
         * @param pheight {Number} - the height of the bounding box.
         * @param col1 {String} - the unselected color of the curve.
         * @param col2 {String} - the selected color of the curve.
         */
        _makeGauss : function( x, y, pwidth, pheight, col1, col2) {
            var cx = x;
            var cy = y;
            var width = pwidth;
            var height = pheight;
            var color1 = col1;
            var color2 = col2;
            var hover = false;
            return {
                getdsq: function( pt) {
                    if( pt.x >= cx && pt.x <= cx + width && pt.y >= cy && pt.y <= cy + height) {
                        var dx = pt.x - cx - width/2;
                        var dy = pt.y - cy - height/2;
                        dx /= width;
                        dy /= height;
                        return (dx * dx + dy * dy);
                    }
                    else {
                        return 1e9;
                    }
                },
                draw: function( ctx) {
                    ctx.strokeStyle = hover ? color1 : color2;
                    ctx.beginPath();
                    var percent = 3/8;
                    var mid = cy + height * percent;
                    ctx.moveTo( cx, mid );
                    ctx.quadraticCurveTo( cx + width/2, cy -height * percent, cx + width, mid );
                    ctx.stroke();
                    ctx.restore();
                },
                setHover: function( val) {
                    hover = val;
                }
            };
        },
        
        /**
         * Make the widgets that allow the specification of manual guesses.
         */
        _makeMovables : function(){
            if ( this.m_guesses !== null ){
                // regenerate movables
                this.m_movables = [];
                var fillColorStr = skel.theme.Color.colors.border;
                var curveColorStr = skel.theme.Color.colors.dialogBackground;
                var controlColorStr = skel.theme.Color.colors.warning;
                var controlColorHoverStr = skel.theme.Color.colors.error;
                var fillColor = qx.util.ColorUtil.stringToRgb( fillColorStr );
                var rectBackground="rgba("+fillColor[0]+","+fillColor[1]+","+fillColor[2]+",0.4)";
                var rectBackground2="rgba("+fillColor[0]+","+fillColor[1]+","+fillColor[2]+",0.2)";
               
                for( var i = 0 ; i < this.m_guesses.length ; i ++ ) {
                    var guess = this.m_guesses[i];
                    var height = this.m_plotHeight - guess.peakPixel;
                    var obj;
                    
                    //Filled rectangle indicating entire marker area
                    var x = guess.centerPixel - guess.fbhwPixel;
                    var width = guess.fbhwPixel*2;
                    obj = this._makeRectH( x, guess.peakPixel, width, height,
                            rectBackground, rectBackground2 );
                    obj.guess = i; 
                    obj.type = "bar";
                    this.m_movables.push( obj);
                    
                    //Horizontal line at bottom of marker
                    obj = this._makeRectH( guess.centerPixel - guess.fbhwPixel, 
                            guess.peakPixel + height, guess.fbhwPixel * 2, 2,
                            controlColorStr, controlColorStr );
                    obj.guess = i; 
                    obj.type = "none";
                    obj.getdsq = function() { return 1e9; };
                    this.m_movables.push( obj);
                    
                    //Gaussian curve indicator
                    obj = this._makeGauss( guess.centerPixel - guess.fbhwPixel, 
                            guess.peakPixel, guess.fbhwPixel*2, 
                            height,"#000000","#000000");
                    obj.guess = i;
                    obj.type = "none";
                    obj.getdsq = function() {return 1e9;};
                    this.m_movables.push( obj );
    
                    //Top (amplitude) control point
                    obj = this._makeSq( guess.centerPixel, guess.peakPixel, 5, 
                            controlColorHoverStr, controlColorStr );
                    obj.guess = i; 
                    obj.type = "top";
                    this.m_movables.push( obj);
                    
                    //Right center/width control point
                    obj = this._makeTri( guess.centerPixel + guess.fbhwPixel, 
                            guess.peakPixel + height - 3, 5, 
                            controlColorHoverStr, controlColorStr );
                    obj.guess = i; 
                    obj.type = "right";
                    this.m_movables.push( obj);
                    
                    //Left center/width control point
                    obj = this._makeTri( guess.centerPixel - guess.fbhwPixel, 
                            guess.peakPixel + height - 3, 5, 
                            controlColorHoverStr, controlColorStr );
                    obj.guess = i; 
                    obj.type = "left";
                    this.m_movables.push( obj);
                }
                this._setDirty();
            }
        },
        
        /**
         * Draw a rectangle.
         * @param x {Number} - the left corner of the bounding box.
         * @param y {Number} - the upper corner of the bounding box.
         * @param pwidth {Number} - the width of the bounding box.
         * @param pheight {Number} - the height of the bounding box.
         * @param col1 {String} - the unselected color of the rectangle.
         * @param col2 {String} - the selected color of the rectangle.
         */
        _makeRectH : function( x, y, pwidth, pheight, col1, col2) {
            var cx = x;
            var cy = y;
            var width = pwidth;
            var height = pheight;
            var color1 = col1;
            var color2 = col2;
            var hover = false;
            return {
                getdsq: function( pt) {
                    if( pt.x >= cx && pt.x <= cx + width && pt.y >= y && pt.y <= cy + height) {
                        var dx = pt.x - cx - width/2;
                        var dy = pt.y - cy - height/2;
                        dx /= width;
                        dy /= height;
                        return (dx * dx + dy * dy);
                    }
                    else {
                        return 1e9;
                    }
                },
                draw: function( ctx) {
                    ctx.fillStyle = hover ? color1 : color2;
                    ctx.fillRect( cx, cy, width, height);
                },
                setHover: function( val) {
                    hover = val;
                }
            };
        },
        

        /**
         * Draw a square.
         * @param x {Number} - the left corner of the bounding box.
         * @param y {Number} - the upper corner of the bounding box.
         * @param pside {Number} - the length of a side of the square in pixels.
         * @param col1 {String} - the unselected color of the square.
         * @param col2 {String} - the selected color of the square.
         */
        _makeSq : function( x, y, psize, col1, col2) {
            return this._makeRectH( x - psize, y - psize, psize * 2, psize * 2, col1, col2);
        },
        
        /**
         * Draw an equalateral triangle.
         * @param x {Number} - the center of the bounding box.
         * @param y {Number} - the center of the bounding box.
         * @param psize {Number} - the half the side of the triangle.
         * @param col1 {String} - the unselected color of the triangle.
         * @param col2 {String} - the selected color of the triangle.
         */
        _makeTri : function( x, y, psize, col1, col2) {
            var cx = x;
            var cy = y;
            var size = psize;
            var color1 = col1;
            var color2 = col2;
            var hover = false;
            return {
                getdsq: function( pt) {
                    var dx = pt.x - cx;
                    var dy = pt.y - cy;
                    dx /= psize;
                    dy /= psize;
                    if( Math.abs(dx) > 1 || Math.abs(dy) > 1) return 1e9;
                    return dx * dx + dy * dy;
                },
                draw: function( ctx) {
                    ctx.fillStyle = hover ? color1 : color2;
                    ctx.beginPath();
                    ctx.moveTo( cx, cy - size);
                    ctx.lineTo( cx + size, cy + size);
                    ctx.lineTo( cx - size, cy + size);
                    ctx.lineTo( cx, cy - size);
                    ctx.fill();
                    ctx.closePath();
                },
                setHover: function( val) {
                    hover = val;
                }
            };
        },
        
        /**
         * Mouse moved out of the draw area.
         * @param event {qx.event.type.Mouse} - the event.
         */
        _mouseOutCB: function (event) {
            this.m_lastMousePosition = null;
        },
        
        /**
         * Update from the server concerning fit information.
         * @param fitInfo {Object} - fit information from the server.
         */
        profileFitUpdate : function( fitInfo ){
            var manualChanged = this.setManual( fitInfo.fit.manualGuess);
            var sizeChanged = this.setPlotBounds( fitInfo.plotLeft, fitInfo.plotTop, fitInfo.plotWidth, fitInfo.plotHeight );
            var guessesChanged = this.setInitialGuesses( fitInfo.fit.fitGuesses );
            if ( guessesChanged || sizeChanged ){
                this._makeMovables();
                this._setDirty();
            }
            else if ( manualChanged ){
                this._setDirty();
            }
        },
        
        /**
         * Send a command to the server to update initial fit guesses.
         */
        _sendInitialFitGuessesCmd : function(){
            // resend the state of initial conditions
            var str = "";
            if ( this.m_guesses !== null ){
                for( var i = 0 ; i < this.m_guesses.length ; i ++ ) {
                    if( i > 0) str += " ";
                    str += this.m_guesses[i].centerPixel + " "
                      + this.m_guesses[i].peakPixel + " "
                      + this.m_guesses[i].fbhwPixel;
                }
                if ( this.m_id !== null && this.m_connector !== null ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + "setFitManualGuesses";
                    var params = "fitGuesses:"+str;
                    this.m_connector.sendCommand( cmd, params, null );
                }
            }
        },

        /**
         * Arranges to have everything recalculated and repainted.
         */
        _setDirty: function () {
            this.m_updateDefer.schedule();
        },
        
        
        /**
         * Set the server-side id of the object managing profile fits.
         */
        setId : function( id ){
            this.m_id = id;
        },
        

        /**
         * Set an initial guess for the curve based on serve-side guesses.
         */
        setInitialGuesses : function( guesses ){
            //Check to see if there was any change from the previous set.
           var guessesChanged = this._isGuessesChanged( guesses );
           if ( guessesChanged ){
                this.m_guesses = guesses;
                this._makeMovables();
           }
        },
        
        
        /**
         * Set whether or not manual controls for initial guesses
         * are enabled.
         * @param manual {boolean} - true if initial fit guesses are set
         *      manually; false otherwise.
         */
        setManual : function( manual ){
            var changed = false;
            if ( this.m_manual != manual ){
                this.m_manual = manual;
            }
            return changed;
        },
      
        /**
         * Set whether manual guesses should be shown.
         * @param manualShow {boolean} - true if manual guesses should be
         *      drawn; false otherwise.
         */
        //Note: Even if manualShow is set to true, manual guesses will not be
        //drawn unless we are also in manual mode.
        setManualShow : function( manualShow ){
            var changed = false;
            if ( this.m_manualShow != manualShow ){
                this.m_manualShow = manualShow;
                if ( this.m_manualShow ){
                    this._makeMovables();
                }
                else {
                    this.m_movables = [];
                }
                this._setDirty();
            }
            return changed;
        },
        
        /**
         * Set the upper left corner and size of the plotting area.
         * @param left {Number} - the x-coordinate, in pixels, of the left corner of the plotting area.
         * @param top {Number} - the y-coordinate, in pixels, of the top corner of the plotting area.
         * @param width {Number} - the pixel width of the plotting area.
         * @param height {Number} - the pixel height of the plotting area.
         */
        setPlotBounds : function( left, top, width, height ){
            var update = false;
            if ( this.m_plotTop != top ){
                this.m_plotTop = top;
                update = true;
            }
            if ( this.m_plotLeft != left ){
                this.m_plotLeft = left;
                update = true;
            }
            if ( this.m_plotWidth != width ){
                this.m_plotWidth = width;
                update = true;
            }
            if ( this.m_plotHeight != height ){
                this.m_plotHeight = height;
                update = true;
            }
            return update;
        },

        /**
         * Updates m_hoverIndex based on the contents of m_movables[]
         */
        _updateHoverInfo: function() {
            this.m_hoverIndex = null;

            var minD = Number.NaN;
            for( var i = 0 ; i < this.m_movables.length ; i ++ ) {
                var d = this.m_movables[i].getdsq( this.m_lastMousePosition);
                if( d > 15) continue; // at least min. distance
                if( d < minD || this.m_hoverIndex === null) {
                    this.m_hoverIndex = i;
                    minD = d;
                }
            }
            // set hover state for all objects
            for( var i = 0 ; i < this.m_movables.length ; i ++ ) {
                this.m_movables[i].setHover( i === this.m_hoverIndex);
            }
        },

        m_connector : null,
        m_guesses : null,
        m_id : null,
        m_manual : false,
        m_manualShow : false,
        m_plotHeight : 0,
        m_plotWidth : 0,
        m_plotTop : 0,
        m_plotLeft : 0,
        m_hoverIndex : null,
        m_mouseDownPosition : null,
        m_lastMousePosition : null,
        // list of movable objects
        m_movables: [],
        m_updateDefer : null

    }
  

});

