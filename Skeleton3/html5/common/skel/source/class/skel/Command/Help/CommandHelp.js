/**
 * Container for Help/About commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Help.CommandHelp", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Help", null );
        this.m_cmds = [];
        this.setValue( this.m_cmds );
    }
});