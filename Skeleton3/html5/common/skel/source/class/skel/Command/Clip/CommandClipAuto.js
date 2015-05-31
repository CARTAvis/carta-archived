/**
 * Auto clip command.
 */

qx.Class.define("skel.Command.Clip.CommandClipAuto", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + path.AUTO_CLIP;
        this.base( arguments, "Recompute Clips on New Frame", cmd);
        this.m_toolBarVisible = false;
        this.setEnabled( false );
        this.m_global = false;
        this.setValue( false );
    },
    
    members : {
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
        doAction : function( vals,  undoCB ){
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                for ( var i = 0; i < skel.Command.Command.m_activeWins.length; i++ ){
                    var windowInfo = skel.Command.Command.m_activeWins[i];
                    var id = windowInfo.getIdentifier();
                    var params = this.m_params + vals;
                    this.sendCommand( id, params, undoCB );
                }
            }
        },
        
        /**
         * Reset whether or not the command is enabled based on the windows that
         * are selected.
         */
        _resetEnabled : function( ){
            var parentCmd = skel.Command.Clip.CommandClip.getInstance();
            var enabled = parentCmd.isEnabled();
            this.setEnabled( enabled );
        },
        
        m_params : "autoClip:"
    }
});