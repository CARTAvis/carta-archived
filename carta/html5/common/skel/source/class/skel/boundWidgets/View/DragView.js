/**
 * Sends mouse up, mouse move, and mouse down events to the server for simulating a mouse drag event.
 */

/*global qx, mImport, skel */

/**
 @ignore( mImport)
 */

qx.Class.define( "skel.boundWidgets.View.DragView", {

    extend: skel.boundWidgets.View.ViewWithInputDivSuffixed,

    /**
     * Constructor.
     */
    construct: function( viewId )
    {
        this.base( arguments, viewId );

        // monitor mouse move
        this.addListener( "mousedown", this._mouseDownCB.bind(this));
        this.addListener( "mousemove", this._mouseMoveCB.bind(this));
        this.addListener( "mouseup", this._mouseUpCB.bind(this));
        this.addListener( "mouseout", this._mouseUpCB.bind(this));

        this.m_viewId = viewId;
        this.m_connector = mImport( "connector");

        var path = skel.widgets.Path.getInstance();
        this.m_prefix = this.m_viewId + path.SEP+ path.VIEW + path.SEP +"pointer-move";
        this.m_viewSharedVar = this.m_connector.getSharedVar(this.m_prefix);
    },

    members: {
        /**
         * Callback for a mouse move event.
         * @param ev {qx.event.type.Mouse}.
         */
        _mouseMoveCB : function (ev) {
            if ( this.m_drag ){
                var box = this.overlayWidget().getContentLocation( "box" );
                var pt = {
                        x: ev.getDocumentLeft() - box.left,
                        y: ev.getDocumentTop() - box.top
                };
                this.m_viewSharedVar.set( "" + pt.x + " " + pt.y);
            }
        },
        /**
         * Callback for a mouse down event.
         * @param ev {qx.event.type.Mouse}.
         */
        
        _mouseDownCB : function (ev) {
            this.m_drag = true;
            var box = this.overlayWidget().getContentLocation( "box" );
            var x = ev.getDocumentLeft() - box.left;
            var path = skel.widgets.Path.getInstance();
            var cmd;
            if ( !ev.isShiftPressed() ){
                cmd = this.m_viewId + path.SEP_COMMAND + path.MOUSE_DOWN;
            }
            else {
                cmd = this.m_viewId + path.SEP_COMMAND + path.MOUSE_DOWN_SHIFT;
            }
            this.m_connector.sendCommand( cmd, "x:" + x );
        },
        
        /**
         * Callback for a mouse up event.
         * @param ev {qx.event.type.Mouse}.
         */
        _mouseUpCB : function(ev) {
            if ( this.m_drag ){
                this.m_drag = false;
                var box = this.overlayWidget().getContentLocation( "box" );
                var x = ev.getDocumentLeft() - box.left;
                var path = skel.widgets.Path.getInstance();
                var cmd;
                if ( ! ev.isShiftPressed() ){
                    cmd = this.m_viewId + path.SEP_COMMAND + path.MOUSE_UP;
                }
                else {
                    cmd = this.m_viewId + path.SEP_COMMAND + path.MOUSE_UP_SHIFT;
                }
                this.m_connector.sendCommand( cmd, "x:" + x );
            }
        },
        
        m_viewId : null,
        m_drag : false

    }
} );
