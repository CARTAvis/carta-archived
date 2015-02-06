/**
 * Container for clip commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Command.CommandClip", {
    extend : skel.widgets.Command.CommandComposite,
    type : "singleton",

    construct : function( ) {
        this.base( arguments, "Clipping" );
        this.m_cmds = [];
        this.m_cmds[0] = skel.widgets.Command.CommandClipAuto.getInstance();
        this.m_cmds[1] = skel.widgets.Command.CommandClipValues.getInstance();
    }
});