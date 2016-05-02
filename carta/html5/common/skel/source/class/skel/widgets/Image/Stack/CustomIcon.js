/**
 * Used to display a colored square next to a leaf node of the stack.
 */


qx.Class.define("skel.widgets.Image.Stack.CustomIcon", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this._init( );
    },

    members : {

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setDecorator( this.m_BORDER_LINE );
            this.setWidth(10);
            this.setHeight(10);
            this.setMaxHeight(10);
        },
        
        /**
         * Set the color of the square.
         * @param red {Number} - amount of red in [0,255].
         * @param green {Number} - amount of green in [0,255].
         * @param blue {Number} - amount of blue in [0,255].
         * @param colorSupport {boolean} - true if the node is in a group with color support;
         *      false otherwise.
         */
        setColor : function( red, green, blue, colorSupport ){
            if ( colorSupport ){
                var colorArray = [];
                colorArray[0] = red;
                colorArray[1] = green;
                colorArray[2] = blue;
                var colorStr = qx.util.ColorUtil.rgbToHexString( colorArray );
                this.setBackgroundColor( colorStr );
            }
            else {
                this.setBackgroundColor( "transparent" );
                this.setDecorator( "no-border");
            }
        },
        
        /**
         * Needed so it can pretend to be an icon.
         */
        setSource : function( /*url*/ ){
        },
        
        m_BORDER_LINE : "line-border"
    }

});