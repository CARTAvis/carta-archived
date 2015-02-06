/**
 * Command to show/hide the status bar.
 */

qx.Class.define("skel.widgets.Command.CommandShowStatus", {
    extend : skel.widgets.Command.Command,
    type : "singleton",

    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setStatusVisible";
        this.base( arguments, "Show Status", cmd );
    },
    
    members : {
        
        doAction : function( vals, objectIDs, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var params = "statusVisible:"+vals;
            this.sendCommand( path.PREFERENCES, params, undoCB );
        },
        
        getType : function(){
            return skel.widgets.Command.Command.TYPE_BOOL;
        }
    }
});