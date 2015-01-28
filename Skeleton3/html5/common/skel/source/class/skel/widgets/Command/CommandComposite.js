/**
 * Base class for commands that serve as containers for other commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Command.CommandComposite", {
    extend : skel.widgets.Command.Command,
    
    /**
     * Constructor
     */
    construct : function( label ) {
        this.base( arguments, label, null );
    },
    
    members : {
        
        /**
         * Returns a string identifying this command as a composite.
         * @return {String} a string indicating this command is a composite.
         */
        getType : function(){
            return skel.widgets.Command.Command.TYPE_COMPOSITE;
        },
        
        /**
         * Returns the command corresponding to the name if it matches the name of this
         * command or one of the children of this command; otherwise, returns null.
         * @param cmdName {String} the name of a command to look for.
         * @return {skel.widgets.Command.Command} the corresponding command or null if no matching command exists in the
         *       children of this object.
         */
        getCommand : function( cmdName ){
            var cmd = null;
            if ( this.isMatch( cmdName )){
                cmd = this;
            }
            return cmd;
        },

        /**
         * Returns the child commands of this composite.
         * @return {Array} an array containing the immediate children of this command.
         */
        getValue : function(){
            var allCmds = [];
            return allCmds;
        }
        
    }
});