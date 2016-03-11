/**
 * Displays controls for customizing the color map.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.Settings", {
    extend : qx.ui.core.Widget, 
    include : skel.widgets.MTabMixin,
    
    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
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
            this.m_tabListenId = this.m_tabView.addListener( "changeSelection", this._sendTabIndex, this );
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this.m_tabView.setBarPosition( "left" );
            this._add( this.m_tabView );
            
            this.m_pages= [];
            this.m_pages[this.m_INDEX_COLOR] = new skel.widgets.Colormap.PageColorMap();
            this.m_pages[this.m_INDEX_TRANSFORM] = new skel.widgets.Colormap.PageTransform();
            this.m_pages[this.m_INDEX_NAN] = new skel.widgets.Colormap.PageNan();
            this.m_pages[this.m_INDEX_BORDER] = new skel.widgets.Colormap.PageBorderBackground();
            
            for ( var i = 0; i < this.m_pages.length; i++ ){
                this.m_tabView.add( this.m_pages[i]);
            }
        },
        
        /**
         * Update user colormap settings from the server.
         * @param controls {Object} - server user color map settings.
         */
        setControls : function( controls ){
            for ( var i = 0; i < this.m_pages.length; i++ ){
                this.m_pages[i].setControls( controls );
            }
        },
        
        /**
         * Callback for when profile preference state changes on the server.
         */
        _prefCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var prefs = JSON.parse( val );
                    var tabIndex = prefs.tabIndex;
                    this._selectTab( tabIndex );
                }
                catch( err ){
                    console.log( "Could not parse: "+val+" error: "+err );
                }
            }
        },
        
        /**
         * Register to get updates on the preferences.
         */
        _register : function(){
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id );
            this.m_sharedVar.addCB( this._prefCB.bind( this));
            this._prefCB();
        },
        
        
        /**
         * Send a command to the server to get the grid control id.
         * @param id {String} the server side id of the image object.
         */
        setId : function( id ){
            this.m_id = id;
            for ( var i = 0; i < this.m_pages.length; i++ ){
                this.m_pages[i].setId( id );
            }
            this._register();
        },
        
        m_sharedVar : null,
        
        m_INDEX_COLOR : 0,
        m_INDEX_NAN : 1,
        m_INDEX_TRANSFORM : 2,
        m_INDEX_BORDER : 3
    }
});