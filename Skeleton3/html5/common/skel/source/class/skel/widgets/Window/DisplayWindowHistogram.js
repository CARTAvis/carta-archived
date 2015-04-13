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
             * Add commands specific to this window.
             */
            addWindowSpecificCommands : function(){
                arguments.callee.base.apply(this, arguments);
                var prefCmd = skel.Command.Preferences.CommandPreferences.getInstance();
                var winSpecific = [];
                winSpecific[0] = skel.Command.Preferences.CommandPreferencesHistogram.getInstance();
                prefCmd.addWindowSpecific( winSpecific );
            },
            
            /**
             * Return the number of significant digits to display.
             * @return {Number} the number of significant digits the user wants to see.
             */
            getSignificantDigits : function(){
                var digits = 6;
                if ( this.m_histogram !== null ){
                    digits = this.m_histogram.getSignificantDigits();
                }
                return digits;
            },

            
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
             * Initialize the list of commands this window supports.
             */
            _initSupportedCommands : function(){
                arguments.callee.base.apply(this, arguments);
                var clipCmd = skel.Command.Clip.CommandClip.getInstance();
                this.m_supportedCmds.push( clipCmd.getLabel() );
                var histPrefCmd = skel.Command.Preferences.CommandPreferencesHistogram.getInstance();
                this.m_supportedCmds.push( histPrefCmd.getLabel() );
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
                return linkable;
            },
            
            /**
             * Set the number of significant digits to display.
             * @param digits {Number} the number of significant digits to display.
             */
            setSignificantDigits : function( digits ){
                if ( this.m_histogram !== null ){
                    this.m_histogram.setSignificantDigits( digits );
                }
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
