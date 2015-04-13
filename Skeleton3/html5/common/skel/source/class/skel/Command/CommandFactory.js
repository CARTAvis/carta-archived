/**
 * Generates a tree containing the available commands.
 */

qx.Class.define("skel.Command.CommandFactory", {
    extend : qx.core.Object,
    type : "singleton",
    
    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments );
        this.m_all = skel.Command.CommandAll.getInstance();
    },
    
    members : {
        
        /**
         * Returns the command with the given name or null if no such command exists.
         * @param cmdName {String} a command identifier.
         * @return {skel.Command.Command} the corresponding command or null if none exists.
         */
        getCommand : function(cmdName){
            var cmd = this.m_all.getCommand( cmdName);
            return cmd;
        },
        
        /**
         * Returns a tree containing the available commands in hierarchical order.
         * @return {Object} a tree containing available commands.
         */
        getCommandTree : function() {
            var cmds = {
                name : this.m_all.getLabel(),
                value : this.m_all.getValue()
            };
            return cmds;
        },
        
        m_all : null
       
    }
});