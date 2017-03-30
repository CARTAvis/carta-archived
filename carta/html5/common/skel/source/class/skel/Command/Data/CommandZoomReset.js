/**
 * Command to reset the zoom in an image view.
 */

qx.Class.define("skel.Command.Data.CommandZoomReset", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();

        // no use now.  use resetZoomToFitWindow instead 
        var cmd = path.SEP_COMMAND + "resetZoom";

        this.base( arguments, "Zoom Reset", cmd);
        this.setEnabled( false );
        this.m_global = false;
        this.setToolTipText( "Reset the zoom level to its original value.");
    },

    members : {
        doAction : function( vals, undoCB ){
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                var emptyFunc = function(){};
                for ( var i = 0; i < activeWins.length; i++ ){
                    if ( activeWins[i].isCmdSupported( this ) ){

                        if (activeWins[i].resetZoomToFitWindow) {
                            activeWins[i].resetZoomToFitWindow();
                        }

                        // var id = activeWins[i].getIdentifier();
                        // var params = "";
                        // this.sendCommand( id, params, emptyFunc);
                    }
                }
            }
        }
    }
});
