/**
 * Container for commands that open/close image data.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Data.CommandData", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments, "Data" );
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds = [];
        this.m_cmds[0] = skel.Command.Data.CommandDataOpen.getInstance();
        this.m_cmds[1] = skel.Command.Data.CommandDataClose.getInstance();
        this.m_cmds[2] = skel.Command.Data.CommandDataHide.getInstance();
        this.m_cmds[3] = skel.Command.Data.CommandDataShow.getInstance();
        this.setValue( this.m_cmds );
    },
    
    members : {
        
        /**
         * Called when an image window has been selected and the images available
         * for closing, etc, have changed.
         */
        datasChanged : function(){
            for ( var i = 1; i < this.m_cmds.length; i++ ){
                this.m_cmds[i].datasChanged();
            }
        },
        
        //Note:  Overriden so that the _resetEnabled method of CommandDataClose will
        //be called, giving it the opportunity to add CommandDataCloseImage commands
        //based on the window(s) selected.
        _resetEnabled : function( ){
            arguments.callee.base.apply(this, arguments);
            this.m_cmds[1]._resetEnabled();
            this.m_cmds[2]._resetEnabled();
            var enabled = this.isEnabled();
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                this.m_cmds[i].setEnabled( enabled );
            }
        }
    }
});