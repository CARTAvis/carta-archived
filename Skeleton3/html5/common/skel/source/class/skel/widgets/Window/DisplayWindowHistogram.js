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
         * @param row {Number} the row location.
         * @param col {Number} the column location.
         * @param index {Number} an index in case of multiple windows displaying color maps.
         * @param detached {boolean} true for a pop-up; false for an in-line display.
         */
        construct : function(row, col, index, detached ) {
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
             * @return {boolean} true if the link changed status; false otherwise.
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
             * Display specific UI initialization.
             */
            _initDisplaySpecific : function() {
                if (this.m_histogram === null ) {
                    this.m_histogram = new skel.widgets.Histogram.Histogram();
                    this.m_histogram.m_settingsVisible = false;
                    this.m_histogram.setHeight( 200 );
                    this.m_histogram.setWidth( 200 );
                    this.m_histogram.setId( this.m_identifier);
                    this.m_content.add( this.m_histogram);
                }
            },
           
            /**
             * Returns whether or not this window can be linked to a window
             * displaying a named plug-in.
             * 
             * @param pluginId {String} a name identifying a plug-in.
             * @return {boolean} true if this window can be linked to the plug-in;
             *          false otherwise.
             */
            isLinkable : function(pluginId) {
                var linkable = false;
                var path = skel.widgets.Path.getInstance();
                if (pluginId == path.CASA_LOADER ) {
                    linkable = true;
                } 
                return linkable;
            },
            
            toggleSettings : function(){
                if(this.m_histogram.m_settingsVisible){
                    this.m_histogram.m_settingsVisible = false;
                    this.m_histogram.layoutControls();
                }
                else{
                    this.m_histogram.m_settingsVisible = true;
                    this.m_histogram.layoutControls();
                }
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
