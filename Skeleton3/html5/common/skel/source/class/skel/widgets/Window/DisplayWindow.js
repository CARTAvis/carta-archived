/**
 * Base class for Windows displaying plugins.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 * @asset(skel/icons/swheel12.png)
 ******************************************************************************/

qx.Class.define("skel.widgets.Window.DisplayWindow", {
    extend : skel.widgets.Window.MoveResizeWindow,
  
    /**
     * Constructor.
     * 
     * @param pluginId {String} the name of the plugin that will be displayed.
     * @param row {Number} a row in the screen grid.
     * @param col {Number} a column in the screen grid.
     * @param index {Number} an identification index for the case where we have more than one window for a given pluginId;
     */
    construct : function(pluginId, row, col, index, detached ) {
        this.base(arguments, detached );
        this.m_pluginId = pluginId;
        
        this.m_row = row;
        this.m_col = col;
        var pathDict = skel.widgets.Path.getInstance();
        
        this._init();
        this._initWindowBar();
        
        this._initContextMenu();
        
        if ( this.m_pluginId && this.m_plugInd != pathDict.HIDDEN ){
            this.setTitle( this.m_pluginId );
        }
        
        //Get the shared variable that indicates the plugins that have been loaded so
        //we can display the view options in the context menu.
        this.m_connector = mImport("connector");
        
        var paramMap = "pluginId:" + pathDict.PLUGINS +",index:0";
        var regViewCmd = pathDict.getCommandRegisterView();
        this.m_connector.sendCommand( regViewCmd, paramMap, this._viewPluginsCB( this ) );
        if ( this.m_pluginId && this.m_plugInd != pathDict.HIDDEN ){
            var regNeeded = skel.widgets.Window.WindowFactory.isRegistrationNeeded( this.m_pluginId );
            if ( regNeeded ){
                this.initID( index );
            }
            var id = this.addListener("appear", function() {
                var container = this.getContentElement().getDomElement();
                if ( this.m_identifier !==""){
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
        "iconify" : "qx.event.type.Data",
        "maximizeWindow" : "qx.event.type.Data"
    },

    statics : {
        EXCLUDED : "Hidden"
    },

    members : {
        
        /**
         * Adds a button to the window's caption bar.
         * @param label {String} the text to display.
         * @param icon {String} path to the button's icon.
         */
        _addToolButton: function (label, icon) {
            icon = icon || null;
            var button = new qx.ui.form.Button( label, icon);
            button.setAllowGrowY(false);
            button.setFocusable(false);
            this.m_toolHolder.add(button);
            return button;
        },

        
        /**
         * Returns true if the link from the source window to the destination window was successfully added or removed; false otherwise.
         * @param sourceWinId {String} an identifier for the link source.
         * @param destWinId {String} an identifier for the link destination.
         * @param addLink {boolean} true if the link should be added; false if the link should be removed.
         */
        changeLink : function(sourceWinId, destWinId, addLink) {
            var linkChanged = false;
            if ( sourceWinId == this.m_identifier ){
                if ( !addLink ){
                    this.removeLink( sourceWinId, destWinId);
                }
            }
            else if (destWinId == this.m_identifier) {
                var linkIndex = this.m_links.indexOf(sourceWinId);
                if (addLink && linkIndex < 0) {
                    linkChanged = true;
                    this.m_links.push(sourceWinId);
                    this._sendLinkCommand(sourceWinId, addLink);
                } 
                else if (!addLink && linkIndex >= 0) {
                    this.m_links.splice(linkIndex, 1);
                    linkChanged = true;
                    this._sendLinkCommand(sourceWinId, addLink);
                }
            }
            return linkChanged;
        },
        
        /**
         * Window specific action to be taken when a link is removed.
         * @param sourceWinId {String} server side id of the source window.
         * @param destWinId {String} server side id of the destination window.
         */
        removeLink : function( sourceWinId, destWinId ){
            console.log( "Remove link not implemented for "+this.m_pluginId);
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
            var linkInfo = new skel.widgets.Link.LinkInfo();
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
            this.setShowMinimize(false);
            this.setShowMaximize(false);
            this.setShowClose(false);
            this.setUseResizeFrame(false);
            this.setContentPadding(0, 0, 0, 0);

            this.setAllowStretchX(true);
            this.setAllowStretchY(true);
            this.setMovable(false);
            this.maximize();
            //this.setCaption( "win"+Math.random());

            this.setLayout(new qx.ui.layout.VBox(0));
            this.m_scrollArea = new qx.ui.container.Scroll();
            this.m_content = new qx.ui.container.Composite();
            this.m_content.setLayout(new qx.ui.layout.VBox(0));
            // this.m_content.setLayout( new qx.ui.layout.Canvas());
            this.m_scrollArea.add(this.m_content);
            this.add(this.m_scrollArea, {
                flex : 1
            });
            this.m_contextMenu = new qx.ui.menu.Menu();
            this.m_contextMenu.addListener( "appear", this._contextMenuEvent, this);

            this.addListener("mousedown", function(ev) {
                this.setSelected(true, ev.isCtrlPressed());
            });
        },

        /**
         * Initializes a generic window context menu.
         */
        _initContextMenu : function() {
            if ( ! this.isDetached() ){
                this.m_linkButton = new qx.ui.menu.Button("Links");
                this.m_linkButton.addListener("execute", function() {
                    if ( this.m_identifier ){
                        var linkData = {
                                "plugin" : this.m_pluginId,
                                "window" : this.m_identifier
                        };
                        qx.event.message.Bus.dispatch(new qx.event.message.Message(
                            "showLinks", linkData));
                    }
                }, this);
                this.m_contextMenu.add(this.m_linkButton);
               
                this.m_windowMenu = this._initWindowMenu();
                var windowButton = new qx.ui.menu.Button("Window");
                windowButton.setMenu(this.m_windowMenu);
                this.m_contextMenu.add(windowButton);
                this.m_pluginButton = new qx.ui.menu.Button( "View");
                this.m_contextMenu.add(this.m_pluginButton);
            }
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
         * Initialize what will be displayed in the window's caption bar.
         */
        _initWindowBar : function(){
            var layout = new qx.ui.layout.HBox();
            var hbox = new qx.ui.container.Composite(layout);
           
            var infoLayout = new qx.ui.layout.HBox(3);
            infoLayout.setAlignY( "middle");
            this.m_infoHolder = new qx.ui.container.Composite( infoLayout);
            hbox.add( this.m_infoHolder, { flex:1});
            
            var toolLayout = new qx.ui.layout.HBox(3);
            toolLayout.setReversed(true);
            toolLayout.setAlignY("middle");
            this.m_toolHolder = new qx.ui.container.Composite(toolLayout);
            hbox.add(new qx.ui.core.Spacer(10, 1), { flex: 0});
            hbox.add(this.m_toolHolder, { flex: 0});
            
            this.getChildControl("captionbar").add(hbox, {row: 0, column: 1});
            
            //Add the settings button.
            this.m_settingsButton = this._addToolButton( null,  "skel/icons/swheel12.png" )
            .set( {
                show: "icon",
                toolTipText: "Settings..."
            } );
            this.m_settingsButton.addListener( "click", function () {
                this.toggleSettings();
            }, this );
        },


        /**
         * Initialize the View menu displaying other plug-ins that have views
         * available.
         */
        _initViewMenu : function() {
            var pluginMenu = new qx.ui.menu.Menu();
            var val = this.m_sharedVarPlugin.get();
            if ( val ){
                try {
                    var plugins = JSON.parse( val );
                    var buttonFunction = function( ){
                        var pluginName = this.getLabel();
                        var data = {
                            row : this.row,
                            col : this.col,
                            plugin : pluginName
                        };
                        qx.event.message.Bus.dispatch(new qx.event.message.Message( "setView", data));
                    };
                    
                    for (var i = 0; i < plugins.pluginCount; i++) {
                        var name = plugins.pluginList[i].name;
                        var errors = plugins.pluginList[i].loadErrors;
                        var loaded = (errors === "");
                        if ( loaded ){
                            var nameButton = new qx.ui.menu.Button(name);
                            nameButton.row = this.m_row;
                            nameButton.col = this.m_col;
                            nameButton.addListener("execute", buttonFunction, nameButton);
                            pluginMenu.add(nameButton);
                        }
                    }
                    
                    //So the user does not permanently lose the menu bar.
                    var showMenuCmd = skel.widgets.Command.CommandShowMenu.getInstance();
                    var label = showMenuCmd.getLabel();
                    var checkBox = new qx.ui.menu.CheckBox( label );
                    checkBox.setValue( showMenuCmd.getValue());

                    //Updates from the GUI to server
                    checkBox.addListener("execute", function() {
                        showMenuCmd.doAction( this.getValue(), null, null);
                    }, checkBox);
                   
                    //Updates from the server to GUI
                    showMenuCmd.addListener( "cmdValueChanged", function(evt){
                        var data = evt.getData();
                        if ( data.value !== this.getValue()){
                            this.setValue( data.value );
                        }
                    }, checkBox);
                    pluginMenu.add( checkBox );
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
                }
            }
            return pluginMenu;
        },
        
        /**
         * Initialize the submenu displaying alternative plug-ins to view in
         * this window.
         */
        _initViewMenuContext : function() {
            if ( ! this.isDetached() ){
                var pluginMenu = this._initViewMenu();
                this.m_pluginButton.setMenu(pluginMenu);
            }
        },
        /**
         * Initialize standard window menu items.
         */
        _initWindowMenu : function() {
            if ( ! this.isDetached() ){
                this.m_windowMenu = new qx.ui.menu.Menu();
                this.m_minimizeButton = new qx.ui.menu.Button("Minimize");
                this.m_minimizeButton.addListener("execute", function() {
                    this.fireDataEvent( "iconify", this );
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
                var closeButton = new qx.ui.menu.Button("Close");
                closeButton.addListener("execute", function() {
                    this.m_closed = true;
                    var removeWindowCmd = skel.widgets.Command.CommandWindowRemove.getInstance();
                    var data = {
                            row : this.m_row,
                            col : this.m_col
                    };
                    removeWindowCmd .doAction( data, null, null );
                }, this);
                var openButton = new qx.ui.menu.Button( "New");
                openButton.addListener( "execute", function(){
                    var data = {
                        row : this.m_row,
                        col : this.m_col
                    };
                    var newWindowCmd = skel.widgets.Command.CommandWindowAdd.getInstance();
                    newWindowCmd.doAction( data, null, null );
                }, this );
                this.m_windowMenu.add(this.m_maximizeButton);
                this.m_windowMenu.add(this.m_minimizeButton);
                this.m_windowMenu.add( closeButton);
                this.m_windowMenu.add( openButton );
            }
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
            }
            var restoreIndex = this.m_windowMenu.indexOf( this.m_restoreButton );
            if ( restoreIndex < 0 ){
                this.m_windowMenu.addAt(this.m_restoreButton, 0);
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
            };
        },
        
        /**
         * Restores the window to its location in the main display.
         */
        _restore : function() {
            var restoreIndex = this.m_windowMenu.indexOf(this.m_restoreButton);
            var maxIndex = this.m_windowMenu.indexOf(this.m_maximizeButton);
            if (maxIndex == -1) {
                this.m_windowMenu.addAt(this.m_maximizeButton, 0);
            }
            if (restoreIndex >= 0) {
                this.m_windowMenu.remove(this.m_restoreButton);
            }
            this.open();
            this.m_closed = false;
            this.restore();
        },
        
        /**
         * Tells the server that this window would like to add or remove a link
         * from the window identified by the sourceWinId to this window.
         * @param sourceWinId {String} an identifier for the window that is the source of the link.
         * @param addLink {boolean} true if the link should be added; false if it should be removed.
         */
        _sendLinkCommand : function(sourceWinId, addLink) {
            //Send a command to link the source window (right now an animator) to us.
            var linkCmd;
            if ( addLink ){
                linkCmd = skel.widgets.Command.CommandLinkAdd.getInstance();
            }
            else {
                linkCmd = skel.widgets.Command.CommandLinkRemove.getInstance();
            }
            linkCmd.link( sourceWinId, this.m_identifier );
        },
        
        /**
         * Callback for a data state change for this window.
         */
        _sharedVarCB : function( ){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var winObj = JSON.parse( val );
                    var i = 0;
                    //Update the new links for this window.
                    this.m_links = [];
                    if ( winObj.links && winObj.links.length > 0 ){
                        for ( i = 0; i < winObj.links.length; i++ ){
                            var destId = winObj.links[i];
                            var link = new skel.widgets.Link.Link( this.m_identifier, destId );
                            qx.event.message.Bus.dispatch(new qx.event.message.Message("addLink", link));
                        }
                    }
                   
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
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
            var console = mImport("console");
            this.setActive( false);
            if( selected) {
                this.getChildControl("captionbar" ).addState( "winsel");
            }
            else {
                this.getChildControl("captionbar" ).removeState( "winsel");
            }

            if (selected &&  !multiple) {
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                    "windowSelected", this));
            }

        },

        /**
         * Set an (optional) title for the window.
         * @param label {String} a title for the window.
         */
        setTitle : function(label) {
            if ( this.m_infoLabel === null ){
                this.m_infoLabel = new qx.ui.basic.Label( label );
                this.m_infoHolder.add( this.m_infoLabel );
            }
            else {
                this.m_infoLabel.setValue( label );
            }
        },
        
        /**
         * Called when the window's setting's button has been toggled; subclasses
         * should implement to show hide settings.
         */
        toggleSettings : function(){
            console.log( "Toggling settings not implemented");
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
            };
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
        
        //Toolbar
        m_toolHolder : null,
        m_infoHolder : null,
        m_settingsButton : null,
        m_infoLabel : null,
        

        m_links : null,


        //Identifies the plugin we are displaying.
        m_pluginId : "",
        m_pluginButton : null,
        
        //Connected variables 
        m_connector : null,
        m_sharedVarPlugin : null,
        m_sharedVar : null,
        
        //Server side object id
        m_identifier : "",
        m_row : 0,
        m_col : 0
    }

});
