/**
 * A display window specialized for viewing images.
 */

/*global mImport */
/**
 @ignore( mImport)
 ************************************************************************ */

qx.Class.define("skel.widgets.Window.DisplayWindowImage", {
    extend : skel.widgets.Window.DisplayWindow,

    /**
     * Constructor.
     */
    construct : function(row, col, index, detached) {
        this.base(arguments, skel.widgets.Path.getInstance().CASA_LOADER, row, col, index, detached );
        this.m_links = [];
        this.m_content.setLayout(new qx.ui.layout.Canvas());
    },

    members : {


        /**
         * Call back that initializes the View when data is loaded.
         */
        _dataLoadedCB : function(){
            if (this.m_view === null) {
                this.m_view = new skel.boundWidgets.View.PanZoomView(this.m_identifier);
            }
           
            /*if ( this.m_drawCanvas === null ){
                this.m_drawCanvas = new skel.widgets.Draw.Canvas( this.m_identifier, this.m_view);
            }*/
            
            var overlayMap = {left:"0%",right:"0%",top:"0%",bottom: "0%"};
            if (this.m_content.indexOf(this.m_view) < 0) {
                this.m_content.add(this.m_view, overlayMap );
            }
            /*if ( this.m_content.indexOf( this.m_drawCanvas) < 0 ){
                this.m_content.add(this.m_drawCanvas, overlayMap);
            }*/
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
            this.m_content.removeAll();
        },

        /**
         * Returns context menu items that should be displayed on the main
         * window when this window is selected.
         */
        getWindowSubMenu : function() {
            var windowMenuList = [];

            var dataButton = new qx.ui.toolbar.MenuButton("Data");
            dataButton.setMenu(this._initDataMenu());

            var regionButton = new qx.ui.toolbar.MenuButton("Region");
            regionButton.setEnabled( false );
            regionButton.setMenu(this._initMenuRegion());

            var renderButton = new qx.ui.toolbar.MenuButton("Render");
            renderButton.setEnabled( false );
            renderButton.setMenu(this._initMenuRender());
            
            var showButton = new qx.ui.toolbar.MenuButton( "Show");
            showButton.setMenu( this._initMenuShow());
            
            windowMenuList.push(dataButton);
            windowMenuList.push(regionButton);
            windowMenuList.push(renderButton);
            windowMenuList.push(showButton);

            return windowMenuList;
        },

        /**
         * Initializes view elements specific to this window such as the context menu.
         */
        _initDisplaySpecific : function() {
            this.m_dataButton = new qx.ui.menu.Button("Data");
            this.m_dataButton.setMenu(this._initDataMenu());
            this.m_contextMenu.add(this.m_dataButton);

            this.m_regionButton = new qx.ui.menu.Button("Region");
            this.m_regionButton.setMenu(this._initMenuRegion());
            this.m_regionButton.setEnabled( false );
            this.m_contextMenu.add(this.m_regionButton);

            this.m_renderButton = new qx.ui.menu.Button("Render");
            this.m_renderButton.setMenu(this._initMenuRender());
            this.m_renderButton.setEnabled( false );
            this.m_contextMenu.add(this.m_renderButton);
            
            var showButton = new qx.ui.menu.Button( "Show");
            showButton.setMenu( this._initMenuShow());
            skel.widgets.TestID.addTestId( showButton, skel.widgets.TestID.SHOW_POPUP_BUTTON);
            this.m_contextMenu.add( showButton );
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
         * Initializes the renders context menu.
         */
        _initMenuRender : function() {
            var renderMenu = new qx.ui.menu.Menu();
            renderMenu.add(new qx.ui.menu.Button("Raster"));
            renderMenu.add(new qx.ui.menu.Button("Contour"));
            renderMenu.add(new qx.ui.menu.Button("Field"));
            renderMenu.add(new qx.ui.menu.Button("Vector"));
            return renderMenu;
        },
        
        /**
         * Initializes the show context menu.
         */
        _initMenuShow : function() {
            var showMenu = new qx.ui.menu.Menu();
            var path = skel.widgets.Path.getInstance();
            var colormapButton = new qx.ui.menu.Button( path.COLORMAP_PLUGIN );
            skel.widgets.TestID.addTestId( colormapButton, skel.widgets.TestID.COLOR_MAP_BUTTON);
            colormapButton.addListener( "execute", function(){
                var data  = {
                    winId : this.m_identifier,
                    pluginId : path.COLORMAP_PLUGIN
                };
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "showPopupWindow", data));
            }, this );
            showMenu.add( colormapButton );
            var histogramButton = new qx.ui.menu.Button( path.HISTOGRAM_PLUGIN );
            skel.widgets.TestID.addTestId( histogramButton, skel.widgets.TestID.HISTOGRAM_BUTTON);
            histogramButton.addListener( "execute", function(){
                var data = {
                    winId : this.m_identifier,
                    pluginId : path.HISTOGRAM_PLUGIN
                };
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "showPopupWindow", data));
            }, this );
            showMenu.add( histogramButton );
            return showMenu;
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
         * Returns whether or not this window can be linked to a window
         * displaying a named plug-in.
         * @param pluginId {String} a name identifying a plug-in.
         */
        isLinkable : function(pluginId) {
            var linkable = false;
            var path = skel.widgets.Path.getInstance();
            if (pluginId == path.ANIMATOR || pluginId == this.m_pluginId ||
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


        


        setDrawMode : function(drawInfo) {
            if (this.m_drawCanvas !== null) {
                this.m_drawCanvas.setDrawMode(drawInfo);
            }
        },
        
 

        /**
         * Implemented to initialize the context menu.
         */
        windowIdInitialized : function() {
            this.m_view = null;
            this._initDisplaySpecific();
            arguments.callee.base.apply(this, arguments);
            this._dataLoadedCB();
        },

        m_regionButton : null,
        m_renderButton : null,
        m_drawCanvas : null,
       
        m_view : null,
        m_dataButton : null,
        m_shapes : [ "Rectangle", "Ellipse", "Point", "Polygon" ]
    }

});
