/**
 * Common properties for a tree node in the stack.
 */
qx.Mixin.define("skel.widgets.Image.Stack.TreeNodeMixin", {


    members : {
        /**
         * Return the identifier for the node.
         * @return {String} - an identifier for the node.
         */
        getId : function(){
            return this.m_id;
        },
        
        /**
         * Return node specific settings.
         * @return {Object} - node specific settings.
         */
        getSettings : function(){
            return this.m_settings;
        },
        
        /**
         * Set node specific settings.
         * @param settings {Object} - node specific settings.
         */
        setSettings : function( settings ){
            this.m_settings = settings;
        },
    
        m_id : null,
        m_settings : null

    }
});