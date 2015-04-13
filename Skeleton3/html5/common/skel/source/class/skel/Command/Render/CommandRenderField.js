/**
 * Command to render the image as a field.
 */

qx.Class.define("skel.Command.Render.CommandRenderField", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Field", null );
        this.m_toolBarVisible = false;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            console.log( "Unimplemented rendering image as a field");
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        }
    }
});