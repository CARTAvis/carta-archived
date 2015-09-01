/**
 * Displays controls for customizing the grid labels.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.Settings.SettingsLabelPage", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Settings", "");
        this.m_connector = mImport("connector");
        this._init();
        
        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVar = this.m_connector.getSharedVar(pathDict.FONTS);
        this.m_sharedVar.addCB(this._fontsChangedCB.bind(this));
        this._fontsChangedCB();
    },

    members : {
        /**
         * Callback from the server when the list of available fonts has changed.
         */
        _fontsChangedCB : function(){
            if ( this.m_sharedVar ){
                var val = this.m_sharedVar.get();
                if ( val ){
                    try {
                        var fonts = JSON.parse( val );
                        this.m_familyCombo.setComboItems( fonts.family );
                        this.m_sizeCombo.setComboItems( fonts.size );
                    }
                    catch( err ){
                        console.log( "Axes coordinate system could not parse: "+val );
                        console.log( "Error: "+err );
                    }
                }
            }
        },
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this._setLayout(new qx.ui.layout.VBox(2));
            this.m_content = new qx.ui.container.Composite();
            this._add( new qx.ui.core.Spacer(), {flex:1});
            this._add( this.m_content);
            this._add( new qx.ui.core.Spacer(), {flex:1});
            this.m_content.setLayout(new qx.ui.layout.VBox());
            this.m_content.add( new qx.ui.core.Spacer(5), {flex:1});
            this._initControls();
            this.m_gridLabels = new skel.widgets.Image.Grid.Settings.GridLabels();
            this.m_content.add( this.m_gridLabels );
            this.m_content.add( new qx.ui.core.Spacer(5), {flex:1});
        },
        
        /**
         * Initialize the font controls.
         */
        _initControls : function(){
            var familyContainer = new qx.ui.container.Composite();
            familyContainer.setLayout( new qx.ui.layout.HBox(2) );
            familyContainer.add( new qx.ui.core.Spacer(), {flex:1});
            
            this.m_familyCombo = new skel.boundWidgets.ComboBox( "setFontFamily", "family");
            this.m_familyCombo.setToolTipText( "Select a label font.");
            var familyLabel = new qx.ui.basic.Label( "Family:");
            familyContainer.add( familyLabel );
            familyContainer.add( this.m_familyCombo );
            
            familyContainer.add( new qx.ui.core.Spacer(), {flex:1});
            this.m_sizeCombo = new skel.boundWidgets.ComboBox( "setFontSize", "size");
            this.m_sizeCombo.setToolTipText( "Select a font size.");
            var sizeLabel = new qx.ui.basic.Label( "Size:");
            familyContainer.add( sizeLabel );
            familyContainer.add( this.m_sizeCombo );
            
            familyContainer.add( new qx.ui.core.Spacer(), {flex:1});
            this.m_content.add( familyContainer );
        },
        
        /**
         * Update from the server when the grid control settings have changed.
         * @param controls {Object} - information about the grid control settings from the server.
         */
        setControls : function( controls ){
            this.m_sizeCombo.setComboValue( controls.grid.font.size );
            this.m_familyCombo.setComboValue( controls.grid.font.family );
            this.m_gridLabels.setControls( controls.grid.labelFormats );
        },
        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains data for this control UI.
         */
        setId : function( gridId ){
            this.m_id = gridId;
            this.m_sizeCombo.setId( gridId );
            this.m_familyCombo.setId ( gridId );
            this.m_gridLabels.setId( gridId );
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_sizeCombo : null,
        m_familyCombo : null,
        m_gridLabels : null
    }
});