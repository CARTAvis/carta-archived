/**
 * Command to display an overlay showing window move locations.
 */

qx.Class.define("skel.Command.Window.WindowMove", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Move...", null );
        this.m_cmds = [];
        this.m_global = false;
        this.setEnabled( false );
        this.setToolTipText( "Show an overlay showing where this window should be moved.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                //Right now just show the link for the first active window.
                var winInfo = skel.Command.Command.m_activeWins[0];
                var linkData = {
                        "window" : winInfo.getIdentifier()
                };
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                    "showMoves", linkData));
            }
            else {
                var errorMan = skel.widgets.ErrorHandler.getInstance();
                errorMan.updateErrors( "Please select a window in order to display move information.");
            }
        }
    }
});