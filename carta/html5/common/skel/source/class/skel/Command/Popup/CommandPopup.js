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
        this.setEnabled( false );
        this.m_cmds = [];
        this.m_cmds[0] = skel.Command.Popup.CommandPopupColormap.getInstance();
        this.m_cmds[1] = skel.Command.Popup.CommandPopupHistogram.getInstance();
        this.setValue( this.m_cmds );
    },

    members : {
        
        //Overriden because for this command, we will only show a color map or a histogram
        //as a pop-up if there is not already an inline colormap or histogram linked to the
        //image.
        //We enable this command and adjust its children based on which pop-ups can be shown.
        _resetEnabled : function( ){
            var supported = this._isCmdSupported();
            if ( supported ){
                //Determine the enabled status of this command.
                var colorPopCmd = skel.Command.Popup.CommandPopupColormap.getInstance();
                var colorLinked = colorPopCmd.isLinked( this.m_windows );
                var histPopCmd = skel.Command.Popup.CommandPopupHistogram.getInstance();
                var histLinked = histPopCmd.isLinked ( this.m_windows );
                var enabled = true;
                if ( colorLinked && histLinked ){
                    enabled = false;
                }
                else {
                    this.m_cmds = [];
                    var i = 0;
                    if ( !colorLinked ){
                        this.m_cmds[i] = skel.Command.Popup.CommandPopupColormap.getInstance();
                        i++;
                    }
                    if ( !histLinked ){
                        this.m_cmds[i] = skel.Command.Popup.CommandPopupHistogram.getInstance();
                    }
                    this.setValue( this.m_cmds );
                }
                if ( this.isEnabled() != enabled ){
                    this.setEnabled( enabled );
                }
            }
            else {
                if ( this.isEnabled() ){
                    this.setEnabled( false );
                }
            }
            qx.event.message.Bus.dispatch(new qx.event.message.Message(
                    "commandsChanged", null));
        },
        
        /**
         * Store the list of all available windows.
         * @param wins {Array} - a list of displayed in-line windows.
         */
        //This list is needed so that later we can enable/disable pop-ups based on whether
        //there are already in-line windows displaying linked histograms, colormaps, etc.
        setWindows : function( wins ){
            this.m_windows = wins;
        },
    
        m_windows : null
    }
});