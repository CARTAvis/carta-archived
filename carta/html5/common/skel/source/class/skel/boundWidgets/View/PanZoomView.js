/**
 * Created by pfederl on 05/01/15.
 */

/*global qx, mImport, skel */

/**
 @ignore( mImport)
 */

qx.Class.define( "skel.boundWidgets.View.PanZoomView", {

    extend: skel.boundWidgets.View.ViewWithInputDivSuffixed,

    /**
     * Constructor
     */
    construct: function( viewId )
    {
        this.base( arguments, viewId );

        // monitor mouse move
        this.addListener( "mousemove", this._mouseMoveCB.bind(this));
        this.addListener( "mousewheel", this._mouseWheelCB.bind(this));
        this.addListener( "click", this._mouseClickCB.bind(this));

        this.m_viewId = viewId;
        this.m_connector = mImport( "connector");

        var path = skel.widgets.Path.getInstance();
        this.m_prefix = this.m_viewId + path.SEP+ path.VIEW + path.SEP +"pointer-move";
        this.m_viewSharedVar = this.m_connector.getSharedVar(this.m_prefix);
    },

    members: {

        _mouseMoveCB : function (ev) {
            var box = this.overlayWidget().getContentLocation( "box" );
            var pt = {
                x: ev.getDocumentLeft() - box.left,
                y: ev.getDocumentTop() - box.top
            };
            this.m_viewSharedVar.set( "" + pt.x + " " + pt.y);

        },

        _mouseWheelCB : function(ev) {
            var box = this.overlayWidget().getContentLocation( "box" );
            var pt = {
                x: ev.getDocumentLeft() - box.left,
                y: ev.getDocumentTop() - box.top
            };
            //console.log( "vwid wheel", pt.x, pt.y, ev.getWheelDelta());
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + path.ZOOM;
            this.m_connector.sendCommand( cmd,
                "" + pt.x + " " + pt.y + " " + ev.getWheelDelta());
        },

        _mouseClickCB : function(ev) {
            var box = this.overlayWidget().getContentLocation( "box" );
            var pt = {
                x: ev.getDocumentLeft() - box.left,
                y: ev.getDocumentTop() - box.top
            };
            //console.log( "vwid click", pt.x, pt.y, ev.getButton());
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + path.CENTER;
            this.m_connector.sendCommand( cmd,
                "" + pt.x + " " + pt.y + " " + ev.getButton(), function(){});
        },

        m_viewId : null

    }
} );
