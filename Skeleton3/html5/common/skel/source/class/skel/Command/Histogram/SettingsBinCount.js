/**
 * Command to show/hide the histogram bin count settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Histogram.SettingsBinCount", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleHistogramBinCount";
        this.base( arguments, "Histogram Bin Count Settings", cmd);
        this.setToolTipText( "Show/hide histogram bin count settings.");
    },
    
    members : {
        
        /**
         * Update the visibility of the bin count settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.histogramBinCount ){
                this.setValue( obj.histogramBinCount );
            }
        }
    }
});