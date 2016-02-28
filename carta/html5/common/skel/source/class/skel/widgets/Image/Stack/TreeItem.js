/**
 * A list item with an in memory constructed icon consisting of a square with a
 * colored background.
 */

qx.Class.define("skel.widgets.Image.Stack.TreeItem", {
    extend : qx.ui.tree.TreeFile,
    include : skel.widgets.Image.Stack.TreeNodeMixin,

    /**
     * Constructor.
     */
    construct : function( label, color, id, settings ) {
        this.base(arguments);
        this.setLabel( label );
        this.m_color = color;
        this.m_id = id;
        this.m_settings = settings;
        if ( this.m_color.length > 0 ){
            this.setIcon( this.m_color );
        }
    },
   

    members : {
        
        /**
         * Constructs an in memory icon.
         * @param id {String} - an identifier for the widget to be constructed.
         */
        _createChildControlImpl : function( id ){
            var control;
            switch( id ){
                case "icon":
                    control = new qx.ui.core.Widget();
                    control.setBackgroundColor( this.m_color );
                    control.setDecorator( this.m_BORDER_LINE );
                    control.setWidth(10);
                    control.setHeight(10);
                    control.setMaxHeight(10);
                    this._add( control );
                    break;
            }
            return control || this.base( arguments, id );
        },
     
        /**
         * Returns whether or not this node supports RGB.
         * @return {boolean} - whether or not this node supports RGB.
         */
        isColorSupport : function(){
            var colorSupport = false;
            if ( this.m_color.length > 0 ){
                colorSupport = true;
            }
            return colorSupport;
        },
        
        /**
         * Returns whether or not this is a group of layers.
         * @return {boolean} - whether or not this node is a group of layers.
         */
        isGroup : function(){
            return false;
        },
      
    
        m_color : null,
        m_BORDER_LINE : "line-border"

    }
});