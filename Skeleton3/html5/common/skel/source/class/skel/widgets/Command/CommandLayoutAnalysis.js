/**
 * Command to change the layout to a preset analysis layout.
 */

qx.Class.define("skel.widgets.Command.CommandLayoutAnalysis", {
    extend : skel.widgets.Command.Command,
    type : "singleton",

    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setAnalysisLayout";
        this.base( arguments, "Analysis Layout", cmd );
    },
    
    members : {
        
        doAction : function( vals, objectIDs, undoCB ){
            var path = skel.widgets.Path.getInstance();
            this.sendCommand( path.BASE_PATH + path.VIEW_MANAGER, "", undoCB );
        },
        
        getType : function(){
            return skel.widgets.Command.Command.TYPE_BOOL;
        }
        
    }
});