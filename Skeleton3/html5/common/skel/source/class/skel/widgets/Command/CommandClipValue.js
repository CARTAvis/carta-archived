/**
 * Command to clip to a specific value.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Command.CommandClipValue", {
    extend : skel.widgets.Command.Command,

    construct : function( label ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + path.CLIP_VALUE;
        this.base( arguments, label, cmd);
        this.m_toolBarVisible = true;
    },
    
    members : {

        doAction : function( vals, objectIDs, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var label = this.m_title;
            //Remove the %
            label = label.substring( 0, label.length - 1);
            //Change to a decimal
            label = label / 100;
            var params = this.m_params + label;
            var errMan = skel.widgets.ErrorHandler.getInstance();
            if ( objectIDs.length > 0 ){
                for ( var i = 0; i < objectIDs.length; i++ ){
                    this.sendCommand( objectIDs[i], params, undoCB );
                }
                errMan.clearErrors();
            }
            else {
                errMan.updateErrors( "Please select an image to clip.");
            }
        },
        
        getToolTip : function(){
            return "Set histogram to show " + this.getLabel() + " of the data.<br>" +
                        "Right click also automatically zooms in to the data.";
        },
        
        getType : function(){
            return "number";
        },
        
        m_connector : null,
        m_params : "clipValue:",
        m_sharedVarClips : null
    }
});