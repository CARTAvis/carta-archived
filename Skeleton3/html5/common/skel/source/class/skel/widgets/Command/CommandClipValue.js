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
    },
    
    members : {

        doAction : function( vals, objectIDs, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var params = this.m_params + vals;
            for ( var i = 0; i < objectIDs.length; i++ ){
               this.sendCommand( objectIDs[i], params, undoCB );
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