/**
 * Displays a color map as a gradient.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColorGradient", {
    extend: qx.ui.embed.Canvas,
    
    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this._init( );
        this.update();
    },

    members : {
        
        
        /**
         * Template method, which can be used by derived classes to redraw the
         * content. It is called each time the canvas dimension change and the
         * canvas needs to be updated.
         *
         * @param width {Integer} New canvas width
         * @param height {Integer} New canvas height
         * @param ctx {CanvasRenderingContext2D} The rendering ctx to draw to
         */
        _draw: function (width, height, ctx) {
            this.base(arguments);
            var grd = ctx.createLinearGradient( 0, 0, width, 0);
            if ( this.m_stops !== null ){
                for ( var i = 0; i < this.m_stops.length; i++ ){
                    var floatStop = i / this.m_stops.length;
                    grd.addColorStop( floatStop, this.m_stops[i] );
                }
            }
            ctx.fillStyle = grd;
            ctx.fillRect( 0, 0, width, height );
        },

        
        
        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            this.setAllowGrowX( true );
            this.setAllowGrowY( true );
            this.m_connector = mImport("connector");
        },
        
        /**
         * Requests information from the server about the colors comprising the 
         *      current color gradient.
         */
        _sendUpdateStopsCmd : function(){
            if ( this.m_connector !== null && this.m_index !== null ){
                var params = "index:"+this.m_index;
                var path = skel.widgets.Path.getInstance();
                var cmd = path.COLORMAPS + path.SEP_COMMAND + "getColorStops";
                this.m_connector.sendCommand( cmd, params, this._updateColorStops(this));
            }
        },
        
        /**
         * Sets the index of the color map to display in the gradient.
         * @param index {Number} an index into the master color map list.
         */
        setColorIndex : function( index ){
            if ( this.m_index !== index ){
                this.m_index = index;
                this._sendUpdateStopsCmd();
            }
        },
        
        /**
         * Returns a callback for updating the display with new color stops.
         * @param anObject {skel.widgets.ColorMap.ColorGradient}.
         * @return {Function} the callback for updating the gradient color stops.
         */
        _updateColorStops : function( anObject ){
            return function( colorStops ){
                if ( colorStops !==null && colorStops != anObject.m_stops ){
                    anObject.m_stops = colorStops.split(",");
                    anObject.update();
                }
            };
        },
        
        m_connector : null,
        m_index : null,
        m_stops : null
    }
});