/**
 * Command to add a window to the left of the selected window(s).
 */

qx.Class.define("skel.Command.Window.WindowAddLeft", {
    extend : skel.Command.Window.WindowAddDirection,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Left");
        this.setToolTipText( "Add an additional window to the left of the current window.");
        this.m_direction = "left";
    }
});