/**
 * Displays controls for customizing the grid font.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Grid.GridFont", {
    extend : qx.ui.core.Widget, 

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
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
            this._setLayout(new qx.ui.layout.VBox(2));
            this.m_content = new qx.ui.groupbox.GroupBox( "Font");
            this.m_content.setContentPadding( 0, 0, 0, 0 );
            this._add( this.m_content );
            this.m_content.setLayout(new qx.ui.layout.VBox());
            this._initControls();
        },
        
        /**
         * Initialize the font controls.
         */
        _initControls : function(){
            var familyContainer = new qx.ui.container.Composite();
            familyContainer.setLayout( new qx.ui.layout.HBox());
            this.m_familyCombo = new skel.boundWidgets.ComboBox( "setFontFamily", "family");
            var familyLabel = new qx.ui.basic.Label( "Family:");
            familyContainer.add( familyLabel );
            familyContainer.add( this.m_familyCombo );
            this.m_content.add( familyContainer );
            
            var sizeContainer = new qx.ui.container.Composite();
            sizeContainer.setLayout( new qx.ui.layout.HBox());
            this.m_sizeCombo = new skel.boundWidgets.ComboBox( "setFontSize", "size");
            var sizeLabel = new qx.ui.basic.Label( "Size:");
            sizeContainer.add( sizeLabel );
            sizeContainer.add( this.m_sizeCombo );
            this.m_content.add( sizeContainer );
        },
        
        /**
         * Update from the server when the font has changed.
         * @param font {Object} - information about the font from the server.
         */
        setControls : function( font ){
            this.m_sizeCombo.setComboValue( font.size );
            this.m_familyCombo.setComboValue( font.family );
        },
        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains data for this control UI.
         */
        setId : function( gridId ){
            this.m_id = gridId;
            this.m_sizeCombo.setId( gridId );
            this.m_familyCombo.setId ( gridId );
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_sizeCombo : null,
        m_familyCombo : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }


});