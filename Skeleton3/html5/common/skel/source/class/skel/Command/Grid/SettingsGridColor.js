/**
 * Command to show/hide the image grid color settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Grid.SettingsGridColor", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleGridColor";
        this.base( arguments, "Grid Color Settings", cmd);
        this.setToolTipText( "Show/hide grid color settings.");
    },
    
    members : {
        
        /**
         * Update the visibility of the grid color settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.gridColor ){
                this.setValue( obj.gridColor );
            }
        }
    }
});