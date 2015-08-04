/**
 * Command to render the image as a vector.
 */

qx.Class.define("skel.Command.Render.CommandRenderVector", {
    extend : skel.Command.Command,
    type : "singleton",
    
    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Vector", null );
        this.m_toolBarVisible = false;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            console.log( "Unimplemented rendering image as vector");
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        }
    }
});