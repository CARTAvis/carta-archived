/**
 * A display window for a generic plug-in.
 */

/*******************************************************************************
 * 
 * 
 * 
 ******************************************************************************/

qx.Class.define("skel.widgets.DisplayWindowGenericPlugin",
        {
            extend : skel.widgets.DisplayWindow,

            /**
             * Constructor.
             */
            construct : function(row, col, pluginId, winId ) {
                this.base(arguments, pluginId, row, col, winId );
                this.m_links = [];
            },

            members : {
                /**
                 * Implemented to initialize a context menu.
                 */
                setPlugin : function(label) {
                    this._initDisplaySpecific();
                    arguments.callee.base.apply(this, arguments);

                },

                /**
                 * Returns plug-in context menu items that should be displayed
                 * on the main menu when this window is selected.
                 */
                getWindowSubMenu : function() {
                    var windowMenuList = []
                    return windowMenuList;
                },

                /**
                 * Display specific UI initialization.
                 */
                _initDisplaySpecific : function() {
                    if (this.m_pluginId == "plugins") {
                        var pluginList = new skel.boundWidgets.PluginList();
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
                    if (pluginId == skel.widgets.Path.getInstance().CASA_LOADER 
                            && this.m_pluginId == "statistics") {
                        linkable = true;
                    } else if (pluginId == "animator"
                            && this.m_pluginId != "plugins") {
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
                        linkChanged = true;
                        var linkIndex = this.m_links.indexOf(sourceWinId);
                        if (addLink && linkIndex < 0) {
                            this.m_links.push(sourceWinId);

                            // Right now only generic support is statistics.
                            // Need to generalize.
                            if (this.m_title.getValue() == "statistics") {
                                var mouseXPath = skel.widgets.Path
                                        .getInstance().MOUSE_X
                                        + sourceWinId;
                                var labelx = new skel.boundWidgets.Label(
                                        "MouseX:", "pix", mouseXPath);
                                this.m_content.add(labelx);
                                var mouseYPath = skel.widgets.Path
                                        .getInstance().MOUSE_Y
                                        + sourceWinId;
                                var labely = new skel.boundWidgets.Label(
                                        "MouseY:", "pix", mouseYPath);
                                this.m_content.add(labely);
                            }
                        } else if (!addLink && linkIndex >= 0) {
                            this.m_links.splice(linkIndex, 1);
                            this.m_content.removeAll();
                        }
                    }
                    return linkChanged;
                }

            }

        });
