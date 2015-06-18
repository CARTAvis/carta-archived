/**
 * Command to show/hide the image grid settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Grid.SettingsGrid", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleGrid";
        this.base( arguments, "Grid Settings", cmd);
        this.setToolTipText( "Show/hide grid settings.");
    },
    
    members : {
        
        /**
         * Update the visibility of the grid settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.grid ){
                this.setValue( obj.grid );
            }
        }
    }
});