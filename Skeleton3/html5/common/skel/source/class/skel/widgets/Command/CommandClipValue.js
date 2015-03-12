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
        this.m_winIds = [];
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
            if ( this.m_winIds.length > 0 ){
                for ( var i = 0; i < this.m_winIds.length; i++ ){
                    this.sendCommand( this.m_winIds[i], params, undoCB );
                }
                errMan.clearErrors();
            }
            else {
                errMan.updateErrors( "Selected window does not support clipping.");
            }
        },
        
        getToolTip : function(){
            return "Show " + this.getLabel() + " of the data.";
        },
        
        getType : function(){
            return "number";
        },
        
        m_connector : null,
        m_params : "clipValue:",
        m_sharedVarClips : null
    }
});