/**
 * Controls for display settings for a particular region.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/


qx.Class.define("skel.widgets.Image.Region.RegionSettings", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        
        this._init( );
    },

    members : {
    	  
        /*
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout( new qx.ui.layout.VBox() );
            this.m_content = new qx.ui.container.Composite();
            
            this.m_content.setLayout( new qx.ui.layout.VBox(1) );
            this._add( this.m_content );
            
            this.m_autoSelect = new qx.ui.form.CheckBox( "Auto Select");
            skel.widgets.TestID.addTestId( this.m_autoSelect, "RegionAutoSelect");
            this.m_autoSelect.setToolTipText( "Auto selection based on animator or manual selection of regions.");
            this.m_autoListenId = this.m_autoSelect.addListener( "changeValue", this._sendAutoSelectCmd, this );
            var checkContainer = new qx.ui.container.Composite();
            checkContainer.setLayout( new qx.ui.layout.HBox(1) );
            checkContainer.add( new qx.ui.core.Spacer(1), {flex:1} );
            checkContainer.add( this.m_autoSelect );
            checkContainer.add( new qx.ui.core.Spacer(1), {flex:1} );
            this.m_content.add( checkContainer );
        },
        
        /**
         * Returns whether or not regions are currently being autoselected.
         * @return {boolean} - true if regions are being autoselected; false, otherwise.
         */
        isAutoSelect : function(){
        	return this.m_autoSelect.getValue();
        },
             
        /**
         * Create a UI widget for displaying the settings of a region of a particular type.
         * @param regionType {String} - the type of region whose settings should be displayed.
         */
        _makeRegion : function( regionType ){
        	var region = null;
        	if ( regionType == "rectangle"){
        		region = new skel.widgets.Image.Region.RegionSettingsRectangle();
        	}
        	else if ( regionType == "ellipse"){
        		region = new skel.widgets.Image.Region.RegionSettingsEllipse();
        	}
        	else if ( regionType == "polygon"){
        		region = new skel.widgets.Image.Region.RegionSettingsPolygon();
        		region.setSignificantDigits( this.m_significantDigits );
        	}
        	else if ( regionType == "Point"){
        		region = new skel.widgets.Image.Region.RegionSettingsPoint();
        	}
        	if ( region !== null ){
        		region.setId( this.m_id );
        	}
        	return region;
        },
        
        /**
         * Send a command to the server to change the auto selection.
         */
        _sendAutoSelectCmd : function(){
    		if ( this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var value = this.m_autoSelect.getValue();
                var cmd = this.m_id + path.SEP_COMMAND + "setAutoSelect";
                var params = "autoSelect:"+value;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Update the UI as to whether or not to auto select regions.
         * @param auto {boolean} - true if regions should be automatically selected; false otherwise.
         */
        setAutoSelect : function( auto ){
        	this.m_autoSelect.removeListenerById( this.m_autoListenId );
        	this.m_autoSelect.setValue( auto );
        	this.m_autoListenId = this.m_autoSelect.addListener( "changeValue", this._sendAutoSelectCmd, this );
        },
     
        /**
         * Set the server-side id of the object that handles region control settings.
         * @param id {String} - server-side id for region control settings.
         */
        setId : function( id ){
            this.m_id = id;
            if ( this.m_region ){
            	this.m_region.setId( this.m_id );
            }
        },
        
        
        
        /**
         * Set the region whose settings should be displayed.
         * @param region {Object} - the region whose settings should be displayed.
         */
        setRegion : function( region ){
        	var regionType = "";
        	if ( region !== null ){
        		regionType = region.regionType;
        	}
        	
        	if ( this.m_region === null || this.m_region.getType() != regionType ){
        		var contentIndex = this.m_content.indexOf( this.m_region);
        		if ( contentIndex >= 0 ){ 
        			this.m_content.remove( this.m_region );
        		}
        	
        		this.m_region = this._makeRegion( regionType );
        		if ( this.m_region ){
        			this.m_content.add( this.m_region );
        		}
        	}
        	if ( region !== null ){
        		this.m_region.setRegion( region );
        	}
        },
        
        /**
         * Set the number of significant digits to round to when doing a computation.
         * @param digits {Number} - the number of significant digits for rounding.
         */
        setSignificantDigits : function( digits ){
        	this.m_significantDigits = digits;
        	if ( this.m_region !== null ){
        		this.m_region.setSignificantDigits( digits );
        	}
        },
        
        m_autoSelect : null,
        m_autoListenId : null,
        m_connector : null,
        m_content : null,
        m_id : null,
        m_region : null,
        m_significantDigits : null
    }
});