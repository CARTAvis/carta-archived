/**
 * Command to show/hide the color mix settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Colormap.SettingsMix", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleColorMix";
        this.base( arguments, "Color Mix Settings", cmd);
        this.setToolTipText( "Show/hide color mix settings.");
    },
    
    members : {
        /**
         * Update the visibility of the color mix settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.colorMix ){
                this.setValue( obj.colorMix );
            }
        }
    }
});