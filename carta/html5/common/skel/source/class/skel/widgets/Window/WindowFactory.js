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
         * Returns a window for displaying the plug-in, if one exists; otherwise,
         * returns null.
         * @param plugin {String}- an identifier for a plug-in.
         * @return {skel.widgets.Window.DisplayWindow} a window displaying the plug-in, if one exists; otherwise, null.
         */
        _findWindow : function( pluginId, pluginIndex){
            var window = null;
            if ( this.m_windows !== null ){
                var windowIndex = -1;
                var pluginCount = -1;
                for ( var i = 0; i < this.m_windows.length; i++ ){
                    if ( this.m_windows[i].getPlugin() === pluginId ){
                        pluginCount = pluginCount + 1;
                        if ( pluginCount == pluginIndex ){
                            windowIndex = i;
                            break;
                        }
                    }
                }
                if ( windowIndex >= 0 ){
                    window = this.m_windows[windowIndex];
                    this.m_windows.splice( windowIndex, 1 );
                }
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
        },
        
        /**
         * Factory method for making window specialized to correct type.
         * 
         * @param pluginId {String} an identifier for the type of plug-in the window will manage.
         * @param index {Number} an index that will be positive when there is more than one window with the same pluginId.
         * @param detached {boolean} true for a pop-up window; false for an in-line window.
         * @return {skel.widgets.Window.Window} the window that was constructed or null if there
         *      is no such window.
         */
        makeWindow : function(pluginId, index, detached ) {
            var window = null;
            if ( !detached ){
                window = this._findWindow( pluginId, index );
            }
            if ( window === null ){
                var path = skel.widgets.Path.getInstance();
                if (pluginId == path.CASA_LOADER) {
                    window = new skel.widgets.Window.DisplayWindowImage( index, detached);
                } 
                else if (pluginId == path.IMAGE_ZOOM) {
                    window = new skel.widgets.Window.DisplayWindowImageZoom( index, detached);
                } 
                else if (pluginId == path.IMAGE_CONTEXT) {
                    window = new skel.widgets.Window.DisplayWindowImageContext( index, detached);
                } 
                else if (pluginId == path.ANIMATOR) {
                    window = new skel.widgets.Window.DisplayWindowAnimation( index, detached );
                } 
                else if ( pluginId == path.COLORMAP_PLUGIN){
                    window = new skel.widgets.Window.DisplayWindowColormap( index, detached );
                }
                else if ( pluginId == path.HISTOGRAM_PLUGIN ){
                    window = new skel.widgets.Window.DisplayWindowHistogram( index, detached );
                }
                else if ( pluginId == path.STATISTICS ){
                    window = new skel.widgets.Window.DisplayWindowStatistics( index, detached );
                }
                else if ( pluginId == path.PROFILE ){
                    window = new skel.widgets.Window.DisplayWindowProfile( index, detached );
                }
                else {
                    window = new skel.widgets.Window.DisplayWindowGenericPlugin( pluginId, index, detached );
                }
            }
            return window;
        },
        
        /**
         * Store the existing windows before a layout change so the factory
         * can recycle them into the new layout cell.s
         * @param windows {Array} existing windows.
         */
        setExistingWindows : function( windows ){
            this.m_windows = windows;
        },
        
        m_windows : null
    }
});
