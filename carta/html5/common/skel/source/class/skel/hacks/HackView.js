/**
 * Created by pfederl on 05/01/15.
 */

/*global qx, mImport, skel, console */

/**
 @ignore( mImport)
 */

qx.Class.define( "skel.hacks.HackView", {

    //extend: skel.boundWidgets.View.ViewWithInputDiv,
    //extend: skel.hacks.VGView,
    extend: skel.boundWidgets.View.VGView,

    /**
     * Constructor
     */
    construct: function( viewId )
    {
        this.base( arguments, viewId );

        // monitor mouse move
        this.overlayWidget().addListener( "mousemove", this._mouseMoveCB.bind(this));
        this.overlayWidget().addListener( "mousewheel", this._mouseWheelCB.bind(this));
        this.overlayWidget().addListener( "click", this._mouseClickCB.bind(this));

        this.m_viewId = viewId;
        this.m_connector = mImport( "connector");

        this.m_prefix = "/hacks/views/" + this.m_viewId + "/";
        this.m_pointerMoveVar = this.m_connector.getSharedVar(
            this.m_prefix + "pointer-move");
    },

    members: {

        _mouseMoveCB : function (ev) {
            var box = this.overlayWidget().getContentLocation( "box" );
            var pt = {
                x: ev.getDocumentLeft() - box.left,
                y: ev.getDocumentTop() - box.top
            };

            this.m_pointerMoveVar.set( "" + pt.x + " " + pt.y);

        },

        _mouseWheelCB : function(ev) {
            var box = this.overlayWidget().getContentLocation( "box" );
            var pt = {
                x: ev.getDocumentLeft() - box.left,
                y: ev.getDocumentTop() - box.top
            };

            this.m_connector.sendCommand( this.m_prefix + "zoom",
                "" + pt.x + " " + pt.y + " " + ev.getWheelDelta());
        },

        _mouseClickCB : function(ev) {
            var box = this.overlayWidget().getContentLocation( "box" );
            var pt = {
                x: ev.getDocumentLeft() - box.left,
                y: ev.getDocumentTop() - box.top
            };

            this.m_connector.sendCommand( this.m_prefix + "center",
                "" + pt.x + " " + pt.y + " " + ev.getButton());
        },

        m_viewId : null

    }
} );
