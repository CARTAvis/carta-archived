/**
 * Root command.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Command.CommandAll", {
    extend : skel.widgets.Command.CommandComposite,
    type : "singleton",
    
    /**
     * Constructor
     */
    construct : function( ) {
        this.base( arguments, "All");
        this.m_cmds = [];
        this.m_cmds[0] = skel.widgets.Command.CommandClip.getInstance();
        this.m_cmds[1] = skel.widgets.Command.CommandLayout.getInstance();
        this.m_cmds[2] = skel.widgets.Command.CommandShow.getInstance();
    }
});