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
             * @param pluginId {String} an identifier the plug-in to be displayed.
             * @param index {Number} index of the plug-in for the case where 
             *          multiple plug-ins of the same type are displayed.
             * @param detached {boolean} true for a dialog type window; 
             *          false for an in-line window.
             */
            construct : function(pluginId, index, detached ) {
                this.base(arguments, pluginId, index, detached );
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
                 * Display specific UI initialization.
                 */
                _initDisplaySpecific : function() {
                    var path = skel.widgets.Path.getInstance();
                    if (this.m_pluginId == path.PLUGINS ) {
                        var pluginList = new skel.boundWidgets.PluginList( this.m_pluginId );
                        this.m_content.add(pluginList);
                    }
                }
            }
        });
