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
        this.m_enabled = true;
        this.m_cmds = [];
        this.m_cmds[0] = skel.Command.Data.CommandDataOpen.getInstance();
        this.m_cmds[1] = skel.Command.Data.CommandDataClose.getInstance();
    }
});