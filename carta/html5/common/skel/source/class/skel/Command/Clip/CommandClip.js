/**
 * Container for clip commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Clip.CommandClip", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments, "Clipping" );
        this.m_cmds = [];
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds[0] = skel.Command.Clip.CommandClipAuto.getInstance();
        this.m_cmds[1] = skel.Command.Clip.CommandClipValues.getInstance();
        this.setValue( this.m_cmds );
    }
});