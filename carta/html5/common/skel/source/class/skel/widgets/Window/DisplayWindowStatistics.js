/**
 * A display window for statistics.
 */

/*******************************************************************************
 * 
 * 
 * 
 ******************************************************************************/

qx.Class.define("skel.widgets.Window.DisplayWindowStatistics", {
        extend : skel.widgets.Window.DisplayWindow,
        include : skel.widgets.Window.PreferencesMixin,

        /**
         * Constructor.
         * @param index {Number} an index in case of multiple windows displaying statistics.
         * @param detached {boolean} true for a pop-up; false for an in-line display.
         */
        construct : function(index, detached ) {
            var statsPlugin= skel.widgets.Path.getInstance().STATISTICS;
            this.base(arguments, statsPlugin, index, detached );
            this.m_links = [];
        },

        members : {
            
            /**
             * Add or remove the statistics settings based on whether the user
             * had configured any of the settings visible.
             * @param content {boolean} - true if the content should be visible; false otherwise.
             */
            _adjustControlVisibility : function(content){
                this.m_controlsVisible = content;
                this._layoutControls();
            },
            
            /**
             * Display specific UI initialization.
             */
            _initDisplaySpecific : function() {
                if (this.m_statistics === null ) {
                    this.m_statistics = new skel.widgets.Statistics.Statistics();
                    this.m_statistics.setId( this.m_identifier);
                    
                }
                if ( this.m_statControls === null ){
                    this.m_statControls = new skel.widgets.Statistics.Settings();
                    this.m_statControls.setId( this.m_identifier);
                }
                this._layoutControls();
            },
            
            /**
             * Initialize the list of commands this window supports.
             */
            _initSupportedCommands : function(){
                this.m_supportedCmds = [];
                var linksCmd = skel.Command.Link.CommandLink.getInstance();
                this.m_supportedCmds.push( linksCmd.getLabel() );
                var settingsCmd = skel.Command.Settings.SettingsStatistics.getInstance();
                this.m_supportedCmds.push( settingsCmd.getLabel());
                arguments.callee.base.apply(this, arguments);
               
            },
           
          
            
            /**
             * Returns whether or not this window can be linked to a window
             * displaying a named plug-in.
             * 
             * @param pluginId {String} a name identifying a plug-in.
             * @return {boolean} true if this window supports linking to the plug-in; false,
             *          otherwise.
             */
            isLinkable : function(pluginId) {
                var linkable = false;
                return linkable;
            },
            
            /**
             * Add/remove content based on user visibility preferences.
             */
            _layoutControls : function(){
                this.m_content.removeAll();
                if ( ! this.m_controlsVisible ){
                    this.m_content.add( this.m_statistics, {flex:1} );
                }
                else {
                    this.m_content.add( this.m_statControls );
                }
            },
            
            /**
             * Callback for updating the visibility of the user settings from the server.
             */
            _preferencesCB : function(){
                if ( this.m_sharedVarPrefs !== null ){
                    var val = this.m_sharedVarPrefs.get();
                    if ( val !== null ){
                        try {
                            var setObj = JSON.parse( val );
                            this._adjustControlVisibility( setObj.settings );
                        }
                        catch( err ){
                            console.log( "ImageDisplay could not parse settings: "+val);
                            console.log( "err="+err);
                        }
                    }
                }
            },

            /**
             * Called when the statistics is selected.
             * @param selected {boolean} - true if the window is selected; false otherwise.
             * @param multiple {boolean} - true if there are multiple window problems selected.
             */
            setSelected : function(selected, multiple) {
                this._initSupportedCommands();
                arguments.callee.base.apply(this, arguments, selected, multiple );
            },
            
            /**
             * Set the number of significant digits to display.
             * @param digits {Number} the number of significant digits to display.
             */
            setSignificantDigits : function( digits ){
                if ( this.m_statistics !== null ){
                    this.m_statistics.setSignificantDigits( digits );
                }
            },

            
            /**
             * Implemented to initialize a context menu.
             */
            windowIdInitialized : function() {
                this._initDisplaySpecific();
                arguments.callee.base.apply(this, arguments);
                this.initializePrefs();
                this.m_statControls.setId( this.getIdentifier());
            },
            
            m_statistics : null,
            m_controlsVisible : false,
            m_statControls : null

        }
});
