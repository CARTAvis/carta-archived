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
        this.setValue( false );
        this.setToolTipText( "Set a predefined layout that displays a single image.");
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