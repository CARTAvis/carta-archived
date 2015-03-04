/**
 * Command to close a specific image that has been loaded.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Command.CommandCloseImage", {
    extend : skel.widgets.Command.Command,

    /**
     * Constructor.
     * @param label {String} name of the image to close.
     */
    construct : function( label ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + path.CLOSE_IMAGE;
        this.base( arguments, label, cmd);
        this.m_toolBarVisible = false;
    },
    
    members : {

        doAction : function( vals, objectIDs, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var label = this.m_title;
            var params = this.m_params + label;
            var errMan = skel.widgets.ErrorHandler.getInstance();
            if ( objectIDs.length > 0 ){
                for ( var i = 0; i < objectIDs.length; i++ ){
                    this.sendCommand( objectIDs[i], params, undoCB );
                }
                errMan.clearErrors();
            }
            else {
                errMan.updateErrors( "Error closing image.");
            }
        },
        
        getToolTip : function(){
            return "Remove the image " + this.getLabel() + ".";
        },
        
        getType : function(){
            return "number";
        },
        
        m_params : "image:"
    }
});