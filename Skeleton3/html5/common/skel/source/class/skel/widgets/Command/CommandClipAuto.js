/**
 * Auto clip command.
 */

qx.Class.define("skel.widgets.Command.CommandClipAuto", {
    extend : skel.widgets.Command.Command,
    type : "singleton",

    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + path.AUTO_CLIP;
        this.base( arguments, "Recompute Clips on New Frame", cmd);
    },
    
    members : {
        
        getType : function(){
            return skel.widgets.Command.Command.TYPE_BOOL;
        },
        
        doAction : function( vals, objectIDs, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var params = this.m_params + vals;
            for ( var i = 0; i < objectIDs.length; i++ ){
               this.sendCommand( objectIDs[i], params, undoCB );
            }
        },
        
        m_params : "autoClip:"
    }
});