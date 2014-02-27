/**
 * This widget is a rectangle selector overlay over top of the main view area. It allows
 * the user to interactively select rectangles. It is tied to the system via the hub.
 *
 */

/* global qx,fv, fv.console */

/**

 @ignore(fv.console.*)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(fv.assert)
 @ignore(fv.lib.now)

 ************************************************************************ */

qx.Class.define("qapp.widgets.GrSlider",
    {
        extend: qx.ui.embed.Canvas,

        construct: function () {
            this.base(arguments);
            this.setSyncDimension(true);

            this.m_mouse = null;

            // add mouse listeners
            this.addListener("mousemove", this._mouseMoveCB);
            this.addListener("mousedown", this._mouseDownCB);
            this.addListener("mouseup", this._mouseUpCB);
            this.addListener("mousewheel", this._mouseWheelCB);
            this.addListener("keyup", this._keyDownCB);
            this.addListener("mouseover", this._mouseOverCB);
            this.addListener("mouseout", this._mouseOutCB);

            this.addListener("touchmove", this._touchMoveCB);
            this.addListener("touchend", this._touchEndCB);
            this.addListener("touchcancel", this._touchCancelCB);
            this.addListener("touchstart", this._touchStartCB);

            // measure the last interaction
            this.m_lastInterraction = fv.lib.now() - 10000000;

            // delay timer for applying value in case it was set but the user was recently
            // interacting
            this.m_timer = new qx.event.Timer();
            this.m_timer.addListener("interval", this._timerCB, this);

            // real current value
            this.m_currentValue = 50;
            this.m_shadowValue = 25;
            this.m_minimum = 0;
            this.m_maximum = 100;
            this.m_kvisStyle = false;
            this.MIN_BAR_LENGTH = 2;
            this.KVIS_BAR_WIDTH = 5;
            this.m_touchDevice = false;
            this.m_touches = [];
        },

        members: {


            _updateValue: function (val) {
//                fv.GLOBAL_DEBUG && fv.console.log("_updateValue: ", val);
                var newVal = val;
                // round it
                newVal = Math.round(newVal);
                // clamp it
                if (newVal < this.m_minimum) newVal = this.m_minimum;
                if (newVal > this.m_maximum) newVal = this.m_maximum;

                var oldVal = this.m_currentValue;

                this.m_currentValue = newVal;
                this.m_lastInterraction = fv.lib.now();
                // move the mouse cursor to the value
                this.m_mouse = { x: this._val2mousef( this.m_currentValue), y: 0 };
                this.update();
                this.fireEvent("changeValue", qx.event.type.Data, [ newVal, oldVal]);
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

                if (this.m_kvisStyle)
                    this._drawKvisStyle(width, height, ctx);
                else
                    this._drawRegularStyle(width, height, ctx);

            },

            _drawKvisStyle: function (width, height, ctx) {

                var textHeight = 10;
                var bottomMargin = textHeight + 4;
                var bw = this.KVIS_BAR_WIDTH;
                var h1 = (height - bottomMargin) * 0.75;
                var h2 = (height - bottomMargin) - h1;
                var x;

                ctx.clearRect(0, 0, width, height);
                ctx.fillStyle = "black";
                ctx.fillRect(0, 0, width, height - bottomMargin);


                // draw the current value
                x = this._val2mousef(this.m_currentValue);
                ctx.fillStyle = "rgb(255,0 ,0)";
                ctx.fillRect(x - bw / 2, 0, bw, h1);

                // draw the shadow value (with different shade if different from current value)
                ctx.save();
                if (this.m_shadowValue !== this.m_currentValue)
                    ctx.setAlpha(0.5);
                x = this._val2mousef(this.m_shadowValue);
                ctx.fillRect(x - bw / 2, h1, bw, h2);
                ctx.restore();


                // draw the cursor & value text
                if (this.m_mouse !== null) {
                    var val = this._mouse2vali(this.m_mouse.x);
                    ctx.fillStyle = "rgba(255,255,255,0.5)";
                    ctx.fillRect(this._val2mousef(val) - 0.5, 0, 1, height);
                    ctx.font = "10pt Arial";
                    ctx.fillStyle = "rgba(128,0,0,1)";
                    var text = "" + val;
                    var metrics = ctx.measureText(text);
                    var tx = this.getCanvasWidth() - metrics.width - 2;
                    tx = this.m_mouse.x - metrics.width / 2;
                    var ty = this.getCanvasHeight() - 2;
                    if (tx + metrics.width > this.getCanvasWidth() - 2)
                        tx = this.getCanvasWidth() - 2 - metrics.width;
                    if (tx < 2) tx = 2;
                    ctx.fillText(text, tx, ty);
                }
            },

            _drawRegularStyle: function (width, height, ctx) {

                var textHeight = 10;
                var bottomMargin = textHeight + 4;
                var h1 = (height - bottomMargin) * 0.75;
                var h2 = (height - bottomMargin) - h1;
                var bLen;

                ctx.clearRect(0, 0, width, height);
                ctx.fillStyle = "rgba(0,0,0,0.3)";
                ctx.fillRect(0, 0, width, height - bottomMargin);


                // draw the current value
                ctx.fillStyle = "#0C88F6";
                bLen = this._val2mousef(this.m_currentValue);
                ctx.fillRect(0, 0, bLen, h1);

                // draw the shadow value (with different shade if different from current value)
                ctx.save();
                if (this.m_shadowValue !== this.m_currentValue)
                    ctx.setAlpha(0.5);
                bLen = this._val2mousef(this.m_shadowValue);
                ctx.fillRect(0, h1, bLen, h2);
                ctx.restore();

                // draw the cursor & value text
                if (this.m_mouse !== null) {
                    var val = this._mouse2vali(this.m_mouse.x);
                    ctx.fillStyle = "rgba(255,255,255,0.5)";
                    ctx.fillRect(this._val2mousef(val) - 0.5, 0, 1, height);
                    ctx.font = "10pt Arial";
                    ctx.fillStyle = "#155792";
                    var text = "" + val;
                    var metrics = ctx.measureText(text);
                    var tx = this.getCanvasWidth() - metrics.width - 2;
                    tx = this.m_mouse.x - metrics.width / 2;
                    var ty = this.getCanvasHeight() - 2;
                    if (tx + metrics.width > this.getCanvasWidth() - 2)
                        tx = this.getCanvasWidth() - 2 - metrics.width;
                    if (tx < 2) tx = 2;
                    ctx.fillText(text, tx, ty);
                }
            },

            /**
             * Convert value to mouse coordinates
             * @param val {Number}
             */
            _val2mousef: function (val) {
                var res;
                var ratio = (val - this.m_minimum)
                    / (this.m_maximum - this.m_minimum);
                if (this.m_kvisStyle) {
                    res = (ratio * (this.getCanvasWidth() - this.KVIS_BAR_WIDTH)
                        + this.KVIS_BAR_WIDTH / 2);

                } else {
                    res = ratio * (this.getCanvasWidth() - this.MIN_BAR_LENGTH)
                        + this.MIN_BAR_LENGTH;
                }
                return res;
            },

            /**
             * Convert mouse coordinate x to value (unclipped and fractional)
             * @param x {Number}
             */
            _mouse2valf: function (x) {
                var ratio, res;
                if (this.m_kvisStyle) {
                    ratio = (x - this.KVIS_BAR_WIDTH / 2)
                        / (this.getCanvasWidth() - this.KVIS_BAR_WIDTH);
                    res = ratio * (this.m_maximum - this.m_minimum) + this.m_minimum;
                } else {
                    ratio = (x - this.MIN_BAR_LENGTH)
                        / (this.getCanvasWidth() - this.MIN_BAR_LENGTH);
                    res = ratio * (this.m_maximum - this.m_minimum) + this.m_minimum;
                }
                return res;
            },

            /**
             * Convert mouse coordinates to value (clipped and integer)
             * @param x {Number}
             */
            _mouse2vali: function (x) {
                var res = this._mouse2valf(x);
                res = Math.round(res);
                if (res < this.m_minimum) res = this.m_minimum;
                if (res > this.m_maximum) res = this.m_maximum;
                return res;
            },


            /**
             * returns mouse event's local position (with respect to this widget)
             * @param event {MouseEvent}
             * @private
             */
            _localPos: function (event) {
                var box = this.getContentLocation("box");
                return {
                    x: event.getDocumentLeft() - box.left,
                    y: event.getDocumentTop() - box.top
                };
            },

            _mouseMoveCB: function (event) {
                var pt = this._localPos(event);
                this.m_mouse = pt;
                if (this.m_isDragging) {
                    this._updateValue(this._mouse2vali(pt.x));
                }
                this.update();
            },

            _mouseDownCB: function (event) {
//                fv.GLOBAL_DEBUG && fv.console.log("mousedown " + event.getButton());
                this.capture();
                var pt = this._localPos(event);
                if (this.m_kvisStyle) {
                    this.m_isDragging = event.getButton() === "middle";
                    if (event.getButton() === "left") {
                        this._updateValue(this.m_currentValue - 1);
                    }
                    else if (event.getButton() === "right") {
                        this._updateValue(this.m_currentValue + 1);
                    }
                    else if (event.getButton() === "middle") {
                        this._updateValue(this._mouse2vali(pt.x));
                    }
                }
                else {
                    this.m_isDragging = true;
                    this._updateValue(this._mouse2vali(pt.x));
                }

                this.update();
            },

            _mouseUpCB: function (event) {
                this.releaseCapture();
                this.m_isDragging = false;
                this._updateValue(this.m_currentValue);
                this.update();
            },

            _touchBoiler: function(e) {
                this.m_touchDevice = true;
                e.preventDefault();
                this.update();

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
            },

            _touchStartCB: function(e) {
                this._touchBoiler(e);

                if( this.m_touches.length !== 1) return;

                this._updateValue(this._mouse2vali( this.m_touches[0].x));
            },

            _touchMoveCB: function(e) {
                this._touchBoiler(e);

                if( this.m_touches.length !== 1) return;
                this._updateValue(this._mouse2vali( this.m_touches[0].x));
            },

            _touchEndCB: function(e) {
                this._touchBoiler(e);

                if( this.m_touches.length !== 0) return;
            },

            _touchCancelCB: function(e) {
                this._touchBoiler(e);
            },

            _keyDownCB: function (event) {
                fv.GLOBAL_DEBUG && fv.console.log("keydown "
                    + event.getKeyIdentifier() + ' ' + event.getKeyCode());
                if( event.getKeyIdentifier() === "Left" || event.getKeyIdentifier() === "-")
                    this._updateValue(this.m_currentValue - 1);
                else if( event.getKeyIdentifier() === "Right" || event.getKeyIdentifier() === "+")
                    this._updateValue(this.m_currentValue + 1);
                else if( event.getKeyIdentifier() === "PageDown")
                    this._updateValue(this.m_currentValue + 10);
                else if( event.getKeyIdentifier() === "PageUp")
                    this._updateValue(this.m_currentValue - 10);
            },

            _mouseOverCB: function (event) {
                this.activate();
            },

            _mouseOutCB: function (event) {
                this.deactivate();
                this.m_mouse = null;
                this.update();
            },

            _mouseWheelCB: function (event) {
//                fv.GLOBAL_DEBUG && fv.console.log("wheel %O %O", event.getWheelDelta("x"), event.getWheelDelta("y"));
                if( event.getWheelDelta() > 0)
                    this._updateValue(this.m_currentValue - 1);
                else
                    this._updateValue(this.m_currentValue + 1);
            },

            _timerCB: function () {
                fv.GLOBAL_DEBUG && fv.console.log("timerCB: ", this.m_shadowValue);
                this.setValue( this.m_shadowValue);
            },

            setValue: function (val) {
                fv.GLOBAL_DEBUG && fv.console.log("setValue(%O)", val);
                this.m_shadowValue = val;

                this.m_timer.stop();
                // if shadow value is the same as current value, we are done
                if( this.m_shadowValue === this.m_currentValue) {
                    this.update();
                    return;
                }
                // if last interraction has been too long ago, set the value immediately
                var rem = this.m_lastInterraction + 1000 - fv.lib.now();
                if (rem <= 0 && ! this.m_isDragging) {
                    this.m_currentValue = val;
                    this.update();
                    return;
                }
                // set the timer to try again later
                this.m_timer.startWith( rem);
                this.update();
            },

            getValue: function () {
                return this.m_currentValue;
            },

            setMinimum: function (val) {
                this.m_minimum = val;
            },

            setMaximum: function (val) {
                this.m_maximum = val;
            },

            /**
             *
             * @param val {Boolean}
             */
            setKvisStyle: function (val) {
                this.m_kvisStyle = val;
                this.update();
            },

            m_mouse: null,
            m_isDragging: false

        },

        properties: {

        },

        events: {
            /**
             * Change event for the value.
             */
            changeValue: 'qx.event.type.Data'

        }


    });

