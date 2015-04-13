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
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                var path = skel.widgets.Path.getInstance();
                for ( var i = 0; i < skel.Command.Command.m_activeWins.length; i++ ){
                    var window = skel.Command.Command.m_activeWins[i];
                    var row = window.getRow();
                    var col = window.getCol();
                    var params = "row:"+row+",col:"+col;
                    this.sendCommand( path.LAYOUT, params, undoCB );
                }
            }
        }
    }
});