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
             * Return the number of significant digits to display.
             * @return {Number} the number of significant digits the user wants to see.
             */
            getSignificantDigits : function(){
                var digits = 6;
                if ( this.m_colormap !== null ){
                    digits = this.m_colormap.getSignificantDigits();
                }
                return digits;
            },
            
            
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
                arguments.callee.base.apply(this, arguments);
                var settingsCmd = skel.Command.Colormap.Colormap.getInstance();
                this.m_supportedCmds.push( settingsCmd.getLabel());
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
                if (pluginId == path.HISTOGRAM_PLUGIN ) {
                    linkable = true;
                } 
                return linkable;
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
                            this._showHideColorMix( setObj.colorMix );
                            this._showHideColorModel( setObj.colorModel );
                            this._showHideColorScale( setObj.colorScale );
                            this._showHideColorTransform( setObj.colorTransform );
                        }
                        catch( err ){
                            console.log( "Colormap could not parse settings");
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
             * Show/hide color mix user settings.
             * @param visible {boolean} true if the color mix settings should be 
             *          visible; false otherwise.
             */
            _showHideColorMix : function( visible ){
                if ( this.m_colormap !== null ){
                    this.m_colormap.showHideColorMix( visible );
                }
            },
            
            /**
             * Show/hide color model settings.
             * @param visible {boolean} true if the color model settings should be 
             *          visible; false otherwise.
             */
            _showHideColorModel : function( visible ){
                if ( this.m_colormap !== null ){
                    this.m_colormap.showHideColorModel( visible );
                }
            },
            
            /**
             * Show/hide color scale user settings.
             * @param visible {boolean} true if the color scale settings should be 
             *          visible; false otherwise.
             */
            _showHideColorScale : function( visible ){
                if ( this.m_colormap !== null ){
                    this.m_colormap.showHideColorScale( visible );
                }
            },
            
            /**
             * Show/hide color transform user settings.
             * @param visible {boolean} true if the color transform settings should be 
             *          visible; false otherwise.
             */
            _showHideColorTransform : function( visible ){
                if ( this.m_colormap !== null ){
                    this.m_colormap.showHideColorTransform( visible );
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
