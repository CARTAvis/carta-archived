/**
 * Command to change the layout to a custom grid of rows and columns.
 */

qx.Class.define("skel.Command.Layout.CommandLayoutCustom", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Custom Layout", null );
        this.m_toolBarVisible = false;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            qx.event.message.Bus.dispatch(new qx.event.message.Message( "showLayoutPopup", vals));
        },
        
        getToolTip : function(){
            return "Specify a layout consisting of a specified number of rows and columns.";
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        }
    }
});