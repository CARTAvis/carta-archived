/**
 * Displays controls for customizing the context image view.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Context.ContextControls", {
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
            this._setLayout( new qx.ui.layout.VBox(1));
            this.m_tabView = new qx.ui.tabview.TabView();
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this.m_tabListenId = this.m_tabView.addListener( "changeSelection", this._sendTabIndex, this );
            this._add( this.m_tabView );
          
            this.m_pages = [];
            this.m_pages[0] = new skel.widgets.Image.Context.SettingsViewBox();
            this.m_tabView.add( this.m_pages[0] );
            
            this.m_pages[1] = new skel.widgets.Image.Context.SettingsCompass();
            this.m_tabView.add( this.m_pages[1] );
        },
        
        /**
         * Set values of UI elements based on server-side values.
         * @param prefs {Object} - information from the server about context image settings.
         */
        setControls : function( prefs ){
            var tabIndex = prefs.tabIndex;
            this._selectTab( tabIndex );
            for ( var i = 0; i < this.m_pages.length; i++ ){
                this.m_pages[i].setControls( prefs );
            }
        },
        

        /**
         * Send a command to the server to get the context id.
         * @param contextId {String} the server side id of the context image object.
         */
        setId : function( contextId ){
           this.m_id = contextId;
           for ( var i = 0; i < this.m_pages.length; i++ ){
               this.m_pages[i].setId( contextId );
           }
        }
    }
});