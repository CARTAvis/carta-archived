/**
 * Command for sharing a session.
 */

qx.Class.define("skel.Command.Session.CommandShare", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Share", null );
        this.m_toolBarVisible = false;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var data = {
                share: vals
            }
            qx.event.message.Bus.dispatch( new qx.event.message.Message("shareSession", data));
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        }
    }
});