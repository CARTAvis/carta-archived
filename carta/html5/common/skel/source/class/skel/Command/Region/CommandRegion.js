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
        	if ( this.m_sendToServer ){
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
        	}
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
        /**
         * Set whether or not the action of this command should send the region shape to the server.
         * @param serverSend {boolean} - true if the sever should be notified when this region shape is activated;
         *      false otherwise.
         */
        setServerSend : function( serverSend ){
            this.m_sendToServer = serverSend;
        },
        
        m_params : "type:",
        m_sendToServer : true
    }
});