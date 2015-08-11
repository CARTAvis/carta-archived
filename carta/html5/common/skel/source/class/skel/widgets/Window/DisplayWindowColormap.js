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
        include : skel.widgets.Window.PreferencesMixin,

        /**
         * Constructor.
         * @param index {Number} an index in case of multiple windows displaying color maps.
         * @param detached {boolean} true for a pop-up; false for an in-line display.
         */
        construct : function( index, detached ) {
            this.base(arguments, skel.widgets.Path.getInstance().COLORMAP_PLUGIN, index, detached );
            this.m_content.setLayout( new qx.ui.layout.HBox(0));
            this.m_links = [];
        },

        members : {
            
            /**
             * Display specific UI initialization.
             */
            _initDisplaySpecific : function() {
                if (this.m_colormap === null ) {
                    this.m_colormap = new skel.widgets.Colormap.Colormap();
                    this.m_colormap.setId( this.m_identifier );
                    this.m_content.add( this.m_colormap, {flex:1});
                }
            },
            
            /**
             * Initialize the list of commands this window supports.
             */
            _initSupportedCommands : function(){
                
                var linksCmd = skel.Command.Link.CommandLink.getInstance();
                this.m_supportedCmds.push( linksCmd.getLabel() );
                var settingsCmd = skel.Command.Settings.SettingsColor.getInstance();
                this.m_supportedCmds.push( settingsCmd.getLabel());
                arguments.callee.base.apply(this, arguments);
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
                            this.m_colormap.showHideSettings( setObj.settings );
                        }
                        catch( err ){
                            console.log( "Colormap could not parse settings: "+err);
                        }
                    }
                }
            },
            
            /**
             * Set the number of significant digits to display.
             * @param digits {Number} the number of significant digits to display.
             */
            setSignificantDigits : function( digits ){
                if ( this.m_colormap !== null ){
                    this.m_colormap.setSignificantDigits( digits );
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
            
            m_colormap : null
        }
});
