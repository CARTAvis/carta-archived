/**
 * Command to show/hide the histogram cube configuration settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Histogram.SettingsCube", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleHistogramCube";
        this.base( arguments, "Histogram Cube Settings", cmd);
        this.setToolTipText( "Show/hide histogram cube settings.");
    },
    
    members : {
        /**
         * Update the visibility of the bin count settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.histogramCube ){
                this.setValue(obj.histogramCube);
            }
        }
    }
});