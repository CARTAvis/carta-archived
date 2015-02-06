/**
 * A factory for generating specialized windows.
 */

/*******************************************************************************
 * 
 * 
 * 
 ******************************************************************************/

qx.Class.define("skel.widgets.Window.WindowFactory",{
    type : "static",
    statics : {
        /**
         * Factory method for making window specialized to correct type.
         * 
         * @param pluginId {String} an identifier for the type of plug-in the window will manage.
         * @param index {Number} an index that will be positive when there is more than one window with the same pluginId.
         * @param row {Number} row position of window or -1 if it is a pop-up.
         * @param col {Number} column position of window of -1 if it is a pop-up.
         * @param detached {boolean} true for a pop-up window; false for an in-line window.
         * @return {skel.widgets.Window.Window} the window that was constructed or null if there
         *      is no such window.
         */
        makeWindow : function(pluginId, index, row, col, detached ) {
            var path = skel.widgets.Path.getInstance();
            var window = null;
            if (pluginId == path.CASA_LOADER) {
                window = new skel.widgets.Window.DisplayWindowImage(row, col, index, detached);
            } 
            else if (pluginId == path.ANIMATOR) {
                window = new skel.widgets.Window.DisplayWindowAnimation(row, col, index, detached );
            } 
            else if ( pluginId == path.COLORMAP_PLUGIN){
                window = new skel.widgets.Window.DisplayWindowColormap(row, col, index, detached );
            }
            else if ( pluginId == path.HISTOGRAM_PLUGIN ){
                window = new skel.widgets.Window.DisplayWindowHistogram(row, col, index, detached );
            }
            else {
                window = new skel.widgets.Window.DisplayWindowGenericPlugin( row, col, pluginId, index, detached );
            }
            return window;
        },
        
        /**
         * Returns true if there can be multiple plug-ins of the specified type; false
         * otherwise.
         * @param plugin {String} an plug-in identifier.
         * @return {boolean} true if multiple plug-ins are supported; false otherwise.
         */
        isRegistrationNeeded : function( plugin ){
            var registrationNeeded = true;
            var path = skel.widgets.Path.getInstance();
            if ( plugin == path.PLUGINS ){
                registrationNeeded = false;
            }
            return registrationNeeded;
        }
 
    }
});
