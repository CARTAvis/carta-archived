/**
 * A display window for a color map and optional histogram.
 */

/*******************************************************************************
 * 
 * 
 * 
 ******************************************************************************/

qx.Class.define("skel.widgets.Window.DisplayWindowColormap", {
        extend : skel.widgets.Window.DisplayWindow,

        /**
         * Constructor.
         */
        construct : function(row, col, index, detached ) {
            this.base(arguments, skel.widgets.Path.getInstance().COLORMAP_PLUGIN, row, col, index, detached );
            this.m_content.setLayout( new qx.ui.layout.HBox(0));
            this.m_links = [];
            this.m_index = index;
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
                if (this.m_colormap === null ) {
                    this.m_colormap = new skel.widgets.Colormap.Colormap();
                    this.m_colormap.setId( this.m_identifier );
                    this.m_content.add( this.m_colormap);
                }
                if ( this.m_showHistogram === null ){
                    this.m_showHistogram = new qx.ui.menu.CheckBox( "Show Histogram");
                    
                    this.m_showHistogram.addListener( "execute", this._layoutHistogram, this );
                    this.m_contextMenu.add( this.m_showHistogram );
                }
            },
            
            /**
             * Instantiate the associated histogram and retrieve its id.
             */
            _initHistogram : function(){
                //Get the id of this histogram.
                this.m_histogram = new skel.widgets.Histogram.Histogram();
                var pathDict = skel.widgets.Path.getInstance();
                var paramMap = "pluginId:" + pathDict.HISTOGRAM_PLUGIN + ",index:"+this.m_index;
                var regCmd = pathDict.getCommandRegisterView();
                this.m_connector.sendCommand( regCmd, paramMap, this._regHistogramCB(this));
            },
            

            
            /**
             * Add/remove the associated histogram from the color map.
             */
            _layoutHistogram : function(){
                if ( this.m_showHistogram.getValue() ){
                    if ( this.m_histogram === null ){
                        this._initHistogram();
                    }
                    if ( this.m_content.indexOf( this.m_histogram) < 0 ){
                        if ( this.m_content.indexOf( this.m_colormap) >= 0 ){
                            this.m_content.remove( this.m_colormap );
                        }
                        this.m_content.add( this.m_histogram );
                        this.m_content.add( this.m_colormap );
                    }
                }
                else {
                    if ( this.m_histogram && this.m_content.indexOf( this.m_histogram ) >= 0 ){
                        this.m_content.remove( this.m_histogram);
                    }
                }
            },

            /**
             * Returns whether or not this window can be linked to a window
             * displaying a named plug-in.
             * 
             * @param pluginId {String} a name identifying a plug-in.
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
             * Callback for setting the id of the histogram.
             * @param anObject {skel.widgets.Window.DisplayWindowColormap}.
             */
            _regHistogramCB : function( anObject ){
                return function( id ){
                    if ( id && id.length > 0 ){
                        anObject.m_histogram.setId( id );
                    }
                };
            },

            
            /**
             * Implemented to initialize a context menu.
             */
            windowIdInitialized : function() {
                this._initDisplaySpecific();
                arguments.callee.base.apply(this, arguments);
            },

            
            m_colormap : null,
            m_showHistogram : null,
            m_histogram : null,
            m_index : null
        }
});
