/**
 * Command to restore a maximized window to its previous in-line position.
 */

qx.Class.define("skel.Command.Window.CommandWindowRestore", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Restore", null );
        this.setToolTipText( "Restore the window to its previous in-line position.");
    },
    
    events : {
        "windowRestored" : "qx.event.type.Data"
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                this.fireDataEvent( "windowRestored", "");
                for ( var i = 0; i < skel.Command.Command.m_activeWins.length; i++ ){
                    var window = skel.Command.Command.m_activeWins[i];
                    window.fireDataEvent( "windowRestored", "");
                }
            }
        }
    }
});