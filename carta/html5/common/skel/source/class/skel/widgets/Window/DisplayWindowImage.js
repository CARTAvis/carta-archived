/**
 * A display window specialized for viewing images.
 */

/*global mImport */
/**
 @ignore( mImport)
 ************************************************************************ */

qx.Class.define("skel.widgets.Window.DisplayWindowImage", {
    extend : skel.widgets.Window.DisplayWindow,
    include : skel.widgets.Window.PreferencesMixin,
    
    /**
     * Constructor.
     */
    construct : function(index, detached) {
        this.base(arguments, skel.widgets.Path.getInstance().CASA_LOADER, index, detached );
        this.m_links = [];
        this.m_viewContent = new qx.ui.container.Composite();
        this.m_viewContent.setLayout(new qx.ui.layout.Canvas());
        
        this.m_content.add( this.m_viewContent, {flex:1} );
        this.m_imageControls = new skel.widgets.Image.ImageControls();
        this.m_imageControls.addListener( "gridControlsChanged", this._gridChanged, this );
        this.m_content.add( this.m_imageControls );
    },

    members : {
        
        /**
         * Add or remove the grid control settings based on whether the user
         * had configured any of the settings visible.
         * @param content {boolean} - true if the content should be visible; false otherwise.
         */
        _adjustControlVisibility : function(content){
            this.m_controlsVisible = content;
            this._layoutControls();
        },
        
        
        /**
         * Clean-up items; this window is going to disappear.
         */
        clean : function(){
            //Remove the view so we don't get spurious mouse events sent to a 
            //controller that no longer exists.
            if ( this.m_view !== null ){
                if ( this.m_viewContent.indexOf( this.m_view) >= 0 ){
                    this.m_viewContent.remove( this.m_view);
                   
                }
            }
        },

        /**
         * Call back that initializes the View when data is loaded.
         */
        _dataLoadedCB : function(){
            if (this.m_view === null) {
                this.m_view = new skel.boundWidgets.View.PanZoomView(this.m_identifier);
            }
            
            if (this.m_viewContent.indexOf(this.m_view) < 0) {
                var overlayMap = {left:"0%",right:"0%",top:"0%",bottom: "0%"};
                this.m_viewContent.add(this.m_view, overlayMap );
                
            }
           
            this.m_view.setVisibility( "visible" );
        },
        
        /**
         * Notify the server that data has been loaded.
         * @param path {String} an identifier for locating the data.
         */
        dataLoaded : function(path) {
            var pathDict = skel.widgets.Path.getInstance();
            var cmd = pathDict.getCommandDataLoaded();
            var params = "id:" + this.m_identifier + ",data:" + path;
            this.m_connector.sendCommand(cmd, params, function() {});
        },

        /**
         * Unloads the data identified by the path.
         */
        dataUnloaded : function(path) {
            this.m_viewContent.removeAll();
        },

        /**
         * Return the list of data that is currently open and could be closed.
         * @return {Array} a list of images that could be closed.
         */
        getCloses : function(){
            return this.m_datas;
        },
        
        /**
         * Notification that the grid controls have changed on the server-side.
         * @param ev {qx.event.type.Data}.
         */
        _gridChanged : function( ev ){
            var data = ev.getData();
            var showStat = data.grid.grid.showStatistics;
            this._showHideStatistics( showStat );
        },

        /**
         * Initializes the region drawing context menu.
         */
        _initMenuRegion : function() {
            var regionMenu = new qx.ui.menu.Menu();
            this._initShapeButtons(regionMenu, false);

            var multiRegionButton = new qx.ui.menu.Button("Multi");
            regionMenu.add(multiRegionButton);
            var multiRegionMenu = new qx.ui.menu.Menu();
            this._initShapeButtons(multiRegionMenu, true);

            multiRegionButton.setMenu(multiRegionMenu);
            return regionMenu;
        },


        /**
         * Initializes a menu button for drawing a shape such as a rectangle or ellipse.
         * @param menu {qx.ui.menu.Menu} the containing menu for the shape button.
         * @param keepMode {boolean} whether the cursor should stay in draw mode or revert
         * 		back when the shape is finished.
         */
        _initShapeButtons : function(menu, keepMode) {
            var drawFunction = function(ev) {
                var buttonText = this.getLabel();
                var data = {
                    shape : buttonText,
                    multiShape : keepMode
                };
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "drawModeChanged", data));
            };
            for (var i = 0; i < this.m_shapes.length; i++) {
                var shapeButton = new qx.ui.menu.Button(this.m_shapes[i]);
                shapeButton.addListener("execute", drawFunction, shapeButton);
                menu.add(shapeButton);
            }
        },
        
        /**
         * Initialize the label for displaying cursor statistics.
         */
        _initStatistics : function(){
            if ( this.m_statLabel === null ){
                var path = skel.widgets.Path.getInstance();
                var viewPath = this.m_identifier + path.SEP + path.VIEW;
                this.m_statLabel = new skel.boundWidgets.Label( "", "", viewPath, function( anObject){
                    return anObject.formattedCursorCoordinates;
                });
                this.m_statLabel.setRich( true );
            }
        },
        
        /**
         * Initialize the list of window specific commands this window supports.
         */
        _initSupportedCommands : function(){
            if ( this.m_supportedCmds.length == 0 ){
                var clipCmd = skel.Command.Clip.CommandClip.getInstance();
                this.m_supportedCmds.push( clipCmd.getLabel() );
                var dataCmd = skel.Command.Data.CommandData.getInstance();
                this.m_supportedCmds.push( dataCmd.getLabel() );
                var saveCmd = skel.Command.Save.CommandSaveImage.getInstance();
                if ( saveCmd.isSaveAvailable() ){
                    this.m_supportedCmds.push( saveCmd.getLabel() );
                }
                var settingsCmd = skel.Command.Settings.SettingsImage.getInstance();
                this.m_supportedCmds.push( settingsCmd.getLabel());
                var popupCmd = skel.Command.Popup.CommandPopup.getInstance();
                this.m_supportedCmds.push( popupCmd.getLabel() );
                var zoomResetCmd = skel.Command.Data.CommandZoomReset.getInstance();
                this.m_supportedCmds.push( zoomResetCmd.getLabel() );
                var panResetCmd = skel.Command.Data.CommandPanReset.getInstance();
                this.m_supportedCmds.push( panResetCmd.getLabel() );
                arguments.callee.base.apply(this, arguments);
            }
        },
        
        /**
         * Returns whether or not this window can be linked to a window
         * displaying a named plug-in.
         * @param pluginId {String} a name identifying a plug-in.
         */
        isLinkable : function(pluginId) {
            var linkable = false;
            var path = skel.widgets.Path.getInstance();
            if (pluginId == path.ANIMATOR || /*pluginId == this.m_pluginId ||*/
                    pluginId == path.COLORMAP_PLUGIN ||pluginId == path.HISTOGRAM_PLUGIN || 
                    pluginId == path.STATISTICS ) {
                linkable = true;
            }
            return linkable;
        },
        
        

        /**
         * Returns whether or not this window supports establishing a two-way
         * link with the given plug-in.
         * @param pluginId {String} the name of a plug-in.
         */
        isTwoWay : function(pluginId) {
            var biLink = false;
            if (pluginId == this.m_pluginId) {
                biLink = true;
            }
            return biLink;
        },
        
        /**
         * Add/remove content based on user visibility preferences.
         */
        _layoutControls : function(){
            this.m_content.removeAll();
            this.m_content.add( this.m_viewContent, {flex:1} );
            if ( this.m_statisticsVisible ){
                this.m_content.add( this.m_statLabel );
            }
            if ( this.m_controlsVisible ){
                this.m_content.add( this.m_imageControls );
            }
        },

        /**
         * Callback for updating the visibility of the user settings from the server.
         */
        _preferencesCB : function(){
            if ( this.m_sharedVarPrefs !== null ){
                var val = this.m_sharedVarPrefs.get();
                if ( val !== null ){
                    try {
                        var setObj = JSON.parse( val );
                        this._adjustControlVisibility( setObj.settings );
                    }
                    catch( err ){
                        console.log( "ImageDisplay could not parse settings: "+val);
                        console.log( "err="+err);
                    }
                }
            }
        },
        
        /**
         * Show/hide the cursor statistics control.
         * @param visible {boolean} - true if the cursor statistics widget
         *      should be shown; false otherwise.
         */
        _showHideStatistics : function( visible ){
            this.m_statisticsVisible = visible;
            this._layoutControls();
        },
        

        setDrawMode : function(drawInfo) {
            if (this.m_drawCanvas !== null) {
                this.m_drawCanvas.setDrawMode(drawInfo);
            }
        },
        
        /**
         * Set the appearance of this window based on whether or not it is selected.
         * @param selected {boolean} true if the window is selected; false otherwise.
         * @param multiple {boolean} true if multiple windows can be selected; false otherwise.
         */
        setSelected : function(selected, multiple) {
            this.updateCmds();
            arguments.callee.base.apply(this, arguments, selected, multiple );
        },
        
        /**
         * Update window specific elements from the shared variable.
         * @param winObj {String} represents the server state of this window.
         */
        _sharedVarDataCB : function(){
            var val = this.m_sharedVarData.get();
            if ( val ){
                try {
                    var winObj = JSON.parse( val );
                    this.m_datas = [];
                    //Add close menu buttons for all the images that are loaded.
                    if ( winObj.data && winObj.data.length > 0){
                        for ( var i = 0; i < winObj.data.length; i++ ){
                            this.m_datas[i] = winObj.data[i];
                        }
                        this._dataLoadedCB();
                    }
                    else {
                        //No images to show so set the view hidden.
                        if ( this.m_view !== null ){
                            this.m_view.setVisibility( "hidden" );
                        }
                    }
                    var closeCmd = skel.Command.Data.CommandDataClose.getInstance();
                    closeCmd.closeChanged();
                    this._initContextMenu();
                }
                catch( err ){
                    console.log( "DisplayWindowImage could not parse: "+val );
                    console.log( "Error: "+err);
                }
            }
        },
        
        /**
         * Update the commands about clip settings.
         */
        updateCmds : function(){
            var autoClipCmd = skel.Command.Clip.CommandClipAuto.getInstance();
            autoClipCmd.setValue( this.m_autoClip );
            var clipValsCmd = skel.Command.Clip.CommandClipValues.getInstance();
            clipValsCmd.setClipValue( this.m_clipPercent );
        },
        
        /**
         * Implemented to initialize the context menu.
         */
        windowIdInitialized : function() {
            arguments.callee.base.apply(this, arguments);
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVarData = this.m_connector.getSharedVar( this.m_identifier+path.SEP +"data" );
            this.m_sharedVarData.addCB( this._sharedVarDataCB.bind( this ));
            this._sharedVarDataCB();
            this._initStatistics();
            this._dataLoadedCB();
            
            //Get the shared variable for preferences
            this.initializePrefs();
            this.m_imageControls.setId( this.getIdentifier());
        },
        
        /**
         * Update from the server.
         * @param winObj {Object} - an object containing server side information values.
         */
        windowSharedVarUpdate : function( winObj ){
            if ( winObj !== null ){
                this.m_autoClip = winObj.autoClip;
                this.m_clipPercent = winObj.clipValueMax - winObj.clipValueMin;
            }
        },
        
        m_autoClip : false,
        m_clipPercent : 0,
        
        m_regionButton : null,
        m_renderButton : null,
        m_drawCanvas : null,
        m_datas : [],
        m_sharedVarData : null,
       
        m_view : null,
        m_viewContent : null,
        m_imageControls : null,
        m_controlsVisible : false,
        m_statLabel : null,
        m_statisticsVisible : false,
        m_shapes : [ "Rectangle", "Ellipse", "Point", "Polygon" ]
    }

});
