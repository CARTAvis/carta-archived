/**
 * Command to render the image as a raster.
 */

qx.Class.define("skel.Command.Render.CommandRenderRaster", {
    extend : skel.Command.Command,
    type : "singleton",
    
    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Raster", null );
        this.m_toolBarVisible = false;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            console.log( "Need to implement action for raster drawing");
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        }
    }
});