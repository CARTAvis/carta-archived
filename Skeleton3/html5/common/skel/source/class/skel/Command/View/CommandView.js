/**
 * Command to view a specific plug-in in a given window.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.View.CommandView", {
    extend : skel.Command.Command,

    /**
     * Constructor.
     * @param label {String} the name of the plug-in that can be viewed.
     */
    construct : function( label ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = null;
        this.base( arguments, label, cmd);
        this.setToolTipText( "Change the plugin view to " + this.getLabel()+".");
    },
    
    members : {

        doAction : function( vals, undoCB ){
            var pluginName = this.getLabel();
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                //For now use the first active window.
                var winInfo = skel.Command.Command.m_activeWins[0];
                var data = {
                    location : winInfo.getLocation(),
                    plugin : pluginName
                };
                //Changing the view will change to a custom layout, but we don't want to show
                //the custom layout dialog in such a case.
                var layoutCmd = skel.Command.Layout.CommandLayout.getInstance();
                layoutCmd.setActive( false );
                var customLayoutCmd = skel.Command.Layout.CommandLayoutCustom.getInstance();
                customLayoutCmd.setValue( true );
                skel.widgets.Window.WindowFactory.setExistingWindows( []);
                qx.event.message.Bus.dispatch(new qx.event.message.Message( "setView", data));
                layoutCmd.setActive( true );
            }
        }
    }
});