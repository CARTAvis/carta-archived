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

        // this.m_scheduleZoomFit = false;
        this.m_viewContent.addListener( "resize", function( /*e*/ ) {
            console.log("grimmer aspect display window, setupt m_scheduleZoomFit !!!");
            //先這個(還沒選檔前就會被call一次). 再layoutlead , 再來才是view.js 那邊
            // 之後主動改視窗大小就靠這個
            //TODO: grimmer.
            //0. 手動zoom , pan後, 再改視窗. 也要維持那個view區塊?
            //1. resetZoom 要改
            //2. 切換檔案, 沒有zoom all
            //3. 切換檔案, 有zoom all
            //4. 轉置

            //x 測 image layout. x

            this.m_scheduleZoomFit = true;
        }, this);

        // this._handleWheelEvent = this._handleWheelEvent.bind(this);
    },

    members : {

        //zoom all mode 很怪???

        resetZoomToFitWindow: function(){

            if(!this.m_datas || !this.m_datas.length) {
                console.log("grimmer aspect somehow this.m_data is something wrong");
            }

            // save as a loop function
            var dataLen = this.m_datas.length;
            for (var i = 0; i < dataLen; i++) {

                var data = this.m_datas[i];
                if (!this.m_zoomAllmode && !data.selected) {
                    continue;
                }
                
                //TODO, m_zoomAllmode
                if (this.m_zoomAllmode) {

                } else {
                    this.m_view.sendZoomLevel(data.m_minimalZoomLevel);
                }
            }

        },

        _handleWheelEvent: function(pt, zoomFactor,  data) {
            console.log("grimmer aspect mouseWheel2");

            // move the logic from Stack.cpp to here. Here is more suitable place.
            //TODO: grimmer. default m_currentZoomLevel =1 needed to be synced with cpp
            if ( zoomFactor < 0 ) {
                newZoom = data.m_currentZoomLevel / 0.9; //??
            } else {
                newZoom = data.m_currentZoomLevel * 0.9; //??
            }

            //console.log( "vwid wheel", pt.x, pt.y, ev.getWheelDelta());
            // var path = skel.widgets.Path.getInstance();
            // var cmd = this.m_viewId + path.SEP_COMMAND + "setPanAndZoomLevel";//path.ZOOM;
            //
            // this.m_connector.sendCommand( cmd,
            //     "" + pt.x + " " + pt.y + " " + newZoom);
            //找有沒有大於最低的, 有就送)
            if (newZoom> data.m_minimalZoomLevel) {

                //TODO, m_zoomAllmode
                if(this.m_zoomAllmode) {

                } else {
                    console.log("this.m_view:", this.m_view);
                    //this.m_view undefined
                    this.m_view.sendPanZoomLevel(pt, newZoom);

                    console.log("grimmer aspect, wheel zoom:"+newZoom,";",pt);

                    data.m_currentZoomLevel = newZoom;
                }
            }
        },

        //TODO: grimmer. save previous mousewheel and prevent wheel delta 1, -1, 1 happen
        _mouseWheelCB : function(ev) {

            console.log("grimmer aspect mouseWheel");
            var box = this.m_view.overlayWidget().getContentLocation( "box" );
            var pt = {
                x: ev.getDocumentLeft() - box.left,
                y: ev.getDocumentTop() - box.top
            };

            // 找出現在的zoom level, 所以要用現在的size嗎?

            var zoomFactor = ev.getWheelDelta();

            // this._loopLayerData(this._handleWheelEvent);

            if(!this.m_datas || !this.m_datas.length) {
                console.log("grimmer aspect somehow this.m_data is something wrong");
            }

            // save as a loop function
            var dataLen = this.m_datas.length;
            for (var i = 0; i < dataLen; i++) {
                console.log("grimmer aspec datas loop:", i);

                var data = this.m_datas[i];
                if (!this.m_zoomAllmode && !data.selected) {
                    continue;
                }

                //TODO: grimmer. if zoomALL.
                // if (this.m_zoomAllmode || data.selected) {

                if (!data.pixelX || !data.pixelY){
                    console.log("not invalid layerdata.pixelXorY");
                    continue;
                }

                this._handleWheelEvent(pt, zoomFactor, data);
                // handler(data);

                // If zoomAll = false
                if (!this.m_zoomAllmode){
                    break;
                }
                // }
            }
        },

        _setupDefaultLayerData: function(data) {

            data.m_minimalZoomLevel = 1;
            data.m_currentZoomLevel = 1;

            data.m_effectZoomLevel = 1;
            // m_currentZoomLevel: 1, //fitToWindow時要存. 手動放大放小時也存
            // m_effectZoomLevel:  1, //???
        },

        // _loopLayerData: function(handler){
        //
        //
        // },

        _adjustZoomToFitWindow : function(view_width, view_height){
            console.log("grimmer displaywindow, adjustZoomToFitWindow:"+view_width+";"+view_height);
            console.log("grimmer aspec datas:", this.m_datas);
            if (!view_width || !view_height) {
                console.log("invalid width or height");
                return;
            }

            if (!this.m_scheduleZoomFit) {
                return;
            }

            //now the default window size is 638, 666
            // if (this.m_scheduleZoomFit) {
            console.log("grimmer aspec datas2");
            this.m_scheduleZoomFit = false;

            if(!this.m_datas || !this.m_datas.length) {
                console.log("grimmer aspect somehow this.m_data is something wrong");
            }

            // save as a loop function
            var dataLen = this.m_datas.length;
            for (var i = 0; i < dataLen; i++) {
                console.log("grimmer aspec datas loop:", i);

                var data = this.m_datas[i];

                if (!this.m_zoomAllmode && !data.selected) {
                    continue;
                }

                //TODO: grimmer. if zoomALL.
                // if (this.m_zoomAllmode || data.selected) {

                if (!data.pixelX || !data.pixelY){
                    console.log("not invalid layerdata.pixelXorY");
                    continue;
                }

                var zoomX = view_width / data.pixelX;
                var zoomY = view_height / data.pixelY;
                var zoom = -1;

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

                // If zoomAll = false
                if (!this.m_zoomAllmode){

                    data.m_minimalZoomLevel = zoom;

                    //zoom level 一定會有個下限, 每次新的stack都要重算.
                    console.log("try to send zoom level:", zoom);
                    if (zoom != 1) {
                        this.m_view.sendZoomLevel(zoom);

                        //TODO: grimmer. need to passive-sync m_currentZoomLevel with cpp
                        data.m_currentZoomLevel = zoom;
                    }

                    break;
                } else {
                    //TODO, zoomAll mode part
                }
                // }
            }
            // }
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
            console.log("grimmer aspect displayWindow dataloadedCB !!!");
            if (this.m_view === null) {
                this.m_view = new skel.boundWidgets.View.PanZoomView(this.m_identifier);
                this.m_view.installHandler( skel.boundWidgets.View.InputHandler.Drag );

                // grimmer
                this.m_view.m_updateViewCallback = this._adjustZoomToFitWindow.bind(this);
                // move the code from PanZoomView to here.
                this.m_view.addListener( "mousewheel", this._mouseWheelCB.bind(this));

            }

            if (this.m_viewContent.indexOf(this.m_view) < 0) {
                var overlayMap = {left:"0%",right:"0%",top:"0%",bottom: "0%"};
                console.log("grimmer aspect dataloadedCB m_viewContent");
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
            console.log("grimmer menu region")
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
            console.log("grimmer menu shape")
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
            var popupCmd = skel.Command.Popup.CommandPopup.getInstance();
            this.m_supportedCmds.push( popupCmd.getLabel() );
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
            var val = this.m_sharedVarStack.get();
            if ( val ){
                try {
                    // console.log("grimmer aspect: sharedStack1:", val);
                    var winObj = JSON.parse( val );
                    console.log("grimmer aspect: sharedStack2:", val);
                    this.m_datas = [];
                    //Add close menu buttons for all the images that are loaded.
                    var dataObjs = winObj.layers;
                    var visibleData = false;
                    if ( dataObjs ){
                        for ( var j = 0; j < dataObjs.length; j++ ){
                            if ( dataObjs[j].visible ){
                                visibleData = true;
                                break;
                            }
                        }
                    }
                    // console.log("grimmer aspect: sharedStack3:", val);

                    if ( dataObjs && dataObjs.length > 0 ){
                        for ( var i = 0; i < dataObjs.length; i++ ){
                            // 存起來
                            this.m_datas[i] = dataObjs[i];
                            this._setupDefaultLayerData(this.m_datas[i]);
                            console.log("grimmer aspect data1");
                        }
                        if ( visibleData ){
                            this._dataLoadedCB();
                        }
                    }

                    // if (this.m_datas && this.m_datas.length > 0){
                    //     console.log("grimmer aspect data2, setup m_scheduleZoomFit");
                    //     // this.m_scheduleZoomFit =true;
                    // }
                    //
                    // console.log("grimmer aspect: sharedStack4:", val);


                    if ( !visibleData ){
                        //No images to show so set the view hidden.
                        if ( this.m_view !== null ){
                            this.m_view.setVisibility( "hidden" );
                        }
                    }
                    var dataCmd = skel.Command.Data.CommandData.getInstance();
                    dataCmd.datasChanged();
                    this._initContextMenu();

                    // console.log("grimmer aspect: sharedStack, after dataLoadedCB");

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
          console.log("grimmer DisplayWindowImage");

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
            console.log("grimmer DisplayWindowImage-stack");

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
                console.log("grimmer clip,max:",winObj.clipValueMax,";min:",winObj.clipValueMin);
                //grimmer clip,max: 0.975 ;min: 0.025
            }
        },

        m_zoomAllmode : false,
        m_scheduleZoomFit : false,
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
        m_statisticsVisible : false,
        m_shapes : [ "Rectangle", "Ellipse", "Point", "Polygon" ]
    }

});
