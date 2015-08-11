/**
 * Represents a link from a source to a destination.
 */

/*global mImport, qx, skel, console */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/


qx.Class.define("skel.widgets.Draw.Shape", {

    extend : qx.core.Object,

    construct : function( winId, shapeType ){
        this.m_winId = winId;
        this.m_shapeType = shapeType;
        this.m_connector = mImport("connector");
    },
    
    statics : {
        CURSOR_RESIZE_NW : "nw-resize",
        CURSOR_RESIZE_SW : "sw-resize",
        CURSOR_RESIZE_NE : "ne-resize",
        CURSOR_RESIZE_SE : "se-resize",
        CURSOR_RESIZE_W : "w-resize",
        CURSOR_RESIZE_E : "e-resize",
        CURSOR_RESIZE_N : "n-resize",
        CURSOR_RESIZE_S : "s-resize",
        CURSOR_MOVE : "move",
        CURSOR_CROSSHAIR : "crosshair",
        CURSOR_DEFAULT : "default"
    },
    
    members : {
        
        /**
         * Move this shape.
         * @param mouseDownPt {Object} the (x,y) coordinate of the last mouse down.
         * @param growPt {Object} the (x,y) coordinate of the current location of the mouse.
         */
        doMove : function( mouseDownPt, mouseMovePt ){
            return false;
        },
        
        /**
         * Adjust one of the sides of the shape as appropriate.
         * @param mouseMovePt {Object} the amount the side should move in x-,y-coordinates.
         */
        doResize : function( mouseMovePt ){
            return false;
        },
        
        /**
         * Draws this shape.
         * @param ctx {Object} the painting context.
         */
        draw : function( ctx ){
        },
        
        /**
         * Draws this shape with decorators to reflect various states.
         * @param ctx {Object} the painting context.
         * @param active {boolean} whether or not this shape is selected.
         * @param hover {boolean} whether or not the mouse is hovered over this shape.
         * @param resize {boolean} whether or not the mouse is around the edge of this shape for resizing.
         * @param translator {skel.widgets.Draw.ImageMouseTranslator} used to translate image/mouse coordinates.
         */
        _drawDecorators : function( ctx, active, hover, resize, translator ){
            
        },
        
        /**
         * Draws this shape with decorators to reflect various states.
         * @param ctx {Object} the painting context.
         * @param active {boolean} whether or not this shape is selected.
         * @param resizing {boolean} whether or not the mouse is around the edge of this shape for resizing.
         * @param translator {skel.widgets.Draw.ImageMouseTranslator} used to translate image/mouse coordinates.
         */
        drawRegion : function( ctx, active,  resizing, translator ){
            var resize = resizing || this.m_resize;
            var hovered = this.m_hover || resize;
            this._drawDecorators( ctx, active, hovered, resize, translator );
        },
        
        /**
         * Returns whether or not the mouse is currently hovered over this shape.
         */
        isHover : function(){
            return this.m_hover;
        },
        
        /**
         * Returns this shape's server id.
         */
        getShapeId : function(){
            return this.m_shapeId;
        },
        
        /**
         * Returns this shape's type, for example, RectangleRegion.
         */
        getType : function(){
            return this.m_shapeType;
        },
        
        /**
         * Returns whether or not this shape is visible.
         */
        isVisible : function(){
            return this.m_visible;
        },
        
        /**
         * Returns a string representation of this shape.
         * @return {String} a string representation of this shape.
         */
        getShapeInfo: function () {
        },
        
        /**
         * Returns true if the type and id of this shape matches those pased in;
         * otherwise, returns false.
         * @param type {String} the shape type, Rectangle, etc.
         * @param id {String} the server unique id.
         */
        matches : function ( type, id ){
            //var matching = false;
            //if ( type == this.m_shapeType ){
            //    if ( id == this.m_shapeId ){
            //        matching = true;
            //    }
            //}
            //return matching;
            return type === this.m_shapeType && id === this.m_shapeId;
        },
        
        /**
         * Either resizes or moves this shape, based on the mouse status, by the
         * given amounts.
         * @param dx {Number} the move amount in the x-direction.
         * @param dy {Number} the move amount in the y_direction.
         */
        mouseMove : function( dx, dy ){
            var shapeChanged = false;
            if (this.m_hover ) {
                if( this.m_hoverInfo === skel.widgets.Draw.Shape.CURSOR_MOVE) {
                    var mouseTL = {
                        x: this.m_topLeft.x + dx,
                        y: this.m_topLeft.y + dy
                    };
                    var mouseBR = {
                        x: this.m_bottomRight.x + dx,
                        y: this.m_bottomRight.y + dy
                    };
                    shapeChanged = this.doMove( mouseTL, mouseBR );
                }
                else {
                    var mouseMovePt = {
                        x: dx,
                        y: dy
                    };
                    shapeChanged = this.doResize( mouseMovePt );
                }
            }
            if ( shapeChanged ){
                //Update the server with the new shape information.
                this.sendShapeChangedCommand();
            }
        },
        
        /**
         * Sends a command to register this shape with the server and get back
         * this shape's unique id.
         * @param index {Number} the index of the shape or -1 for a new one.
         */
        register : function( index ){
            var pathDict = skel.widgets.Path.getInstance();
            var regShapeCmd = pathDict.getCommandRegisterShape( this.m_winId );
            var params = "type:"+this.m_shapeType+",index:"+index;

            // this is weird (Pavol)
            // this.m_connector.sendCommand( regShapeCmd, params, this._shapeCB( this ) );

            // did you mean this?
            this.m_connector.sendCommand( regShapeCmd, params, this.setShapeId.bind(this));
        },
        
        /**
         * Notify the server that this shape has changed.
         */
        sendShapeChangedCommand : function(){
            if ( this.m_shapeId !== null ){
                var shapeInfo = this.getShapeInfo();
                var pathDict = skel.widgets.Path.getInstance();
                var shapeCmd = pathDict.getCommandShapeChanged( this.m_shapeId );
                var params = "info:"+shapeInfo;
                this.m_connector.sendCommand( shapeCmd, params, function(){} );
            }
        },
        
        /**
         * Set the initial point of this shape.
         * @param initialPt {Object} the (x,y) coordinate of a corner of this shape.
         */
        setInitialPt : function ( initialPt ){
            
        },
        
        setShapeId : function( id ){
            this.m_shapeId = id;
            //Initialize the shared variable so we can listen for shape changes from the server.
            this.m_sharedVar = this.m_connector.getSharedVar(id);
            this.m_sharedVar.addCB(this._shapeChangedCB.bind(this));
            this._shapeChangedCB();
        },
        
        /**
         * Callback given the id of this shape on the server.
         * @param anObject {skel.widgets.Draw.Shape} this shape.
         */
        _shapeCB : function( anObject ){
             return function( id ){
                 anObject.setShapeId( id );
             };
        },
        
        /**
         * Callback indicating that this shape has been updated on
         * the server.
         */
        _shapeChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val !== null ){
                try {
                    var shape = JSON.parse( val );
                    var updated = this.updateShape( shape );
                    //Trigger a redraw if the shape was updated.
                    if ( updated ){
                        qx.event.message.Bus.dispatch(new qx.event.message.Message("shapeChanged", ""));
                    }
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
                }
            }
        },
        
        /**
         * Returns a string representation of this shape.
         */
        toString : function(){
            var str = "shapeId="+this.m_shapeId + "\n";
            str = str + "type=" + this.m_shapeType + "\n";
            str = str + this.getShapeInfo();
            return str;
        },
        

        
        /**
         * Determine the hover status of the mouse with regard to this shape.
         * @param lastMouse{Object} the (x,y) coordinates of the mouse.
         * @param margin {Number} an error margin for determining closeness.
         */
        updateHoverStatus : function ( lastMouse, margin ){
            return this.m_hoverInfo;
        },
        
        /**
         * Copy the location of the new shape into this one.
         * @param newShape {skel.widgets.Draw.Shape} the new shape to copy.
         */
        updateShape : function( shape ){
            return false;
        },
        
        m_fillStr : "rgba(255,255,0, 0.1)",
        m_strokeStr : "rgb(255,255,0)",
        m_lineWidth : 2,
        m_visible : true,
        m_hoverInfo : "",
        m_hover : false,
        m_resize : false,
        m_label : null,
        m_winId : null,
        m_shapeId : null,
        m_sharedVar : null,
        m_shapeType : null,
        m_connector : null
    }
});