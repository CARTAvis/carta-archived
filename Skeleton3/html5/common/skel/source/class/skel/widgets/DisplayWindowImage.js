/**
 * A display window specialized for viewing images.
 */

/*global mImport */
/**
 @ignore( mImport)
 ************************************************************************ */

qx.Class.define("skel.widgets.DisplayWindowImage", {
    extend : skel.widgets.DisplayWindow,

    /**
     * Constructor.
     */
    construct : function(row, col, index) {
        this.base(arguments, skel.widgets.Path.getInstance().CASA_LOADER, row, col, index );
        this.m_links = [];

    },

    members : {

        /**
         * Returns true if the link from the source window to the destination window was successfully added or removed; false otherwise.
         * @param sourceWinId {String} an identifier for the link source.
         * @param destWinId {String} an identifier for the link destination.
         * @param addLink {boolean} true if the link should be added; false if the link should be removed.
         */
        changeLink : function(sourceWinId, destWinId, addLink) {
            var linkChanged = false;
            if (destWinId == this.m_identifier) {
                var linkIndex = this.m_links.indexOf(sourceWinId);
                if (addLink && linkIndex < 0) {
                    linkChanged = true;
                    this.m_links.push(sourceWinId);
                    this._sendLinkCommand(sourceWinId, addLink);
                } else if (!addLink && linkIndex >= 0) {
                    this.m_links.splice(linkIndex, 1);
                    linkChanged = true;
                    this._sendLinkCommand(sourceWinId, addLink);
                }
            }
            return linkChanged;
        },

        /**
         * Call back that initializes the View when data is loaded.
         */
        _dataLoadedCB : function(){
            if (this.m_content.indexOf(this.m_title) >= 0) {
                this.m_content.remove(this.m_title);
                this.m_content.setLayout(new qx.ui.layout.Canvas());
            }
           
            
            
            if (this.m_view == null) {
                this.m_view = new skel.boundWidgets.View(this.m_identifier + "/view");
            }
            
            if ( this.m_drawCanvas == null ){
                this.m_drawCanvas = new skel.widgets.Draw.Canvas( this.m_identifier, this.m_view);
            }
            
            var overlayMap = {left:"0%",right:"0%",top:"0%",bottom: "0%"};
            if (this.m_content.indexOf(this.m_view) < 0) {
                this.m_content.add(this.m_view, overlayMap );
            }
            if ( this.m_content.indexOf( this.m_drawCanvas) < 0 ){
                this.m_content.add(this.m_drawCanvas, overlayMap);
            }
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
            this.m_content.add(this.m_title);
        },

        /**
         * Returns context menu items that should be displayed on the main
         * window when this window is selected.
         */
        getWindowSubMenu : function() {
            var windowMenuList = []

            var dataButton = new qx.ui.toolbar.MenuButton("Data");
            dataButton.setMenu(this._initDataMenu());

            var regionButton = new qx.ui.toolbar.MenuButton("Region");
            regionButton.setMenu(this._initMenuRegion());

            var renderButton = new qx.ui.toolbar.MenuButton("Render");
            renderButton.setMenu(this._initMenuRender());
            windowMenuList.push(dataButton);
            windowMenuList.push(regionButton);
            windowMenuList.push(renderButton);

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
            this.m_contextMenu.add(this.m_regionButton);

            this.m_renderButton = new qx.ui.menu.Button("Render");
            this.m_renderButton.setMenu(this._initMenuRender());
            this.m_contextMenu.add(this.m_renderButton);
        },

        /**
         * Initializes the region drawing context menu.
         */
        _initMenuRegion : function() {
            var regionMenu = new qx.ui.menu.Menu;
            this._initShapeButtons(regionMenu, false);

            var multiRegionButton = new qx.ui.menu.Button("Multi");
            regionMenu.add(multiRegionButton);
            var multiRegionMenu = new qx.ui.menu.Menu;
            this._initShapeButtons(multiRegionMenu, true);

            multiRegionButton.setMenu(multiRegionMenu);
            return regionMenu;
        },

        /**
         * Initializes the rendes context menu.
         */
        _initMenuRender : function() {
            var renderMenu = new qx.ui.menu.Menu;
            renderMenu.add(new qx.ui.menu.Button("Raster"));
            renderMenu.add(new qx.ui.menu.Button("Contour"));
            renderMenu.add(new qx.ui.menu.Button("Field"));
            renderMenu.add(new qx.ui.menu.Button("Vector"));
            return renderMenu;
        },

        /**
         * Initializes a menu button for drawing a shape such as a rectangle or ellipse.
         * @param menu {qx.ui.menu.Menu} the containing menu for the shape button.
         * @param keepMode {boolean} whether the cursor should stay in draw mode or revert
         * 		back when the shape is finished.
         */
        _initShapeButtons : function(menu, keepMode) {
            for (var i = 0; i < this.m_shapes.length; i++) {
                var shapeButton = new qx.ui.menu.Button(this.m_shapes[i]);
                shapeButton.addListener("execute", function(ev) {
                    var buttonText = this.getLabel();
                    var data = {
                        shape : buttonText,
                        multiShape : keepMode
                    };
                    qx.event.message.Bus.dispatch(new qx.event.message.Message(
                            "drawModeChanged", data));
                }, shapeButton);
                menu.add(shapeButton);
            }
        },
        
        /**
         * Returns whether or not this window can be linked to a window
         * displaying a named plug-in.
         * @param pluginId {String} a name identifying a plug-in.
         */
        isLinkable : function(pluginId) {
            var linkable = false

            if (pluginId == "animator" || pluginId == this.m_pluginId) {
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
         * Tells the server that this window would like to add or remove a link
         * from the window identified by the sourceWinId to this window.
         * @param sourceWinId {String} an identifier for the window that is the source of the link.
         * @param addLink {boolean} true if the link should be added; false if it should be removed.
         */
        _sendLinkCommand : function(sourceWinId, addLink) {
            //Send a command to link the source window (right now an animator) to us.
            var animId = sourceWinId;
            if (!addLink) {
                animId = "";
            }
            var paramMap = "winId:" + this.m_identifier + ",animId:" + animId;
            var pathDict = skel.widgets.Path.getInstance();
            var linkPath = pathDict.getCommandLinkAnimator();
            this.m_connector.sendCommand(linkPath, paramMap, function(val) {});
        },

        setDrawMode : function(drawInfo) {
            if (this.m_drawCanvas != null) {
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
