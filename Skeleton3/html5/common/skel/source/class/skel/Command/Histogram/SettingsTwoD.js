/**
 * Command to show/hide the histogram 2D user configuration settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Histogram.SettingsTwoD", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setVisibleHistogram2D";
        this.base( arguments, "Histogram Two Dimensional Settings", cmd);
        this.setToolTipText( "Show/hide histogram two dimensional settings.");
    },
    
    members : {
        /**
         * Update the visibility of the 2D settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.histogram2D ){
                this.setValue(obj.histogram2D);
            }
        }
    }
});