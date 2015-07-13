/**
 * Command to add a window above the selected windows.
 */

qx.Class.define("skel.Command.Window.WindowAddTop", {
    extend : skel.Command.Window.WindowAddDirection,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Top");
        this.setToolTipText( "Add an additional window above the current window.");
        this.m_direction = "top";
    }
});