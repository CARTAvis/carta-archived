/**
 * Command to display a histogram of the data in a selected window.
 */

qx.Class.define("skel.Command.Popup.CommandPopupHistogram", {
    extend : skel.Command.Command,
    include : skel.Command.Popup.SingletonLinkMixin,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        this.base( arguments, path.HISTOGRAM_PLUGIN, null );
        this.m_toolBarVisible = false;
        this.m_plugin = path.HISTOGRAM_PLUGIN;
        this.m_global = false;
        this.setToolTipText( "Show a histogram of this data set as a popup window.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                //Use the first one in the list that supports this cmd.
                var cmdPopup = skel.Command.Popup.CommandPopup.getInstance();
                var path = skel.widgets.Path.getInstance();
                for ( var i = 0; i < activeWins.length; i++ ){
                    if ( activeWins[i].isCmdSupported( cmdPopup ) ){
                        var data = {
                            winId : activeWins[i].getIdentifier(),
                            pluginId : path.HISTOGRAM_PLUGIN
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