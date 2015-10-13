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
            this.m_mapControl = new skel.widgets.Colormap.ColorMapsWidget();
            this._initView();
            this._initSettings( );
            this._layoutContent( false );
        },
        

        /**
         * Initialize the colormap settings (controls).
         */
        _initSettings : function(){
            this.m_settings = new skel.widgets.Colormap.Settings();
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
        _layoutContent : function( settingsVisible ){
            this.m_content.removeAll();
            this.m_content.setLayout( new qx.ui.layout.VBox());
            if ( settingsVisible ){
                this.m_content.add( this.m_view );
                this.m_content.add( this.m_mapControl );
                this.m_content.add( this.m_settings);
            }
            else {
                //this.m_content.setLayout( new qx.ui.layout.Grow());
                this.m_content.add( this.m_view, {flex:1} );
                this.m_content.add( this.m_mapControl );
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
                        
                        if ( this.m_view !== null ){
                            this.m_view.setColorName( cMap.colorMapName );
                            this.m_view.setInvert( cMap.invert );
                            this.m_view.setReverse( cMap.reverse );
                            this.m_view.setScales( cMap.colorMix.redPercent, cMap.colorMix.greenPercent, cMap.colorMix.bluePercent );
                        }
                        this.m_settings.setControls( cMap );
                        
                    }
                    catch( err ){
                        console.log( "Colormap could not parse: "+val );
                        console.log( err );
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
                    this.m_settings.setId( id );
                    this.m_mapControl.setId( id );
                    this._registerMapCB();
                }
            }
        },
        
        
        /**
         * Show/hide the color mix settings.
         * @param visible {boolean} true if the color mix settings should be visible;
         *      false otherwise.
         */
        showHideSettings : function( visible ){
            this._layoutContent( visible );
        },
       
        //Layout for the settings
        m_settings : null,

        //Layout for the permanent parts gradient, lines, etc
        m_content : null,
        
        //Colormap gradient
        m_view : null,
        
        //Color map selection
        m_mapControl : null,
        
        m_connector : null,
        m_sharedVar : null,
        m_id : null
    }
});
