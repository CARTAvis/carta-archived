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
        var path = skel.widgets.Path.getInstance();
        var cmd = path.getCommandSaveState();
        this.base( arguments, "Save Session", cmd );
        this.m_toolBarVisible = false;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            this.sendCommand( "", this.m_SAVE_STATE, function(val){} );
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
        m_SAVE_STATE: "statename:firstSave"
    }
});