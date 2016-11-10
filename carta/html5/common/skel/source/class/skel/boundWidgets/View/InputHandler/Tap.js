/**
 * Tap handler.
 *
 * Taps could be clicks for example.
 *
 * \todo make this a subclass of some common class (e.g. InputEventBase) and put common
 * functionality there.
 */



qx.Class.define( "skel.boundWidgets.View.InputHandler.Tap", {

    extend: qx.core.Object,

    construct : function( vgview){
        this.m_vgView = vgview;
        this.m_listenerIDs = [];

        var ow = this.m_vgView.overlayWidget();
        this.m_listenerIDs.push(
            ow.addListener( "click", this._mouseClickCB.bind(this)));
    },

    members: {

        deactivate: function()
        {
            this.m_listenerIDs.forEach( function( id )
            {
                this.m_vgView.overlayWidget().removeListenerById( id );
            } );
            this.m_listenerIDs = [];
        },

        _mouseClickCB: function( e ){
            if( e.getButton() != "left") {
                return;
            }
            var box = this.m_vgView.overlayWidget().getContentLocation();
            var mouseX = e.getDocumentLeft() - box.left
            var mouseY = e.getDocumentTop() - box.top
            this.m_vgView.sendInputEvent( {type: "tap", x: mouseX, y: mouseY} );
        },

        m_listenerIDs: null,
        m_vgView     : null
    }
});

