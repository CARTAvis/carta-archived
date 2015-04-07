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
        
        m_params : "autoClip:"
    }
});