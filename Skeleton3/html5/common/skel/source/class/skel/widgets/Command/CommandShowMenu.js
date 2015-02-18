/**
 * Command to show/hide the menu bar.
 */

qx.Class.define("skel.widgets.Command.CommandShowMenu", {
    extend : skel.widgets.Command.Command,
    type : "singleton",

    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setMenuVisible";
        this.base( arguments, "Show Menu", cmd );
    },
    
    members : {
        
        doAction : function( vals, objectIDs, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var params = "menuVisible:"+vals;
            this.sendCommand( path.PREFERENCES, params, undoCB );
        },
        
        getType : function(){
            return skel.widgets.Command.Command.TYPE_BOOLEAN;
        }
    }
});