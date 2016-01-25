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
             * Display specific UI initialization.
             */
            _initDisplaySpecific : function() {
                if (this.m_profile === null ) {
                    this.m_profile = new skel.widgets.Profile.Profile();
                    this.m_profile.setId( this.m_identifier);
                    this.m_content.add( this.m_profile, {flex:1});
                }
            },
            
            /**
             * Initialize the list of commands this window supports.
             */
            _initSupportedCommands : function(){
                this.m_supportedCmds = [];
                var linksCmd = skel.Command.Link.CommandLink.getInstance();
                this.m_supportedCmds.push( linksCmd.getLabel() );
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
                                this.m_profile.showHideSettings( setObj.settings );
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
             * Called when the profiler is selected.
             * @param selected {boolean} - true if the window is selected; false otherwise.
             * @param multiple {boolean} - true if there are multiple window problems selected.
             */
            setSelected : function(selected, multiple) {
                this._initSupportedCommands();
                arguments.callee.base.apply(this, arguments, selected, multiple );
            },
           
            
            /**
             * Implemented to initialize a context menu.
             */
            windowIdInitialized : function() {
                this._initDisplaySpecific();
                arguments.callee.base.apply(this, arguments);
                this.initializePrefs();
            },
            
            m_profile : null

        }
});
