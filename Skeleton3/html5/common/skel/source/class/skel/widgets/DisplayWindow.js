/**
 * Base class for Windows displaying plugins.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.DisplayWindow", {
    extend : qx.ui.window.Window,
    /**
     * Constructor.
     * 
     * @param pluginId
     *                {String} the name of the plugin that will be displayed.
     * @param row
     *                {Number} a row in the screen grid.
     * @param col
     *                {Number} a column in the screeen grid.
     */
    construct : function(pluginId, row, col, winId ) {
        this.base(arguments);
        this.m_pluginId = pluginId;
        if ( winId && winId.length > 0 ){
            this.m_identifier = winId;
        }
        else {
            this.m_identifier = "win" + skel.widgets.DisplayWindow.windowCounter;
        }

        skel.widgets.DisplayWindow.windowCounter++;
        var paramMap = "pluginId:" + this.m_pluginId + ",winId:"
                + this.m_identifier;
        this.m_connector = mImport("connector");
        this.m_connector.sendCommand("registerView", paramMap, function() {
        });
        var id = this.addListener("appear", function() {
            var container = this.getContentElement().getDomElement();
            container.id = this.m_identifier;
            this.removeListenerById(id);
        }, this);

        this.m_row = row;
        this.m_col = col;

        this._init();

        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVarPlugin = this.m_connector
                .getSharedVar(pathDict.PLUGIN_LIST_STAMP);
        this.m_sharedVarPlugin.addCB(this._initViewMenuContext.bind(this));

    },

    events : {
        "maximizeWindow" : "qx.event.type.Data",
        "restoreWindow" : "qx.event.type.Data",
        "closeWindow" : "qx.event.type.Data"
    },

    statics : {
        // Used for generating unique window ids.
        windowCounter : 0,
        EXCLUDED : "Hidden"
    },

    members : {

        /**
         * Returns true if the link from the source window to the destination
         * window was successfully added or removed; false otherwise.
         * 
         * @param sourceWinId
         *                {String} an identifier for the link source.
         * @param destWinId
         *                {String} an identifier for the link destination.
         * @param addLink
         *                {boolean} true if the link should be added; false if
         *                the link should be removed.
         */
        changeLink : function(sourceWinId, destWinId, addLink) {
            return false;
        },

        /**
         * Closes the window
         */
        _close : function() {
            // Send signal to other windows that they can reclaim space.
            this.m_closed = true;
            this.fireDataEvent("closeWindow", this);
            
            //Save the name of
            var pathDict = skel.widgets.Path.getInstance();
            var basePath = pathDict.LAYOUT_PLUGIN;
            var pluginPath = basePath + pathDict.SEP + this.m_identifier;
            var pluginName = this.m_connector.getSharedVar( pluginPath).get();
            pluginName.set( skel.widgets.DisplayWindow.EXCLUDED );
        },

        /**
         * Implemented by subclasses that display particular types of data.
         */
        dataLoaded : function(path) {

        },

        /**
         * Implemented by subclasses that display particular types of data.
         */
        dataUnloaded : function(path) {

        },

        /**
         * Return the window title.
         */
        getIdentifier : function() {
            return this.m_identifier;
        },

        /**
         * Returns this window's information concerning establishing a link from
         * the window identified by the sourceWinId to this window.
         * 
         * @param pluginId
         *                {String} the name of the plug-in displayed by the
         *                source window.
         * @param sourceWinId
         *                {String} an identifier for the window displaying the
         *                plug-in that wants information about the links that
         *                can emanate frome it.
         */
        getLinkInfo : function(pluginId, sourceWinId) {
            var linkInfo = new skel.widgets.LinkInfo();
            if (this.m_identifier == sourceWinId) {
                linkInfo.source = true;
            }
            var midPoint = skel.widgets.Util.getCenter(this);
            linkInfo.locationX = midPoint[0];
            linkInfo.locationY = midPoint[1];
            if (this.m_links.indexOf(sourceWinId) >= 0) {
                linkInfo.linked = true;
            }

            linkInfo.winId = this.m_identifier;
            linkInfo.linkable = this.isLinkable(pluginId);
            linkInfo.twoWay = this.isTwoWay(pluginId);

            return linkInfo;

        },

        /**
         * Returns the name of the plug-in this window is displaying.
         */
        getPlugin : function() {
            return this.m_pluginId;
        },

        /**
         * Return specialized menu items for this window.
         */
        getWindowMenu : function() {
            var specializedMenu = this.getWindowSubMenu();
            return specializedMenu;
        },

        /**
         * Implemented by subclasses having context menu items that should be
         * displayed on the main menu when they are selected.
         */
        getWindowSubMenu : function() {
            return [];
        },

        /**
         * Initialize the GUI properties of this window.
         */
        _init : function() {
            // Make the window decorations invisible.
//            this.getChildControl("captionbar").setVisibility("excluded");
            this.setShowMinimize(false);
            this.setShowMaximize(false);
            this.setShowClose(false);
            this.setUseResizeFrame(false);
            this.setContentPadding(0, 0, 0, 0);
            this.setAllowGrowX(true);
            this.setAllowGrowY(true);
            this.setMovable(false);
            this.maximize();
            this.setCaption( "win"+Math.random());

            this.setLayout(new qx.ui.layout.VBox(0));
            this.m_scrollArea = new qx.ui.container.Scroll();
            this.m_content = new qx.ui.container.Composite();
            this.m_content.setLayout(new qx.ui.layout.VBox(0));
            // this.m_content.setLayout( new qx.ui.layout.Canvas());
            this.m_scrollArea.add(this.m_content);
            this.add(this.m_scrollArea, {
                flex : 1
            });
            this.m_contextMenu = new qx.ui.menu.Menu;

            this.addListener("mousedown", function(ev) {
                this.setSelected(true, ev.isCtrlPressed());
            });
        },

        /**
         * Initializes a generic window context menu.
         */
        _initContextMenu : function() {
            this.m_linkButton = new qx.ui.menu.Button("Links");
            this.m_linkButton.addListener("execute", function() {
                var linkData = {
                    "plugin" : this.m_pluginId,
                    "window" : this.m_identifier
                }
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "showLinks", linkData));
            }, this);
            this.m_contextMenu.add(this.m_linkButton);

            this.m_windwMenu = this._initWindowMenu();
            var windowButton = new qx.ui.menu.Button("Window");
            windowButton.setMenu(this.m_windowMenu);
            this.m_contextMenu.add(windowButton);

            this.m_pluginButton = new qx.ui.menu.Button("View");
            this.m_contextMenu.add(this.m_pluginButton);
            this._initViewMenuContext();

            this.setContextMenu(this.m_contextMenu);
        },

        /**
         * Initializes the 'data' context menu.
         */
        _initDataMenu : function() {
            var dataMenu = new qx.ui.menu.Menu();
            var openButton = new qx.ui.menu.Button("Open...");
            openButton.addListener("execute", function() {
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "showFileBrowser", this));
            }, this);
            dataMenu.add(openButton);

            var closeButton = new qx.ui.menu.Button("Close...");
            closeButton.addListener("execute", function() {
                this.dataUnloaded("bogusFilePath");
            }, this);
            dataMenu.add(closeButton);
            return dataMenu;
        },

        /**
         * Initialize standard window menu items.
         */
        _initWindowMenu : function() {
            this.m_windowMenu = new qx.ui.menu.Menu;
            this.m_minimizeButton = new qx.ui.menu.Button("Minimize");
            this.m_minimizeButton.addListener("execute", function() {
                this.hide();
            }, this);
            this.m_maximizeButton = new qx.ui.menu.Button("Maximize");
            this.m_maximizeButton.addListener("execute", function() {
                this._maximize();
            }, this);
            this.m_restoreButton = new qx.ui.menu.Button("Restore");
            this.m_restoreButton.addListener("execute", function() {
                this._restore();
            }, this);
            this.m_closeButton = new qx.ui.menu.Button("Close");
            this.m_closeButton.addListener("execute", function() {
                this._close();
            }, this);
            this.m_windowMenu.add(this.m_maximizeButton);
            this.m_windowMenu.add(this.m_minimizeButton);
            this.m_windowMenu.add(this.m_closeButton);
            return this.m_windowMenu;
        },

        /**
         * Initialize the submenu displaying alternative plug-ins to view in
         * this window.
         */
        _initViewMenuContext : function() {
            var pluginMenu = this._initViewMenu();
            this.m_pluginButton.setMenu(pluginMenu);
        },

        /**
         * Initialize the View menu displaying other plug-ins that have views
         * available.
         */
        _initViewMenu : function() {
            var pluginMenu = new qx.ui.menu.Menu;
            var val = this.m_sharedVarPlugin.get();
            var pluginCount = parseInt(val);
            var pathDict = skel.widgets.Path.getInstance();
            var base = pathDict.PLUGIN_LIST_NAME + pathDict.SEP;
            for (var i = 0; i < pluginCount; i++) {
                var pIndex = "p" + i;
                var pluginLookup = base + pIndex;
                var name = this.m_connector.getSharedVar(pluginLookup).get();

                var nameButton = new qx.ui.menu.Button(name);
                nameButton.row = this.m_row;
                nameButton.col = this.m_col;
                nameButton.addListener("execute", function() {
                    var pluginName = this.getLabel();
                    var data = {
                        row : this.row,
                        col : this.col,
                        plugin : pluginName
                    }
                    qx.event.message.Bus.dispatch(new qx.event.message.Message(
                            "setView", data));
                }, nameButton);
                pluginMenu.add(nameButton);
            }
            return pluginMenu;
        },

        /**
         * Returns whether or not this window is closed.
         */
        isClosed : function() {
            return this.m_closed;
        },

        /**
         * Returns whether or not this window can be linked to a window
         * displaying a named plug-in.
         * 
         * @param pluginId
         *                {String} a name identifying a plug-in.
         */
        isLinkable : function(pluginId) {
            return false;
        },

        /**
         * Returns whether or not this window supports establishing a two-way
         * link with the given plug-in.
         * 
         * @param pluginId
         *                {String} the name of a plug-in.
         */
        isTwoWay : function(pluginId) {
            return false;
        },

        /**
         * Maximizes the window
         */
        _maximize : function() {
            var maxIndex = this.m_windowMenu.indexOf(this.m_maximizeButton);
            if (maxIndex >= 0) {
                this.m_windowMenu.remove(this.m_maximizeButton);
                this.m_windowMenu.addAt(this.m_restoreButton, maxIndex);
            }
            this.fireDataEvent("maximizeWindow", this);
            this.maximize();
        },

        /**
         * Restores the window to its location in the main display.
         */
        _restore : function() {
            var restoreIndex = this.m_windowMenu.indexOf(this.m_restoreButton);
            var maxIndex = this.m_windowMenu.indexOf(this.m_maximizeButton);
            if (maxIndex == -1) {
                this.m_windowMenu.addAt(this.m_maximizeButton, restoreIndex);
            }
            if (restoreIndex >= 0) {
                this.m_windowMenu.remove(this.m_restoreButton);
            }
            this.open();
            this.m_closed = false;
            this.fireDataEvent("restoreWindow", this);
            this.restore();
        },

        setDrawMode : function(drawInfo) {

        },

        /**
         * Set the identifier for the plugin that will be displayed.
         * @param label {String} an identifier for the plugin.
         */
        setPlugin : function(label) {
            //Right now the pluginId is the title, but this will change.
            this.setTitle(label);
            this._initContextMenu();
            if ( label ){
                //Save the name of the plugin being displayed
                var pathDict = skel.widgets.Path.getInstance();
                var basePath = pathDict.LAYOUT_PLUGIN;
                var pluginPath = basePath + pathDict.SEP + this.m_identifier;
                var pluginName = this.m_connector.getSharedVar( pluginPath);
                pluginName.set( label );
            }
        },

        /**
         * Set the appearance of this window based on whether or not it is selected.
         * @param selected {boolean} true if the window is selected; false otherwise.
         * @param multiple {boolean} true if multiple windows can be selected; false otherwise.
         */
        setSelected : function(selected, multiple) {
            var console = mImport("console");
            console.log( "setSelected", selected, this.m_identifier);
            this.setActive( false);
            if( selected) {
//                this.addState( "winsel");
                this.getChildControl("captionbar" ).addState( "winsel");
            }
            else {
//                this.removeState( "winsel");
                this.getChildControl("captionbar" ).removeState( "winsel");
            }
//            this.syncAppearance();
//            this.getChildControl("captionbar" ).syncAppearance();

            if (selected &&  !multiple) {
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                    "windowSelected", this));
            }

