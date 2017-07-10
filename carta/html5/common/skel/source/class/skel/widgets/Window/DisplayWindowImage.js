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

        // grimmer: click/select will trigger 4 times of call of _initMenu, why?
        // start CARTA, number of _initMenu's call is 7. So after simulating, becomes 11
        this.simulateSelecteFun = this.simulateSelecteFun.bind(this);
        setTimeout(this.simulateSelecteFun, 1200);

        // this.m_content.add( this.m_imageControls );
    },

    members : {

        simulateSelecteFun: function() {
            this.setSelected(true, false);
        },

        resetZoomToFitWindowForData: function(data) {
            this.m_view.sendZoomLevel(data.m_minimalZoomLevel, data.id);
            data.m_currentZoomLevel = data.m_minimalZoomLevel;
            data.m_effectZoomLevel = 1;
        },

        resetZoomToFitWindow: function(){

            if(!this.m_datas || !this.m_datas.length) {
                return;
            }

            var zoomAll = this.m_imageControls.m_pages[2].m_panZoomAllCheck.getValue();

            var dataLen = this.m_datas.length;
            for (var i = 0; i < dataLen; i++) {

                var data = this.m_datas[i];

                if (zoomAll) {
                    this.resetZoomToFitWindowForData(data);
                } else if (data.selected) {
                    this.resetZoomToFitWindowForData(data);
                    break;
                } else {
                    continue;
                }
            }
        },

        _handleWheelEvent: function(pt, wheelFactor,  data, zoomAll) {

            if (!data.pixelX || !data.pixelY){
                console.log("not invalid layerdata.pixelXorY");
                return;
            }

            // move the logic (->0.9) from Stack.cpp to here. Here is more suitable place.
            //TODO: grimmer. default m_currentZoomLevel =1 needed to be synced with cpp
            if ( wheelFactor < 0 ) {
                var newZoom = data.m_currentZoomLevel / 0.9;
            } else {
                var newZoom = data.m_currentZoomLevel * 0.9;
            }

            // console.log("aspect Wheel-newZoom:", newZoom,";min:", data.m_minimalZoomLevel);

            // Due to this limitation will have some behaviors which are expeceted by users, so just git rid of it
            // if (newZoom >= data.m_minimalZoomLevel) {

                data.m_currentZoomLevel = newZoom;

                // this.m_view.sendPanZoomLevel(pt, newZoom, data.id);
                // console.log("Aspect debug: send wheel zoom:"+newZoom,";",pt+";id:", data.id);

                // data.m_effectZoomLevel = data.m_currentZoomLevel / data.m_minimalZoomLevel;
            // }
        },

        //TODO: grimmer. save previous mousewheel and prevent wheel delta 1, -1, 1 happen
        _mouseWheelCB : function(ev) {

            var box = this.m_view.overlayWidget().getContentLocation( "box" );
            var pt = {
                x: ev.getDocumentLeft() - box.left,
                y: ev.getDocumentTop() - box.top
            };

            var wheelFactor = ev.getWheelDelta();

            if(!this.m_datas || !this.m_datas.length) {
                console.log("Aspect debug: somehow storing layerDatas (m_data) is something wrong");
                return;
            }

            var zoomAll = this.m_imageControls.m_pages[2].m_panZoomAllCheck.getValue();

            var dataLen = this.m_datas.length;
            for (var i = 0; i < dataLen; i++) {

                var data = this.m_datas[i];

                if (zoomAll) {
                    this._handleWheelEvent(pt, wheelFactor, data, zoomAll);
                } else if (data.selected) {
                    this._handleWheelEvent(pt, wheelFactor, data, zoomAll);
                    break;
                } else {
                    continue;
                }
            }

            // each this.m_view.sendPanZoomLevel(pt, newZoom, data.id); in loop
            //->
            this.m_view.sendPanZoom(pt, wheelFactor);

        },

        _setupDefaultLayerData: function(data, oldDatas) {


            data.m_minimalZoomLevel = 1;
            data.m_currentZoomLevel = 1;

            data.m_effectZoomLevel = 1; // not use this anymore, will remove later

            data.m_scheduleZoomFit = true;

            var len = oldDatas.length;
            for (var i = 0; i < len; i++) {
                var oldData = oldDatas[i];
                //because now Carta can open duplicate dataset, so not use data.name
                if (oldData.id == data.id) {
                    data.m_currentZoomLevel = oldData.m_currentZoomLevel;
                    // console.log("Aspect debug: inherit old zoomLevel");

                    if (data.pixelX == oldData.pixelX && data.pixelY == oldData.pixelY) {
                        data.m_scheduleZoomFit = oldData.m_scheduleZoomFit;
                    } else {
                        //console.log("Aspect debug:aspect not same x, y ratio, should be permutation case");
                    }

                    break;
                }
            }
        },

        _calculateFitZoomLevel: function(view_width, view_height, data){
            var zoomX = view_width / data.pixelX;
            var zoomY = view_height / data.pixelY;
            var zoom = 1;

            if (zoomX < 1 || zoomY < 1) {

                if (zoomX > zoomY){
                    zoom = zoomY; //aj.fits, 512x1024, slim
                } else {
                    zoom = zoomX; //502nmos.fits, 1600x1600, fat
                }

            }  else { //equual or smaller than window size
                if (zoomX > zoomY){
                    zoom = zoomY; // M100_alma.fits,52x62 slim
                } else {
                    zoom = zoomX; // cube_x220_z100_17MB,220x220 fat
                }
            }

            return zoom;
        },

        // Trigger timing:
        // 0. init UI, 638x649: 2 times, 0x0: 1 time
        // 1. after load file,  638x664: 3 times
        // 2. When users adjsut the window size, the view will be updated
        // 3. any view updated. e.g. open two files, switch to another, get 2 times callback here

        // now the default window size is 638, 666
        useViewUpdateInfoToTryFitWindowSize: function(view_width, view_height) {

            // console.log("Aspect debug, view updateded, size:"+view_width+";"+view_height);
            if (!view_width || !view_height) {
                console.log("Aspect debug: invalid width or height");
                return;
            }

            if(!this.m_datas || !this.m_datas.length) {
                console.log("Aspect debug: somehow storing layerDatas (m_data) is something wrong");
                return;
            }

            var dataLen = this.m_datas.length;
            for (var i = 0; i < dataLen; i++) {

                var data = this.m_datas[i];

                if (!data.pixelX || !data.pixelY){
                    console.log("not invalid layerdata.pixelXorY");
                    break;
                }

                var zoom = this._calculateFitZoomLevel(view_width, view_height, data);

                //TODO maybe move to after !data.selecte
                // console.log("Aspect debug. set minimal for file:", data.name,";", zoom );
                data.m_minimalZoomLevel = zoom;

                // if (!data.selected) {
                //     continue;
                // }

                if (!data.m_scheduleZoomFit) {
                    continue;
                }

                data.m_scheduleZoomFit = false;

                var finalZoom = zoom; //*data.m_effectZoomLevel

                // m_curentZoomLevel == 1 means, this dataset is initially loaded
                // if not equual to 1, means like A->B->A, does not fit to Window Size automatically
                // 20170502 update: after add data.m_scheduleZoomFit, the check == 1 may not be needed
                if (finalZoom != data.m_currentZoomLevel) {
                    // console.log("Aspect debug: try to send zoom level");
                    this.m_view.sendZoomLevel(finalZoom, data.id);

                    //TODO: grimmer. need to passive-sync m_currentZoomLevel with cpp
                    data.m_currentZoomLevel = finalZoom;
                }
            }
        },


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
                this.m_view.installHandler( skel.boundWidgets.View.InputHandler.Drag );

                // for fitToWinowSize and setup minimal zoom level functions.
                this.m_view.m_updateViewCallback = this.useViewUpdateInfoToTryFitWindowSize.bind(this);
                this.m_view.addListener( "mousewheel", this._mouseWheelCB.bind(this));
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
        getDatas : function(){
            return this.m_datas;
        },

        /**
         * Return the identifier for the region controller.
         * @return {String} - the identifier of the region controller.
         */
        getRegionIdentifier : function(){
        	return this.m_regionId;
        },

        /**
         * Return a list of regions in the image.
         * @return {Array} - a list of regions in the image.
         */
        getRegions : function(){
            return this.m_regions;
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
                    // this callback function will be called for everytime it receives new cursor pixel value coordinate
                    // from flushstate
                    return anObject.formattedCursorCoordinates;
                });
                this.m_statLabel.setRich( true );
            }
        },

        /**
         * Initialize the label for displaying current file name.
         */
        _initFileLabel: function() {
            if ( this.m_fileLabel === null ){
                this.m_fileLabel = new qx.ui.basic.Label();
            }
        },

        _getCurrentFileName: function() {
            if (!this.m_datas) {
                return ""
            }

            var len = this.m_datas.length;

            for (var i = 0; i < len; i++) {
                var layerData = this.m_datas[i];
                if (layerData.selected == true && layerData.name) {
                    return layerData.name;
                }
            }

            return "";
        },

        _updateFileLabel: function() {
            if (this.m_fileLabel) {
                this.m_fileLabel.setValue(this._getCurrentFileName());
            }
        },

        /**
         * Initialize the list of window specific commands this window supports.
         */
        _initSupportedCommands : function(){
            this.m_supportedCmds = [];

            var clipCmd = skel.Command.Clip.CommandClip.getInstance();
            this.m_supportedCmds.push( clipCmd.getLabel() );
            var dataCmd = skel.Command.Data.CommandData.getInstance();
            this.m_supportedCmds.push( dataCmd.getLabel() );
            var regionCmd = skel.Command.Region.CommandRegions.getInstance();
            this.m_supportedCmds.push( regionCmd.getLabel() );
            var saveCmd = skel.Command.Save.CommandSaveImage.getInstance();
            if ( saveCmd.isSaveAvailable() ){
                this.m_supportedCmds.push( saveCmd.getLabel() );
            }
            var settingsCmd = skel.Command.Settings.SettingsImage.getInstance();
            this.m_supportedCmds.push( settingsCmd.getLabel());
            // var popupCmd = skel.Command.Popup.CommandPopup.getInstance();
            // this.m_supportedCmds.push( popupCmd.getLabel() );
            var zoomResetCmd = skel.Command.Data.CommandZoomReset.getInstance();
            this.m_supportedCmds.push( zoomResetCmd.getLabel() );
            var panResetCmd = skel.Command.Data.CommandPanReset.getInstance();
            this.m_supportedCmds.push( panResetCmd.getLabel() );
            arguments.callee.base.apply(this, arguments);
        },

        /**
         * Returns whether or not this window can be linked to a window
         * displaying a named plug-in.
         * @param pluginId {String} a name identifying a plug-in.
         */
        isLinkable : function(pluginId) {
            var linkable = false;
            var path = skel.widgets.Path.getInstance();
            if (pluginId == path.ANIMATOR ||
                    pluginId == path.COLORMAP_PLUGIN ||pluginId == path.HISTOGRAM_PLUGIN ||
                    pluginId == path.STATISTICS || pluginId == path.PROFILE ||
                    pluginId == path.IMAGE_CONTEXT || pluginId == path.IMAGE_ZOOM ) {
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
                // this.m_content.add(this.m_fileLabel);
                // this._updateFileLabel();
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
         * Register to get updates on stack data settings from the server.
         */
        _registerControlsRegion : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_identifier + path.SEP_COMMAND + "registerRegionControls";
            var params = "";
            this.m_connector.sendCommand( cmd, params, this._registrationRegionCallback( this));
        },

        /**
         * Register to get updates on stack data settings from the server.
         */
        _registerControlsStack : function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_identifier + path.SEP_COMMAND + "registerStack";
            var params = "";
            this.m_connector.sendCommand( cmd, params, this._registrationStackCallback( this));
        },


        /**
         * Callback for when the registration is complete and an id is available.
         * @param anObject {skel.widgets.Image.Region.RegionControls}.
         */
        _registrationRegionCallback : function( anObject ){
            return function( id ){
                anObject._setRegionId( id );
            };
        },

        /**
         * Callback for when the registration is complete and an id is available.
         * @param anObject {skel.widgets.Image.Stack.StackControls}.
         */
        _registrationStackCallback : function( anObject ){
            return function( id ){
                anObject._setStackId( id );
            };
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
            this._initSupportedCommands();
            this.updateCmds();
            arguments.callee.base.apply(this, arguments, selected, multiple );
        },

        /**
         * Update region specific elements from the shared variable.
         */
        _sharedVarRegionsCB : function(){
            var val = this.m_sharedVarRegions.get();
            if ( val ){
                try {
                    var winObj = JSON.parse( val );
                    var regionShape = winObj.createType;

                    var regionDrawCmds = skel.Command.Region.CommandRegions.getInstance();
                    regionDrawCmds.setDrawType( regionShape );
                }
                catch( err ){
                    console.log( "DisplayWindowImage could not parse region update: "+val );
                    console.log( "Error: "+err);
                }
            }
        },

        /**
         * Update region data specific elements from the shared variable.
         */
        _sharedVarRegionsDataCB : function(){
            var val = this.m_sharedVarRegionsData.get();
            if ( val ){
                try {
                    var regionObj = JSON.parse( val );
                    this.m_regions = [];
                    for ( var i = 0; i < regionObj.regions.length; i++ ){
                    	this.m_regions[i] = regionObj.regions[i];
                    }
                    var dataCmd = skel.Command.Data.CommandData.getInstance();
                    dataCmd.datasChanged();
                    this._initContextMenu();
                }
                catch( err ){
                    console.log( "DisplayWindowImage could not parse region data update: "+val );
                    console.log( "Error: "+err);
                }
            }
        },

        /**
         * Update window specific elements from the shared variable.
         * @param winObj {String} represents the server state of this window.
         */
        _sharedVarStackCB : function(){

            // Trigger timing:
            // 1. open any new file (two times callback, but the later one has correct selected info)
            // 2. switch a opened data (1 callback)
            var val = this.m_sharedVarStack.get();
            if (val) {
                try {
                    var winObj = JSON.parse( val );

                    var oldDatas = this.m_datas;
                    this.m_datas = [];
                    //Add close menu buttons for all the images that are loaded.
                    var dataObjs = winObj.layers;
                    var visibleData = false;
                    if (dataObjs) {
                        for ( var j = 0; j < dataObjs.length; j++ ){
                            if ( dataObjs[j].visible ){
                                visibleData = true;
                                break;
                            }
                        }
                    }

                    if (dataObjs) {
                        var len = dataObjs.length;
                        for (var i = 0; i < len; i++) {
                            var newDataObj = dataObjs[i];
                            this._setupDefaultLayerData(newDataObj, oldDatas);

                            // there is a cpp bug
                            // open two files, close 2nd file, only get 1 changed stack but the info of 1 layer is
                            // selected=false !!!
                            if (len == 1) {
                                newDataObj.selected = true;
                            }

                            this.m_datas.push(newDataObj);
                        }
                        if (len > 0 && visibleData) {
                            this._dataLoadedCB();
                        }
                    }

                    if (!visibleData) {
                        //No images to show so set the view hidden.
                        if (this.m_view !== null) {
                            this.m_view.setVisibility( "hidden" );
                        }
                    }
                    var dataCmd = skel.Command.Data.CommandData.getInstance();
                    dataCmd.datasChanged();
                    this._initContextMenu();

                    // to show the file name of the current image
                    this._updateFileLabel();
                }
                catch( err ){
                    console.log( "DisplayWindowImage could not parse: "+val );
                    console.log( "Error: "+err);
                }
            }
        },


        /**
         * Set the identifier for the server-side object that manages the stack.
         * @param id {String} - the server-side id of the object that manages the stack.
         */
        _setRegionId : function( id ){
            this.m_regionId = id;
            this.m_sharedVarRegions = this.m_connector.getSharedVar( id );
            this.m_sharedVarRegions.addCB(this._sharedVarRegionsCB.bind(this));
            this._sharedVarRegionsCB();

            var path = skel.widgets.Path.getInstance();
        	var regionDataId = id + path.SEP + path.DATA;
        	this.m_sharedVarRegionsData = this.m_connector.getSharedVar( regionDataId );
        	this.m_sharedVarRegionsData.addCB( this._sharedVarRegionsDataCB.bind(this));
        	this._sharedVarRegionsDataCB();
        },

        /**
         * Set the identifier for the server-side object that manages the stack.
         * @param id {String} - the server-side id of the object that manages the stack.
         */
        _setStackId : function( id ){
            this.m_stackId = id;
            this.m_sharedVarStack = this.m_connector.getSharedVar( id );
            this.m_sharedVarStack.addCB(this._sharedVarStackCB.bind(this));
            this._sharedVarStackCB();
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

            this._registerControlsStack();
            this._registerControlsRegion();
            this._initStatistics();
            this._initFileLabel();

            // commnet this to avoild splash black empty image when launching CARTA, use lazy loading
            // this._dataLoadedCB();

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
                // console.log("Clip debug, max:",winObj.clipValueMax,";min:",winObj.clipValueMin);
            }
        },

        m_zoomAllmode : false,
        // m_scheduleZoomFit : false,
        m_autoClip : false,
        m_clipPercent : 0,

        m_regionButton : null,
        m_renderButton : null,
        m_drawCanvas : null,
        m_datas : [],
        m_regions : [],
        m_sharedVarStack : null,
        m_sharedVarRegions : null,
        m_sharedVarRegionsData : null,
        m_stackId : null,
        m_regionId : null,

        m_view : null,
        m_viewContent : null,
        m_imageControls : null,
        m_controlsVisible : false,
        m_statLabel : null,
        m_fileLabel : null,
        m_statisticsVisible : false,
        m_shapes : [ "Rectangle", "Ellipse", "Point", "Polygon" ]
    }

});
