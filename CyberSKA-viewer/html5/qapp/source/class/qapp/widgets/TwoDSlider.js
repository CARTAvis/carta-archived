/**
 * This widget allows selection of two values each in the range [-1,1]
 *
 */

/* global qx,fv, fv.console */

/**

 @ignore(fv.console.log)
 @ignore(fv.assert)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(qx)

 ************************************************************************ */

qx.Class.define("qapp.widgets.TwoDSlider",
    {
        extend: qx.ui.embed.Canvas,

        construct: function (hub) {
            this.base(arguments);

            this.setSyncDimension( true);

            this.m_hub = hub;

            this.m_mouse = null;

            this.m_currentValue = { x: 0, y: 0};

            // subscribe to parsed state
            this.m_hub.subscribe("toui.parsedState", function (val) {
                fv.console.log("parsedState");
                this.m_parsedStateRef = val;
                this.updateFromState();
            }, this);

            // let the controller know we can receive parsed state updates
            this.emit("ui.parsedStateRequest");

            // add mouse listeners
            this.addListener("mousemove", this._mouseMoveCB);
            this.addListener("mousedown", this._mouseDownCB);
            this.addListener("mouseup", this._mouseUpCB);
            this.addListener("keyup", this._keyDownCB);
            this.addListener("mouseover", function(){
                fv.GLOBAL_DEBUG && fv.console.log( "mouse in");
                this.m_mouseIn = true;
                this.activate();
                this.update();
            }, this);
            this.addListener("mouseout", function(){
                fv.GLOBAL_DEBUG && fv.console.log( "mouse out");
                this.deactivate();
                this.m_mouseIn = false;
                this.update();
            }, this);

            this.update();
        },

        events : {
          /**
           * Change event for the value.
           */
          changeValue: 'qx.event.type.Data'
        },


        members: {

            getValue: function() {
                return this.m_currentValue;
            },

            m_hub: null,
            m_parsedStateRef: null,

            emit: function (path, data) {
                this.m_hub.emit(path, data);
            },

            updateFromState: function () {
                fv.assert(this.m_parsedStateRef !== null, "parsed state not set!");
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

                this._ttUpdate( width, height);

                // draw background
//                ctx.fillStyle = "#ffffff";
//                ctx.fillRect(0, 0, width, height);
                ctx.clearRect(0, 0, width, height);

                // draw the grid
                ctx.strokeStyle = this.GRID_COLOR;
                ctx.lineWidth = this.GRID_WIDTH;
                ctx.beginPath();
                var xi, yi, nx = 11, ny = 11, x, y;
                for( xi = 0 ; xi < nx ; xi ++ ) {
                    x = xi / (nx-1) * 2 - 1;
/*
                    ctx.moveTo( this._ttx(x), this._tty(-1));
                    ctx.lineTo( this._ttx(x), this._tty(1));
*/
                    ctx.moveTo( Math.round(this._ttx(x))+0.5, this._tty(-1));
                    ctx.lineTo( Math.round(this._ttx(x))+0.5, this._tty(1));
                }
                for( yi = 0 ; yi < ny ; yi ++ ) {
                    y = yi / (ny-1) * 2 - 1;
/*
                    ctx.moveTo( this._ttx(-1), this._tty(y));
                    ctx.lineTo( this._ttx(1), this._tty(y));
*/
                    ctx.moveTo( Math.round(this._ttx(-1))+0.5, this._tty(y));
                    ctx.lineTo( Math.round(this._ttx(1))+0.5, this._tty(y));
                }
                ctx.stroke();
                ctx.strokeStyle = this.GRID_COLOR2;
                ctx.beginPath();
                ctx.moveTo( Math.round(this._ttx(0))+0.5, this._tty(-1));
                ctx.lineTo( Math.round(this._ttx(0))+0.5, this._tty(1));
                ctx.moveTo( Math.round(this._ttx(-1))+0.5, this._tty(0));
                ctx.lineTo( Math.round(this._ttx(1))+0.5, this._tty(0));
                ctx.stroke();

                // draw the current value
                ctx.fillStyle = "rgba(255,0,0,0.5)";
                ctx.strokeStyle = "rgba(255,0,0,1)";
                ctx.lineWidth = 1;
                ctx.beginPath();
                ctx.arc( this._ttx( this.m_currentValue.x), this._tty( this.m_currentValue.y),
                    5, 0, 2 * Math.PI, false);
                ctx.fill();
                ctx.stroke();

                ctx.font = "8pt Arial";
                if( this.m_mouseIn) {
                    ctx.fillStyle = "rgba( 0, 0, 0, 0.9)";
                } else {
                    ctx.fillStyle = "rgba( 0, 0, 0, 0.0)";
                }
                ctx.fillText( "Right click or ESC to reset.", this.MARGIN+2, height - this.MARGIN-2);


/*
                if (this.m_mouse !== null) {
                    ctx.strokeStyle = "rgb(0,255,0)";
                    ctx.lineWidth = 1.5;
                    ctx.strokeRect(this.m_mouse.x - 5, this.m_mouse.y - 5, 10, 10);
                }
*/
            },

            /**
             * returns mouse event's local position (with respect to this widget)
             * @param event {MouseEvent}
             */
            _localPos: function (event) {
                var box = this.getContentLocation("box");
                return {
                    x: event.getDocumentLeft() - box.left,
                    y: event.getDocumentTop() - box.top
                };
            },

            /**
             * Updates ttya, ttyb, ttxa and ttxb based on width/height
             * @param width {Number}
             * @param height {Number}
             */
            _ttUpdate: function( width, height) {
                this.m_ttxa = width / 2 - this.MARGIN;
                this.m_ttxb = width / 2;
                this.m_ttya = this.MARGIN - height / 2;
                this.m_ttyb = height / 2;
            },

            /**
             * Translate x from model coordinates to widget coordinates.
             * @param x {Number}
             */
            _ttx: function( x) {
                return x * this.m_ttxa + this.m_ttxb;
            },
            _tty: function( y) {
                return y * this.m_ttya + this.m_ttyb;
            },
            _ttxinv: function( x) {
                return (x - this.m_ttxb) / this.m_ttxa;
            },
            _ttyinv: function( y) {
                return (y - this.m_ttyb) / this.m_ttya;
            },

            _mouseMoveCB: function (event) {
                var pt = this._localPos(event);
                this.m_mouse = pt;

                // if the mouse is down, adjust the values
                if (this.m_mouseDownPt !== null) {
                    this.m_currentValue.x = this._ttxinv( pt.x);
                    this.m_currentValue.y = this._ttyinv( pt.y);
                    if( this.m_currentValue.x < -1) this.m_currentValue.x = -1;
                    if( this.m_currentValue.x > 1) this.m_currentValue.x = 1;
                    if( this.m_currentValue.y < -1) this.m_currentValue.y = -1;
                    if( this.m_currentValue.y > 1) this.m_currentValue.y = 1;

                    this.fireEvent( "changeValue",
                        qx.event.type.Data,
                        {x: this.m_currentValue.x, y: this.m_currentValue.y });
                }

                this.update();
            },

            _mouseDownCB: function (event) {
                fv.GLOBAL_DEBUG && fv.console.log("mouse down in canvas - button ", event.getButton());

                if( event.getButton() === "right") {
                    this.m_mouseDownPt = null;
                    this._setValue( 0, 0);
                    return;
                } else {

                    this.capture();
                    var pt = this._localPos(event);
                    this.m_mouseDownPt = pt;
                    this.m_mouse = pt;

                    this.m_currentValue.x = this._ttxinv( pt.x);
                    this.m_currentValue.y = this._ttyinv( pt.y);
                    if( this.m_currentValue.x < -1) this.m_currentValue.x = -1;
                    if( this.m_currentValue.x > 1) this.m_currentValue.x = 1;
                    if( this.m_currentValue.y < -1) this.m_currentValue.y = -1;
                    if( this.m_currentValue.y > 1) this.m_currentValue.y = 1;
                }

                this.fireEvent( "changeValue",
                    qx.event.type.Data,
                    {x: this.m_currentValue.x, y: this.m_currentValue.y });

                this.update();
            },

            _mouseUpCB: function (event) {
                this.releaseCapture();
                var pt = this._localPos(event);
                this.m_mouseDownPt = null;
                this.m_mouse = pt;

                this.update();
            },

            _keyDownCB: function (event) {
                fv.console.log("keydown " + event.getKeyIdentifier() + ' ' + event.getKeyCode());
                if( event.getKeyCode() == 27) {
                    this._setValue( 0, 0);
                }
            },

            _setValue: function ( x, y) {
                this.m_currentValue.x = x;
                this.m_currentValue.y = y;
                this.fireEvent( "changeValue",
                    qx.event.type.Data,
                    {x: this.m_currentValue.x, y: this.m_currentValue.y });
                this.update();
            },

            m_mouse: null,
            m_mouseDownPt: null,
            m_currentValue: null,
            m_mouseIn: false,
            m_ttxa: 1, m_ttxb: 1, m_ttya: 1, m_ttyb: 1,
            GRID_COLOR: "rgba(0,0,64,0.2)",
            GRID_COLOR2: "rgba(0,0,128,0.7)",
            GRID_WIDTH: 1,
            MARGIN: 5
        },

        statics : {
        },

        properties: {

        }

    });

