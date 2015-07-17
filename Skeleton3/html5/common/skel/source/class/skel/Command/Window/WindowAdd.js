/**
 * Command to add a window to the display.
 */

qx.Class.define("skel.Command.Window.WindowAdd", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Add", null );
        this.setToolTipText( "Add an additional window.");
        this.m_cmds = [];
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds[0] = skel.Command.Window.WindowAddBottom.getInstance();
        this.m_cmds[1] = skel.Command.Window.WindowAddLeft.getInstance();
        this.m_cmds[2] = skel.Command.Window.WindowAddRight.getInstance();
        this.m_cmds[3] = skel.Command.Window.WindowAddTop.getInstance();
        this.setValue( this.m_cmds );
    }
});