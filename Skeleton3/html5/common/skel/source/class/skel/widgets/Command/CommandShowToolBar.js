/**
 * Command to show/hide the customizable toolbar.
 */

qx.Class.define("skel.widgets.Command.CommandShowToolBar", {
    extend : skel.widgets.Command.Command,
    type : "singleton",

    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setToolBarVisible";
        this.base( arguments, "Show Toolbar", cmd);
        this.m_value = true;
    },
    
    members : {
        
        doAction : function( vals, objectIDs, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var params = "toolBarVisible:"+vals;
            this.sendCommand( path.PREFERENCES, params, undoCB );
            
        },
        
        getType : function(){
            return skel.widgets.Command.Command.TYPE_BOOL;
        }
    }
});