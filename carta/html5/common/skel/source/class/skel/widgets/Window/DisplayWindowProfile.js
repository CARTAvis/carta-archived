/**
 * A display window for profiles.
 */

/*******************************************************************************
 * 
 * 
 * 
 ******************************************************************************/

qx.Class.define("skel.widgets.Window.DisplayWindowProfile", {
        extend : skel.widgets.Window.DisplayWindow,
        include : skel.widgets.Window.PreferencesMixin,

        /**
         * Constructor.
         * @param index {Number} an index in case of multiple windows displaying color maps.
         * @param detached {boolean} true for a pop-up; false for an in-line display.
         */
        construct : function(index, detached ) {
            this.base(arguments, skel.widgets.Path.getInstance().PROFILE, index, detached );
            this.m_links = [];
        },

        members : {
            /**
             * Add or remove the profile settings based on whether the user
             * had configured any of the settings visible.
             * @param content {boolean} - true if the content should be visible; false otherwise.
             */
            _adjustControlVisibility : function(content){
                this.m_controlsVisible = content;
                this._layoutControls();
            },
            
            /**
             * Notification that one or more controls have changed on the server-side.
             * @param ev {qx.event.type.Data}.
             */
            _controlVisibilityChanged : function( ev ){
                var data = ev.getData();
                var showStat = data.showStats;
                this._showHideFitStatistics( showStat );
            },
            
            /**
             * Display specific UI initialization.
             */
            _initDisplaySpecific : function() {
                if (this.m_profile === null ) {
                    this.m_profile = new skel.widgets.Profile.Profile();
                    this.m_profile.addListener( "statVisibilityChanged", this._controlVisibilityChanged, this );
                }
                if ( this.m_profileControls === null ){
                    this.m_profileControls = new skel.widgets.Profile.Settings();
                    this.m_profile.setControls( this.m_profileControls );
                    this.m_profileControls.setId( this.m_identifier );
                }
                this.m_profile.setId( this.m_identifier);
                if ( this.m_fitStatLabel === null ){
                    this._initStatistics();
                }
                this._layoutControls();
            },
            
            /**
             * Initialize the label for displaying fit statistics.
             */
            _initStatistics : function(){
                if ( this.m_fitStatLabel === null ){
                    var path = skel.widgets.Path.getInstance();
                    var viewPath = this.m_identifier + path.SEP + "Fit";
                    this.m_fitStatLabel = new skel.boundWidgets.Label( "", "", viewPath, function( anObject){
                        return anObject.fitStatistics;
                    });
                    this.m_fitStatLabel.setRich( true );
                    
                }
            },
            
            /**
             * Initialize the list of commands this window supports.
             */
            _initSupportedCommands : function(){
                this.m_supportedCmds = [];
                var linksCmd = skel.Command.Link.CommandLink.getInstance();
                this.m_supportedCmds.push( linksCmd.getLabel() );
                var settingsCmd = skel.Command.Settings.SettingsProfile.getInstance();
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
                this.m_content.add( this.m_profile, {flex:1} );
                if ( this.m_statisticsVisible && this.m_fitStatLabel !== null){
                    this.m_content.add( this.m_fitStatLabel );
                }
                if ( this.m_controlsVisible ){
                    this.m_content.add( this.m_profileControls );
                }
            },
            
            /**
             * Callback to show/hide user settings based on updates from the
             * server.
             */
            _preferencesCB : function(){
                if ( this.m_sharedVarPrefs !== null ){
                    var val = this.m_sharedVarPrefs.get();
                    if ( val !== null ){
                        try {
                            var setObj = JSON.parse( val );
                            if ( setObj.settings !== null ){
                                this._adjustControlVisibility( setObj.settings );
                            }
                          
                           
                        }
                        catch( err ){
                            console.log( "Profile window could not parse settings: "+val);
                            console.log( "Error: "+err);
                        }
                    }
                }
            },
            
            /**
             * Callback for updates of fit statistics.
             */
            _profileFitStatsCB : function(){
                var val = this.m_sharedVarFitStats.get();
                if ( val ){
                    try {
                        var stats = JSON.parse( val );
                        if ( this.m_fitStatLabel === null ){
                            this._initStatistics();
                        }
                        this.m_fitStatLabel.setValue( stats.fitStats );
                    }
                    catch( err ){
                        console.log( "Could not parse fit statistics: "+val+" error: "+err );
                    }
                }
            },
            
            /**
             * Register to receive updates to server-side fit statistics.
             */
            _registerStatistics : function(){
                var path = skel.widgets.Path.getInstance();
                var statPath = this.m_identifier + path.SEP + "fitStatistics";
                this.m_sharedVarFitStats = this.m_connector.getSharedVar( statPath );
                this.m_sharedVarFitStats.addCB( this._profileFitStatsCB.bind( this));
                this._profileFitStatsCB();
            },
            
            /**
             * Called when the profiler is selected.
             * @param selected {boolean} - true if the window is selected; false otherwise.
             * @param multiple {boolean} - true if there are multiple window problems selected.
             */
            setSelected : function(selected, multiple) {
                this._initSupportedCommands();
                arguments.callee.base.apply(this, arguments, selected, multiple );
            },
            
            /**
             * Show/hide the cursor statistics control.
             * @param visible {boolean} - true if the cursor statistics widget
             *      should be shown; false otherwise.
             */
            _showHideFitStatistics : function( visible ){
                this.m_statisticsVisible = visible;
                this._layoutControls();
            },
           
            
            /**
             * Implemented to initialize a context menu.
             */
            windowIdInitialized : function() {
                this._initDisplaySpecific();
                arguments.callee.base.apply(this, arguments);
                this.initializePrefs();
                this.m_profileControls.setId( this.getIdentifier());
                this._registerStatistics();
            },
            
            m_controlsVisible : false,
            m_fitStatLabel : null,
            m_profile : null,
            m_profileControls : null,
            m_statisticsVisible : null,
            m_sharedVarFitStats : null

        }
});
