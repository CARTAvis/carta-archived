/**
 * Command to minimize (iconify) a window.
 */

qx.Class.define("skel.Command.Window.CommandWindowMinimize", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Minimize", null );
        this.setToolTipText( "Minimize the selected window.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                for ( var i = 0; i < skel.Command.Command.m_activeWins.length; i++ ){
                    var window = skel.Command.Command.m_activeWins[i];
                    window.fireDataEvent( "iconify", "");
                }
            }
        }
    }
});