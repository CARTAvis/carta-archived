/**
 * Draws widgets for setting up manual guesses for fits.
 */

qx.Class.define("skel.widgets.Profile.FitOverlay", {
        extend: qx.ui.embed.Canvas,

        /**
         * Constructor
         */
        construct: function () {
            this.base(arguments);
            this.setSyncDimension( true );

            // setup defered call for updating everything
            this.m_updateDefer = new qx.util.DeferredCall(this.update, this);
            
            this.addListener("mousemove", this._canvasMouseMoveCB, this);
            this.addListener("mousedown", this._canvasMouseDownCB, this);
            this.addListener("mouseup", this._canvasMouseUpCB, this);
            
            var x = 248.5;
            var h = 135.5;
            var bm = 62.125;
            this._initialGuessCB( x + " "+ bm +" "+ h );
        },

        members: {

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
                this.m_plotAreaTLX = 0;
                this.m_plotAreaTLY = 0;
                this.m_plotAreaBLX = width;
                this.m_plotAreaBLY = height - 30;
                ctx.clearRect(0, 0, width, height);
                if ( this.isVisible() && this.m_manual ){
                    this._drawFitBox( ctx, width, height );
                }
            },

            /**
             * Set an initial guess for the curve.
             */
            _initialGuessCB: function( val) {
                //fv.console.group( "initiaGuess changed = " + val);
                val = val || "";
                this.m_ics = { num: 0, vals: []};
                val = val.split(" ");
                this.m_ics.num = Math.floor( val.length / 3);
                this.m_ics.vals = [];
                for( var i = 0 ; i < this.m_ics.num ; i ++ ) {
                    this.m_ics.vals[i] = {
                        centerx: parseFloat( val[i*3+0]),
                        fwhmdx:  parseFloat( val[i*3+1]),
                        amply:   parseFloat( val[i*3+2])
                    };
                }
                
                /**
                 * Draw an equalateral triangle.
                 * @param x {Number} - the center of the bounding box.
                 * @param y {Number} - the center of the bounding box.
                 * @param psize {Number} - the half the side of the triangle.
                 * @param col1 {String} - the unselected color of the triangle.
                 * @param col2 {String} - the selected color of the triangle.
                 */
                function makeTri( x, y, psize, col1, col2) {
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
                }
                
                /**
                 * Draw a rectangle.
                 * @param x {Number} - the left corner of the bounding box.
                 * @param y {Number} - the upper corner of the bounding box.
                 * @param pwidth {Number} - the width of the bounding box.
                 * @param pheight {Number} - the height of the bounding box.
                 * @param col1 {String} - the unselected color of the rectangle.
                 * @param col2 {String} - the selected color of the rectangle.
                 */
                function makeRectH( x, y, pwidth, pheight, col1, col2) {
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
                            ctx.fillStyle = hover ? color1 : color2;
                            ctx.fillRect( cx, cy, width, height);
                        },
                        setHover: function( val) {
                            hover = val;
                        }
                    };
                }
                
                /**
                 * Draw a square.
                 * @param x {Number} - the left corner of the bounding box.
                 * @param y {Number} - the upper corner of the bounding box.
                 * @param pside {Number} - the length of a side of the square in pixels.
                 * @param col1 {String} - the unselected color of the square.
                 * @param col2 {String} - the selected color of the square.
                 */
                function makeSq( x, y, psize, col1, col2) {
                    return makeRectH( x - psize, y - psize, psize * 2, psize * 2, col1, col2);
                }
                
                /**
                 * Make a curve that mimics a Gaussian.
                 * @param x {Number} - the left corner of the bounding box.
                 * @param y {Number} - the upper corner of the bounding box.
                 * @param pwidth {Number} - the width of the bounding box.
                 * @param pheight {Number} - the height of the bounding box.
                 * @param col1 {String} - the unselected color of the curve.
                 * @param col2 {String} - the selected color of the curve.
                 */
                function makeGauss( x, y, pwidth, pheight, col1, col2) {
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
                }

                // regenerate movables
                this.m_movables = [];
                for( var i = 0 ; i < this.m_ics.num ; i ++ ) {
                    var ic = this.m_ics.vals[i];
                    var obj;
                    //Filled rectangle indicating entire marker area
                    obj = makeRectH( ic.centerx - ic.fwhmdx, ic.amply, ic.fwhmdx * 2, 
                            this.m_plotAreaBLY + 10 - ic.amply,
                        "rgba(255,255,0,0.4)", "rgba(255,255,0,0.2)");
                    obj.ic = ic; obj.ict = "bar";
                    this.m_movables.push( obj);
                    
                    //Horizontal line at bottom of marker
                    obj = makeRectH( ic.centerx - ic.fwhmdx, this.m_plotAreaBLY + 10, ic.fwhmdx * 2, 2,
                        "#f00", "#f88");
                    obj.ic = ic; obj.ict = "none";
                    obj.getdsq = function() { return 1e9; };
                    this.m_movables.push( obj);
                    
                    //Gaussian curve indicator
                    obj = makeGauss( ic.centerx - ic.fwhmdx, ic.amply, ic.fwhmdx*2, 
                            this.m_plotAreaBLY + 10 -ic.amply, "#0f0", "#0f0");
                    obj.ic = ic;
                    obj.ict = "none";
                    obj.getdsq = function() {return 1e9;};
                    this.m_movables.push( obj );

                    //Top (amplitude) control point
                    obj = makeSq( ic.centerx, ic.amply, 5, "#f00", "rgba(255,0,0,0.5)");
                    obj.ic = ic; obj.ict = "top";
                    this.m_movables.push( obj);
                    
                    //Right center/width control point
                    obj = makeTri( ic.centerx + ic.fwhmdx, this.m_plotAreaBLY + 7, 5, "#f00", "#f88");
                    obj.ic = ic; obj.ict = "right";
                    this.m_movables.push( obj);
                    
                    //Left center/width control point
                    obj = makeTri( ic.centerx - ic.fwhmdx, this.m_plotAreaBLY + 7, 5, "#f00", "#f88");
                    obj.ic = ic; obj.ict = "left";
                    this.m_movables.push( obj);
                }
                //this._setDirty();
            },
            

            /**
             * returns mouse event's local position (with respect to this widget)
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
             * Draws the manual fit controls
             */
            _drawFitBox: function ( ctx, width, height ) {
                
                // draw all movable objects
                if( this.m_manual ) {
                    for( var i = 0 ; i < this.m_movables.length ; i ++ ) {
                        this.m_movables[i].draw( ctx);
                    }
                }
            },

            /**
             * Updates m_hoverObjectIndex based on the contents of m_movables[]
             *
             * @private
             */
            _updateHoverInfo: function() {
                this.m_hoverObjectIndex = null;

                var minD = Number.NaN;
                for( var i = 0 ; i < this.m_movables.length ; i ++ ) {
                    var d = this.m_movables[i].getdsq( this.m_lastMousePosition);
                    if( d > 15) continue; // at least min. distance
                    if( d < minD || this.m_hoverObjectIndex === null) {
                        this.m_hoverObjectIndex = i;
                        minD = d;
                    }
                }
                // set hover state for all objects
                for( var i = 0 ; i < this.m_movables.length ; i ++ ) {
                    this.m_movables[i].setHover( i === this.m_hoverObjectIndex);
                }
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
                    //this._uiAction( "cursor1mouse", pt.x );
                    // call redraw anyways because the cursor probably changed y position
                    //this._setDirty();
                    return;
                }
                // if there are no movables on the screen, we are in zoom mode
                if( this.m_movables.length < 1) {
                    console.log( "No movables to move");
                }
                else if( this.m_hoverObjectIndex !== null) {
                    console.log( "Hover index="+this.m_hoverObjectIndex);
                    var obj = this.m_movables[ this.m_hoverObjectIndex];
                    // update the appropriate ics
                    if( obj.ict === "top") {
                        obj.ic.centerx = this.m_lastMousePosition.x;
                        obj.ic.amply = this.m_lastMousePosition.y;
                    }
                    else if( obj.ict === "right") {
                        obj.ic.fwhmdx = this.m_lastMousePosition.x - obj.ic.centerx;
                    }
                    else if( obj.ict === "left") {
                        obj.ic.fwhmdx = obj.ic.centerx - this.m_lastMousePosition.x;
                    }
                    else if( obj.ict === "bar") {
                        obj.ic.centerx = this.m_lastMousePosition.x;
                    }
                   /* else {
                        fv.console.warn( "Not sure what to do with " + obj.ict);
                    }*/
                    if( obj.ic.fwhmdx < 1) obj.ic.fwhmdx = 1;

                    // resend the state of initial conditions
                    var str = "";
                    for( var i = 0 ; i < this.m_ics.vals.length ; i ++ ) {
                        if( i > 0) str += " ";
                        str += this.m_ics.vals[i].centerx + " "
                          + this.m_ics.vals[i].fwhmdx + " "
                          + this.m_ics.vals[i].amply;
                    }
                    //fv.console.log( "setting ic=", this.m_profileID, str);
                    //this.m_vars.initialGuess.set( str);
                    this._initialGuessCB( str);
                }
                this._setDirty();
            },

            /**
             * Callback for a mouse down event.
             * @param event {qx.event.type.Mouse} - the event.
             */
            _canvasMouseDownCB: function (event) {
                //this.capture();
                var pt = this._localPos(event);
                this.m_mouseDownPosition = pt;
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
             * Arranges to have everything recalculated and repainted.
             */
            _setDirty: function () {
                this.m_updateDefer.schedule();
            },
            
            /**
             * Set whether or not manual controls for initial guesses
             * should be shown.
             */
            setManual : function( manual ){
                this.m_manual = manual;
                this.update();
            },

/*
            _keyDownCB: function (event) {
                if (event.getKeyIdentifier() === "Escape") {
                    this.m_mouseDownPosition = null;
                    this.m_zoomType = "none";
                    this.resetZoom();
                    this._setDirty();
                }
            },

            _mouseOverCB: function (event) {
                this.activate();
            },

            _mouseOutCB: function (event) {
                this.deactivate();
                this.m_lastMousePosition = null;
                this._uiAction( "cursor1mouse", null);
            },*/

            /*_uiAction: function( cmd , val1, val2, val3, ...  ) {
                var data = {
                    id: this.m_profileID,
                    cmd: cmd
                };
                for( var i = 1 ; i < arguments.length ; i ++ ) {
                    data[ "val" + i.toString()] = arguments[i];
                }
                this.fireDataEvent( "uiAction", data);
            }*/
            m_ics : null,
            m_manual : true,
            m_plotAreaTLX : 0,
            m_plotAreaTLY : 0,
            m_plotAreaBLX : 497,
            m_plotAreaBLY : 240,
            m_hoverObjectIndex : null,
            m_mouseDownPosition : null,
            m_lastMousePosition : null,
            // list of movable objects
            m_movables: [],
            m_updateDefer : null

        }
      

    });

