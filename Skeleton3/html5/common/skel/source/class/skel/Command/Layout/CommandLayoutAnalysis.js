/**
 * Command to change the layout to a preset analysis layout.
 */

qx.Class.define("skel.Command.Layout.CommandLayoutAnalysis", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setAnalysisLayout";
        this.base( arguments, "Analysis Layout", cmd);
        this.m_toolBarVisible = false;
        this.setValue( false );
        this.setToolTipText("Set a predefined layout optimized for analyzing an image.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            if ( vals ){
                var path = skel.widgets.Path.getInstance();
                this.sendCommand( path.BASE_PATH + path.VIEW_MANAGER, "", undoCB );
            }
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        }
        
    }
});