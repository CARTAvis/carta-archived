/**
 * Command to open a dialog for selecting a new image to view.
 */

qx.Class.define("skel.Command.Data.CommandDataOpen", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Open...", null );
        this.setEnabled( false );
        this.m_global = false;
        this.setToolTipText( "Open a data set.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                //Use the first one in the list that supports this cmd.
                var dataCmd = skel.Command.Data.CommandData.getInstance();
                for ( var i = 0; i < activeWins.length; i++ ){
                    if ( activeWins[i].isCmdSupported( dataCmd ) ){
                        qx.event.message.Bus.dispatch(new qx.event.message.Message(
                                "showFileBrowser", activeWins[i]));
                        break;
                    }
                }
            }
        }
    }
});