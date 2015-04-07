/**
 * Container for commands that change the layout.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Layout.CommandLayout", {
    extend : skel.Command.CommandGroup,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Layout", null );
        this.m_cmds = [];
        this.m_cmds[0] = skel.Command.Layout.CommandLayoutImage.getInstance();
        this.m_cmds[1] = skel.Command.Layout.CommandLayoutAnalysis.getInstance();
        this.m_cmds[2] = skel.Command.Layout.CommandLayoutCustom.getInstance();
    }
});