/**
 * Container for commands that save/restore a snapshot of the session.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Session.CommandSnapshot", {
    extend : skel.Command.CommandGroup,
    type : "singleton",

    construct : function() {
        this.base( arguments, "Snapshot", null );
        this.m_cmds = [];
        this.m_cmds[0] = skel.Command.Session.CommandSave.getInstance();
        this.m_cmds[1] = skel.Command.Session.CommandRestore.getInstance();
    }
});