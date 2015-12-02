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
         * User has changed the auto-select mode of the list.
         */
        _autoSelectChanged : function(){
            var auto = this.m_autoSelectCheck.getValue();
            this.m_imageList.setEnabled( !auto );
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
                    this.m_datas = controls.data;
                    this.m_imageList.setListItems( controls.data );
                   
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
            this._initList();
            this._initMaskControls();
            this._add( new qx.ui.core.Spacer(), {flex:1} );
        },
        
        /**
         * Initialize the list of loaded images.
         */
        _initList : function(){
            var listContainer = new qx.ui.container.Composite();
            listContainer.setLayout( new qx.ui.layout.VBox(1));
            
            //Check container 
            var selectContainer = new qx.ui.container.Composite();
            selectContainer.setLayout( new qx.ui.layout.HBox(1) );
            selectContainer.add( new qx.ui.core.Spacer(), {flex:1});
            
            //Auto select check
            this.m_autoSelectCheck = new qx.ui.form.CheckBox( "Auto Select");
            skel.widgets.TestID.addTestId( this.m_autoSelectCheck, "autoSelectImages" ); 
            this.m_autoSelectCheck.setToolTipText( "Auto selection based on animator or manual selection of layer(s).");
            this.m_autoSelectCheck.addListener( "changeValue", this._autoSelectChanged, this );
            selectContainer.add( this.m_autoSelectCheck );
            selectContainer.add( new qx.ui.core.Spacer(), {flex:1});
            
            //Pan/zoom all check
            this.m_panZoomAllCheck = new qx.ui.form.CheckBox( "Pan/Zoom All"); 
            this.m_panZoomAllCheck.setToolTipText( "Pan/Zoom all images in the stack instead of just the top image.");
            this.m_panZoomAllCheck.addListener( "changeValue", this._sendPanZoomAllCmd, this );
            selectContainer.add( this.m_panZoomAllCheck );
            selectContainer.add( new qx.ui.core.Spacer(), {flex:1});
            
            //List
            this.m_imageList = new skel.widgets.Image.Stack.DragDropList( 250 );
            this.m_imageList.addListener( "listReordered", this._sendReorderCmd, this );
            this.m_imageList.addListener( "listSelection", this._listItemSelected, this );
            
            //Add to main container.
            listContainer.add( selectContainer );
            listContainer.add( this.m_imageList );
            this._add( listContainer );
        },
        
        /**
         * Initialize the available mask controls for each list item.
         */
        _initMaskControls : function(){
            this.m_maskControls = new skel.widgets.Image.Stack.MaskControls();
            this._add( this.m_maskControls );
        },
        
        /**
         * Update the mask controls with the selected item and notify the server
         * that data was selected.
         */
        _listItemSelected : function(){
            var indices = this.m_imageList.getSelectedIndices();
            if ( indices.length > 0 && this.m_datas.length > 0 ){
                var firstIndex = indices[0];
                this.m_maskControls.setControls( this.m_datas[firstIndex].mask );
                this._sendSelectionCmd();
            }
        },
        

        /**
         * Register to get updates on stack settings from the server.
         */
        _registerControls : function(){
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id );
            this.m_sharedVar.addCB(this._controlsChangedCB.bind(this));
            this._controlsChangedCB();
        },

        /**
         * Register to get updates on stack data settings from the server.
         */
        _registerControlsData : function(){
            var path = skel.widgets.Path.getInstance();
            var dataPath = this.m_id + path.SEP + "data";
            this.m_sharedVarData = this.m_connector.getSharedVar( dataPath );
            this.m_sharedVarData.addCB(this._controlsDataChangedCB.bind(this));
            this._controlsDataChangedCB();
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
         * Send a command to the server to reorder the images in the stack.
         * @param msg {Array} - a list specifying the new image order.
         */
        _sendReorderCmd : function( msg ){
            var imageList = msg.getData().listItems;
            if ( imageList.length > 0 ){
                var params = imageList.join(";");
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setImageOrder";
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        
        /**
         * Send a command to the server to select particular images in the stack.
         */
        _sendSelectionCmd : function(){
            //Only send list selections if the user is manually doing a
            //selection.
            if ( ! this.m_autoSelectCheck.getValue() ){
                var indices = this.m_imageList.getSelectedIndices();
                if ( indices.length > 0 ){
                    var params = indices.join(";");
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + "setLayersSelected";
                    this.m_connector.sendCommand( cmd, params, function(){});
                }
            }
        },
        
        
        /**
         * Send a command to the server to reorder the images in the stack.
         * @param msg {Array} - a list specifying the new image order.
         */
        _sendStackAutoSelectCmd : function( msg ){
            var autoSelect = this.m_autoSelectCheck.getValue();
            var params = "stackAutoSelect:"+autoSelect;
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + "setStackSelectAuto";
            this.m_connector.sendCommand( cmd, params, function(){});
        },

        /**
         * Send a command to the server to get the stack control id.
         * @param imageId {String} the server side id of the image object.
         */
        setId : function( imageId ){
            this.m_id = imageId;
            this.m_maskControls.setId( imageId );
            this._registerControls();
            this._registerControlsData();
        },

        m_id : null,
        m_connector : null,
        m_datas : null,
        m_panZoomAllCheck : null,
        m_sharedVar : null,
        m_sharedVarData : null,
        m_autoSelectCheck : null,
        m_maskControls : null,
        m_imageList : null
    }
});