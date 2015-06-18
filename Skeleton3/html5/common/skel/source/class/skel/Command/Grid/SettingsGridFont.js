/**
 * Command to show/hide the image grid font settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Grid.SettingsGridFont", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleGridFont";
        this.base( arguments, "Grid Font Settings", cmd);
        this.setToolTipText( "Show/hide grid font settings.");
    },
    
    members : {
        
        /**
         * Update the visibility of the grid font settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.gridFont ){
                this.setValue( obj.gridFont );
            }
        }
    }
});