/**
 * Command to show/hide the color scale settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Colormap.SettingsScale", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleColorScale";
        this.base( arguments, "Color Scale Settings", cmd);
        this.setToolTipText( "Show/hide color scale settings.");
    },
    
    members : {
        
        /**
         * Update the visibility of the scale settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.colorScale ){
                this.setValue(obj.colorScale);
            }
        }
    }
});