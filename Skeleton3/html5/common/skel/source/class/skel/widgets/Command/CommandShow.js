/**
 * Container for commands that determine whether the menubar, toolbar, statusbar,
 * etc should be shown..
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Command.CommandShow", {
    extend : skel.widgets.Command.CommandComposite,
    type : "singleton",

    construct : function( ) {
        this.base( arguments, "Show" );
        this.m_cmds = [];
        this.m_cmds[0] = skel.widgets.Command.CommandShowMenu.getInstance();
        this.m_cmds[1] = skel.widgets.Command.CommandShowStatus.getInstance();
        this.m_cmds[2] = skel.widgets.Command.CommandShowToolBar.getInstance();
    }
});