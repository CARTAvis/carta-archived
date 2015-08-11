/**
 * Displays controls for customizing particular aspects of the grid such as
 * axes, ticks, labels, and grid lines.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.GridPage.GridPage", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     * @param title {String} - the aspect of the grid being customized.
     */
    construct : function( title ) {
        this.base(arguments, title, "");
        this._init( );
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this.setMargin( 1, 1, 1, 1 );
            this._setLayout(new qx.ui.layout.VBox(2));
           
            this.m_tabView = new qx.ui.tabview.TabView("left");
            this.m_tabView.addListener( "changeSelection", this._pageChanged, this );
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this._makeColorPage( );
            this._makeSettingsPage();
            this._add( this.m_tabView );
            this.m_tabView.add( this.m_settingsPage );
            this.m_tabView.add( this.m_colorPage );
        },
        
        /**
         * Construct a page for customizing color.
         */
        //Overriden by subclasses.
        _makeColorPage : function(  ){
           
        },
        
        /**
         * Construct a page for customizing the settings.
         */
        //Overriden by subclasses.
        _makeSettingsPage : function(){
           
        },
        
        /**
         * Update server-side settings when the user changes a page.
         */
        //Because a page is not constructed until it is shown, we can't
        //put the controls in until the user shows the page.
        _pageChanged : function(){
            var pages = this.m_tabView.getSelection();
            if ( pages.length > 0 && this.m_controls !== null ){
                if ( this.m_colorPage == pages[0]){
                    this._setColorControls();
                }
                else if ( this.m_settingsPage == pages[0]){
                    this._setSettingControls();
                }
            }
        },
        
        
        /**
         * Update from the server when the grid controls have changed.
         * @param controls {Object} - information about the grid controls from the server.
         */
        setControls : function( controls ){
            //Store the controls because the pages may not have been created yet.
            this.m_controls = controls;
            this._pageChanged();
        },
        
        /**
         * Update the color controls with server information.
         */
        //Overriden by subclasses.
        _setColorControls : function(){
            
        },
        
        /**
         * Update the setting controls with server information.
         */
        //Overriden by subclasses.
        _setSettingControls : function(){
            
        },
        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains 
         *      data for this control UI.
         */
        setId : function( gridId ){
            this.m_id = gridId;
            this.m_colorPage.setId( gridId );
            this.m_settingsPage.setId ( gridId );
        },
        
        m_id : null,
        m_controls : null,
        
        m_tabView : null,
        m_colorPage : null,
        m_settingsPage : null
    }
});