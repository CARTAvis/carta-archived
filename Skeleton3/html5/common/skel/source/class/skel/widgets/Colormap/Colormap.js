/**
 * A dialog for color map settings.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/


qx.Class.define("skel.widgets.Colormap.Colormap",
{
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this.setAllowGrowX( true );
        this.setAllowGrowY( true );
        this._setLayout(new qx.ui.layout.Grow());
        this.m_content = new qx.ui.container.Composite();
        this._add( this.m_content );
        this.m_connector = mImport("connector");
        this._init( );
    },

    members : {
        
        /**
         * Return the number of significant digits used in the histogram display fields.
         * @return {Number} the significant digits to display.
         */
        getSignificantDigits : function(){
            return this.m_significantDigits;
        },
        
        /**
         * Initialize the GUI.
         */
        _init : function( ) {
            this.m_content.setLayout(new qx.ui.layout.VBox());
            this._initView();
            this._initSettings( );
            //this._layoutControls( );
            this._layoutContent( false );
        },
        

        /**
         * Initialize the colormap settings (controls).
         */
        _initSettings : function(){
            this.m_settingsComposite = new qx.ui.container.Composite();
            this.m_settingsComposite.setAllowGrowX( true );
            this.m_settingsComposite.setAllowGrowY( true );
            this.m_settingsComposite.setLayout( new qx.ui.layout.Flow());
           
            this.m_scaleSettings = new skel.widgets.Colormap.ColorScale();
            this.m_transformSettings = new skel.widgets.Colormap.ColorTransform();
            this.m_modelSettings = new skel.widgets.Colormap.ColorModel();
            this.m_colorMixSettings = new skel.widgets.Colormap.ColorMix();
            //this.m_content.add( this.m_settingsComposite);
        },
        
        /**
         * Initialize the view that shows the colormap.
         */
        _initView : function(){
            if ( this.m_view === null ){
                this.m_view = new skel.widgets.Colormap.ColorGradient();
                this.m_view.setAllowGrowX( true );
                this.m_view.setAllowGrowY( true );
            }
        },
        
        /**
         * Returns whether or not the color mix settings are currently visible.
         * @return {boolean} true if the color mix settings are visible; false otherwise.
         */
        _isColorMixVisible : function(){
            var colorMixVisible = false;
            if ( this.m_content.indexOf( this.m_colorMixSettings) >= 0 ){
                colorMixVisible = true;
            }
            return colorMixVisible;
        },
        
        /**
         * Layout the content based on which configuration settings are
         * visible.
         * @param colorMixVisible {boolean} true if the sliders controlling the color
         *      mix should be visible; false otherwise.
         */
        _layoutContent : function( colorMixVisible ){
            this.m_content.removeAll();
            var layoutItems = this.m_settingsComposite.getChildren();
            var settingsCount = layoutItems.length;
            if ( colorMixVisible || settingsCount >= 1 ){
                this.m_content.setLayout( new qx.ui.layout.VBox());
                if ( colorMixVisible ){
                    this.m_content.add( this.m_colorMixSettings );
                }
                this.m_content.add( this.m_view );
                if ( settingsCount >= 1 ){
                    this.m_content.add( this.m_settingsComposite);
                }
            }
            else {
                this.m_content.setLayout( new qx.ui.layout.Grow());
                this.m_content.add( this.m_view );
            }
        },
        
        /**
         * Add/remove color map settings based on user preferences.
         */
        _layoutControls : function( widget, visible ){
            if ( visible ){
                
                //Add the widget if it is not already there.
                if ( this.m_settingsComposite.indexOf( widget ) < 0 ){
                    this.m_settingsComposite.add( widget );
                }
                //Make sure the settings composite is added to the content.
                var mixVisible = this._isColorMixVisible();
                this._layoutContent( mixVisible );
            }
            else {
                //Remove the widget from the settings container.
                if ( this.m_settingsComposite.indexOf( widget ) >= 0 ){
                    this.m_settingsComposite.remove( widget );
                }
                //If this is the last widget in the container, then remove the container.
                var layoutItems = this.m_settingsComposite.getChildren();
                var childCount = layoutItems.length;
                if ( childCount === 0 ){
                    var colorMixVisible = this._isColorMixVisible();
                    this._layoutContent( colorMixVisible );
                }
            }
        },
        
        
        /**
         * Callback for when the selected color map changes on the server.
         */
        _mapChangedCB : function(){
            if ( this.m_sharedVar ){
                var val = this.m_sharedVar.get();
                if ( val ){
                    try {
                        var cMap = JSON.parse( val );
                        if ( this.m_scaleSettings !== null ){
                            this.m_scaleSettings.setMapName( cMap.colorMapName);
                            this.m_scaleSettings.setReverse( cMap.reverse );
                            this.m_scaleSettings.setInvert( cMap.invert );
                        }
                        if ( this.m_modelSettings !== null ){
                            this.m_modelSettings.setGamma( cMap.gamma );
                        }
                        if ( this.m_colorMixSettings !== null ){
                            this.m_colorMixSettings.setMix( cMap.colorMix.redPercent, cMap.colorMix.greenPercent, cMap.colorMix.bluePercent );
                        }
                        /*if ( this.m_cacheSettings !== null ){
                            this.m_cacheSettings.setCache( cMap.cacheMap );
                            this.m_cacheSettings.setInterpolate( cMap.interpolatedCaching );
                            this.m_cacheSettings.setCacheSize( cMap.cacheSize );
                        }*/
                        if ( this.m_view !== null ){
                            this.m_view.setColorName( cMap.colorMapName );
                            this.m_view.setInvert( cMap.invert );
                            this.m_view.setReverse( cMap.reverse );
                            this.m_view.setScales( cMap.colorMix.redPercent, cMap.colorMix.greenPercent, cMap.colorMix.bluePercent );
                        }
                        this.m_significantDigits = cMap.significantDigits;
                    }
                    catch( err ){
                        console.log( "Could not parse: "+val );
                    }
                }
            }
        },
        
        /**
         * Register to get updates on the selected color map from the server.
         */
        _registerMapCB : function( ){
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id);
            this.m_sharedVar.addCB(this._mapChangedCB.bind(this));
            this._initView();
            this._mapChangedCB();
        },
        
        /**
         * Set the server side is of the colormap.
         * @param id {String} unique identifier for the color map.
         */
        setId : function( id ){
            if ( id !== null ){
                if (  this.m_id != id ){
                    this.m_id = id;
                    this.m_colorMixSettings.setId( this.m_id );
                    this.m_modelSettings.setId( this.m_id );
                    this.m_scaleSettings.setId( this.m_id );
                    //this.m_cacheSettings.setId( this.m_id );
                    this.m_transformSettings.setId( this.m_id );
                    this._registerMapCB();
                }
            }
        },
        
        /**
         * Set the number of significant digits to use for display purposes.
         * @param digits {Number} a positive integer indicating significant digits.
         */
        setSignificantDigits : function( digits ){
            if ( this.m_connector !== null ){
                //Notify the server of the new value.
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setSignificantDigits";
                var params = "significantDigits:"+digits;
                this.m_connector.sendCommand( cmd, params, null);
            }
        },
        
        /**
         * Show/hide the color mix settings.
         * @param visible {boolean} true if the color mix settings should be visible;
         *      false otherwise.
         */
        showHideColorMix : function( visible ){
            this._layoutContent( visible );
        },
        
        /**
         * Show/hide the color transform settings.
         * @param visible {boolean} true if the color transform settings should be visible;
         *      false otherwise.
         */
        showHideColorTransform : function( visible ){
            this._layoutControls( this.m_transformSettings, visible );
        },
        
        /**
         * Show/hide the color model settings.
         * @param visible {boolean} true if the color model settings should be visible;
         *      false otherwise.
         */
        showHideColorModel : function( visible ){
            this._layoutControls( this.m_modelSettings, visible );
        },
        
        /**
         * Show/hide the color scale settings.
         * @param visible {boolean} true if the color scale settings should be visible;
         *      false otherwise.
         */
        showHideColorScale : function( visible ){
            this._layoutControls( this.m_scaleSettings, visible );
        },
       
        //Layout for the settings
        m_settingsComposite : null,

        //Layout for the permanent parts gradient, lines, etc
        m_content : null,
        
        //Colormap
        m_view : null,
        
        //Settings
        //m_cacheSettings: null,
        m_colorMixSettings : null,
        m_transformSettings : null,
        m_modelSettings : null,
        m_scaleSettings : null,
        
        m_significantDigits : null,
        
       
        m_connector : null,
        m_sharedVar : null,
        m_id : null

    }
});
