/**
 * Command to save a user's session.
 */

qx.Class.define("skel.Command.Session.CommandSave", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Save...", null );
        this.m_toolBarVisible = false;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            qx.event.message.Bus.dispatch(new qx.event.message.Message( "showSessionSaveDialog", vals));
        }
    }
});