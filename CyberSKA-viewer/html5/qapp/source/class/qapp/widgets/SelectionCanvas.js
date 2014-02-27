/**
 * This widget is a rectangle selector overlay over top of the main view area. It allows
 * the user to interactively select rectangles. It is tied to the system via the hub.
 *
 */

/* global qx,fv, fv.console */

/**

 @ignore(fv.console.log)
 @ignore(fv.lib.closure)
 @ignore(fv.assert)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(fv.makeGlobalVariable)
 @ignore(qx)



 ************************************************************************ */

qx.Class.define("qapp.widgets.SelectionCanvas",
    {
        extend: qx.ui.embed.Canvas,

        construct: function (hub) {
            this.base(arguments);
            this.setSyncDimension(true);

            this.m_hub = hub;

            this.m_mouse = null;
            this.m_touchDevice = false;

            // add mouse listeners
            this.addListener("mousemove", this._mouseMoveCB);
            this.addListener("mousedown", this._mouseDownCB);
            this.addListener("mouseup", this._mouseUpCB);
            this.addListener("mousewheel", this._mouseWheelCB);
            this.addListener("keydown", this._keyDownCB);
            this.addListener("keyup", this._keyUpCB);
            this.addListener("mouseover", function () {
                this.m_mouseIn = true;
                this.activate();
            }, this);
            this.addListener("mouseout", function () {
                this.m_mouseIn = false;
                this.deactivate();
            }, this);
            this.addListener("touchmove", this._touchMoveCB);
            this.addListener("touchend", this._touchEndCB);
            this.addListener("touchcancel", this._touchCancelCB);
            this.addListener("touchstart", this._touchStartCB);

            // request listening to transformation changes for main view
            this.m_hub.subscribe("toui.mainWindow.setTx", this._setTxCB, this);

            // listen for x-profile cursor changes so that we can plot it
            this.m_hub.subscribe("toui.mainWindow.profile-cursor-x", function(val) {
                this.m_profileCursorX = val;
                this.update();
            }, this);
            this.m_hub.subscribe("toui.mainWindow.profile-cursor-y", function(val) {
                this.m_profileCursorY = val;
                this.update();
            }, this);

            this.m_isShift = false;
            this.m_mouseIn = false;
            this.m_tx = null;
            this.m_lastMouse = null;
            this.m_mouseHoverInfo = {
                rect: -1,
                part: ""
            };
            this.m_mouseDownRect = { x1: 0, y1: 0, x2: 0, y2: 0 };
            this.m_frozenCursor = null; // in mouse/screen coordinates
            this.m_profileCursorX = null; // in image coordinates
            this.m_profileCursorY = null;

            // if region stats window becomes active, move the corresponding rectangle to the top
            // this should perhaps be part of some controller so that we don't have a direct
            // coupling here between selection canvas and region stats window...
            this._updateRegionRect = function( rect, val) {
                var ind = -1;
                this.m_regionRectangles.forEach( function(e,i) {
                    if( e === rect) {
                        ind = i; return;
                    }
                }, this);
                fv.assert( ind !== -1, "cannot find regionstats rectangle...!?!?!");
                if( ! val.visible) {
                    fv.console.log( "Making " + rect.id + " invisible and null");
                    rect.iBr.x = rect.iBr.y = rect.iTl.x = rect.iTl.y = -100000;
                    this._sendRegionSetMessage( rect);
                }
                rect.visible = val.visible;
                // move window to top if the active flag is set
                if( val.active) {
                    qx.lang.Array.removeAt(this.m_regionRectangles, ind);
                    qx.lang.Array.insertAt(this.m_regionRectangles, rect, 0);
                }
                this._updateMouseHoverStatus();
                this.update();
            };

            this.m_hub.subscribe( "ui.regionStats.windowStatus", function(val) {
                this._updateRegionRect( this.m_regionStatsRect, val);
            }, this);
            this.m_hub.subscribe( "ui.g2dfit.windowStatus", function(val) {
                this._updateRegionRect( this.m_g2dRect, val);
            }, this);
            this.m_hub.subscribe( "ui.threeDsurfacePlotRect.windowStatus", function(val) {
                this._updateRegionRect( this.m_threeDsurfacePlotRect, val);
            }, this);

            this.m_regionRectangles = [];
            this.m_regionStatsRect = {
                label: "Region stats",
                id: "regionStats",
                iTl: { x: 5, y: 3},
                iBr: { x: 6, y: 4},
                mTl: { x: 0, y: 0},
                mBr: { x: 0, y: 0},
                visible: false
            };
            this.m_regionRectangles.push( this.m_regionStatsRect);

            this.m_g2dRect = {
                label: "2D gauss. fit",
                id: "g2dFit",
                iTl: { x: 0, y: 0},
                iBr: { x: 7, y: 1},
                mTl: { x: 0, y: 0},
                mBr: { x: 0, y: 0},
                visible: false
            };
            this.m_regionRectangles.push( this.m_g2dRect);

            this.m_threeDsurfacePlotRect = {
                label: "3D plot",
                id: "threeDsurfacePlot",
                iTl: { x: 0, y: 0},
                iBr: { x: 7, y: 1},
                mTl: { x: 0, y: 0},
                mBr: { x: 0, y: 0},
                visible: false
            };
            this.m_regionRectangles.push( this.m_threeDsurfacePlotRect);

            // create shared variable for communicating interactive zoom
            this.m_vars = {};
            this.m_vars.iZoomInit = fv.makeGlobalVariable( "/mainWindow/iZoom-init");
            this.m_vars.iZoomSet = fv.makeGlobalVariable( "/mainWindow/iZoom-set");

            fv.GLOBAL_DEBUG && fv.console.log("SelectionCanvas constructed.");
        },

        members: {
            m_hub: null,

            setFrozenCursor: function ( x, y) {
                if( x == null)
                    this.m_frozenCursor = null;
                else
                    this.m_frozenCursor = { x: x, y: y};
                this.update();
            },

            _emit: function (path, data) {
                this.m_hub.emit(path, data);
            },

            _setTxCB: function (val) {
//                fv.GLOBAL_DEBUG && fv.console.log("tx =", val);
                this.m_tx = val;
                this.update();
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

                ctx.clearRect(0, 0, width, height);

                if (this.m_rect !== null) {
                    ctx.fillStyle = "rgba(255,255,0, 0.1)";
                    ctx.fillRect(this.m_rect.x1, this.m_rect.y1,
                        this.m_rect.x2 - this.m_rect.x1, this.m_rect.y2 - this.m_rect.y1);

                    ctx.strokeStyle = "rgb(255,255,0)";
                    ctx.lineWidth = 2;
                    ctx.strokeRect(this.m_rect.x1, this.m_rect.y1,
                        this.m_rect.x2 - this.m_rect.x1, this.m_rect.y2 - this.m_rect.y1);
                }

                if (false && this.m_mouse != null && this.m_touchDevice) {
                    ctx.strokeStyle = "rgb(0,255,0)";
                    ctx.lineWidth = 2;
                    ctx.strokeRect(this.m_mouse.x - 5, this.m_mouse.y - 5, 10, 10);
                }

                if( this.m_touches != null) {
                    ctx.fillStyle = "rgba(255,0,0,0.5)";
                    ctx.strokeStyle = "rgba(255,255,255,0.5)";
                    ctx.lineWidth = 2;
                    for( var i = 0 ; i < this.m_touches.length ; i ++ ) {
                        var x = this.m_touches[i].x;
                        var y = this.m_touches[i].y;
                        ctx.beginPath();
                        ctx.arc( x, y, 20, 2 * Math.PI, false);
                        ctx.fill();
                        ctx.stroke();
                    }
                }

//                if (false && this.m_mouse !== null && this.m_tx !== null) {
//                    ctx.strokeStyle = "rgb(0,255,0)";
//                    ctx.lineWidth = 2;
//                    var pt1 = this._mouse2serverImage(this.m_mouse);
//                    pt1.x = Math.floor(pt1.x);
//                    pt1.y = Math.floor(pt1.y);
//                    var pt2 = { x: pt1.x + 1, y: pt1.y + 1};
//                    pt1 = this._serverImage2mouse(pt1);
//                    pt2 = this._serverImage2mouse(pt2);
//                    var minCursorSize = 10;
//                    var cx = (pt1.x + pt2.x) / 2;
//                    var cy = (pt1.y + pt2.y) / 2;
//                    /*
//                     if (pt2.x - pt1.x < minCursorSize) {
//                     pt1.x = cx - minCursorSize / 2;
//                     pt2.x = cx + minCursorSize / 2;
//                     pt1.y = cy - minCursorSize / 2;
//                     pt2.y = cy + minCursorSize / 2;
//                     }
//
//                     ctx.strokeRect(pt1.x, pt1.y, pt2.x - pt1.x, pt2.y - pt1.y);
//                     */
//                    ctx.beginPath();
//                    ctx.moveTo(cx - minCursorSize, cy);
//                    ctx.lineTo(cx + minCursorSize, cy);
//                    ctx.moveTo(cx, cy - minCursorSize);
//                    ctx.lineTo(cx, cy + minCursorSize);
//                    ctx.closePath();
//                    ctx.stroke();
//                }

                /*
                 if( this.m_tx !== null) {
                 var ix = 0, iy = 0, iw = 2, ih = 7;
                 var pt1 = this._serverImage2mouse( { x: ix, y: iy });
                 var pt2 = this._serverImage2mouse( { x: ix + iw, y: iy + ih });
                 ctx.strokeStyle = "rgb(255,0,255)";
                 ctx.lineWidth = 3;
                 ctx.strokeRect(pt1.x, pt1.y, pt2.x - pt1.x, pt2.y - pt1.y);
                 }
                 */

                // update coordinates of all rectangles
                this.m_regionRectangles.forEach(function (rect) {
                    rect.mTl = this._serverImage2mouse(rect.iTl);
                    rect.mBr = this._serverImage2mouse(rect.iBr);
                }, this);

                // update mouse hover info
                this._updateMouseHoverStatus();

                // draw all rectangles in reverse order
                var fistVisibleRectInd = this._getFirstVisibleRectInd();
                for (var i = this.m_regionRectangles.length - 1; i >= 0; i--) {
                    var rect = this.m_regionRectangles[i];
                    var isActive = i === fistVisibleRectInd;
                    var isHovered = (i === this.m_mouseHoverInfo.rect && this.m_mouseHoverInfo.part !== "crosshair");
                    var isResizing = this.m_mouseDownPt !== null && i === this.m_mouseHoverInfo.rect;
                    isHovered = isHovered || isResizing;
                    this._drawRegionRect(ctx, rect, isActive, isHovered, isResizing);
                }

                // draw profile cursors
                if( this.m_profileCursorX != null) {
                    var ptx = this._serverImage2mouse( { x: this.m_profileCursorX+1/2, y: 0}).x;
                    ctx.strokeStyle = "rgba(0,255,0,0.5)";
                    ctx.lineWidth = 1;
                    ctx.beginPath();
                    ctx.moveTo( ptx, 0);
                    ctx.lineTo( ptx, height);
                    ctx.stroke();
                }
                if( this.m_profileCursorY != null) {
                    var pty = this._serverImage2mouse( { x:0, y: this.m_profileCursorY+1/2}).y;
                    ctx.strokeStyle = "rgba(0,255,0,0.5)";
                    ctx.lineWidth = 1;
                    ctx.beginPath();
                    ctx.moveTo( 0, pty);
                    ctx.lineTo( width, pty);
                    ctx.stroke();

                }

                // draw frozen cursor
                if( this.m_frozenCursor != null) {
                    ctx.strokeStyle = "rgb(0,255,0)";
                    ctx.lineWidth = 2;
                    ctx.beginPath();
                    ctx.moveTo( this.m_frozenCursor.x - 10, this.m_frozenCursor.y);
                    ctx.lineTo( this.m_frozenCursor.x + 10, this.m_frozenCursor.y);
                    ctx.moveTo( this.m_frozenCursor.x, this.m_frozenCursor.y - 10);
                    ctx.lineTo( this.m_frozenCursor.x, this.m_frozenCursor.y + 10);
                    ctx.stroke();
                }
            },

            _updateMouseHoverStatus: function () {
                // if mouse is down, do nothing at all
                if( this.m_mouseDownPt !== null) { return; }
                this.m_mouseHoverInfo.rect = -1;
                this.m_mouseHoverInfo.part = "default";
                var margin = 5;
                if (this.m_lastMouse !== null && !this.m_isShift) {
                    var mx = this.m_lastMouse.x;
                    var my = this.m_lastMouse.y;
//                    var firstVisibleRectInd = -1;
                    this.m_regionRectangles.forEach(function (rect, ind) {
                        if (this.m_mouseHoverInfo.rect > -1) return;
                        if (!rect.visible) return;
//                        if( firstVisibleRectInd === -1) firstVisibleRectInd = ind;
                        var inXrange = mx > rect.mTl.x - margin && mx < rect.mBr.x + margin;
                        var inYrange = my > rect.mTl.y - margin && my < rect.mBr.y + margin;
                        var left = rect.mTl.x - mx < margin && rect.mTl.x - mx > -5 && inYrange;
                        var right = mx - rect.mBr.x < margin && mx - rect.mBr.x > -5 && inYrange;
                        var top = rect.mTl.y - my < margin && rect.mTl.y - my > -5 && inXrange;
                        var bottom = my - rect.mBr.y < margin && my - rect.mBr.y > -5 && inXrange;

                        var that = this;

                        function set(part) {
                            that.m_mouseHoverInfo.rect = ind;
                            that.m_mouseHoverInfo.part = part;
                        }
                        if (left && top) {
                            set("nw-resize");
                        }
                        else if (left && bottom) {
                            set("sw-resize");
                        }
                        else if (right && top) {
                            set("ne-resize");
                        }
                        else if (right && bottom) {
                            set("se-resize");
                        }
                        else if (left) {
                            set("w-resize");
                        }
                        else if (right) {
                            set("e-resize");
                        }
                        else if (top) {
                            set("n-resize");
                        }
                        else if (bottom) {
                            set("s-resize");
                        }
                        else if (inXrange && inYrange) {
                            set("move");
                        }
                    }, this);
/*
                    if( firstVisibleRectInd > -1 && this.m_mouseHoverInfo.rect === -1) {
                        this.m_mouseHoverInfo.rect = firstVisibleRectInd;
                        this.m_mouseHoverInfo.part = "crosshair";
                    }
*/
                    // if the mouse is not over any visible recangle, make the hover info to point
                    // to the first visible rectangle
                    if( this.m_mouseHoverInfo.rect === -1) {
                        this.m_mouseHoverInfo.rect = this._getFirstVisibleRectInd();
                        if( this.m_mouseHoverInfo.rect !== -1)
                            this.m_mouseHoverInfo.part = "crosshair";
                    }
                }
                if( ! this.m_touchDevice) {
                    this.setCursor(this.m_mouseHoverInfo.part);
                }
//                fv.console.log("hover info = ", this.m_mouseHoverInfo);
            },

            _drawRegionRect: function (ctx, rect, isActive, isHovered, isResizing) {

                // don't draw invisible rectangles
                if( ! rect.visible) return;

                var x1 = rect.mTl.x, x2 = rect.mBr.x, y1 = rect.mTl.y, y2 = rect.mBr.y;
                var w = x2 - x1, h = y2 - y1;

                // don't draw null rectangles
                if( w < 2 && h < 2) return;

                ctx.save();
//                ctx.globalAlpha = isHovered ? 1 : 0.7;

                var fontHeight = Math.round(Math.min( w, h));
                if( fontHeight < 6) fontHeight = 6;
                if( fontHeight > 10) fontHeight = 10;

                // draw the caption at the bottom
                {
                    ctx.save();
//                    ctx.font = "6pt Arial";
                    ctx.font = fontHeight.toString() + "pt Arial";
                    var metrics = ctx.measureText(rect.label);
                    metrics.height = fontHeight;
                    var margin = 4;
                    var tx = rect.mBr.x - metrics.width - margin;
                    var ty = rect.mBr.y + metrics.height + 2;
                    var tw = metrics.width + margin * 2;
                    var th = metrics.height + margin * 2;
                    ctx.shadowColor = "black";
                    ctx.shadowBlur = 3;
//                ctx.fillStyle = "rgba(255,0,0,0.5)";
//                ctx.fillRect( tx - margin, ty + margin, tw, - th);
                    ctx.fillStyle = "white";
                    ctx.fillText(rect.label, tx, ty);
                    ctx.restore();
                }

                if (isHovered) {
                    ctx.fillStyle = "rgba(255,255,255,0.1)";
                    ctx.fillRect(rect.mTl.x, rect.mTl.y, rect.mBr.x - rect.mTl.x, rect.mBr.y - rect.mTl.y);
                }

                if (isActive) {
                    ctx.strokeStyle = "rgb(0,255,0)";
                }
                else {
                    ctx.strokeStyle = "rgb(255,255,255)";
                }
//                ctx.lineWidth = isHovered ? 3 : 2;
                ctx.lineWidth = 2;
                ctx.strokeRect(rect.mTl.x, rect.mTl.y, rect.mBr.x - rect.mTl.x, rect.mBr.y - rect.mTl.y);

                // exagerate corners if hovered
                var cornerSize = Math.min(w / 2 - 2, h / 2 - 2, 7);
                if (isHovered && cornerSize > 3) {
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

                // draw the resize info
                if( isResizing) fv.lib.closure( function(){
                    ctx.save();
//                    ctx.font = "10pt Arial";
                    ctx.font = fontHeight.toString() + "pt Arial";
                    var txt = (rect.iBr.x - rect.iTl.x) + " x " + (rect.iBr.y - rect.iTl.y);
                    var metrics = ctx.measureText(txt);
                    metrics.height = fontHeight;
                    var cx = x1 + w/ 2, cy = y1 + h/2;
                    var tx = cx - metrics.width / 2 ;
                    var ty = cy + metrics.height / 2;
                    if( w > metrics.width && h > metrics.height) {
                        ctx.shadowColor = "black";
                        ctx.shadowBlur = 3;
                        ctx.fillStyle = "white";
                        ctx.fillText( txt, tx, ty);
                    }
                    ctx.restore();

                }, this);


                ctx.restore();
            },

            _getFirstVisibleRectInd: function() {
                var ind = -1;
                this.m_regionRectangles.forEach( function(r, i) {
                    if( ind > -1) return;
                    if(r.visible) ind = i;
                }, this);
                return ind;
            },

            _sendRegionSetMessage: function (rect) {
                this._emit( "ui.mainWindow.regionSet", {
                    regionId: rect.id,
                    x1: rect.iTl.x,
                    x2: rect.iBr.x-1,
                    y1: rect.iTl.y,
                    y2: rect.iBr.y-1
                });
            },

            _serverImage2mouse: function (pt) {
                if (this.m_tx === null) return pt;
                return this.m_tx.serverImage2mousef(pt);
            },

            _mouse2serverImage: function (pt) {
                if (this.m_tx === null) return pt;
                return this.m_tx.mouse2serverImagef(pt);
            },

            _mouse2serverImagei: function (pt) {
                var res = this._mouse2serverImage( pt);
                res.x = Math.floor( res.x);
                res.y = Math.floor( res.y);
                return res;
            },

            /**
             * returns mouse event's local position (with respect to this widget)
             */
            _localPos: function (event) {
                var box = this.getContentLocation("box");
                return {
                    x: event.getDocumentLeft() - box.left,
                    y: event.getDocumentTop() - box.top
                };
            },

            _mouseWheelCB: function (event) {
                if( this.m_touchDevice) return;
                var pt = this._localPos(event);
                var data = {
                    x: pt.x,
                    y: pt.y,
                    delta: -event.getWheelDelta(),
                    viewName: "MainView"
                };
                this._emit("ui.mainWindow.mouseWheel", data);
            },

            _touchStartCB: function (e) {
                fv.console.log( "touchstart");
                this.m_touchDevice = true;
                e.preventDefault();

                this.m_canBeClickEvent = false;
                this.m_rect = null;

                // make a copy of all touches
                var box = this.getContentLocation("box");
                this.m_touches = [];
                var a = e.getAllTouches();
                for( var i = 0 ; i < a.length ; i ++ ) {
                    this.m_touches[i] = {
                        x: e.getDocumentLeft(i) - box.left,
                        y: e.getDocumentTop(i) - box.top,
                        id: e.getIdentifier(i)
                    };
                }

                // deferred update
                this.update();

                // three finger tap will reset zoom
                if(e.getAllTouches().length > 2) {
                    this._emit("ui.mainWindow.zoomPreset", { zoom: -1 });
                    return;
                }

                // ignore two finger touch (for now)
                if(e.getAllTouches().length !== 1) {
                    return;
                }

                // single touch...
                this.m_canBeClickEvent = true;
                this.m_mouse = { x: this.m_touches[0].x, y: this.m_touches[0].y };
                this.m_lastMouse = { x: this.m_touches[0].x, y: this.m_touches[0].y };
                this.m_canBeClickEvent = this.m_mouseHoverInfo.rect == -1;
                this.m_mouseDownPt = null;
                this._updateMouseHoverStatus();

                this.m_mouseDownPt = { x: this.m_touches[0].x, y: this.m_touches[0].y };

                fv.console.log( "hoverrect=", this.m_mouseHoverInfo.rect);

                // if a region rectangle was clicked, make it top rectangle and remember its shape
                if (this.m_mouseHoverInfo.rect > -1) {
                    var r = qx.lang.Array.removeAt(this.m_regionRectangles, this.m_mouseHoverInfo.rect);
                    qx.lang.Array.insertAt(this.m_regionRectangles, r, 0);
                    fv.console.log( "r=", r);
                    this.m_mouseHoverInfo.rect = 0;
                    this.m_mouseDownRect = { x1: r.iTl.x, x2: r.iBr.x, y1: r.iTl.y, y2: r.iBr.y};
                }

                else {
                    // convert local widget coordinates to image coordinates
                    var pt = this.m_mouse;
                    var ipt = this._mouse2serverImagei( pt);
                    // remember the last mouse position
//                this.m_lastMouse = pt;
                    // tell everyone mouse have moved
                    this._emit("ui.mainWindow.mouseMove", {
                        x: pt.x,
                        y: pt.y,
                        imagex: ipt.x,
                        imagey: ipt.y,
                        viewName: "MainView"
                    });

                }

            },

            _touchEndCB: function(e) {
                fv.console.log( "touchend");
                this.m_touchDevice = true;
                e.preventDefault();

                fv.console.log( "touchend", e.getAllTouches(), e.getChangedTargetTouches());

                this.m_mouseHoverInfo.rect = -1;
                this.m_mouseHoverInfo.part = "default";

                // make a copy of all touches
                var box = this.getContentLocation("box");
                this.m_touches = [];
                var a = e.getAllTouches();
                for( var i = 0 ; i < a.length ; i ++ ) {
                    this.m_touches[i] = {
                        x: e.getDocumentLeft(i) - box.left,
                        y: e.getDocumentTop(i) - box.top,
                        id: e.getIdentifier(i)
                    };
                }

                // deferred update
                this.update();

                // only continue if a single finger was lifted, and there are no touches left
                if(e.getAllTouches().length !== 0 || e.getChangedTargetTouches().length !== 1) {
                    return;
                }

                // if we still think this can be a click event (I mean 'tap' event :), handle it here
                if( this.m_canBeClickEvent && this.m_mouseDownPt != null) {
                    this._emit("ui.mainWindow.mouseClick", {
                        x: this.m_mouseDownPt.x,
                        y: this.m_mouseDownPt.y,
                        viewName: "MainView"
                    });
                    this.m_rect = null;
                    this.m_mouseDownPt = null;
                    return;
                }

                this.m_mouseDownPt = null;

                // if there is no rectangle, do not continue either
                if( this.m_rect == null) {
                    return;
                }

                // send the zoom rectangle
                this._emit("ui.mainWindow.zoomRect", {
                    x1: Math.min(this.m_rect.x1, this.m_rect.x2),
                    y1: Math.min(this.m_rect.y1, this.m_rect.y2),
                    x2: Math.max(this.m_rect.x1, this.m_rect.x2),
                    y2: Math.max(this.m_rect.y1, this.m_rect.y2),
                    viewName: "MainView"
                });
                // zoom rectangle is now invisible
                this.m_rect = null;
            },

            _touchCancelCB: function(e) {
                fv.console.log( "touchcancel");
                this.m_touchDevice = true;
                e.preventDefault();

                this.m_rect = null;
                this.m_mouse = null;
                this.m_touches = [];
                this.m_mouseDownPt = null;

                this.update();
            },

            _touchMoveCB: function(e) {
                fv.console.log( "touchmove");
                this.m_touchDevice = true;
                e.preventDefault();

                // make a copy of all touches
                var box = this.getContentLocation("box");
                this.m_touches = [];
                var a = e.getAllTouches();
                for( var i = 0 ; i < a.length ; i ++ ) {
                    this.m_touches[i] = {
                        x: e.getDocumentLeft(i) - box.left,
                        y: e.getDocumentTop(i) - box.top,
                        id: e.getIdentifier(i)
                    };
                }

                // deferred update
                this.update();

                // if not exacly one touch, do nothing
                if(a.length !== 1) return;

                this.m_mouse = { x: this.m_touches[0].x, y: this.m_touches[0].y };
                this.m_lastMouse = { x: this.m_touches[0].x, y: this.m_touches[0].y };
                var pt = this.m_mouse;
                var ipt = this._mouse2serverImagei( pt);


                fv.console.log( "part = ", this.m_mouseHoverInfo.part);

                // if region rectangle is being moved/modified, modify it
                if (this.m_mouseHoverInfo.rect > -1) {
                    var rect = this.m_regionRectangles[ this.m_mouseHoverInfo.rect];
                    var c = qx.lang.Array.contains;
                    var part = this.m_mouseHoverInfo.part;
                    if( c( ["e-resize","ne-resize","se-resize"], part)) {
                        rect.iBr.x = Math.max( ipt.x+1, rect.iTl.x + 1);
                    }
                    if( c( ["w-resize","nw-resize","sw-resize"], part)) {
                        rect.iTl.x = Math.min( ipt.x, rect.iBr.x - 1);
                    }
                    if( c( ["n-resize","ne-resize","nw-resize"], part)) {
                        rect.iTl.y = Math.min( ipt.y, rect.iBr.y - 1);
                    }
                    if( c( ["s-resize","se-resize","sw-resize"], part)) {
                        rect.iBr.y = Math.max( ipt.y+1, rect.iTl.y + 1);
                    }
                    if( part === "move") {
                        var pt1 = this._mouse2serverImage( pt);
                        var pt2 = this._mouse2serverImage( this.m_mouseDownPt);
                        var dx = Math.round(pt2.x - pt1.x), dy = Math.round(pt2.y - pt1.y);
                        rect.iTl.x = this.m_mouseDownRect.x1 - dx;
                        rect.iBr.x = this.m_mouseDownRect.x2 - dx;
                        rect.iTl.y = this.m_mouseDownRect.y1 - dy;
                        rect.iBr.y = this.m_mouseDownRect.y2 - dy;
                    }
                    if( part === "crosshair") {
                        var pt1 = this._mouse2serverImage( pt);
                        var pt2 = this._mouse2serverImage( this.m_mouseDownPt);
                        var x1 = Math.min( Math.floor( pt1.x), Math.floor( pt2.x));
                        var x2 = Math.max( Math.floor( pt1.x+1), Math.floor( pt2.x+1));
                        var y1 = Math.min( Math.floor( pt1.y), Math.floor( pt2.y));
                        var y2 = Math.max( Math.floor( pt1.y+1), Math.floor( pt2.y+1));

                        rect.iTl.x = x1;
                        rect.iBr.x = x2;
                        rect.iTl.y = y1;
                        rect.iBr.y = y2;
                    }
                    this.update();
                    this._sendRegionSetMessage( rect);
                }

                // we are in zoom mode then
                else {
                    // if mouse is down, we check how far the user dragged it, and if it's more
                    // than some threshold, we know it's not a click, but a rectangle select
                    if (this.m_canBeClickEvent) {
                        var dx = this.m_mouseDownPt.x - this.m_touches[0].x;
                        var dy = this.m_mouseDownPt.y - this.m_touches[0].y;
                        var distSq = dx * dx + dy * dy;
                        if (distSq > 13) {
                            this.m_canBeClickEvent = false;
                        }
                    }

                    if (!this.m_canBeClickEvent) {
                        // update the selection rectangle if this cannot be click event
                        this.m_rect = {
                            x1: this.m_mouseDownPt.x,
                            y1: this.m_mouseDownPt.y,
                            x2: this.m_touches[0].x,
                            y2: this.m_touches[0].y
                        };
                        this.m_mouse = null;
                    }
                }
            },

            _mouseMoveCB: function (event) {
//                fv.console.log( "mousemove", event.getDocumentLeft(), event.getDocumentTop());

                if( this.m_touchDevice) return;
                // convert event to local widget coordinates
                var pt = this._localPos(event);
                // convert local widget coordinates to image coordinates
                var ipt = this._mouse2serverImagei( pt);
                // remember the last mouse position
                this.m_lastMouse = pt;

                // if this is not a drag event, just update the mouse hover info & send move event
                if( this.m_mouseDownPt === null) {
                    this._updateMouseHoverStatus();
                    this.m_mouse = pt;
                    this._emit("ui.mainWindow.mouseMove", {
                        x: pt.x,
                        y: pt.y,
                        imagex: ipt.x,
                        imagey: ipt.y,
                        viewName: "MainView"
                    });
                    this.update();
                }

                // if this is a right mouse button drag, we are doing interactive zoom
                else if( event.getButton() === "right") {
                    // TODO: fix this please, this is just a performance test to see
                    // if interactive zoom would perform ok
//                    var d = (previousLastMouse.y - pt.y)/2;
//                    this._emit("ui.mainWindow.mouseWheel", {
//                        x: pt.x,
//                        y: pt.y,
//                        delta: d,
//                        viewName: "MainView"
//                    });

                    // send the vertical difference between initial point and current point
                    var d = pt.y - this.m_mouseDownPt.y;
                    this.m_vars.iZoomSet.set( "" + d);
                }

                // if region rectangle is being moved/modified, modify it
                else if (this.m_mouseHoverInfo.rect > -1) {
                    var rect = this.m_regionRectangles[ this.m_mouseHoverInfo.rect];
                    var c = qx.lang.Array.contains;
                    var part = this.m_mouseHoverInfo.part;
                    if( c( ["e-resize","ne-resize","se-resize"], part)) {
                        rect.iBr.x = Math.max( ipt.x+1, rect.iTl.x + 1);
                    }
                    if( c( ["w-resize","nw-resize","sw-resize"], part)) {
                        rect.iTl.x = Math.min( ipt.x, rect.iBr.x - 1);
                    }
                    if( c( ["n-resize","ne-resize","nw-resize"], part)) {
                        rect.iTl.y = Math.min( ipt.y, rect.iBr.y - 1);
                    }
                    if( c( ["s-resize","se-resize","sw-resize"], part)) {
                        rect.iBr.y = Math.max( ipt.y+1, rect.iTl.y + 1);
                    }
                    if( part === "move") {
                        var pt1 = this._mouse2serverImage( pt);
                        var pt2 = this._mouse2serverImage( this.m_mouseDownPt);
                        var dx = Math.round(pt2.x - pt1.x), dy = Math.round(pt2.y - pt1.y);
                        rect.iTl.x = this.m_mouseDownRect.x1 - dx;
                        rect.iBr.x = this.m_mouseDownRect.x2 - dx;
                        rect.iTl.y = this.m_mouseDownRect.y1 - dy;
                        rect.iBr.y = this.m_mouseDownRect.y2 - dy;
                    }
                    if( part === "crosshair") {
                        var pt1 = this._mouse2serverImage( pt);
                        var pt2 = this._mouse2serverImage( this.m_mouseDownPt);
                        var x1 = Math.min( Math.floor( pt1.x), Math.floor( pt2.x));
                        var x2 = Math.max( Math.floor( pt1.x+1), Math.floor( pt2.x+1));
                        var y1 = Math.min( Math.floor( pt1.y), Math.floor( pt2.y));
                        var y2 = Math.max( Math.floor( pt1.y+1), Math.floor( pt2.y+1));

                        rect.iTl.x = x1;
                        rect.iBr.x = x2;
                        rect.iTl.y = y1;
                        rect.iBr.y = y2;
                    }
                    this.update();
                    this._sendRegionSetMessage( rect);
                }

                // we are in zoom mode then
                else {
                    // if mouse is down, we check how far the user dragged it, and if it's more
                    // than some threshold, we know it's not a click, but a rectangle select
                    if (this.m_canBeClickEvent) {
                        var dx = this.m_mouseDownPt.x - pt.x;
                        var dy = this.m_mouseDownPt.y - pt.y;
                        var distSq = dx * dx + dy * dy;
                        if (distSq > 13) {
                            this.m_canBeClickEvent = false;
                        }
                    }
                    if (!this.m_canBeClickEvent) {
                        this.m_rect = {
                            x1: this.m_mouseDownPt.x,
                            y1: this.m_mouseDownPt.y,
                            x2: pt.x,
                            y2: pt.y
                        };
                        this.m_mouse = null;
                    }
                    this.update();
                }

            },

            _mouseDownCB: function (event) {
                if( this.m_touchDevice) return;
                this.capture();
                var pt = this._localPos(event);
                this.m_lastMouse = pt;
                this.m_mouseDownPt = pt;
                this.m_canBeClickEvent = false;

                // if this is the right mouse button, tell the server
                if( event.getButton() === "right") {
//                    this._emit("togs.directCommand", {
//                        cmd: "mainWindow.iZoom.setInitialPoint",
//                        x: pt.x,
//                        y: pt.y
//                    });
                    fv.console.log("cmd iZoom");
//                    fv.sendCommand( "mainWindow.iZoom.setInitialPoint", "" + pt.x + "_" + pt.y);
                    this.m_vars.iZoomInit.set( "" + pt.x + " " + pt.y);
//                    fv.sendCommand( "mainWindow.iZoom.setInitialPoint2", "" + pt.x + " " + pt.y);
                }
                else if( event.getButton() === "left") {
                    this.m_canBeClickEvent = this.m_mouseHoverInfo.rect == -1;

                    this._updateMouseHoverStatus();
                    // if a region rectangle was clicked, make it top rectangle and remember its shape
                    if (this.m_mouseHoverInfo.rect > -1) {
                        var r = qx.lang.Array.removeAt(this.m_regionRectangles, this.m_mouseHoverInfo.rect);
                        qx.lang.Array.insertAt(this.m_regionRectangles, r, 0);
                        this.m_mouseHoverInfo.rect = 0;
                        this.m_mouseDownRect = { x1: r.iTl.x, x2: r.iBr.x, y1: r.iTl.y, y2: r.iBr.y};
                    }
                }

                this.update();
            },

            _mouseUpCB: function (event) {
                if( this.m_touchDevice) return;
                this.releaseCapture();
                var pt = this._localPos(event);
                this.m_mouseDownPt = null;
                this.m_mouse = pt;

                if( event.getButton() !== "left") {
                    return;
                }

                // if we are not modifying region rectangle, either send click event or
                // zoom rectangle event
                if (this.m_mouseHoverInfo.rect == -1) {
                    // if this can be a click, send the click message
                    if (this.m_canBeClickEvent) {
                        this._emit("ui.mainWindow.mouseClick", {
                            x: pt.x,
                            y: pt.y,
                            button: event.getButton(),
                            viewName: "MainView"
                        });
                    }
                    // if this is not click event, send zoom rect message
                    else {
                        this._emit("ui.mainWindow.zoomRect", {
                            x1: Math.min(this.m_rect.x1, this.m_rect.x2),
                            y1: Math.min(this.m_rect.y1, this.m_rect.y2),
                            x2: Math.max(this.m_rect.x1, this.m_rect.x2),
                            y2: Math.max(this.m_rect.y1, this.m_rect.y2),
                            viewName: "MainView"
                        });
                    }
                    // zoom rectangle is now invisible
                    this.m_rect = null;
                }


                this.update();
            },

            _keyDownCB: function (event) {

                if( event.getKeyCode() == 32) {
                    this._emit( "ui.mainWindow.keyDown", {
                        keyCode: event.getKeyCode(),
                        mouseX: this.m_mouse.x,
                        mouseY: this.m_mouse.y
                    });
                    return;
                }
                /*
                 if ( event.keyCode == 32 )
                 {
                 var mp:Point = new Point();
                 if ( stage != null )
                 {
                 mp = view.localToImage( view.globalToLocal( new Point( stage.mouseX, stage.mouseY ) ) );
                 }
                 Framework.instance.client.queueCommand( view.viewName + "-keyDown",
                 { keyCode : event.keyCode, charCode: event.charCode,
                 mouseX: mp.x, mouseY: mp.y} );
                 event.preventDefault();
                 event.stopImmediatePropagation();
                 }

                 */
                if (event.getKeyIdentifier() === "Shift") {
                    this.m_isShift = true;
                }
                this._updateMouseHoverStatus();
                this.update();
            },

            _keyUpCB: function (event) {
                if (event.getKeyIdentifier() === "Shift") {
                    this.m_isShift = false;
                }
                this._updateMouseHoverStatus();
                this.update();
            },

            m_mouse: null,
            m_rect: null,
            m_mouseDownPt: null,
            m_canBeClickEvent: true

        },

        properties: {

        }

    });

