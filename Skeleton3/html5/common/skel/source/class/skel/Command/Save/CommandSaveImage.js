/**
 * Command to save an image in an active window.
 */

qx.Class.define("skel.Command.Save.CommandSaveImage", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Save...", null );
        this.setEnabled( false );
        this.m_global = false;
        this.setToolTipText( "Save the window image.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                //Use the first one in the list that supports this cmd.
                for ( var i = 0; i < activeWins.length; i++ ){
                    if ( activeWins[i].isCmdSupported( this ) ){
                        qx.event.message.Bus.dispatch(new qx.event.message.Message(
                                "showFileSaver", activeWins[i]));
                        break;
                    }
                }
            }
        }
    }
});