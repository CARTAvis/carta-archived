/**
 * A display window for a generic plug-in.
 */

/*******************************************************************************
 * 
 * 
 * 
 ******************************************************************************/

qx.Class.define("skel.widgets.Window.DisplayWindowGenericPlugin",
        {
            extend : skel.widgets.Window.DisplayWindow,

            /**
             * Constructor.
             * @param row {Number} the row location of the window.
             * @param col {Number} the column location of the window.
             * @param pluginId {String} an identifier the plug-in to be displayed.
             * @param index {Number} index of the plug-in for the case where 
             *          multiple plug-ins of the same type are displayed.
             * @param detached {boolean} true for a dialog type window; 
             *          false for an in-line window.
             */
            construct : function(row, col, pluginId, index, detached ) {
                this.base(arguments, pluginId, row, col, index, detached );
                this.m_links = [];
            },

            members : {
                /**
                 * Implemented to initialize a context menu.
                 */
                windowIdInitialized : function() {
                    this._initDisplaySpecific();
                    arguments.callee.base.apply(this, arguments);
                },

                /**
                 * Returns plug-in context menu items that should be displayed
                 * on the main menu when this window is selected.
                 */
                getWindowSubMenu : function() {
                    var windowMenuList = [];
                    return windowMenuList;
                },
                
               
                
                _mouseXLookup : function( anObject ){
                    return anObject.mouse.X;
                },
                
                _mouseYLookup : function( anObject ){
                    return anObject.mouse.Y;
                },

                /**
                 * Display specific UI initialization.
                 */
                _initDisplaySpecific : function() {
                    var path = skel.widgets.Path.getInstance();
                    if (this.m_pluginId == path.PLUGINS ) {
                        var pluginList = new skel.boundWidgets.PluginList( this.m_pluginId );
                        this.m_content.add(pluginList);
                    }
                },

                /**
                 * Returns whether or not this window can be linked to a window
                 * displaying a named plug-in.
                 * 
                 * @param pluginId
                 *                {String} a name identifying a plug-in.
                 */
                isLinkable : function(pluginId) {
                    var linkable = false;
                    var path = skel.widgets.Path.getInstance();
                    if (pluginId == path.CASA_LOADER && this.m_pluginId == "statistics") {
                        linkable = true;
                    } 
                    else if (pluginId == path.ANIMATOR && this.m_pluginId != path.PLUGINS) {
                        linkable = true;
                    }
                    return linkable;
                },

                /**
                 * Returns true if the link from the source window to the
                 * destination window was successfully added or removed; false
                 * otherwise.
                 * 
                 * @param sourceWinId
                 *                {String} an identifier for the link source.
                 * @param destWinId
                 *                {String} an identifier for the link
                 *                destination.
                 * @param addLink
                 *                {boolean} true if the link should be added;
                 *                false if the link should be removed.
                 */
                changeLink : function(sourceWinId, destWinId, addLink) {
                    var linkChanged = false;
                    if (destWinId == this.m_identifier) {
                        linkChanged = arguments.callee.base.apply(this, arguments, sourceWinId, destWinId, addLink);
                        var linkIndex = this.m_links.indexOf(sourceWinId);
                        if (addLink && linkIndex < 0) {

                            // Right now only generic support is statistics.
                            // Need to generalize.
                            if (this.m_pluginId == "statistics") {
                                var path = skel.widgets.Path.getInstance();
                                var viewPath = sourceWinId + path.SEP + path.VIEW;
                                var labelx = new skel.boundWidgets.Label("MouseX:", "pix", viewPath, function(anObject){
                                    return anObject.mouse.y;
                                } );
                                this.m_content.add(labelx);
                                var labely = new skel.boundWidgets.Label("MouseY:", "pix", viewPath, function(anObject){
                                    return anObject.mouse.x;
                                });
                                this.m_content.add(labely);
                            }
                        } else if (!addLink && linkIndex >= 0) {
                            this.m_content.removeAll();
                        }
                    }
                    return linkChanged;
                }

            }

        });
