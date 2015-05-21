/**
 * Command to show/hide the color transform settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Colormap.SettingsTransform", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleColorTransform";
        this.base( arguments, "Color Transform Settings", cmd);
        this.setToolTipText( "Show/hide color transform settings.");
    },
    
    members : {
        
        /**
         * Update the visibility of the transform settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.colorTransform ){
                this.setValue( obj.colorTransform);
            }
        }
    }
});