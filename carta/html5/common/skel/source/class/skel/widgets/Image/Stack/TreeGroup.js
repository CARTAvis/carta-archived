/**
 * A list item with an in memory constructed icon consisting of a square with a
 * colored background.
 */

qx.Class.define("skel.widgets.Image.Stack.TreeGroup", {
    extend : qx.ui.tree.TreeFolder,
    include : skel.widgets.Image.Stack.TreeNodeMixin,

    /**
     * Constructor.
     */
    construct : function( label, id, visible ) {
        this.base(arguments);
        this.m_label = label;
        this.setLabel( label );
        this.m_id = id;
        this.m_visible = visible;
        this._initContextMenu();
    },
    

    members : {
        
        
        
        /**
         * Returns whether or not this node supports RGB.
         * @return {boolean} - whether or not this node supports RGB.
         */
        isColorSupport : function(){
            return false;
        },
        
        /**
         * Returns whether or not this is a group of layers.
         * @return {boolean} - whether or not this node is a group of layers.
         */
        isGroup : function(){
            return true;
        }

    }
});