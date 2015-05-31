/**
 * Command to show a dialog allowing the user to customize which commands should
 * appear on the menu bar and tool bar.
 */

qx.Class.define("skel.Command.Customize.CommandShowCustomizeDialog", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Customize...", null );
        this.m_global = true;
        this.setToolTipText("Select functionality to appear on toolbar and/or menu.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var data = {
                    menu: vals
            };
            qx.event.message.Bus.dispatch(new qx.event.message.Message(
                    "showCustomizeDialog", data));
        },

        
        getType : function(){
            return skel.Command.Command.TYPE_BUTTON;
        }
    }
});
