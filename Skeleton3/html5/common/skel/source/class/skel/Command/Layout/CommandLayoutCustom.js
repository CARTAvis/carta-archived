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
        this.setValue( false );
        this.setToolTipText("Specify a layout consisting of a specified number of rows and columns.");
    },
    
    members : {
        
        doAction : function( vals ){
            if ( vals && this.m_active ){
                qx.event.message.Bus.dispatch(new qx.event.message.Message( "showLayoutPopup", vals));
            }
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
        /**
         * Sets whether or not this command's action will be performed.
         * @param active {boolean} true if the action should be performed; false otherwise.
         */
        //Note:  This was inspired by the fact if we add or remove a window in the layout,
        //the server will change to a custom layout.  In such a case, the value of this command
        //may be changed to true(if it is not already a custom layout), but we don't want to
        //show the custom layout popup.
        setActive : function( active ){
            this.m_active = active;
        },
        
        m_active : true
    }
});