//            if (selected) {
//                this.setAppearance("display-window-selected");
//                this.getChildControl("captionbar" ).setAppearance("winSel");
//                if (!multiple) {
//                    qx.event.message.Bus.dispatch(new qx.event.message.Message(
//                            "windowSelected", this));
//                }
//            } else {
//                this.setAppearance("display-window");
//                this.getChildControl("captionbar" ).setAppearance("winUnSel");
//            }
        },

        /**
         * Set an (optional) title for the window.
         * @param label {String} a title for the window.
         */
        setTitle : function(label) {
            this.setCaption( label);
            if (this.m_title == null) {
                this.m_title = new skel.boundWidgets.Label(label, "", "");
                this.m_content.add(this.m_title);
            }
            this.m_title.setValue(label);
        },

        m_closed : false,
        m_contextMenu : null,
        m_windowMenu : null,
        m_scrollArea : null,
        m_content : null,

        m_links : null,
        m_connector : null,

        //Identifies the plugin we are displaying.
        m_pluginId : "",
        m_pluginButton : null,
        m_sharedVarPlugin : null,
        m_identifier : "",
        //For now a display friendly title.
        m_title : null,
        m_row : 0,
        m_col : 0
    },

    properties : {
//        appearance : {
//            refine : true,
//            init : "display-window"
//        }

    }

});
