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
             * Display specific UI initialization.
             */
            _initDisplaySpecific : function() {
                if (this.m_histogram === null ) {
                    this.m_histogram = new skel.widgets.Histogram.Histogram();
                    this.m_histogram.setId( this.m_identifier);
                    this.m_content.add( this.m_histogram, {flex:1});
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
                if (pluginId == path.ANIMATOR ){
                    linkable = true;
                }
                return linkable;
            },
            
            /**
             * Toggle the visibility of histogram control settings.
             */
            toggleSettings : function(){
                this.m_histogram.toggleSettings();
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
