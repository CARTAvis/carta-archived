/**
 * Command to close a window and remove it from the display.
 */

qx.Class.define("skel.Command.Window.CommandWindowRemove", {
    extend : skel.Command.Command,
    type : "singleton",
    
    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "removeWindow";
        this.base( arguments, "Remove", cmd );
        this.setToolTipText( "Close the selected window.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins.length > 0 ){
                //Removing a window from the layout can change the layout from a recognized one
                //to a custom one.  So that we don't get the custom layout popup to appear signifying
                //a layout change, we temporarily disable it.
                var layoutCmd = skel.Command.Layout.CommandLayout.getInstance();
                layoutCmd.setActive( false );
                var customLayoutCmd = skel.Command.Layout.CommandLayoutCustom.getInstance();
                customLayoutCmd.setValue( true );
                var path = skel.widgets.Path.getInstance();
                for ( var i = 0; i < activeWins.length; i++ ){
                    var locationId = activeWins[i].getLocation();
                    activeWins[i].closeWindow();
                    var params = "id:"+locationId;
                    this.sendCommand( path.LAYOUT, params, undoCB );
                }
                layoutCmd.setActive( true );
            }
        }
    }
});