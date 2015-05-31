/**
 * Command to render the image as contour lines.
 */

qx.Class.define("skel.Command.Render.CommandRenderContour", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Contour", null );
        this.m_toolBarVisible = false;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            console.log( "Unimplemented rendering image as contour");
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        }
    }
});