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
       
        
        this.m_popup = new qx.ui.popup.Popup( new qx.ui.layout.Canvas()).set({
            backgroundColor: "#FFFAD3",
            padding: [4,4],
            offset : 10,
            offsetBottom : 5
        });
        this.m_popupText = new qx.ui.basic.Atom();
        this.m_popup.add(this.m_popupText);
        
        var pathText = this.m_viewId + path.SEP + path.VIEW;
        this.m_sharedVarText = this.m_connector.getSharedVar( pathText );
        this.m_sharedVarText.addCB(this._cursorChangedCB.bind(this));
        this._cursorChangedCB();
        var qualityValue = this.m_connector.supportsRasterViewQuality() ? 90 : 101;
        this.setQuality( qualityValue );
    },

    members: {
    
        _cursorChangedCB : function(){
            if ( this.m_sharedVarText ){
                var val = this.m_sharedVarText.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        if ( this.m_popupText !== null){
                            if ( typeof obj.cursorText != 'undefined' ){
                                this.m_popupText.setLabel( obj.cursorText );
                                if ( obj.cursorText.length > 0 ){
                                    this.m_popup.show();
                                }
                                else {
                                    this.m_popup.hide();
                                }
                            }
                            else {
                                this.m_popup.hide();
                            }
                        }
                    }
                    catch( err ){
                        console.log( "Could not drag view text: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
        /**
         * Callback for a mouse move event.
         * @param ev {qx.event.type.Mouse}.
         */
        _mouseMoveCB : function (ev) {
            //if ( this.m_drag ){
            if ( !ev.isCtrlPressed() ){
                var box = this.overlayWidget().getContentLocation( "box" );
                var pt = {
                        x: ev.getDocumentLeft() - box.left,
                        y: ev.getDocumentTop() - box.top
                };
               
                var width = box.right - box.left;
                var height = box.bottom - box.top;
                this.m_viewSharedVar.set( "" + pt.x + " " + pt.y+" "+width+" "+height);
                var helpLocation = {
                    left : ev.getViewportLeft(),
                    top : ev.getViewportTop() - 20
                };
                
                this.m_popup.placeToPoint(helpLocation);
            }
            //}
        },
        /**
         * Callback for a mouse down event.
         * @param ev {qx.event.type.Mouse}.
         */
        
        _mouseDownCB : function (ev) {
            if ( !ev.isCtrlPressed() ){
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
            }
        },
        
        /**
         * Callback for a mouse up event.
         * @param ev {qx.event.type.Mouse}.
         */
        _mouseUpCB : function(ev) {
            if ( !ev.isCtrlPressed() ){
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
            }
        },
        
        m_viewId : null,
        m_drag : false,
        m_popup : null,
        m_popupText : null,
        m_sharedVarText : null

    }
} );
