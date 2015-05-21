/**
 * Command to show/hide the histogram range configuration settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Histogram.SettingsRange", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleHistogramRange";
        this.base( arguments, "Histogram Range Settings", cmd);
        this.setToolTipText( "Show/hide histogram range settings.");
    },
    
    members : {
        
        /**
         * Update the visibility of the range settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.histogramRange ){
                this.setValue(obj.histogramRange);
            }
        }
    }
});