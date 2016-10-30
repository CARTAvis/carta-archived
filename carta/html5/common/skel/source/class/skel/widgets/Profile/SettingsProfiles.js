/**
 * Allows the user to manage profile curves.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Profile.SettingsProfiles", {
    extend : qx.ui.tabview.Page,


    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments, "Profiles", "");
        this._init();
        
        //Initiate connector.
        if ( typeof mImport !== "undefined"){
             this.m_connector = mImport("connector");
                 
             var path = skel.widgets.Path.getInstance();

             //Profile statistics
             this.m_sharedVarStats = this.m_connector.getSharedVar(path.PROFILE_STATISTICS);
             this.m_sharedVarStats.addCB(this._statsChangedCB.bind(this));
             this._statsChangedCB();
             
             //Generate modes
             this.m_sharedVarModes = this.m_connector.getSharedVar(path.PROFILE_GEN_MODES);
             this.m_sharedVarModes.addCB(this._genModesChangedCB.bind(this));
             this._genModesChangedCB();
         }
    },

    members : {
        
        /**
         * Callback for a change in the list of available images and regions on the
         * server.
         */
        _controlDataChangedCB : function(){
            if ( this.m_sharedVarControl ){
                var val = this.m_sharedVarControl.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        this._updateImageNames( obj.layers );
                    }
                    catch( err ){
                        console.log( "Could not parse profile control information: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
        
        
        /**
         * Update from the server.
         * @param curveUpdate {Object} - information from the server about the profile
         *      curves.
         */
        dataUpdate : function( curveUpdate ){
            if ( typeof curveUpdate.curves != "undefined"){
                this.m_curveInfo = curveUpdate.curves;
                this._updateCurveNames();
                var selectName = curveUpdate.selectCurve;
                this._updateSelection( selectName );
                this._updateSelectionImage( curveUpdate.imageSelect );
                this._updateSelectionRegion( curveUpdate.regionSelect );
            }
        },
        
        
       
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.HBox(1);
            this._setLayout(widgetLayout);
            
            var overallContainer = new qx.ui.container.Composite();
            overallContainer.setLayout( new qx.ui.layout.VBox(1));
            
            //Custom Name
            var nameContainer = new qx.ui.container.Composite();
            nameContainer.setLayout( new qx.ui.layout.HBox(1));
            this.m_nameSelect = new qx.ui.form.ComboBox();
            this.m_nameSelect.addListener( "changeValue", this._nameChangedCB, this );
            skel.widgets.TestID.addTestId( this.m_nameSelect, "profileNameSelect" ); 
            this.m_nameSelect.setToolTipText( "Specify a custom name for the profile.");
            var nameLabel = new qx.ui.basic.Label( "Name:");
            nameContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            nameContainer.add( nameLabel );
            nameContainer.add( this.m_nameSelect, {flex:5} );
            nameContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            overallContainer.add( nameContainer );
            
            //Initialize image selection
            var imageLabel = new qx.ui.basic.Label( "Image:");
            this.m_imageSelect = new skel.widgets.CustomUI.SelectBox();
            this.m_imageSelectListenId = this.m_imageSelect.addListener( "selectChanged", this._sendImageSelectCmd, this );
            this.m_imageSelect.setToolTipText( "Specify the image used to generate the profile.");
            skel.widgets.TestID.addTestId( this.m_imageSelect, "ProfileSelectedImage" );
            var selectContainer = new qx.ui.container.Composite();
            var gridLayout = new qx.ui.layout.Grid();
            gridLayout.setSpacing( 2 );
            gridLayout.setColumnFlex( 1, 1 );
            gridLayout.setColumnMinWidth( 1, 200 );
            gridLayout.setColumnFlex( 3, 1 );
            gridLayout.setColumnMinWidth( 3, 200 );
            gridLayout.setRowAlign( 0, "right", "middle");
            selectContainer.setLayout( gridLayout );
            selectContainer.add( imageLabel, {row:0, column:0} );
            selectContainer.add( this.m_imageSelect, {row:0, column:1} );
            
            //Initialize region selection
            var regionLabel = new qx.ui.basic.Label( "Region:");
            this.m_regionSelect = new skel.widgets.CustomUI.SelectBox();
            skel.widgets.TestID.addTestId( this.m_regionSelect, "ProfileSelectedRegion" );
            var names = [];
            names[0] = this.m_NONE;
            this.m_regionSelect.setSelectItems( names );
            this.m_regionSelectListenId = this.m_regionSelect.addListener( "selectChanged", this._sendRegionSelectCmd, this );
            this.m_regionSelect.setToolTipText( "Specify the region used to generate the profile.");
            selectContainer.add( regionLabel, {row:0, column:2} );
            selectContainer.add( this.m_regionSelect, {row:0, column:3} );
            
            //Initialize rest frequency
            this.m_restWidget = new skel.widgets.Profile.SettingsProfilesRest("","");
            selectContainer.add( this.m_restWidget, {row:1, column:0, colSpan:2, rowSpan:2} );
            
            //Initialize the statistic
            var statLabel = new qx.ui.basic.Label( "Statistic:");
            this.m_statSelect = new skel.widgets.CustomUI.SelectBox();
            skel.widgets.TestID.addTestId( this.m_statSelect, "profileAggregateStatistics")
            this.m_statSelect.addListener( "selectChanged", this._sendStatCmd, this );
            this.m_statSelect.setToolTipText( "Specify the method used to generate the profile.");
            selectContainer.add( statLabel, {row:1, column:2});
            selectContainer.add( this.m_statSelect, {row:1, column:3});
            
            this.m_autoCheck = new qx.ui.form.CheckBox();
            skel.widgets.TestID.addTestId( this.m_autoCheck, "profileAutoGenerate");
            this.m_autoCheck.setToolTipText( "Automatically generate profiles.");
            this.m_autoListenId = this.m_autoCheck.addListener( "changeValue", this._sendAutoGenerateCmd, this );
            
            this.m_genSelect = new skel.widgets.CustomUI.SelectBox();
            skel.widgets.TestID.addTestId( this.m_genSelect, "profileGenerateMode" ); 
            this.m_genSelect.setToolTipText( "Specify which images should be profiled by default when they are loaded.")
            this.m_genSelect.addListener( "selectChanged", this._genModeChangedCB, this );
            var genLabel = new qx.ui.basic.Label( "Auto Generate:");  
            
            var genContainer = new qx.ui.container.Composite();
            genContainer.setLayout( new qx.ui.layout.HBox(1) );
            genContainer.add( this.m_autoCheck );
            genContainer.add( genLabel );
            genContainer.add( this.m_genSelect );
            selectContainer.add( genContainer, {row:2, column:2, colSpan:2} );
            
            overallContainer.add( selectContainer );
            
            var butContainer = this._initButtons();
            overallContainer.add( butContainer );
            
            this._add( overallContainer );
        },
        
        /**
         * Initialize the buttons responsible for managing profiles.
         */
        _initButtons : function(){
            var butContainer = new qx.ui.container.Composite();
            butContainer.setLayout( new qx.ui.layout.HBox(1));
            butContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            this.m_addButton = new qx.ui.form.Button( "New");
            skel.widgets.TestID.addTestId( this.m_addButton, "profileNewButton");
            this.m_addButton.setToolTipText( "Create a new profile using default settings.");
            this.m_addButton.addListener( "execute", this._sendNewCmd, this );
            this.m_removeButton = new qx.ui.form.Button( "Remove");
            skel.widgets.TestID.addTestId( this.m_removeButton, "profileRemoveButton" ); 
            this.m_removeButton.setToolTipText( "Delete the selected profile.");
            this.m_removeButton.addListener( "execute",this._sendRemoveCmd, this );
            butContainer.add( new qx.ui.core.Spacer(1), {flex:0.25});
            butContainer.add( this.m_addButton );
            butContainer.add( this.m_removeButton );
            butContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            return butContainer;
        },
        
        /**
         * Server update when available methods of generating profiles changes.
         */
        _genModesChangedCB : function(){
            if ( this.m_sharedVarModes ){
                var val = this.m_sharedVarModes.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        var modes = obj.genModes;
                        this.m_genSelect.setSelectItems( modes );
                    }
                    catch( err ){
                        console.log( "Could not parse profile generation modes: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
        /**
         * Callback for when the profile generation mode changes in the UI.
         */
        _genModeChangedCB : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var mode = this.m_genSelect.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setGenerationMode";
                var params = "mode:"+mode;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Return the id of the controller object.
         */
        _getControlId : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "registerController";
         
            var params = "";
            this.m_connector.sendCommand( cmd, params, this._setControlId(this) );
        },
        
        /**
         * Return the id of the region controller.
         */
        _getRegionId : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "registerRegionControls";
            var params = "";
            this.m_connector.sendCommand( cmd, params, this._setRegionId(this) );
        },
        
        /**
         * Callback for when a profile name changes through the UI.
         */
        _nameChangedCB : function(){
            //Decide if the current name matches one of the servers.
            var existingIndex = -1;
            var curveCount = this.m_curveInfo.length;
            var targetName = this.m_nameSelect.getValue();
            for ( var i = 0; i < curveCount; i++ ){
                if ( targetName == this.m_curveInfo[i].name ){
                    existingIndex = i;
                    break;
                }
            }
            //If it is an existing one, the user clicked the combo box
            //and we need to update the other UI fields to match it.
            if ( existingIndex >= 0 ){
                this._updateSelection( existingIndex);
            }
            else {
                //If it is a new name, the user renamed the existing curve
                //and we need to notify the server.
                this._sendRenameCmd();
            }
        },
        
        /**
         * Update the UI based on new preference settings on the server.
         * @param prefUpdate {Object} - server preference settings.
         */
        prefUpdate : function( prefUpdate ){
        	this.m_restWidget.update(prefUpdate.restFrequency, 
        			prefUpdate.restFrequencyUnits, prefUpdate.restUnits);
            this.m_genSelect.setSelectValue( prefUpdate.genMode );
            this.m_statSelect.setSelectValue( prefUpdate.stat );
            this._updateAutoGenerate( prefUpdate.autoGenerate );
        },
        
        /**
         * Update the list of regions available for profiles.
         */
        _regionDataChangedCB : function(){
            if ( this.m_sharedVarControl ){
                var val = this.m_sharedVarRegion.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        this._updateRegionNames( obj.regions, obj.regionIndex );
                    }
                    catch( err ){
                        console.log( "Could not parse profile region information: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
        /**
         * Notify the server as to whether profiles should be automatically generated.
         */
        _sendAutoGenerateCmd : function(){
        	if ( this.m_id !== null && this.m_connector !== null ){
        		var auto = this.m_autoCheck.getValue();
        		var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setAutoGenerate";
                var params = "autoGenerate:"+auto;
                this.m_connector.sendCommand( cmd, params, null );
        	}
        },
        
        /**
         * Send a command to the server indicating an image was selected.
         */
        _sendImageSelectCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var name = this.m_imageSelect.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setSelectedLayer";
                var params = "name:"+name;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Send a command to the server indicating a region was selected.
         */
        _sendRegionSelectCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var name = this.m_regionSelect.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setSelectedRegion";
                var params = "name:"+name;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Notify the server the user has renamed a profile.
         */
        _sendRenameCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var newName = this.m_nameSelect.getValue();
                var oldName = this.m_curveInfo[this.m_selectIndex].name;
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setCurveName";
                var params = "name:"+newName+",oldName:"+oldName;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Send a command to the server to generate a new profile.
         */
        _sendNewCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "newProfile";
                var params = "";
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        
        /**
         * Send a command to the server to remove the current profile.
         */
        _sendRemoveCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var newName = this.m_nameSelect.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "removeProfile";
                var params = "name:"+newName;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
        /**
         * Send a command to the server to change the method used to summarize
         * profile points.
         */
        _sendStatCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var newName = this.m_nameSelect.getValue();
                var newStat = this.m_statSelect.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setStatistic";
                var params = "name:"+newName+",stat:"+newStat;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },
        
      
        
       
        /**
         * Callback containing information about the server-side object containing control
         * information.
         * @param object {Object} - server-side object containing control information.
         */
        _setControlId : function( object ){
            return function( id ){
                object.m_sharedVarControl = object.m_connector.getSharedVar(id);
                object.m_sharedVarControl.addCB(object._controlDataChangedCB.bind(object));
                object._controlDataChangedCB();
               
            }
        },
              
        /**
         * Callback containing information about the server-side object containing region
         * information.
         * @param object {Object} - server-side object containing region information.
         */
        _setRegionId : function( object ){
            return function( id ){
            	if ( object.m_sharedVarRegion == null ){
	            	var path = skel.widgets.Path.getInstance();
	                object.m_sharedVarRegion = object.m_connector.getSharedVar(id + path.SEP + path.DATA);
	                object.m_sharedVarRegion.addCB(object._regionDataChangedCB.bind(object));
	                object._regionDataChangedCB();
            	}
            }
        },
        
        /**
         * Set the server side id of the object managing profile information.
         * @param id {String} the server side id of the managing profiles.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_restWidget.setId( id );
            this._getControlId();
            this._getRegionId();
        },
        
        /**
         * Server update when available methods of generating profiles changes.
         */
        _statsChangedCB : function(){
            if ( this.m_sharedVarStats ){
                var val = this.m_sharedVarStats.get();
                if ( val ){
                    try {
                        var obj = JSON.parse( val );
                        var stats = obj.profileStats;
                        this.m_statSelect.setSelectItems( stats );
                    }
                    catch( err ){
                        console.log( "Could not parse profile information: "+val );
                        console.log( "Err: "+err);
                    }
                }
            }
        },
        
        /**
         * Update whether or not profiles should be automatically generated based on
         * server-side values.
         * @param val {boolean} - true if profiles should be automatically generated;
         * 		false, otherwise.
         */
        _updateAutoGenerate : function( val ){
        	if ( this.m_autoListenId !== null ){
                this.m_autoCheck.removeListenerById( this.m_autoListenId );
            }
            this.m_autoCheck.setValue( val );
            this.m_addButton.setEnabled( !val );
            this.m_removeButton.setEnabled( !val );
            this.m_imageSelect.setEnabled( !val );
            this.m_genSelect.setEnabled( val );
            this.m_regionSelect.setEnabled( !val );
            this.m_autoListenId = this.m_autoCheck.addListener( "changeValue", 
                    this._sendAutoGenerateCmd, this );
        },
        
        /**
         * Update the names of profile curves based on server information.
         */
        _updateCurveNames : function(){
            try {
                var oldName = this.m_nameSelect.getValue();
                var nameCount = this.m_curveInfo.length;
                this.m_nameSelect.removeAll();
                for ( var i = 0; i < nameCount; i++ ){
                    var profileName = this.m_curveInfo[i].name;
                    var tempItem = new qx.ui.form.ListItem( profileName );
                    this.m_nameSelect.add( tempItem );
                }
                //Try to reset the old selection
                if ( oldName !== null ){
                    this.m_nameSelect.setValue( oldName );
                }
                //Select the first item
                else if ( nameCount > 0 ){
                    var selectables = this.m_nameSelect.getChildrenContainer().getSelectables(true);
                    if ( selectables.length > 0 ){
                        this.m_nameSelect.setValue( selectables[0].getLabel());
                    }
                }
                this.m_restWidget.setCurveName( this.m_nameSelect.getValue());
            }
            catch( err ){
                console.log( "Could not parse profile names." );
                console.log( "Err: "+err);
            }
        },
        
        /**
         * Update the names of the images used to generate profile curves based
         * on server information.
         * @param data {Array} - a list of available images.
         */
        _updateImageNames : function( data){
            var names = [];
            var selectedImage = this.m_imageSelect.getValue();
            var dataIndex = -1;
            for ( var i = 0; i < data.length; i++ ){
                names[i] = data[i].name;
                if ( data[i].selected ){
                    dataIndex = i;
                }
            }
            
            this.m_imageSelect.setSelectItems( names );
            //If we can retain the user's previous selection, do so.  Otherwise,
            //try to use the data selection.
            var newSelectedImage = this.m_imageSelect.getValue();
            if ( newSelectedImage != selectedImage && dataIndex >= 0 ){
                if ( this.m_imageSelectListenId !== null ){
                    this.m_imageSelect.removeListenerById( this.m_imageSelectListenId );
                }
                this.m_imageSelect.setSelectValue( data[dataIndex].name );
                this.m_imageSelectListenId = this.m_imageSelect.addListener( "selectChanged", 
                        this._sendImageSelectCmd, this );
            }
        },
        
        /**
         * Update the names of the available regions based on server-side values.
         * @param data {Array} - a list of regions that have been loaded.
         * @param dataIndex {Number} - the index of the current region.
         */
        _updateRegionNames : function( data, dataIndex){
            var names = [];
            var selectedRegion = this.m_regionSelect.getValue();
            for ( var i = 0; i < data.length; i++ ){
                names[i] = data[i].name;
            }
            //Add in None so that the user can profile the whole plane.
            names[data.length] = this.m_NONE;
            if ( names.length == 1 ){
            	dataIndex = 0;
            }
            this.m_regionSelect.setSelectItems( names );
            if ( dataIndex >= 0 ){
                if ( this.m_regionSelectListenId !== null ){
                    this.m_regionSelect.removeListenerById( this.m_regionSelectListenId );
                }
                this.m_regionSelect.setSelectValue( names[dataIndex] );
                this.m_regionSelectListenId = this.m_regionSelect.addListener( "selectChanged", 
                        this._sendRegionSelectCmd, this );
            }
        },
        
        /**
         * Update the controls based on the selected profile curve.
         * @param selectIndex {Number} - the index of the selected curve.
         */
        _updateSelection : function( selectName ){
            var selectIndex = -1;
            for ( var i = 0; i < this.m_curveInfo.length; i++ ){
                if ( selectName == this.m_curveInfo[i].name ){
                   selectIndex = i;
                   break;
                }
            }
            if ( selectIndex >= 0 ){
               if ( this.m_nameSelect.getValue() != selectName ){
                  this.m_nameSelect.setValue( selectName );
               }
               var statName = this.m_curveInfo[selectIndex].stat;
               if ( statName != this.m_statSelect.getValue() ){
                  this.m_statSelect.setSelectValue( statName );
               }
           }
        },
        
        /**
         * Update the image that was selected.
         * @param selectName {String} - an identifier for the selected image.
         */
        _updateSelectionImage : function( selectName ){
            if ( this.m_imageSelectListenId !== null ){
                this.m_imageSelect.removeListenerById( this.m_imageSelectListenId );
            }
            this.m_imageSelect.setSelectValue( selectName );
            this.m_imageSelectListenId = this.m_imageSelect.addListener( "selectChanged", 
                    this._sendImageSelectCmd, this );
        },
        
        /**
         * Update the region that was selected.
         * @param selectName {String} - an identifier for the selected region.
         */
        _updateSelectionRegion : function( selectName ){
            if ( this.m_regionSelectListenId !== null ){
                this.m_regionSelect.removeListenerById( this.m_regionSelectListenId );
            }
            this.m_regionSelect.setSelectValue( selectName );
            this.m_regionSelectListenId = this.m_regionSelect.addListener( "selectChanged", 
                    this._sendRegionSelectCmd, this );
        },
        
        m_id : null,
        m_selectIndex : null,
        m_addButton : null,
        m_autoCheck : null,
        m_autoListenId : null,
        m_removeButton : null,
        m_imageSelect : null,
        m_imageSelectListenId : null,
        m_NONE : "None",
        m_regionSelectListenId : null,
        m_genSelect : null,
        m_nameSelect : null,
        m_regionSelect : null,
        m_restWidget : null,
        m_statSelect : null,
        m_connector : null,
        m_sharedVarStats : null,
        m_sharedVarModes : null,
        m_sharedVarControl : null,
        m_sharedVarRegion : null,
        m_curveInfo : null
    }
});
