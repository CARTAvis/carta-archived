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
        if ( typeof mImport !== "undefined"){
        	this.m_connector = mImport("connector");
        }
        this._init();
        
        if ( this.m_connector != null ){
        	var pathDict = skel.widgets.Path.getInstance();
        	this.m_sharedVar = this.m_connector.getSharedVar(pathDict.FONTS);
        	this.m_sharedVar.addCB(this._fontsChangedCB.bind(this));
        	this._fontsChangedCB();
        }
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
                        this.m_fontSizeSpin.setMinimum( fonts.fontSizeMin );
                        this.m_fontSizeSpin.setMaximum( fonts.fontSizeMax );
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
            this.m_fontSizeSpin = new qx.ui.form.Spinner();
            this.m_fontSizeSpin.addListener( skel.widgets.Path.CHANGE_VALUE,
                    this._sendFontSize, this );
            this.m_fontSizeSpin.setToolTipText( "Select a font size.");
            var sizeLabel = new qx.ui.basic.Label( "Size:");
            familyContainer.add( sizeLabel );
            familyContainer.add( this.m_fontSizeSpin );
            
            familyContainer.add( new qx.ui.core.Spacer(), {flex:1});
            this.m_decimalSpin = new qx.ui.form.Spinner();
            this.m_decimalSpin.setToolTipText( "Set the precision of the axis labels");
            this.m_decimalSpin.addListener( skel.widgets.Path.CHANGE_VALUE, 
                    this._sendDecimals, this );
            var decimalLabel = new qx.ui.basic.Label( "Precision:");
            familyContainer.add( decimalLabel );
            familyContainer.add( this.m_decimalSpin );
            
            familyContainer.add( new qx.ui.core.Spacer(), {flex:1});
            this.m_content.add( familyContainer );
        },
        
        
        
        /**
         * Update from the server when the grid control settings have changed.
         * @param controls {Object} - information about the grid control settings from the server.
         */
        setControls : function( controls ){
            if ( typeof controls.grid.font !== "undefined"){
            	this.setFontSize( controls.grid.font.size );
                this.m_familyCombo.setComboValue( controls.grid.font.family );
            }
            if ( typeof controls.grid.labelFormats !== "undefined"){
                this.m_gridLabels.setControls( controls.grid.labelFormats );
            }
            if ( typeof controls.grid.decimalsMax !== "undefined"){
                this.m_decimalSpin.setMaximum( controls.grid.decimalsMax );
            }
            if ( typeof controls.grid.decimals !== "undefined"){
            	this.setPrecision( controls.grid.decimals );
            }
        },
        
        /**
         * Set the font size.
         * @param fontSize {Number} - the size of the font.
         */
        setFontSize : function( fontSize ){
        	if ( this.m_fontSizeSpin.getValue() != fontSize ){
        		this.m_fontSizeSpin.setValue( fontSize );
        	}
        },
        
        /**
         * Set the precision of the labels.
         * @param precision {Number} - the precision to use when
         * 		displaying labels.
         */
        setPrecision : function( precision ){
        	if ( this.m_decimalSpin.getValue() != precision ){
        		this.m_decimalSpin.setValue( precision );
        	}
        },
        
        /**
         * Send the number of decimals to use for axis labels to the server.
         */
        _sendDecimals : function(){
        	if ( this.m_connector != null ){
        		var params = "decimals:"+this.m_decimalSpin.getValue();
        		var path = skel.widgets.Path.getInstance();
        		var cmd = this.m_id + path.SEP_COMMAND + "setLabelDecimals";
        		this.m_connector.sendCommand( cmd, params, function(){});
        	}
        },
        
        /**
         * Send the label font size to the server.
         */
        _sendFontSize : function(){
        	if ( this.m_connector != null ){
        		var params = "size:"+this.m_fontSizeSpin.getValue();
        		var path = skel.widgets.Path.getInstance();
        		var cmd = this.m_id + path.SEP_COMMAND + "setFontSize";
        		this.m_connector.sendCommand( cmd, params, function(){});
        	}
        },
        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains data for this control UI.
         */
        setId : function( gridId ){
            this.m_id = gridId;
            this.m_familyCombo.setId ( gridId );
            this.m_gridLabels.setId( gridId );
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        
        m_fontSizeSpin : null,
        m_familyCombo : null,
        m_gridLabels : null,
        m_decimalSpin : null
    }
});