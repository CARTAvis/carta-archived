/**
 * A specialization of a composite where only one command can be chosen at a
 * single time. 
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.CommandGroup", {
    extend : skel.Command.CommandComposite,
    
    /**
     * Constructor
     */
    construct : function( label ) {
        this.base( arguments, label, null );
    },
    
    members : {
        
        /**
         * Returns a string identifying this command as a group.
         * @return {String} a string indicating this command is a group.
         */
        getType : function(){
            return skel.Command.Command.TYPE_GROUP;
        }
    }
});