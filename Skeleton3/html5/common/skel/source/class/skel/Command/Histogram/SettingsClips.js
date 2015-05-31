/**
 * Command to show/hide the histogram clip configuration settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Histogram.SettingsClips", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleHistogramClips";
        this.base( arguments, "Histogram Clip Settings", cmd);
        this.setToolTipText( "Show/hide histogram clip settings.");
    },
    
    members : {
        
        /**
         * Update the visibility of the clip settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( obj.histogramClips != this.getValue()){
                this.setValue(obj.histogramClips);
            }
        }
    }
});