/**
 * Command to center an image view.
 */

qx.Class.define("skel.Command.Data.CommandPanReset", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "resetPan";
        this.base( arguments, "Pan Reset", cmd);
        this.setEnabled( false );
        this.m_global = false;
        this.setToolTipText( "Center the image.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                var emptyFunc = function(){};
                for ( var i = 0; i < activeWins.length; i++ ){
                    if ( activeWins[i].isCmdSupported( this ) ){
                        var id = activeWins[i].getIdentifier();
                        var params = "";
                        this.sendCommand( id, params, emptyFunc);
                    }
                }
            }
        }
    }
});