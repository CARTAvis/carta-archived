
/**
 * Displays controls for customizing the compass on the image.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Context.SettingsCompass", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Compass", "");
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this._setLayout(new qx.ui.layout.VBox(2));
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content, {flex:1} );
            this.m_content.setLayout(new qx.ui.layout.HBox(5));
            this.m_tabView = new qx.ui.tabview.TabView("left");
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this.m_tabListenId = this.m_tabView.addListener( "changeSelection", this._sendTabIndex, this );
            this._add( this.m_tabView );
          
            this.m_pages = [];
            this.m_pages[this.m_neIndex] = new skel.widgets.Image.Context.SettingsCompassNE();
            this.m_tabView.add( this.m_pages[this.m_neIndex] );
            
            this.m_pages[this.m_xyIndex] = new skel.widgets.Image.Context.SettingsCompassXY();
            this.m_tabView.add( this.m_pages[this.m_xyIndex] );
        },
        
        _sendTabIndex : function(){
            
        },
      
      
        /**
         * Update the UI based on server-side  settings.
         * @param controls {Object} - server side  settings.
         */
        setControls : function( controls ){
            for ( var i = 0; i < this.m_pages.length; i++ ){
                this.m_pages[i].setControls( controls );
            }
        },
        
        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains data for this control UI.
         */
        setId : function( id ){
            this.m_id = id;
            for ( var i = 0; i < this.m_pages.length; i++ ){
                this.m_pages[i].setId( id );
            }
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_neIndex  : 0,
        m_xyIndex  : 1,
        
        m_tabView : null,
        m_pages : null
    }
});