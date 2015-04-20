/**
 * Command to restore a user's session.
 */

qx.Class.define("skel.Command.Session.CommandRestore", {
    extend : skel.Command.Command,
    type : "singleton",
    
    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.getCommandRestoreState();
        this.base( arguments, "Restore...", cmd );
        this.m_toolBarVisible = false;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            qx.event.message.Bus.dispatch(new qx.event.message.Message( "showSessionRestoreDialog", vals));
        }
    }
});