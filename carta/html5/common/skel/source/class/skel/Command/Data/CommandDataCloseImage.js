/**
 * Command to close a specific image that has been loaded.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Data.CommandDataCloseImage", {
    extend : skel.Command.Command,

    /**
     * Constructor.
     * @param label {String} name of the image to close.
     */
    construct : function( label ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + path.CLOSE_IMAGE;
        this.base( arguments, label, cmd);
        this.m_toolBarVisible = false;
        this.setEnabled( true );
        this.m_global = false;
        this.setToolTipText("Remove the image " + this.getLabel() + ".");
    },
    
    members : {

        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var label = this.getLabel();
            var params = this.m_params + label;
            var errMan = skel.widgets.ErrorHandler.getInstance();
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                for ( var i = 0; i < skel.Command.Command.m_activeWins.length; i++ ){
                    var windowInfo = skel.Command.Command.m_activeWins[i];
                    var id = windowInfo.getIdentifier();
                    this.sendCommand( id, params, undoCB );
                }
                errMan.clearErrors();
            }
            else {
                errMan.updateErrors( "Error closing image.");
            }
        },
        
        m_params : "image:"
    }
});