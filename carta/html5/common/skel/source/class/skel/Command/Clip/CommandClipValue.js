/**
 * Command to clip to a specific value.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Clip.CommandClipValue", {
    extend : skel.Command.Command,

    /**
     * Constructor.
     * @param label {String} the amount to clip.
     */
    construct : function( label ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + path.CLIP_VALUE;
        this.base( arguments, label, cmd);
        this.m_toolBarVisible = true;
        this.setEnabled( false );
        this.m_global = false;
        this.setValue( false );
        this.setToolTipText("Show " + this.getLabel() + " of the data.");
    },
    
    members : {

        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var label = this.getLabel();
            //Remove the %
            label = label.substring( 0, label.length - 1);
            //Change to a decimal
            label = label / 100;
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
                errMan.updateErrors( "Selected window does not support clipping.");
            }
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
        m_params : "clipValue:"
    }
});