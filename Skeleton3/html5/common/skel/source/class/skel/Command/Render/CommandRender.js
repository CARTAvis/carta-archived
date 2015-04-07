/**
 * Container for rendering images in different formats.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Render.CommandRender", {
    extend : skel.Command.CommandGroup,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments, "Render" );
        this.m_global = false;
        this.m_enabled = false;
        this.m_cmds = [];
        this.m_cmds[0] = skel.Command.Render.CommandDataOpen.getInstance();
        this.m_cmds[1] = skel.Command.Render.CommandDataClose.getInstance();
    }
});