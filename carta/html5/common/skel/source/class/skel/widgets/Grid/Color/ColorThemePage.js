/**
 * Displays overall color themes for the grid.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Grid.Color.ColorThemePage", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Color", "");
        this.m_connector = mImport("connector");
        this._init();
        
        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVar = this.m_connector.getSharedVar(pathDict.THEMES);
        this.m_sharedVar.addCB(this._themesChangedCB.bind(this));
        this._themesChangedCB();
    },

    members : {
        
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout(new qx.ui.layout.VBox(2));
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content, {flex:1} );
            this.m_content.setLayout(new qx.ui.layout.VBox());
            this.m_content.add( new qx.ui.core.Spacer(5), {flex:1});
            this._initControls();
            this.m_content.add( new qx.ui.core.Spacer(5), {flex:1});
        },
        
        /**
         * Initialize the overall color theme controls.
         */
        _initControls : function(){
            var colorContainer = new qx.ui.container.Composite();
            colorContainer.setLayout( new qx.ui.layout.HBox());
            this.m_themeCombo = new skel.boundWidgets.ComboBox( "setTheme", "theme");
            this.m_themeCombo.setToolTipText( "Select a color theme for the canvas.");
            this.m_themeCombo.setEnabled( false );
            var themeLabel = new qx.ui.basic.Label( "Theme:");
            colorContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            colorContainer.add( themeLabel );
            colorContainer.add( this.m_themeCombo );
            colorContainer.add( new qx.ui.core.Spacer(5), {flex:1});
            this.m_content.add( colorContainer );
        },
        
        /**
         * Update from the server when the color theme has changed.
         * @param theme {Object} - information about the color theme from the server.
         */
        setControls : function( theme ){
            console.log( "Setting theme="+theme);
            this.m_themeCombo.setComboValue( theme );
        },
        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains data for this control UI.
         */
        setId : function( gridId ){
            this.m_id = gridId;
            this.m_themeCombo.setId( gridId );
        },
        
        /**
         * Callback from the server when the list of available fonts has changed.
         */
        _themesChangedCB : function(){
            if ( this.m_sharedVar ){
                var val = this.m_sharedVar.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        this.m_themeCombo.setComboItems( obj.themes );
                    }
                    catch( err ){
                        console.log( "Canvas themes could not parse: "+val );
                        console.log( "Error: "+err );
                    }
                }
            }
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_themeCombo : null
    }
});