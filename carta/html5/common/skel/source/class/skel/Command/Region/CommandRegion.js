/**
 * Command to draw a region of a specific type.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Region.CommandRegion", {
    extend : skel.Command.Command,

    /**
     * Constructor.
     * @param label {String} the type of region to draw.
     */
    construct : function( label ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setRegionType";
        this.base( arguments, label, cmd);
        this.m_toolBarVisible = true;
        this.setEnabled( false );
        this.m_global = false;
        this.setValue( false );
        this.setToolTipText("Draw a " + this.getLabel() + " region.");
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
                errMan.updateErrors( "Selected window does not support regions.");
            }
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
        m_params : "type:"
    }
});