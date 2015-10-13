/**
 * Displays controls for customizing the color map.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.Settings", {
    extend : qx.ui.core.Widget, 

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this._init();
    },
    

    members : {
        
       
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );

            this._setLayout( new qx.ui.layout.VBox(1));

            this.m_tabView = new qx.ui.tabview.TabView();
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this.m_tabView.setBarPosition( "left" );
            this._add( this.m_tabView );
            
            this.m_colormapPage = new skel.widgets.Colormap.PageColorMap();
            this.m_transformPage = new skel.widgets.Colormap.PageTransform();
            
            this.m_tabView.add( this.m_colormapPage );
            this.m_tabView.add( this.m_transformPage );
        },
        
        /**
         * Update user colormap settings from the server.
         * @param controls {Object} - server user color map settings.
         */
        setControls : function( controls ){
            this.m_colormapPage.setControls( controls );
            this.m_transformPage.setControls( controls );
        },
        
        
        /**
         * Send a command to the server to get the grid control id.
         * @param id {String} the server side id of the image object.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_colormapPage.setId( id );
            this.m_transformPage.setId( id );
        },
        
        m_id : null,
        
        m_tabView : null,
        
        m_colormapPage : null,
        m_transformPage : null
    }
});