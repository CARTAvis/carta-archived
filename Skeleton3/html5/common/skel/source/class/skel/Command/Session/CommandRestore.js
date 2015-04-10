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
        this.base( arguments, "Restore Session", cmd );
        this.m_toolBarVisible = false;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            this.sendCommand( "", this.m_SAVE_STATE, function(val){} );
        },
        

        m_SAVE_STATE: "statename:firstSave"
    }
});