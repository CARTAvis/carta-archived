/**
 * Container for commands that open a new window displaying information about the
 * selected window.  For example, an image loader could popup a window showing a histogram
 * or a colormap of the image.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Popup.CommandPopup", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments, "Popup" );
        this.m_global = false;
        this.m_enabled = true;
        this.m_cmds = [];
        this.m_cmds[0] = skel.Command.Popup.CommandPopupColormap.getInstance();
        this.m_cmds[1] = skel.Command.Popup.CommandPopupHistogram.getInstance();
    }
    
   
});