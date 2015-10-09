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
        
        /**
         * Return the value displayed by this command as a number between 0 and 1.
         * @return {Number} - the amount to clip as a number between 0 and 1.
         */
        getClipPercent : function(){
            var label = this.getLabel();
            var val = label.substring( 0, label.length - 1 );
            val = val / 100;
            return val;
        },

        doAction : function( vals, undoCB ){
            if ( this.m_sendToServer ){
                var path = skel.widgets.Path.getInstance();
                var label = this.getClipPercent();
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
            }
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
        /**
         * Set whether or not the action of this command should send the clip to the server.
         * @param serverSend {boolean} - true if the sever should be notified when this clip is activated;
         *      false otherwise.
         */
        setServerSend : function( serverSend ){
            this.m_sendToServer = serverSend;
        },
        
        // Flag was added to prevent sending a clip command to the server when an image loaded
        // is selected and sets its currently selected clip value.
        m_sendToServer : true,
        m_params : "clipValue:"
    }
});