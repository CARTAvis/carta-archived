/**
 * Command to change the layout to display a single image.
 */

qx.Class.define("skel.Command.Layout.CommandLayoutImage", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setImageLayout";
        this.base( arguments, "Image Layout", cmd );
        this.m_toolBarVisible = false;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            this.sendCommand( path.BASE_PATH + path.VIEW_MANAGER, "", undoCB );
        },
        
        getToolTip : function(){
            return "Set a predefined layout that displays a single image.";
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        }
    }
});