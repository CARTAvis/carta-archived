/**
 * Drag event handler.
 *
 * Adds tap,hover and drag type events to VGView
 *
 * Note: capable of sending double tap without sending tap first... I am not sure if I like
 * this because a single tap is delayed as a result of this. Maybe this type of
 * differentiation should be handled by the receiver if that's the behaviour wanted.
 *
 * \todo make this a subclass of some common class (e.g. InputEventBase) and put common
 * functionality there.
 */



qx.Class.define( "skel.hacks.inputHandler.Drag", {

    extend: qx.core.Object,

    construct : function( vgview)
    {
        console.log("Installing Drag handler for", vgview);
        this.m_vgView = vgview;
        this.m_listenerIDs = [];

        var ow = this.m_vgView.overlayWidget();
        this.m_listenerIDs.push(
            ow.addListener( "mousedown", this._mouseDownCB.bind(this)));
        this.m_listenerIDs.push(
            ow.addListener( "mouseup", this._mouseUpCB.bind(this)));
        this.m_listenerIDs.push(
            ow.addListener( "mousemove", this._mouseMoveCB.bind(this)));
        this.m_listenerIDs.push(
            ow.addListener( "mouseout", this._mouseOutCB.bind(this)));

        this._reset();
    },

    members: {

        deactivate: function()
        {
            // remove all listeners
            this.m_listenerIDs.forEach( function( id )
            {
                this.m_vgView.overlayWidget().removeListenerById( id );
            } );
            this.m_listenerIDs = [];
        },

        _mouseDownCB: function( e )
        {
            // flag the button
            this.m_buttons[e.getButton()] = true;
            this.m_count = this._count( this.m_buttons);
            // set the last known mouse down
            var box = this.m_vgView.overlayWidget().getContentLocation();
            var mouseX = e.getDocumentLeft() - box.left
            var mouseY = e.getDocumentTop() - box.top
            this.m_lastMouseDownPt = { x: mouseX, y: mouseY };
            this.m_vgView.sendInputEvent( {type: "touch", x: mouseX, y: mouseY} );
            // we are not dragging yet
            this.m_dragging = false;
        },

        _mouseUpCB  : function( e )
        {
            delete this.m_buttons[e.getButton()];
            this.m_count = this._count( this.m_buttons);
            if( this.m_count === 0) {
                var box = this.m_vgView.overlayWidget().getContentLocation();
                var mouseX = e.getDocumentLeft() - box.left
                var mouseY = e.getDocumentTop() - box.top
                if( ! this.m_dragging) {
                    this._sendTapOrDoubleTap( mouseX, mouseY);
                    this.m_dragging = false;
                    //this.m_vgView.sendInputEvent( {type: "tap", x: mouseX, y: mouseY} );
                    return;
                } else {
                    //this.m_vgView.sendInputEvent( {type: "dragdone", x: mouseX, y: mouseY} );
                    this.m_vgView.sendInputEvent( {
                        type: "drag2",
                        phase: "end",
                        x: mouseX,
                        y: mouseY
                    });
                    return;
                }
            }
        },

        _sendTapOrDoubleTap : function( mouseX, mouseY) {
            this.m_tapCount ++;
            if( this.m_tapCount > 1) {
                this.m_vgView.sendInputEvent( {type: "dbltap", x: mouseX, y: mouseY} );
                this.m_tapCount = 0;
            } else {
                setTimeout( this._tapTimeoutCB.bind(this,mouseX,mouseY), 250);
            }
        },

        _tapTimeoutCB: function( mouseX, mouseY) {
            if( this.m_tapCount === 1) {
                this.m_vgView.sendInputEvent( {type: "tap", x: mouseX, y: mouseY} );
            }
            //else if( this.m_tapCount === 1) {
            //    this.m_vgView.sendInputEvent( {type: "dbltap", x: mouseX, y: mouseY} );
            //}
            this.m_tapCount = 0;
        },

        _mouseOutCB : function( e )
        {
            this._reset();
        },
        _mouseMoveCB: function( e )
        {
            var box = this.m_vgView.overlayWidget().getContentLocation();
            var mouseX = e.getDocumentLeft() - box.left
            var mouseY = e.getDocumentTop() - box.top
            if( this.m_count === 0 ) {
                this.m_vgView.sendInputEvent( {type: "hover", x: mouseX, y: mouseY} );
                return;
            }
            if( ! this.m_dragging) {
                //this.m_vgView.sendInputEvent( {
                //    type: "dragstart",
                //    x: this.m_lastMouseDownPt.x,
                //    y: this.m_lastMouseDownPt.y
                //} );
                this.m_vgView.sendInputEvent( {
                    type: "drag2",
                    phase: "start",
                    x: this.m_lastMouseDownPt.x,
                    y: this.m_lastMouseDownPt.y
                } );
            }
            //this.m_vgView.sendInputEvent( {
            //    type: "drag",
            //    x: mouseX,
            //    y: mouseY
            //} );
            this.m_vgView.sendInputEvent( {
                type: "drag2",
                phase: "progress",
                x: mouseX,
                y: mouseY
            } );
            this.m_dragging = true;
        },
        _reset      : function()
        {
            this.m_buttons = {};
            this.m_count = this._count( this.m_buttons);
            this.m_dragging = false;
        },
        _count: function(obj)
        {
            var size = 0;
            for( var key in obj ) {
                if( obj.hasOwnProperty( key ) ) size ++;
            }
            return size;
        },

        m_listenerIDs: null,
        m_vgView     : null,
        m_buttons : {},
        m_count  : 0,
        m_dragging : false,
        m_lastMouseDownPt: null,
        m_tapCount : 0

    }
});

