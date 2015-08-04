/**
 * Command to add a window below the selected window(s).
 */

qx.Class.define("skel.Command.Window.WindowAddBottom", {
    extend : skel.Command.Window.WindowAddDirection,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Bottom");
        this.setToolTipText( "Add an additional window below the current window.");
        this.m_direction = "bottom";
    }
});