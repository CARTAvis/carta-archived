/**
 * Hover handler.
 *
 * Hovers are mousemove/pointermove/whatever events that are passive, i.e. the user is just
 * moving the pointer around without clicking any buttons.
 *
 * \todo make this a subclass of some common class (e.g. InputEventBase) and put common
 * functionality there.
 */



qx.Class.define( "skel.hacks.inputHandler.Hover", {

    extend: qx.core.Object,

    construct : function( vgview)
    {
        console.log("Installing Hover handler for", vgview);
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
            this.m_hoverMode = false;
        },

        _mouseUpCB  : function( e )
        {
            delete this.m_buttons[e.getButton()];
            var count = this._count( this.m_buttons);
            this.m_hoverMode = (count === 0);
        },
        _mouseOutCB : function( e )
        {
            this._reset();
        },
        _mouseMoveCB: function( e )
        {
            if( ! this.m_hoverMode ) {
                return;
            }
            console.log( "hover handler", e.getButton() );
            var box = this.m_vgView.overlayWidget().getContentLocation();
            var mouseX = e.getDocumentLeft() - box.left
            var mouseY = e.getDocumentTop() - box.top
            this.m_vgView.sendInputEvent( {type: "hover", x: mouseX, y: mouseY} );
        },
        _reset      : function()
        {
            this.m_buttons = {};
            this.m_hoverMode = true;
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
        m_hoverMode  : true

    }
});

