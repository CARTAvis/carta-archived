/**
 * Displays controls for customizing the image stack.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Stack.StackControls", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments, "Stack", "");
        this.m_connector = mImport("connector");
        this.m_datas = [];
        this._init();
    },

    members : {

        /**
         * Add or remove the control for combining layers into a group or splitting
         * layers from a group.
         * @param add {boolean} - true if the control should be added; false otherwise.
         */
        _addGroupCheck : function( add ){
            var groupIndex = this.m_selectContainer.indexOf( this.m_groupCheck );
            if ( add ){
                if ( groupIndex < 0 ){
                    this.m_selectContainer.add( this.m_groupCheck );
                    this.m_selectContainer.add( this.m_lastSpacer, {flex:1});
                }
            }
            else {
                if ( groupIndex >= 0 ){
                    this.m_selectContainer.remove( this.m_groupCheck );
                    this.m_selectContainer.remove( this.m_lastSpacer );
                }
            }
        },

        /**
         * User has changed the auto-select mode of the list.
         */
        _autoSelectChanged : function(){
            var auto = this.m_autoSelectCheck.getValue();
            this.m_imageTree.setAutoSelect( auto );
            this._sendStackAutoSelectCmd();
        },

        /**
         * Callback for a change in stack settings.
         */
        _controlsChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var controls = JSON.parse( val );
                    this.m_autoSelectCheck.setValue( controls.stackAutoSelect );
                    this.m_panZoomAllCheck.setValue( controls.panZoomAll );

                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.clearErrors();
                }
                catch( err ){
                    console.log( "Stack controls could not parse: "+val+" error: "+err );
                }
            }
        },

        /**
         * Callback for a change in stack data settings.
         */
        _controlsDataChangedCB : function(){
            var val = this.m_sharedVarData.get();
            if ( val ){
                try {
                    var controls = JSON.parse( val );
                    this.m_datas = controls.layers;
                    this.m_imageTree._updateTree( controls.layers );
                    this._updateSettings();
                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.clearErrors();
                }
                catch( err ){
                    console.log( "Stack controls could not parse data: "+val+" error: "+err );
                }
            }
        },

        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );
            this._setLayout( new qx.ui.layout.HBox(1));
            this._initTree();
            this._initMaskControls();
        },

        /**
         * Initialize the available mask controls for each list item.
         */
        _initMaskControls : function(){
            this.m_maskControls = new skel.widgets.Image.Stack.LayerSettings();
            this.m_splitter.add( this.m_maskControls, 1 );
        },


        /**
         * Initialize the list of loaded images.
         */
        _initTree : function(){
            var listContainer = new qx.ui.container.Composite();
            listContainer.setLayout( new qx.ui.layout.VBox(1));

            //Check container
            this.m_selectContainer = new qx.ui.container.Composite();
            this.m_selectContainer.setLayout( new qx.ui.layout.HBox(1) );
            this.m_selectContainer.add( new qx.ui.core.Spacer(), {flex:1});

            //Auto select check
            this.m_autoSelectCheck = new qx.ui.form.CheckBox( "Auto Select");
            skel.widgets.TestID.addTestId( this.m_autoSelectCheck, "autoSelectImages" );
            this.m_autoSelectCheck.setToolTipText( "Auto selection based on animator or manual selection of layer(s).");
            this.m_autoSelectCheck.addListener( "changeValue", this._autoSelectChanged, this );
            this.m_selectContainer.add( this.m_autoSelectCheck );
            this.m_selectContainer.add( new qx.ui.core.Spacer(), {flex:1});

            //Pan/zoom all check
            this.m_panZoomAllCheck = new qx.ui.form.CheckBox( "Pan/Zoom All");
            this.m_panZoomAllCheck.setToolTipText( "Pan/Zoom all images in the stack instead of just the top image.");
            this.m_panZoomAllCheck.addListener( "changeValue", this._sendPanZoomAllCmd, this );
            this.m_selectContainer.add( this.m_panZoomAllCheck );
            this.m_selectContainer.add( new qx.ui.core.Spacer(), {flex:1});



            this.m_imageTree = new skel.widgets.Image.Stack.StackTree();
            this.m_imageTree.addListener( "treeSelection", this._treeItemSelected, this );

            //Add to main container.
            listContainer.add( this.m_selectContainer );
            this.m_splitter = new qx.ui.splitpane.Pane( "horizontal");
            this.m_splitter.add( this.m_imageTree, 0 );
            listContainer.add( this.m_splitter, {flex:1} );
            this._add( listContainer, {flex:1} );
        },

        /**
         * Register to get updates on stack settings from the server.
         */
        _registerControls : function(){
          console.log("grimmer StackControls");
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id );
            this.m_sharedVar.addCB(this._controlsChangedCB.bind(this));
            this._controlsChangedCB();
        },

        /**
         * Register to get updates on stack data settings from the server.
         */
        _registerControlsData : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "registerStack";
            var params = "";
            this.m_connector.sendCommand( cmd, params, this._registrationCallback( this));
        },

        /**
         * Callback for when the registration is complete and an id is available.
         * @param anObject {skel.widgets.Image.Stack.StackControls}.
         */
        _registrationCallback : function( anObject ){
            return function( id ){
                anObject._setStackId( id );
            };
        },


        /**
         * Send a command to the server indicating whether to pan/zoom all images
         * in the stack or only the top one.
         */
        _sendPanZoomAllCmd : function(){
            var panZoomAll = this.m_panZoomAllCheck.getValue();
            var params = "panZoomAll:"+panZoomAll;
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setPanZoomAll";
            this.m_connector.sendCommand( cmd, params, function(){});
        },


        /**
         * Send a command to the server to select particular images in the stack.
         */
        _sendSelectionCmd : function(){
            //Only send tree selections if the user is manually doing a
            //selection.
            if ( ! this.m_autoSelectCheck.getValue() ){
                var selectIds = this.m_imageTree.getSelectedPaths();
                if ( selectIds.length > 0 ){
                    var params = selectIds.join( ";");
                    if ( params.length > 0 ){
                        var path = skel.widgets.Path.getInstance();
                        var cmd = this.m_id + path.SEP_COMMAND + "setLayersSelected";
                        this.m_connector.sendCommand( cmd, params, function(){});
                    }
                }
            }
        },


        /**
         * Send a command to the server to change auto selection of the images in the stack.
         */
        _sendStackAutoSelectCmd : function( ){
            var autoSelect = this.m_autoSelectCheck.getValue();
            var params = "stackAutoSelect:"+autoSelect;
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setStackSelectAuto";
            this.m_connector.sendCommand( cmd, params, function(){});
        },

        /**
         * Set the identifier for the server-side object managing the stack.
         * @param id {String} - the server-side id of the object managing the stack.
         */
        _setStackId : function( id ){
                this.m_sharedVarData = this.m_connector.getSharedVar( id );
                this.m_sharedVarData.addCB(this._controlsDataChangedCB.bind(this));
                this._controlsDataChangedCB();
        },

        /**
         * Send a command to the server to get the stack control id.
         * @param imageId {String} the server side id of the image object.
         */
        setId : function( imageId ){
            this.m_id = imageId;
            this.m_imageTree.setId( this.m_id );
            this.m_maskControls.setId( imageId );
            this._registerControls();
            this._registerControlsData();
        },

        /**
         * Update the group layers control and the layer settings based on
         * the tree item(s) selected.
         */
        _treeItemSelected : function( msg ){
            var sendCmd = msg.getData().send;
            this._updateSettings();
            if ( sendCmd ){
                this._sendSelectionCmd();
            }
        },


        /**
         * Update the layer settings based on the node that is selected.
         */
        _updateSettings : function(){
            var nameSelected = this.m_imageTree.getSelectedName();
            this.m_maskControls.setName( nameSelected );
            var idSelected = this.m_imageTree.getSelectedId();
            this.m_maskControls.setLayerId( idSelected );
            var groupSelected = this.m_imageTree.isGroupSelected();
            if ( groupSelected ){
                this.m_maskControls.setModeGroup();
            }
            else {
                if ( this.m_imageTree.isColorNodeSelected() ){
                    this.m_maskControls.setModeColor();
                }
                else {
                    this.m_maskControls.setModeAlpha();
                }
            }
            var settings = this.m_imageTree.getSelectedSettings();
            if ( settings !== null ){
                this.m_maskControls.setSettings( settings );
            }
        },


        m_id : null,
        m_connector : null,
        m_datas : null,
        m_panZoomAllCheck : null,
        m_sharedVar : null,
        m_sharedVarData : null,
        m_autoSelectCheck : null,
        m_maskControls : null,
        m_imageTree : null,
        m_selectContainer : null,
        m_splitter : null
    }
});
