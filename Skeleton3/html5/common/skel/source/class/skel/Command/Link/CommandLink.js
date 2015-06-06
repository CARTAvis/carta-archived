/**
 * Command to show/hide the link overlay.
 */

qx.Class.define("skel.Command.Link.CommandLink", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Links...", null );
        this.m_cmds = [];
        this.m_global = false;
        this.setEnabled( false );
        this.setToolTipText( "Show an overlay indicating windows linked to this one.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            if ( this.getValue() != vals ){
                this.setValue( vals );
            }
            if ( this.getValue() ){
                if ( skel.Command.Command.m_activeWins.length > 0 ){
                    //Right now just show the link for the first active window.
                    var winInfo = skel.Command.Command.m_activeWins[0];
                    var linkData = {
                            "plugin" : winInfo.getPlugin(),
                            "window" : winInfo.getIdentifier()
                    };
                    qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "showLinks", linkData));
                }
                else {
                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.updateErrors( "Please select a window in order to display link information.");
                    this.setValue( false );
                }
            }
            else {
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "linkingFinished", ""));
            }
        }
    }
});