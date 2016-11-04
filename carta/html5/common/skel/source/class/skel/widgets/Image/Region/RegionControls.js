/**
 * Displays controls for customizing regions.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Region.RegionControls", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments, "Regions", "");
        this.m_connector = mImport("connector");
        this._init();
    },

    members : {
    	
    	/**
    	 * Callback for a server-side change in region control preferences.
    	 */
    	 _controlsChangedCB : function(){
             var val = this.m_sharedVar.get();
             if ( val ){
                 try {
                     var controls = JSON.parse( val );       
                     this.m_regionSettings.setAutoSelect( controls.regionAutoSelect );
                     this.m_regionSettings.setSignificantDigits( controls.significantDigits );
                     this.m_regionList.setEnabled( !controls.regionAutoSelect );
                     var errorMan = skel.widgets.ErrorHandler.getInstance();
                     errorMan.clearErrors();
                 }
                 catch( err ){
                     console.log( "Region controls could not parse: "+val+" error: "+err );
                 }
             }
         },
         
    	
    	 /**
         * Callback for a change in regions.
         */
        _controlsChangedDataCB : function(){
            var val = this.m_sharedVarData.get();
            if ( val ){
                try {
                    var controls = JSON.parse( val );
                    this.m_regions = [];
                    var regionNames = [];
                    var selectedIndex = controls.regionIndex;
                    var selections = [];
                    for ( var i = 0; i < controls.regions.length; i++ ){
                    	this.m_regions[i] = controls.regions[i];
                    	regionNames[i] = controls.regions[i].name;
                    	if ( controls.regions[i].selected ){
                    		selections.push( i );
                    	}
                    }
                    
                    this.m_regionList.setItems( regionNames );
                    var selectedRegions = [];
                    //If it is auto select, use the animated region.
                    if ( this.m_regionSettings.isAutoSelect() && selections.length > 0 ){
                    	selectedRegions[0] = selectedIndex;
                    }
                    //If it is user selected, use the ones selected by the user.
                    else {
                    	selectedRegions = selections;
                    }
                    this.m_regionList.removeListenerById( this.m_regionListListenId );
                    this.m_regionList.setSelected( selectedRegions );
                    	
                    this.m_regionListListenId = this.m_regionList.addListener( "itemsSelected", 
                         		this._updateSelectedRegion, this );
                    	
                    if ( selectedIndex >= 0 && selectedIndex < this.m_regions.length ){
                    	this.m_regionSettings.setRegion( this.m_regions[selectedIndex] );
                    }
                    else {
                    	this.m_regionSettings.setRegion( null );
                    }
                    
                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.clearErrors();
                }
                catch( err ){
                    console.log( "Region controls data could not parse: "+val+" error: "+err );
                }
            }
        },
        
       
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this._setLayout( new qx.ui.layout.HBox(1));
            
            var TABLE_WIDTH = 200;
            this.m_regionList  = new skel.widgets.CustomUI.ItemTable( "Regions", TABLE_WIDTH);
            skel.widgets.TestID.addTestId( this.m_regionList, "RegionsTable");
            this.m_regionList.setTestId( "regionsTable");
            this.m_regionList.setToolTipText( "Select a region to customize." );
            this.m_regionList.setWidth( TABLE_WIDTH );
            this.m_regionListListenId = this.m_regionList.addListener( "itemsSelected", 
            		this._updateSelectedRegion, this );
            this._add( this.m_regionList );
            
            this.m_regionSettings = new skel.widgets.Image.Region.RegionSettings();
            this._add( this.m_regionSettings );
        },
         
        
        /**
         * Register to receive region updates from the server.
         */
        _register : function( imageId ){
        	 var path = skel.widgets.Path.getInstance();
             var cmd = imageId + path.SEP_COMMAND + "registerRegionControls";
             var params = "";
             this.m_connector.sendCommand( cmd, params, this._registrationCallback( this));
        },
        
        /**
         * Register to get updates on region information from the server.
         */
        _registerControls : function(){
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id);
            this.m_sharedVar.addCB(this._controlsChangedCB.bind(this));
            this._controlsChangedCB();
            
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVarData = this.m_connector.getSharedVar( this.m_id + path.SEP + path.DATA);
            this.m_sharedVarData.addCB(this._controlsChangedDataCB.bind(this));
            this._controlsChangedDataCB();
        },
        
        /**
         * Callback for when the registration is complete and an id is available.
         * @param anObject {skel.widgets.Image.Region.RegionControls}.
         */
        _registrationCallback : function( anObject ){
            return function( id ){
                anObject._setRegionControlId( id );
            };
        },
        
        /**
         * Notify the server of the user's region selections.
         */
        _sendRegionSelections : function(){
        	if ( this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setRegionsSelected";
                var indices = this.m_regionList.getSelectedIndices();
                //We match the indices to the ids.
                var ids = [];
                var selectionsChanged = false;
                for ( var i = 0; i < indices.length; i++ ){
                	ids[i] = this.m_regions[indices[i]].id;
                	if ( !this.m_regions[indices[i]].selected ){
                		selectionsChanged = true;
                	}
                }
                if ( selectionsChanged ){
                	var params = ids.join(";");
                	this.m_connector.sendCommand( cmd, params, function(){});
                }
            }
        },
       
        /**
         * Set the id of the controller.
         * @param imageId {String} - the id of the controller.
         */
        setId : function( imageId ){
            this._register( imageId );
        },
        
        /**
         * Set the server-side id of the region control object and initiate the
         * process for receiving server-side updates.
         * @param id {String} - the id of the server-side object responsible for region controls.
         */
        _setRegionControlId : function( id ){
        	if ( id !== null && id.length > 0 ){
                this.m_id = id;
                this.m_regionSettings.setId( this.m_id );
                this._registerControls();
            }
        },
        
        /**
         * Update the settings when a new region selection has been made and notify
         * the server.
         */
        _updateSelectedRegion : function(){
        	var selectedIndex = this.m_regionList.getSelectedIndex();
        	if ( 0 <= selectedIndex && selectedIndex < this.m_regions.length ){
        		this.m_regionSettings.setRegion( this.m_regions[selectedIndex] );
        	}
        	this._sendRegionSelections();
        },
        

        m_id : null,
        m_connector : null,
        m_regionList : null,
        m_regionListListenId : null,
        m_regionSettings : null,
        m_regions : null,
        m_sharedVar : null,
        m_sharedVarData : null
    }
});