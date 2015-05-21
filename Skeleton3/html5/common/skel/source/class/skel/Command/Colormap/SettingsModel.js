/**
 * Command to show/hide the color maodel settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Colormap.SettingsModel", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleColorModel";
        this.base( arguments, "Color Model Settings", cmd);
        this.setToolTipText( "Show/hide color model settings.");
    },
    
    members : {
        /**
         * Update the visibility of the model settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.colorModel ){
                this.setValu(obj.colorModel);
            }
        }
    }
});