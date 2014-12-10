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
        this.set({
            width : 700,
            height : 400
        });
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

            this._add( this.m_controlComposite);
        },
        

        /**
         * Initialize the colormap settings (controls).
         */
        _initControls : function(){
            this.m_controlComposite = new qx.ui.container.Composite();
            this.m_controlComposite.setLayout( new qx.ui.layout.HBox(2));
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
            
            var settingsComposite = new qx.ui.container.Composite();
            settingsComposite.setLayout( new qx.ui.layout.HBox(2));
            
            this.m_scaleVisible = new qx.ui.form.CheckBox( "Map...");
            this.m_scaleVisible.setValue( true );
            this.m_scaleVisible.addListener("execute", this._layoutControls, this);
            this.m_modelVisible = new qx.ui.form.CheckBox( "Model...");
            this.m_modelVisible.setValue( true );
            this.m_modelVisible.addListener("execute", this._layoutControls, this);
            this.m_paramsVisible = new qx.ui.form.CheckBox( "Parameters...");
            this.m_paramsVisible.setValue( true );
            this.m_paramsVisible.addListener("execute", this._layoutControls, this);
            this.m_colorMixVisible = new qx.ui.form.CheckBox( "Color Values...");
            this.m_colorMixVisible.setValue( true );
            this.m_colorMixVisible.addListener("execute", this._layoutControls, this);

            settingsComposite.add( this.m_colorMixVisible );
            settingsComposite.add( this.m_scaleVisible );
            settingsComposite.add( this.m_modelVisible );
            settingsComposite.add( this.m_paramsVisible );
            
            
            this.m_mainComposite._add( settingsComposite );
            this.m_content.add(this.m_mainComposite, {flex:1});
        },
        
        /**
         * Initialize the view that shows the colormap.
         */
        _initView : function(){
            if ( this.m_view === null ){
                var path = skel.widgets.Path.getInstance();
                this.m_view = new skel.boundWidgets.View( this.m_id );
                this.m_view.setAllowGrowX( true );
                this.m_view.setAllowGrowY( true );
                
                this.m_colorComposite = new qx.ui.container.Composite();
                this.m_colorComposite.setAllowGrowX( true );
                this.m_colorComposite.setAllowGrowY( true );
                this.m_colorComposite.setLayout( new qx.ui.layout.HBox(2));
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
                if ( this.m_colorMixVisible.getValue() ){
                    this.m_colorComposite.add( this.m_colorMixSettings );
                }
                this.m_colorComposite.add( this.m_view, {flex:1} );
            }
        },
        
        /**
         * Add/remove color map settings based on user preferences.
         */
        _layoutControls : function( ){
            this.m_controlComposite.removeAll();
            if ( this.m_scaleVisible.getValue()){
                if ( !this.m_scaleSettings ){
                    this.m_scaleSettings = new skel.widgets.Colormap.ColorScale();
                }
                this.m_controlComposite.add( this.m_scaleSettings );
            }
            if ( this.m_modelVisible.getValue()){
                if ( this.m_modelSettings === null ){
                    this.m_modelSettings = new skel.widgets.Colormap.ColorModel();
                }
                this.m_controlComposite.add( this.m_modelSettings );
            }
            if ( this.m_paramsVisible.getValue()){
                if ( this.m_paramSettings === null ){
                    this.m_paramSettings = new skel.widgets.Colormap.ColorModelParameters();
                }
                this.m_controlComposite.add( this.m_paramSettings );
            }
            if ( this.m_colorMixVisible.getValue()){
                if ( this.m_colorMixSettings === null ){
                    this.m_colorMixSettings = new skel.widgets.Colormap.ColorMix();
                }
                
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
                            this.m_scaleSettings.setMapIndex( cMap.colorMapIndex );
                            this.m_scaleSettings.setReverse( cMap.reverse );
                            this.m_scaleSettings.setInvert( cMap.invert );
                        }
                        if ( this.m_modelSettings !== null ){
                            console.log( "Need to figure out model settings better");
                        }
                        if ( this.m_colorMixSettings !== null ){
                            this.m_colorMixSettings.setMix( cMap.colorMix.redPercent, cMap.colorMix.greenPercent, cMap.colorMix.bluePercent );
                        }
                        if ( this.m_paramSettings !== null ){
                            console.log( "Need to figure out parameter settings better");
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
         * @param anObject {skel.widgets.Colormap.ColorMapDialog}
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
                    this.m_paramSettings.setId( this.m_id );
                    
                    this._registerMapCB();
                }
            }
        },
       
        //Layout
        m_controlComposite : null,
        m_mainComposite : null,
        m_colorComposite : null,
        m_content : null,
        
        //Colormap
        m_view : null,
        
        //Settings
        m_colorMixVisible : null,
        m_colorMixSettings : null,
        m_modelVisible : null,
        m_modelSettings : null,
        m_paramsVisible : null,
        m_paramSettings : null,
        m_scaleVisible : null,
        m_scaleSettings : null,
        
       
        m_connector : null,
        m_sharedVar : null,
        m_id : null

    }
});
