/**
 * This widget is a shape selector overlay over top of the main view area. It allows
 * the user to interactively draw and select shapes.
 *
 */

/* global mImport, qx, skel */
/* jshint strict: false */

/**
 @ignore( mImport)
 ************************************************************************ */

qx.Class.define("skel.widgets.Draw.Canvas",
    {
        extend: qx.ui.embed.Canvas,

        construct: function (winId, view) {
        
            this.base(arguments);
            this.setSyncDimension(true);
            
            this.m_mouse = null;
            this.m_view = view;
            this.m_touchDevice = false;
            this.m_drawMode = this.m_drawModes.ZOOM;
            this.m_winId = winId;
            this.m_connector = mImport( "connector");
            
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
            qx.event.message.Bus.subscribe("shapeChanged", function( message) {
                this.update();
            }, this);

            this.m_isShift = false;
            this.m_mouseIn = false;
            this.m_tx = null;

            this.m_frozenCursor = null; // in mouse/screen coordinates

            this.m_drawRegion = new skel.widgets.Draw.Regions( winId);
            var drawMode = {
                        shape : "Rectangle",
                        multiShape : false
                    };
            this.setDrawMode( drawMode );
            
            qx.event.message.Bus.subscribe("mainOffsetsChanged", function(message) {
                var data = message.getData();
                this.m_offsetX = data.offsetX;
                this.m_offsetY = data.offsetY;
            }, this);
        },

        members: {
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
                this._updateMouseHoverStatus( this.m_mouse );
                this.m_drawRegion._draw( width, height, ctx );

                // draw frozen cursor
                if( this.m_frozenCursor !== null) {
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

            _keyDownCB: function (event) {

                if( event.getKeyCode() === 32) {
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
            

            /**
             * Callback for mouse move events.
             */
            _mouseMoveCB: function (event) {
                if( this.m_touchDevice)  { return; }
                // convert event to local widget coordinates
                var pt = skel.widgets.Util.localPos(this, event);

                // if this is not a drag event, just update the mouse location & tell the view
                if( ! this.m_drawRegion.isMouseDown() ) {
                    this.m_mouse = pt;
                    if ( this.m_view !== null ){
                        event.pageX = pt.x - this.m_offsetX;
                        event.pageY = pt.y + this.m_offsetY;
                        // generating fake events is not that simple :(
                        // We could probably fudge it to work with
                        // desktop connector where we control the sources, but I doubt it would
                        // be easy to do with pureweb...
                        //this.m_view._mouseMoveCB(event);
                    }
                }
                else {
                    this.m_drawRegion._mouseMoveCB( pt );
                    
                }
                this.update();
            },
            
            
            /**
             * Callback for the mouse down event.
             */
            _mouseDownCB: function (event) {
                if( this.m_touchDevice) return;
                this.capture();
                var pt = skel.widgets.Util.localPos(this, event);
                if( event.getButton() === "left") {
                    this.m_drawRegion._mouseDownCB( pt );
                }
                this.update();
            },

            /**
             * Callback for the mouse up event.
             */
            _mouseUpCB: function (event) {
                if( this.m_touchDevice) return;
                this.releaseCapture();
                var pt = skel.widgets.Util.localPos(this, event);
                if( event.getButton() !== "left") {
                    return;
                }

                // if we are not modifying region rectangle, either send click event or
                // zoom rectangle event
                this.m_drawRegion._mouseUpCB( pt );
                this.update();
            },
            
            _mouseWheelCB: function (event) {
                if( this.m_touchDevice) return;
                var pt = skel.widgets.Util.localPos(this, event);
                var data = {
                    x: pt.x,
                    y: pt.y,
                    delta: -event.getWheelDelta(),
                    viewName: "MainView"
                };
                this._emit("ui.mainWindow.mouseWheel", data);
            },


            setDrawMode: function( drawInfo ){
                this.m_drawMode = this.m_drawModes.SHAPE;
                this.m_drawRegion.setDrawMode( drawInfo );
            },
            
            setFrozenCursor: function ( x, y) {
                if( x === null)
                    this.m_frozenCursor = null;
                else
                    this.m_frozenCursor = { x: x, y: y};
                this.update();
                
            },
            
            /**
             * Update the cursor based on its hover location relative to shapes.
             * @param pt {Object} the {x,y} position of the mouse.
             */
            _updateMouseHoverStatus: function ( pt ) {
                // if mouse is down, do nothing at all
                if( this.m_drawRegion.isMouseDown() ) { 
                    return; 
                }
                var mouseHoverInfo = this.m_drawRegion.updateMouseHoverStatus( pt );
                this.setCursor( mouseHoverInfo);
            },

            m_mouse: null,

            m_drawModes : {
                ZOOM: 0,
                SHAPE: 1
            },
            m_view : null,
            m_offsetX : 0,
            m_offsetY : 0,
            m_drawMode : -1,
            m_zoomRect : null,
            m_drawRegion : null,
            m_connector: null,
            m_winId : ""
        }


    });

