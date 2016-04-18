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
         * Update from the server.
         * @param curveUpdate {Object} - information from the server about the profile
         *      curves.
         */
        dataUpdate : function( curveUpdate ){
            if ( typeof curveUpdate.curves != "undefined"){
                this.m_curveInfo = curveUpdate.curves;
                this._updateCurveNames();
                this._updateImageNames( curveUpdate );
                var selectIndex = curveUpdate.selectCurve;
                this._updateSelection( selectIndex );
                this.m_genSelect.setSelectValue( curveUpdate.genMode );
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
            
            //Init generate.
            var genContainer = new qx.ui.container.Composite();
            genContainer.setLayout( new qx.ui.layout.HBox(1));
            this.m_genSelect = new skel.widgets.CustomUI.SelectBox();
            this.m_genSelect.addListener( "changeValue", this._genModeChangedCB, this );
            var genLabel = new qx.ui.basic.Label( "Auto Generate:");
            genContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            genContainer.add( genLabel );
            genContainer.add( this.m_genSelect );
            genContainer.add( new qx.ui.core.Spacer(1), {flex:1});
            overallContainer.add( genContainer );
            
            //Custom Name
            var nameContainer = new qx.ui.container.Composite();
            nameContainer.setLayout( new qx.ui.layout.HBox(1));
            this.m_nameSelect = new qx.ui.form.ComboBox();
            this.m_nameSelect.addListener( "changeValue", this._nameChangedCB, this );
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
            this.m_imageSelect.setToolTipText( "Specify the image used to generate the profile.");
            var selectContainer = new qx.ui.container.Composite();
            var gridLayout = new qx.ui.layout.Grid();
            gridLayout.setSpacing( 2 );
            gridLayout.setColumnFlex( 1, 1 );
            gridLayout.setColumnMinWidth( 1, 200 );
            gridLayout.setColumnFlex( 3, 1 );
            gridLayout.setRowAlign( 0, "right", "middle");
            selectContainer.setLayout( gridLayout );
            selectContainer.add( imageLabel, {row:0, column:0} );
            selectContainer.add( this.m_imageSelect, {row:0, column:1} );
            
            //Initialize region selection
            var regionLabel = new qx.ui.basic.Label( "Region:");
            this.m_regionSelect = new skel.widgets.CustomUI.SelectBox();
            this.m_regionSelect.setToolTipText( "Specify the region used to generate the profile.");
            selectContainer.add( regionLabel, {row:0, column:2} );
            selectContainer.add( this.m_regionSelect, {row:0, column:3} );
            
            //Initialize rest frequency
            gridLayout.setRowAlign( 1, "right", "middle");
            var restFreqLabel = new qx.ui.basic.Label( "Rest Frequency:");
            this.m_restFreqText = new skel.widgets.CustomUI.NumericTextField( 0, null );
            this.m_restFreqText.setToolTipText( "Specify the rest frequency.");
            selectContainer.add( restFreqLabel, {row:1, column:0});
            selectContainer.add( this.m_restFreqText, {row:1, column:1} );
            
            //Initialize the statistic
            var statLabel = new qx.ui.basic.Label( "Statistic:");
            this.m_statSelect = new skel.widgets.CustomUI.SelectBox();
            this.m_statSelect.setToolTipText( "Specify the method used to generate the profile.");
            selectContainer.add( statLabel, {row:1, column:2});
            selectContainer.add( this.m_statSelect, {row:1, column:3});
            
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
            this.m_addButton.setToolTipText( "Create a new profile using default settings.");
            this.m_addButton.addListener( "execute", this._sendNewCmd, this );
            this.m_copyButton = new qx.ui.form.Button( "Copy");
            this.m_copyButton.setToolTipText( "Create a new profile using the same settings as the selected profile.");
            this.m_copyButton.addListener( "execute", this._sendCopyCmd, this );
            this.m_removeButton = new qx.ui.form.Button( "Remove");
            this.m_removeButton.setToolTipText( "Delete the selected profile.");
            this.m_removeButton.addListener( "execute",this._sendRemoveCmd, this );
            butContainer.add( this.m_addButton );
            butContainer.add( this.m_copyButton );
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
                this._updateSelecting( existingIndex);
            }
            else {
                //If it is a new name, the user renamed the existing curve
                //and we need to notify the server.
                this._sendRenameCmd();
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
         * Send a command to the server to generate a new profile
         * based on a copy of the current profile.
         */
        _sendCopyCmd : function(){
            if ( this.m_id !== null && this.m_connector !== null ){
                var newName = this.m_nameSelect.getValue();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "copyProfile";
                var params = "name:"+newName;
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
         * Set the server side id of the object managing profile information.
         * @param id {String} the server side id of the managing profiles.
         */
        setId : function( id ){
            this.m_id = id;
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
            }
            catch( err ){
                console.log( "Could not parse profile names." );
                console.log( "Err: "+err);
            }
        },
        
        /**
         * Update the names of the images used to generate profile curves based
         * on server information.
         */
        _updateImageNames : function( data){
            this.m_imageSelect.setSelectItems( data.images );
        },
        
        /**
         * Update the controls based on the selected profile curve.
         * @param selectIndex {Number} - the index of the selected curve.
         */
        _updateSelection : function( selectIndex ){
            this.m_selectIndex = selectIndex;
            var name = this.m_curveInfo[selectIndex].name;
            if ( this.m_nameSelect.getValue() != name ){
                this.m_nameSelect.setValue( name );
            }
            var imageName = this.m_curveInfo[selectIndex].image;
            if ( imageName != this.m_imageSelect.getValue() ){
                this.m_imageSelect.setSelectValue( imageName );
            }
            var restFreq = Number(this.m_curveInfo[selectIndex].restFrequency);
            this.m_restFreqText.setValue( restFreq );
            var statName = this.m_curveInfo[selectIndex].stat;
            if ( statName != this.m_statSelect.getValue() ){
                this.m_statSelect.setSelectValue( statName );
            }
        },
        
        m_id : null,
        m_selectIndex : null,
        m_addButton : null,
        m_copyButton : null,
        m_removeButton : null,
        m_imageSelect : null,
        m_genSelect : null,
        m_nameSelect : null,
        m_regionSelect : null,
        m_restFreqText : null,
        m_statSelect : null,
        m_connector : null,
        m_sharedVarStats : null,
        m_sharedVarModes : null,
        m_curveInfo : null
    }
});
