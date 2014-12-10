/**
 * A display window for a histogram.
 */

/*******************************************************************************
 * 
 * 
 * 
 ******************************************************************************/

qx.Class.define("skel.widgets.Window.DisplayWindowHistogram", {
        extend : skel.widgets.Window.DisplayWindow,

        /**
         * Constructor.
         */
        construct : function(row, col, pluginId, index, detached ) {
            this.base(arguments, skel.widgets.Path.getInstance().HISTOGRAM_PLUGIN, row, col, index, detached );
            this.m_links = [];
        },

        members : {

            /**
             * Returns true if the link from the source window to the
             * destination window was successfully added or removed; false
             * otherwise.
             * 
             * @param sourceWinId {String} an identifier for the link source.
             * @param destWinId {String} an identifier for the link
             *                destination.
             * @param addLink {boolean} true if the link should be added;
             *                false if the link should be removed.
             */
            changeLink : function(sourceWinId, destWinId, addLink) {
                var linkChanged = false;
                if (destWinId == this.m_identifier) {
                    linkChanged = true;
                    var linkIndex = this.m_links.indexOf(sourceWinId);
                    if (addLink && linkIndex < 0) {
                        this.m_links.push(sourceWinId);

                    } else if (!addLink && linkIndex >= 0) {
                        this.m_links.splice(linkIndex, 1);
                        this.m_content.removeAll();
                    }
                }
                return linkChanged;
            },
            
            /**
             * Returns plug-in context menu items that should be displayed
             * on the main menu when this window is selected.
             */
            getWindowSubMenu : function() {
                var windowMenuList = [];
                return windowMenuList;
            },
            

            /**
             * Display specific UI initialization.
             */
            _initDisplaySpecific : function() {
                if (this.m_histogram === null ) {
                    this.m_histogram = new skel.widgets.Histogram.Histogram();
                    this.m_histogram.setId ( this.m_identifier );
                    this.m_histogram.setHeight( 200 );
                    this.m_histogram.setWidth( 200 );
                    
                    this.m_content.add( this.m_histogram);
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
                if (pluginId == path.CASA_LOADER ) {
                    linkable = true;
                } 
                return linkable;
            },
            
            
            /**
             * Implemented to initialize a context menu.
             */
            windowIdInitialized : function() {
                this._initDisplaySpecific();
                arguments.callee.base.apply(this, arguments);
            },

            
            m_histogram : null

        }
});
