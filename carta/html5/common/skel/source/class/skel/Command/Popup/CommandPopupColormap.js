/**
 * Command to show colormap information for the data in a specific window.
 */

qx.Class.define("skel.Command.Popup.CommandPopupColormap", {
    extend : skel.Command.Command,
    include : skel.Command.Popup.SingletonLinkMixin,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        this.base( arguments, path.COLORMAP_PLUGIN, null );
        this.m_toolBarVisible = false;
        this.m_plugin = path.COLORMAP_PLUGIN;
        this.m_global = false;
        this.setToolTipText( "Show the colormap associated with this data set as a popup window.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                //Use the first one in the list that supports this cmd.
                var path = skel.widgets.Path.getInstance();
                var popupCmd = skel.Command.Popup.CommandPopup.getInstance();
                for ( var i = 0; i < activeWins.length; i++ ){
                    if ( activeWins[i].isCmdSupported( popupCmd ) ){
                        var data  = {
                            winId : activeWins[i].getIdentifier(),
                            pluginId : path.COLORMAP_PLUGIN
                        };
                        qx.event.message.Bus.dispatch(new qx.event.message.Message(
                                "showPopupWindow", data));
                        break;
                    }
                }
            }
        }
    }
});