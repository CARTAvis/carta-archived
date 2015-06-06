/**
 * Command to add a window to the right of the selected windows.
 */

qx.Class.define("skel.Command.Window.WindowAddRight", {
    extend : skel.Command.Window.WindowAddDirection,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Right");
        this.setToolTipText( "Add an additional window to the right of the current window.");
        this.m_direction = "right";
    }
});