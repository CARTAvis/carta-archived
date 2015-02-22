/**
 * Created by pfederl on 05/01/15.
 */

/*global qx, mImport, skel */

/**
 @ignore( mImport)
 */

qx.Class.define( "skel.boundWidgets.View.DragView", {

    extend: skel.boundWidgets.View.ViewWithInputDivSuffixed,

    /**
     * Constructor
     */
    construct: function( viewId )
    {
        this.base( arguments, viewId );
        this.setDraggable(true);

        // monitor mouse move
        this.addListener( "dragstart", this._mouseDownCB.bind(this));
        this.addListener( "drag", this._mouseMoveCB.bind(this));
        // this.addListener( "drag", this._mouseDragCB.bind(this));
        this.addListener( "dragend", this._mouseUpCB.bind(this));

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

        _mouseDownCB : function (ev) {
            console.log("mousedown");
            var box = this.overlayWidget().getContentLocation( "box" );
            var x = ev.getDocumentLeft() - box.left;
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + path.MOUSE_DOWN;
            this.m_connector.sendCommand( cmd, "x:" + x );

        },

        _mouseDragCB : function(ev) {
            console.log("dragging");
            var box = this.overlayWidget().getContentLocation( "box" );
             var x = ev.getDocumentLeft() - box.left;
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + path.MOUSE_DRAG;
            this.m_connector.sendCommand( cmd, "x:" + x );

        },

        _mouseUpCB : function(ev) {
            console.log("mouseup");
            var box = this.overlayWidget().getContentLocation( "box" );
            var x = ev.getDocumentLeft() - box.left;
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_viewId + path.SEP_COMMAND + path.MOUSE_UP;
            this.m_connector.sendCommand( cmd, "x:" + x );

        },

        m_viewId : null

    }
} );
