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
        this._setLayout(new qx.ui.layout.VBox(0));
        this.m_content = new qx.ui.container.Composite();
        this.m_content.setLayout(new qx.ui.layout.VBox(0));
        this._add( this.m_content );
        this.m_connector = mImport("connector");
        this._init( );
    },

    members : {
        /**
         * Initialize the GUI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.HBox(2);
            this.m_content.setLayout(widgetLayout);
            this._initMain();
            this._initControls( );

            this.m_content.add( this.m_controlComposite, {flex: 1});
        },
        

        /**
         * Initialize the colormap settings (controls).
         */
        _initControls : function(){
            this.m_controlComposite = new qx.ui.container.Composite();
            this.m_controlComposite.setAllowGrowX( true );
            this.m_controlComposite.setAllowGrowY( true );
            this.m_controlComposite.setLayout( new qx.ui.layout.Grow());
            this._layoutControls( );
            
        },
        
        /**
         * Initialize the menu for setting controls visible and the main graph.
         */
        _initMain : function(){
            this.m_mainComposite = new qx.ui.container.Composite();
            this.m_mainComposite.setLayout( new qx.ui.layout.VBox(2));
            this.m_mainComposite.setAllowGrowX( true );
            this.m_mainComposite.setAllowGrowY( true );
            
            this.m_settingsComposite = new qx.ui.container.Composite();
            this.m_settingsComposite.setLayout( new qx.ui.layout.HBox(2));
            
            this.m_scaleVisible = new qx.ui.form.CheckBox( "Map");
            this.m_scaleVisible.setValue( true );
            this.m_scaleVisible.addListener("execute", this._layoutControls, this);
            this.m_modelVisible = new qx.ui.form.CheckBox( "Model");
            this.m_modelVisible.setValue( true );
            this.m_modelVisible.addListener("execute", this._layoutControls, this);
            
            this.m_colorMixVisible = new qx.ui.form.CheckBox( "Color Mix");
            this.m_colorMixVisible.setValue( true );
            this.m_colorMixVisible.addListener("execute", this._layoutControls, this);

            this.m_settingsComposite.add( this.m_colorMixVisible );
            this.m_settingsComposite.add( this.m_scaleVisible );
            this.m_settingsComposite.add( this.m_modelVisible );
            
            
            this.m_mainComposite._add( this.m_settingsComposite );
            this.m_content.add(this.m_mainComposite, {flex:1});
            
        },
        
        /**
         * Initialize the view that shows the colormap.
         */
        _initView : function(){
            if ( this.m_view === null ){
                var path = skel.widgets.Path.getInstance();
                this.m_view = new skel.widgets.Colormap.ColorGradient();
                this.m_view.setAllowGrowX( true );
                this.m_view.setAllowGrowY( true );
                
                
                this.m_colorComposite = new qx.ui.container.Composite();
                this.m_colorComposite.setAllowGrowX( true );
                this.m_colorComposite.setAllowGrowY( true );
                this.m_colorComposite.setLayout( new qx.ui.layout.VBox(0) );
                this.m_mainComposite.add( this.m_colorComposite, {flex:1} );
                this._layoutColorPanel();
            }
        },
        
        /**
         * Layout the main area displaying the colormap and color controls.
         */
        _layoutColorPanel : function(){
            if ( this.m_colorComposite !== null ){
                this.m_colorComposite.removeAll();
                //Red,blue green sliders
                if ( this.m_colorMixVisible.getValue() ){
                    this.m_colorComposite.add( this.m_colorMixSettings );
                    this.m_colorMixSettings.addMenuItems( this.m_settingsComposite );
                }
                else {
                    this.m_colorMixSettings.removeMenuItems( this.m_settingsComposite );
                }
                
                //colorGradient
                if ( this.m_view !== null ){
                    this.m_colorComposite.add( this.m_view, {flex: 1} );
                }
                
                //color map settings
                if ( this.m_scaleVisible.getValue()){
                    this.m_colorComposite.add( this.m_scaleSettings);
                }
               
            }
        },
        
        /**
         * Add/remove color map settings based on user preferences.
         */
        _layoutControls : function( ){
            this.m_controlComposite.removeAll();
            
            //Display combo for map choices added on top of the gradient
            if ( this.m_scaleVisible.getValue()){
                if ( this.m_scaleSettings === null ){
                    this.m_scaleSettings = new skel.widgets.Colormap.ColorScale();
                }
            }
            
            //Model displaying the grid
            if ( this.m_modelVisible.getValue()){
                if ( this.m_modelSettings === null ){
                    this.m_modelSettings = new skel.widgets.Colormap.ColorModel();
                }
                if ( this.m_controlComposite.indexOf( this.m_modelSettings) < 0 ){
                    this.m_controlComposite.add( this.m_modelSettings );
                }
                //this.m_modelSettings.addMenuItems( this.m_settingsComposite );
            }
            else {
                if ( this.m_controlComposite.indexOf( this.m_modelSettings ) >= 0 ){
                    this.m_controlComposite.remove( this.m_modelSettings );
                }
            }
          
            //Red,blue,green sliders in color panel.
            if ( this.m_colorMixVisible.getValue()){
                if ( this.m_colorMixSettings === null ){
                    this.m_colorMixSettings = new skel.widgets.Colormap.ColorMix();
                }
            }
            if ( this.m_view !== null ){
                this.m_view.setGradientOnly( !this.m_colorMixVisible.getValue());
            }
           
            this._layoutColorPanel();
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
                            console.log( "Need to figure out model settings better");
                        }
                        if ( this.m_colorMixSettings !== null ){
                            this.m_colorMixSettings.setMix( cMap.colorMix.redPercent, cMap.colorMix.greenPercent, cMap.colorMix.bluePercent );
                        }
                       
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
                    this._registerMapCB();
                }
            }
        },
       
        //Layout
        m_controlComposite : null,
        m_mainComposite : null,
        m_settingsComposite : null,
        m_colorComposite : null,
        m_content : null,
        
        //Colormap
        m_view : null,
        
        //Settings
        m_colorMixVisible : null,
        m_colorMixSettings : null,
        m_modelVisible : null,
        m_modelSettings : null,
        m_scaleVisible : null,
        m_scaleSettings : null,
        
       
        m_connector : null,
        m_sharedVar : null,
        m_id : null

    }
});
