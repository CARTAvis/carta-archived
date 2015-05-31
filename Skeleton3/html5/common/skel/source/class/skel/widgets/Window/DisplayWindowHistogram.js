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
        include : skel.widgets.Window.PreferencesMixin,

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
               
                var histCmd = skel.Command.Histogram.Histogram.getInstance();
                this.m_supportedCmds.push( histCmd.getLabel() );
               
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
             * Callback to show/hide user settings based on updates from the
             * server.
             */
            _preferencesCB : function(){
                if ( this.m_sharedVarPrefs !== null ){
                    var val = this.m_sharedVarPrefs.get();
                    try {
                        var setObj = JSON.parse( val );
                        this._showHideBinCount( setObj.histogramBinCount );
                        this._showHideClips( setObj.histogramClips );
                        this._showHideCube( setObj.histogramCube );
                        this._showHideRange( setObj.histogramRange );
                        this._showHideDisplay( setObj.histogramDisplay );
                        this._showHide2D( setObj.histogram2D );
                    }
                    catch( err ){
                        console.log( "Histogram could not parse settings");
                    }
                }
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
             * Show/hide bin count user settings.
             * @param visible {boolean} true if the bin count settings should be 
             *          visible; false otherwise.
             */
            _showHideBinCount : function( visible ){
                if ( this.m_histogram !== null ){
                    this.m_histogram.showHideBinCount( visible );
                }
            },
            
            /**
             * Show/hide cube user settings.
             * @param visible {boolean} true if the cube settings should be 
             *          visible; false otherwise.
             */
            _showHideClips : function( visible ){
                if ( this.m_histogram !== null ){
                    this.m_histogram.showHideClips( visible );
                }
            },
            
            /**
             * Show/hide cube user settings.
             * @param visible {boolean} true if the cube settings should be 
             *          visible; false otherwise.
             */
            _showHideCube : function( visible ){
                if ( this.m_histogram !== null ){
                    this.m_histogram.showHideCube( visible );
                }
            },
            
            /**
             * Show/hide two-dimensional user settings.
             * @param visible {boolean} true if the 2D settings should be 
             *          visible; false otherwise.
             */
            _showHide2D : function( visible ){
                if ( this.m_histogram !== null ){
                    this.m_histogram.showHide2D( visible );
                }
            },
            
            /**
             * Show/hide range user settings.
             * @param visible {boolean} true if the range settings should be 
             *          visible; false otherwise.
             */
            _showHideRange : function( visible ){
                if ( this.m_histogram !== null ){
                    this.m_histogram.showHideRange( visible );
                }
            },
            
            /**
             * Show/hide display user settings.
             * @param visible {boolean} true if the display settings should be 
             *          visible; false otherwise.
             */
            _showHideDisplay : function( visible ){
                if ( this.m_histogram !== null ){
                    this.m_histogram.showHideDisplay( visible );
                }
            },
            
            
            /**
             * Implemented to initialize a context menu.
             */
            windowIdInitialized : function() {
                this._initDisplaySpecific();
                arguments.callee.base.apply(this, arguments);
                this.initializePrefs();
            },
            
            m_histogram : null

        }
});
