/**
 * Command to add a window to the display.
 */

qx.Class.define("skel.Command.Window.CommandWindowAdd", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "addWindow";
        this.base( arguments, "Add", cmd );
        this.setToolTipText( "Add an additional window.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                //Adding a window will change to a custom layout, but we don't want to show
                //the custom layout dialog in such a case.
                var customLayoutCmd = skel.Command.Layout.CommandLayoutCustom.getInstance();
                customLayoutCmd.setActive( false );
                var path = skel.widgets.Path.getInstance();
                for ( var i = 0; i < skel.Command.Command.m_activeWins.length; i++ ){
                    var window = skel.Command.Command.m_activeWins[i];
                    var row = window.getRow();
                    var col = window.getCol();
                    var params = "row:"+row+",col:"+col;
                    customLayoutCmd.setValue( true );
                    this.sendCommand( path.LAYOUT, params, undoCB );
                }
                customLayoutCmd.setActive( true );
            }
        }
    }
});