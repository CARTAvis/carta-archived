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
         * Initialize the GUI.
         */
        _init : function( ) {
            this.m_content.setLayout(new qx.ui.layout.VBox());
            this._initView();
            this._initSettings( );
            this._layoutControls( );
            this.layout();
        },
        

        /**
         * Initialize the colormap settings (controls).
         */
        _initSettings : function(){
            this.m_settingsComposite = new qx.ui.container.Composite();
            this.m_settingsComposite.setAllowGrowX( true );
            this.m_settingsComposite.setAllowGrowY( true );
            this.m_settingsComposite.setLayout( new qx.ui.layout.Flow());
           
            this.m_content.add( this.m_settingsComposite);
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
         * Layout the main area displaying the colormap and color controls.
         */
        layout : function( ){
            //Toggle the settings.
            var showSettings = false;
            if ( this.m_content.indexOf( this.m_settingsComposite) < 0 ){
                showSettings = true;
            }
            this.m_content.removeAll();
            if ( showSettings ){
                this.m_content.setLayout( new qx.ui.layout.VBox());
                this.m_content.add( this.m_colorMixSettings );
                this.m_content.add( this.m_view );
                this.m_content.add( this.m_settingsComposite);
            }
            else {
                this.m_content.setLayout( new qx.ui.layout.Grow());
                this.m_content.add( this.m_view );
            }
        },
        
        /**
         * Add/remove color map settings based on user preferences.
         */
        _layoutControls : function( ){
            
            //Display combo for map choices added on top of the gradient
            if ( this.m_scaleSettings === null ){
                this.m_scaleSettings = new skel.widgets.Colormap.ColorScale();
                this.m_settingsComposite.add( this.m_scaleSettings );
            }
        
            //Transform settings
            if ( this.m_transformSettings === null ){
                this.m_transformSettings = new skel.widgets.Colormap.ColorTransform();
                this.m_settingsComposite.add( this.m_transformSettings );
            }

            //Model displaying the grid
            if ( this.m_modelSettings === null ){
                this.m_modelSettings = new skel.widgets.Colormap.ColorModel();
                this.m_settingsComposite.add( this.m_modelSettings );
            }
            
            //Red,blue,green sliders in color panel.
            if ( this.m_colorMixSettings === null ){
                this.m_colorMixSettings = new skel.widgets.Colormap.ColorMix();
            }
            //Caching for the map.
            /*if ( this.m_cacheSettings === null ){
                this.m_cacheSettings = new skel.widgets.Colormap.ColorCache();
                this.m_settingsComposite.add( this.m_cacheSettings);
            }*/
            /*if ( this.m_view !== null ){
                this.m_view.setGradientOnly( !this.m_colorMixVisible.getValue());
            }*/
            
            
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
       
        //Layout for the settings
        m_settingsComposite : null,

        //Layout for the permanent parts gradient, lines, etc.
        m_colorComposite : null,
        m_content : null,
        
        //Colormap
        m_view : null,
        
        //Settings
        //m_cacheSettings: null,
        m_colorMixSettings : null,
        m_transformSettings : null,
        m_modelSettings : null,
        m_scaleSettings : null,
        
       
        m_connector : null,
        m_sharedVar : null,
        m_id : null

    }
});
