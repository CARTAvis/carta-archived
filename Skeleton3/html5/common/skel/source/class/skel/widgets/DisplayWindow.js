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
     * @param pluginId {String} the name of the plugin that will be displayed.
     * @param row {Number} a row in the screen grid.
     * @param col {Number} a column in the screeen grid.
     * @param index {Number} an identification index for the case where we have more than one window for a given pluginId;
     */
    construct : function(pluginId, row, col, index ) {
        this.base(arguments);
        this.m_pluginId = pluginId;
        
        this.m_row = row;
        this.m_col = col;
        var pathDict = skel.widgets.Path.getInstance();
        
        this._init();
        this._initContextMenu();
        
        if ( this.m_pluginId && this.m_plugInd != pathDict.HIDDEN ){
            this.setTitle( this.m_pluginId );
        }
        
        //Get the shared variable that indicates the plugins that have been loaded so
        //we can display the view options in the context menu.
        this.m_connector = mImport("connector");
        var paramMap = "pluginId:" + pathDict.PLUGINS +",index:0"
        var regViewCmd = pathDict.getCommandRegisterView();
        this.m_connector.sendCommand( regViewCmd, paramMap, this._viewPluginsCB( this ) );
        
        if ( this.m_pluginId && this.m_plugInd != pathDict.HIDDEN ){
            this.initID( index );
            var id = this.addListener("appear", function() {
                var container = this.getContentElement().getDomElement();
                if ( this.m_identifier !=""){
                    container.id = this.m_identifier;
                }
                else {
                    container.id = this.m_pluginId;
                    this.m_identifier = this.m_pluginId;
                }
                this.removeListenerById(id);
            }, this);
        }
        else {
            console.log( "Not initializing m_pluginId="+this.m_pluginId );
        }
    },

    events : {
        "maximizeWindow" : "qx.event.type.Data",
        "restoreWindow" : "qx.event.type.Data",
        "closeWindow" : "qx.event.type.Data"
    },

    statics : {
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
            this.getChildControl("captionbar").setVisibility("excluded");
            this.setShowMinimize(false);
            this.setShowMaximize(false);
            this.setShowClose(false);
            this.setUseResizeFrame(false);
            this.setContentPadding(0, 0, 0, 0);
            this.setAllowGrowX(true);
            this.setAllowGrowY(true);

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
                if ( this.m_identifier ){
                    var linkData = {
                            "plugin" : this.m_pluginId,
                            "window" : this.m_identifier
                    }
                    qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "showLinks", linkData));
                }
            }, this);
            this.m_contextMenu.add(this.m_linkButton);
           
            this.m_windwMenu = this._initWindowMenu();
            var windowButton = new qx.ui.menu.Button("Window");
            windowButton.setMenu(this.m_windowMenu);
            this.m_contextMenu.add(windowButton);
            
            this.m_pluginButton = new qx.ui.menu.Button("View");
            this.m_contextMenu.add(this.m_pluginButton);
            //this._initViewMenuContext();
            
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
         * Sends a command to the server to get the unique object id (identifier)
         * for this window.
         * @param index {Number} used when there is more than one window displaying
         *      the same plugin.
         */
        initID : function( index ){
          //Get the id of this window.
          var paramMap = "pluginId:" + this.m_pluginId + ",index:"+index;
          var pathDict = skel.widgets.Path.getInstance();
          var regCmd = pathDict.getCommandRegisterView();
          this.m_connector.sendCommand( regCmd, paramMap, this._registrationCallback(this));
        },
        
        /**
         * Initialize the shared variable that represents the state of this window.
         */
        _initSharedVar : function(){
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_identifier );
            this.m_sharedVar.addCB( this._sharedVarCB.bind( this ));
            this._sharedVarCB( this.m_sharedVar.get());
        },


        /**
         * Initialize the View menu displaying other plug-ins that have views
         * available.
         */
        _initViewMenu : function() {
            var pluginMenu = new qx.ui.menu.Menu;
            var val = this.m_sharedVarPlugin.get();
            var plugins = JSON.parse( val );
            for (var i = 0; i < plugins.pluginCount; i++) {
                var name = plugins.pluginList[i].name;
                var errors = plugins.pluginList[i].loadErrors;
                var loaded = (errors === "");
                if ( loaded ){
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
                        qx.event.message.Bus.dispatch(new qx.event.message.Message( "setView", data));
                    }, nameButton);
                    pluginMenu.add(nameButton);
                }
            }
            return pluginMenu;
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
         * Returns whether or not this window is closed.
         */
        isClosed : function() {
            return this.m_closed;
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
         * Callback for when the id of the object containing information about the
         * C++ object has been received; initialize the shared variable and add a CB to it.
         * @param anObject {DisplayWindow}.
         */
        _registrationCallback : function( anObject ){
            return function( id ){
                if ( id && id.length > 0 ){
                    if ( id != anObject.m_identifier ){
                        anObject.m_identifier = id;
                        anObject._initSharedVar();
                        anObject.windowIdInitialized();
                    }
                }
            }
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
        
        /**
         * Callback for a data state change for this window.
         */
        _sharedVarCB : function( ){
            var val = this.m_sharedVar.get();
            if ( val ){
                var winObj = JSON.parse( this.m_sharedVar.get() );
                //Update the links for this window if they exist.
                if ( winObj.links && winObj.links.length > 0 ){
                    qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "clearLinks", this.m_identifier));
                    for ( var i = 0; i < winObj.links.length; i++ ){
                        var destId = winObj.links[i];
                        var link = new skel.widgets.Link( this.m_identifier, destId );
                        qx.event.message.Bus.dispatch(new qx.event.message.Message("addLink", link));
                    }
                }
            }
        },



        setDrawMode : function(drawInfo) {

        },

       
        
        

        /**
         * Set the appearance of this window based on whether or not it is selected.
         * @param selected {boolean} true if the window is selected; false otherwise.
         * @param multiple {boolean} true if multiple windows can be selected; false otherwise.
         */
        setSelected : function(selected, multiple) {
            if (selected) {
                this.setAppearance("display-window-selected");
                if (!multiple) {
                    qx.event.message.Bus.dispatch(new qx.event.message.Message(
                            "windowSelected", this));
                }
            } else {
                this.setAppearance("display-window");
                qx.event.message.Bus.dispatch( new qx.event.message.Message( "windowUnselected", this ));
            }
        },

        /**
         * Set an (optional) title for the window.
         * @param label {String} a title for the window.
         */
        setTitle : function(label) {
            if (this.m_title == null) {
                this.m_title = new skel.boundWidgets.Label(label, "", "");
                this.m_content.add(this.m_title);
            }
            this.m_title.setValue(label);
        },
        
        /**
         * Callback for when the shared variable that represents loaded plugins changes;
         * updates the view menu.
         */
        _viewPluginsCB : function( anObject ){
            return function( id ){
                anObject.m_sharedVarPlugin = anObject.m_connector.getSharedVar(id);
                anObject.m_sharedVarPlugin.addCB(anObject._initViewMenuContext.bind(anObject));
                anObject._initViewMenuContext();
            }
        },
        
        /**
         * Place holder for subclasses to override for code to be executed once the shared
         * variable has been initialized.
         */
        windowIdInitialized : function(){
        },

        m_closed : false,
        m_contextMenu : null,
        m_windowMenu : null,
        m_scrollArea : null,
        m_content : null,

        m_links : null,


        //Identifies the plugin we are displaying.
        m_pluginId : "",
        m_pluginButton : null,
        
        //Connected variables 
        m_connector : null,
        m_sharedVarPlugin : null,
        m_sharedVar : null,
        
        m_identifier : "",
        //For now a display friendly title.
        m_title : null,
        m_row : 0,
        m_col : 0
    },

    properties : {
        appearance : {
            refine : true,
            init : "display-window"
        }

    }

});
