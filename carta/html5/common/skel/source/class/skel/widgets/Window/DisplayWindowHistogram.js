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
         * @param index {Number} an index in case of multiple windows displaying color maps.
         * @param detached {boolean} true for a pop-up; false for an in-line display.
         */
        construct : function(index, detached ) {
            this.base(arguments, skel.widgets.Path.getInstance().HISTOGRAM_PLUGIN, index, detached );
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
             * Initialize the list of commands this window supports.
             */
            _initSupportedCommands : function(){
                if ( this.m_supportedCmds.length == 0 ){
                    var linksCmd = skel.Command.Link.CommandLink.getInstance();
                    this.m_supportedCmds.push( linksCmd.getLabel() );
                    
                    var clipCmd = skel.Command.Clip.CommandClip.getInstance();
                    this.m_supportedCmds.push( clipCmd.getLabel() );
                   
                    var histCmd = skel.Command.Settings.SettingsHistogram.getInstance();
                    this.m_supportedCmds.push( histCmd.getLabel() );
                    var saveCmd = skel.Command.Save.CommandSaveImage.getInstance();
                    if ( saveCmd.isSaveAvailable() ){
                        this.m_supportedCmds.push( saveCmd.getLabel() );
                    }
                    var popupCmd = skel.Command.Popup.CommandPopup.getInstance();
                    this.m_supportedCmds.push( popupCmd.getLabel() );
                    
                    arguments.callee.base.apply(this, arguments);
                }
               
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
                var path = skel.widgets.Path.getInstance();
                if (pluginId == path.COLORMAP_PLUGIN ) {
                    linkable = true;
                } 
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
                                this.m_histogram.showHideSettings( setObj.settings );
                            }
                           
                        }
                        catch( err ){
                            console.log( "Histogram could not parse settings: "+val);
                            console.log( "Error: "+err);
                        }
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
