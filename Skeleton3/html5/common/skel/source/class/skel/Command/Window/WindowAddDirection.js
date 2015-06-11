/**
 * Command to add a window to the display in a particular direction.
 */

qx.Class.define("skel.Command.Window.WindowAddDirection", {
    extend : skel.Command.Command,

    /**
     * Constructor.
     */
    construct : function( label) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "addWindow";
        this.base( arguments, label, cmd );
        this.m_direction = "";
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                //Adding a window will change to a custom layout, but we don't want to show
                //the custom layout dialog in such a case.
                var layoutCmd = skel.Command.Layout.CommandLayout.getInstance();
                layoutCmd.setActive( false );
                var customLayoutCmd = skel.Command.Layout.CommandLayoutCustom.getInstance();
                customLayoutCmd.setValue( true );
                
                
                //Send the command to the server
                var path = skel.widgets.Path.getInstance();
                var idList = "";
                var windowCount = skel.Command.Command.m_activeWins.length;
                for ( var i = 0; i < windowCount; i++ ){
                    var window = skel.Command.Command.m_activeWins[i];
                    var id = window.getLocation();
                    idList = idList + id;
                    if ( i < windowCount - 1){
                        idList = idList + " ";
                    }
                }
                var params = "id:" + idList + ",position:" + this.m_direction;
                this.sendCommand( path.LAYOUT, params, undoCB );
                layoutCmd.setActive( true );
            }
        },
        
        m_direction : null
    }
});