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
    construct : function( label, id, visible ) {
        this.base(arguments);
        this.m_label = label;
        this.setLabel( label );
        this.m_visible = visible;
        this.m_id = id;
        this._initContextMenu();
        this.setIcon( "" );
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
                    this.m_customIcon = new skel.widgets.Image.Stack.CustomIcon( /*this.m_color*/ );
                    control = this.m_customIcon;
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
            if ( this.m_settings != null ){
                colorSupport = this.m_settings.colorSupport;
            }
            return colorSupport;
        },
        
        /**
         * Returns whether or not this is a group of layers.
         * @return {boolean} - whether or not this node is a group of layers.
         */
        isGroup : function(){
            return false;
        }
    }
});