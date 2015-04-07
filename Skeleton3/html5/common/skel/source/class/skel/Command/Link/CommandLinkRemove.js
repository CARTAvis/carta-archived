/**
 * Command to remove a link between a source and a destination.
 */

qx.Class.define("skel.Command.Link.CommandLinkRemove", {
    extend : skel.Command.Command,
    type : "singleton",
    
    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "linkRemove";
        this.base( arguments, "Link Remove", cmd );
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            this.sendCommand( path.BASE_PATH + path.VIEW_MANAGER, vals, undoCB );
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
        /**
         * Tells the server to remove the link.
         * @param sourceId {String} server-side id of the link source.
         * @param destinationId {String} server-side id of the link destination.
         */
        link : function( sourceId, destinationId ){
            var params = "sourceId:"+sourceId+",destId:"+destinationId;
            this.doAction( params, null );
        }
    }
});