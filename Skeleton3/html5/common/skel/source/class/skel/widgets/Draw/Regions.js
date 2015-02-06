/**
 * Manages shape selections.
 */

/**
 * @ignore( mImport)
 **/

/* global mImport, qx, skel, console */

qx.Class.define( "skel.widgets.Draw.Regions", {

    extend   : qx.core.Object,

    construct: function( winId )
    {
        this.m_winId = winId;
        this.m_connector = mImport( "connector" );

        this.m_shapes = [];
        this.m_translator = new skel.widgets.Draw.ImageMouseTranslator( winId );

        //Listen to changes in the supported regions.
        this.m_sharedVar = this.m_connector.getSharedVar( winId );
        this.m_sharedVar.addCB( this._regionsChangedCB.bind( this ) );
        this._regionsChangedCB();
    },

    members: {

        /**
         * Returns true if there is an existing shape with the given type and id;
         * false otherwise.
         * @param type {String} the type of shape, i.e. Rectangle.
         * @param id {String} the unique server id of the shape.
         */
        containsShape: function( type, id )
        {
            var shapeMatch = false;
            for( var i = 0 ; i < this.m_shapes.length ; i ++ ) {

                shapeMatch = this.m_shapes[i].matches( type, id );
                if( shapeMatch ) {
                    break;
                }
            }
            return shapeMatch;
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
        _draw: function( width, height, ctx )
        {

            ctx.clearRect( 0, 0, width, height );

            if( this.m_shape !== null ) {
                this.m_shape.draw( ctx );
            }

            // draw all the shapes in reverse order
            var firstVisibleShapeInd = this._getFirstVisibleShapeInd();
            for( var i = this.m_shapes.length - 1 ; i >= 0 ; i -- ) {
                var shape = this.m_shapes[i];
                if( shape ) {
                    var isActive = i === firstVisibleShapeInd;
                    var resizing = false;
                    if( this.m_mouseDownPt !== null ) {
                        resizing = true;
                    }
                    shape.drawRegion( ctx, isActive, resizing, this.m_translator );
                }
            }
        },

        /**
         * Returns the index of the first visible shape.
         */
        _getFirstVisibleShapeInd: function()
        {
            var ind = - 1;
            this.m_shapes.forEach( function( r, i )
            {
                if( ind > - 1 ) {
                    return;
                }
                if( r && r.isVisible() ) {
                    ind = i;
                }
            }, this );
            return ind;
        },

        /**
         * Factory for initializing shapes.
         */
        _initShape: function()
        {
            if( this.m_drawKey !== null ) {
                if( this.m_drawKey == this.m_RECTANGLE ) {
                    this.m_shape = new skel.widgets.Draw.Rectangle( this.m_winId );
                }
                else {
                    console.log( "Unsupported draw shape: " + this.m_drawKey );
                }
                if( this.m_mouseDownPt !== null && this.m_shape !== null ) {

                    this.m_shape.setInitialPt( this.m_mouseDownPt );
                }
            }
        },

        /**
         * Returns whether or not the mouse has been pressed down.
         */
        isMouseDown: function(){
            return this.m_mouseDownPt !== null;
        },

        /**
         * Returns whether or not the mouse is currently located over one
         * or more of the managed shapes.
         */
        _isShapeHovered: function()
        {
            var shapeHovered = false;
            for( var ind = 0 ; ind < this.m_shapes.length ; ind ++ ) {
                shapeHovered = this.m_shapes[ind].isHover();
                if( shapeHovered ) {
                    break;
                }
            }
            return shapeHovered;
        },

        /**
         * Callback for a mouse move.
         * @param pt {Object} the (x,y) coordinate location of the mouse.
         */
        _mouseMoveCB: function( pt )
        {
            // remember the last mouse position
            this.m_lastMouse = pt;
            this._updateClickStatus();
            if( this._isShapeHovered() ) {
                // Tell all the shapes the mouse has moved so they can act on it, if appropriate.
                if( this.m_mouseDownPt ) {
                    var movePt = this.m_translator.mouse2serverImage( this.m_lastMouse );
                        var downPt = this.m_translator.mouse2serverImage( this.m_mouseDownPt );
                    var dx = movePt.x - downPt.x;
                    var dy = movePt.y - downPt.y;
                    for( var ind = 0 ; ind < this.m_shapes.length ; ind ++ ) {
                        this.m_shapes[ind].mouseMove( dx, dy );
                    }
                    this.m_mouseDownPt = this.m_lastMouse;
                }
            }
            else {
                //Mouse is being dragged while in the down state.
                if( ! this.m_clickEvent && this.m_mouseDownPt ) {
                        if ( this.m_shape === null ){
                        this._initShape();
                    }
                    if( this.m_shape !== null ) {
                        this.m_shape.doMove( this.m_mouseDownPt, this.m_lastMouse );
                    }
                }
            }
        },

        /**
         * Callback for a mouse down event.
         * @param pt {Object} the (x,y) coordinate of the mouse.
         */
        _mouseDownCB: function( pt )
        {
            this.m_lastMouse = pt;
            this.m_mouseDownPt = pt;
            this._resetClickStatus();
        },

        /**
         * Callback for a mouse up event.
         * @param pt {Object} the (x,y) coordinate of the mouse.
         */
        _mouseUpCB: function( pt )
        {
            this.m_mouseDownPt = null;
            if( this.m_shape !== null ) {
                //Note:  Registering shape will result in a call back
                //that adds the shape to the permanent ones.
                this.m_shape.register( - 1 );
                this.m_shape.sendShapeChangedCommand();
                this.m_shape = null;
                if( this.m_drawOnce ) {
                    this.m_drawKey = null;
                }
            }
        },

        /**
         * Callback for a region changed event on the server; updates the
         * managed selections.
         */
        _regionsChangedCB: function()
        {
            var val = this.m_sharedVar.get();
                if ( !val ){
                    return;
                }
                var i = 0;
                try {
                    var controlState = JSON.parse( val );
                    var regionCount = controlState.regions.length;
                    var shapeCount = this.m_shapes.length;
                    var redraw = false;
                   
                   //Remove any shapes the server does not know about.
                   for ( i = shapeCount-1; i >= 0; i--){
                        //If it has an id from the server and is no longer there, remove it.
                        var shapeId = this.m_shapes[i].getShapeId();
                        if ( shapeId !== null ){
                            var serverShape = false;
                            for ( var j = 0; j < regionCount; j++ ){
                                if ( controlState.regions[j].id == shapeId ){
                                    serverShape = true;
                                    break;
                                }
                            }
                            if ( !serverShape ){
                                redraw = true;
                                this.m_shapes.splice(i,1);
                            }
                        }
                    }
                   
                    for ( i = 0; i < regionCount; i++){
                        var shape = controlState.regions[i];
                    
                        //If it is a new shape, add it to the list.
                        if ( !this.containsShape( shape.type, shape.id ) ){
                            var shapeNew = new skel.widgets.Draw.Rectangle( this.m_winId);
                            shapeNew.setShapeId( shape.id );
                            this.m_shapes.splice(i,0,shapeNew);
                            redraw = true;
                        }
                     }
                    
                     
                     //Trigger a redraw
                    if ( redraw ){
                        qx.event.message.Bus.dispatch(new qx.event.message.Message(
                                "shapeChanged", ""));
                    }
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
                    return;
                }
        },

        /**
         * Reset whether the mouse down event could be a click or is instead
         * a selection of an existing shape.
         */
        _resetClickStatus: function()
        {
            this.m_clickEvent = true;
            var hover = false;
            for( var i = 0 ; i < this.m_shapes.length ; i ++ ) {
                if( this.m_shapes[i].isHover() ) {
                    hover = true;
                    break;
                }
            }
            if( hover ) {
                this.m_clickEvent = false;
            }
        },

        /**
         * Sets whether or not a single shape will be drawn or multiple
         * shapes of the given type will be drawn; also sets the type of
         * shape that will be drawn.
         * @param drawInfo {String} information concerning the type of shape
         *      and whether or not multiple shapes will be drawn.
         */
        setDrawMode: function( drawInfo ){
                this.m_drawOnce = !drawInfo.multiShape;
                if ( this.m_drawKey != drawInfo.shape ){
                    this.m_drawKey =  drawInfo.shape;
                }
            },
            
            /**
             * Update whether the mouse was clicked based on the current location
             * of the mouse.
             */
            _updateClickStatus : function(){
                if ( this.m_mouseDownPt ){
                    // if mouse is down, we check how far the user dragged it, and if it's more
                    // than some threshold, we know it's not a click, but a rectangle select
                    if (this.m_clickEvent  ) {
                        var dx = this.m_mouseDownPt.x - this.m_lastMouse.x;
                        var dy = this.m_mouseDownPt.y - this.m_lastMouse.y;
                        var distSq = dx * dx + dy * dy;
                        if (distSq > 13) {
                            this.m_clickEvent = false;
                        }
                    }
                }
            },
            
            /**
             * Evaluates whether or not the mouse is currently over one of the managed
             * shapes and returns an appropriate cursor based on the evaluation.
             * @param pt {Object} the (x,y) coordinates of the current mouse location.
             */
            updateMouseHoverStatus: function ( pt ) {
                var margin = 5;
                var cursorInfo = skel.widgets.Draw.Shape.CURSOR_DEFAULT;
                if (this.m_lastMouse !== null  ) {
                   for ( var ind = 0; ind < this.m_shapes.length; ind++ ) {
                        var shape = this.m_shapes[ind];
                        if (shape.isVisible() ){
                            //TODO:  Only handling one shape not being at default.
                            var shapeCursor = shape.updateHoverStatus( pt, margin );
                            if ( shapeCursor != skel.widgets.Draw.Shape.CURSOR_DEFAULT ){
                                cursorInfo = shapeCursor;
                            }
                        }
                    }
                }
                return cursorInfo;
            },
            
            m_mouseDownPt: null,
            m_lastMouse: null,
            m_clickEvent: true,
            m_shape : null,
            m_shapes : null,
            m_sharedVar : null,
            m_drawOnce : true,
            m_drawKey : null,
            m_connector: null,
            m_winId : "",
            m_imageMouseTranslator : null,
            m_RECTANGLE : "Rectangle"

    }

} );

