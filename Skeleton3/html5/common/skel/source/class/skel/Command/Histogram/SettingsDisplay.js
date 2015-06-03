/**
 * Command to show/hide the histogram display user configuration settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Histogram.SettingsDisplay", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleHistogramDisplay";
        this.base( arguments, "Histogram Display Settings", cmd);
        this.setToolTipText( "Show/hide histogram display settings.");
    },
    
    members : {
        
        /**
         * Update the visibility of the display settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.histogramDisplay ){
                this.setValue( obj.histogramDisplay);
            }
        }
    }
});