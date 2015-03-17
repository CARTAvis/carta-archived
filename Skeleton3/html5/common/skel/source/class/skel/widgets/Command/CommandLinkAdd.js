/**
 * Command to establish a link from a source to a target.
 */

qx.Class.define("skel.widgets.Command.CommandLinkAdd", {
    extend : skel.widgets.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "linkAdd";
        this.base( arguments, "Add Link", cmd );
    },
    
    members : {
        
        doAction : function( vals, objectIDs, undoCB ){
            var path = skel.widgets.Path.getInstance();
            this.sendCommand( path.BASE_PATH + path.VIEW_MANAGER, vals, undoCB );
        },
        
        getType : function(){
            return skel.widgets.Command.Command.TYPE_BOOL;
        },
        
        /**
         * Tells the server to establish the link.
         * @param sourceId {String} server-side id of the link source.
         * @param destinationId {String} server-side id of the link destination.
         */
        link : function( sourceId, destinationId, undoCB ){
            var params = "sourceId:"+sourceId+",destId:"+destinationId;
            this.doAction( params, null, undoCB );
        }
    }
});