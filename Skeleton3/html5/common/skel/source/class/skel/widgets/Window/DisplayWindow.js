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
     * @param index {Number} an identification index for the case where we have more than one window for a given pluginId;
     * @param detached {boolean} - true if this will not be a layout inline window; false otherwise.
     */
    construct : function(pluginId, index, detached ) {
        this.base(arguments, detached );
        this.m_pluginId = pluginId;
        this.m_supportedCmds = [];
        
        var pathDict = skel.widgets.Path.getInstance();
        this._init();
        if ( !detached ){
            this.getChildControl( "captionbar").setVisibility( "excluded");
        }
        
        this._initSupportedCommands();
        this._initContextMenu();
        
        //Get the shared variable that indicates the plugins that have been loaded so
        //we can display the view options in the context menu.
        this.m_connector = mImport("connector");
        
        if ( this.m_pluginId && this.m_plugInd != pathDict.HIDDEN ){
            var regNeeded = skel.widgets.Window.WindowFactory.isRegistrationNeeded( this.m_pluginId );
            if ( regNeeded ){
                this.initID( index );
            }
            skel.widgets.TestID.addTestId( this, this.m_pluginId );
        }
        else {
            console.log( "Not initializing m_pluginId="+this.m_pluginId );
        }
    },

    events : {
        "iconify" : "qx.event.type.Data",
        "maximizeWindow" : "qx.event.type.Data",
        "closeWindow" : "qx.event.type.Data",
        "restoreWindow" : "qx.event.type,Data",
        "registered" : "qx.event.type.Data"
    },

    statics : {
        EXCLUDED : "Hidden",
        EMPTY : "Empty"
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
         * Clean-up items; this window is going to disappear.
         */
        clean : function(){
            //console.log( "No cleaning on this window");
        },
        
        /**
         * Remove the link with the given identifier from the list of links.
         * @param winId {String} a unique identifier for the link to remove.
         */
        clearLink : function( winId ){
            var linkIndex = this.m_links.indexOf(winId);
            if ( linkIndex >= 0 ){
                this.m_links.splice(linkIndex);
            }
        },
        
        /**
         * Send notification that this window should be closed.
         */
        closeWindow : function(){
            this.m_closed = true;
            this.fireDataEvent( "closeWindow", "");
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
         * Overriden by subclasses to return a list of data that can be closed.
         * @return {Array} a list of data that could be closed.
         */
        getCloses : function(){
            return [];
        },

        /**
         * Return the unique server-side identifier for this window.
         * @return {String} the server-side identifier.
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
         *                can emanate from it.
         */
        getLinkInfo : function(pluginId, sourceWinId) {
            var linkInfo = new skel.widgets.Link.LinkInfo();
            if (this.m_identifier == sourceWinId) {
                linkInfo.source = true;
            }
           
            var midPoint = skel.widgets.Util.getCenter(this);
            linkInfo.locationX = midPoint[0];
            linkInfo.locationY = midPoint[1];
            var index = this.m_links.indexOf( sourceWinId );
            if ( index >= 0) {
               
                linkInfo.linked = true;
            }

            linkInfo.winId = this.m_identifier;
            linkInfo.linkable = this.isLinkable(pluginId);
            linkInfo.twoWay = this.isTwoWay(pluginId);

            return linkInfo;
        },

        /**
         * Returns this window's information concerning using it as a possible
         * move location.
         * 
         * @param sourceWinId  {String} an identifier for the window wanting to move to this
         *      window's location.
         */
        getMoveInfo : function( sourceWinId) {
            
            var linkInfo = new skel.widgets.Link.LinkInfo();
            if (this.m_identifier == sourceWinId) {
                linkInfo.source = true;
                //Because we are potentially going to move this window, we need to store it in
                //the window factory so the new destination will be able to reuse it and not make
                //another one.
                var wins = [];
                wins[0] = this;
                skel.widgets.Window.WindowFactory.setExistingWindows( wins );
            }
           
            var midPoint = skel.widgets.Util.getCenter(this);
            linkInfo.locationX = midPoint[0];
            linkInfo.locationY = midPoint[1];
            linkInfo.linked = false;
            linkInfo.winId = this.m_identifier;
            linkInfo.pluginId = this.getPlugin();
            linkInfo.locationId = this.getLocation();
            linkInfo.linkable = true;
            linkInfo.twoWay = false;
            return linkInfo;
        },
        
        getLocation : function(){
            return this.m_locationId;
        },
        
        /**
         * Returns the name of the plug-in this window is displaying.
         */
        getPlugin : function() {
            return this.m_pluginId;
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

            this.setLayout(new qx.ui.layout.VBox(0));
            this.m_scrollArea = new qx.ui.container.Scroll();
            this.m_content = new qx.ui.container.Composite();
            this.m_content.setLayout(new qx.ui.layout.VBox(0));
            this.m_scrollArea.add(this.m_content);
            this.add(this.m_scrollArea, {
                flex : 1
            });
            this.m_contextMenu = new qx.ui.menu.Menu();
            this.m_contextMenu.addListener( "appear", this._contextMenuEvent, this);
            this.setContextMenu(this.m_contextMenu);
            this.addListener("mousedown", function(ev) {
                this.setSelected(true, ev.isCtrlPressed());
            });
        },
        
        
        /**
         * Initializes a generic window context menu.
         */
        _initContextMenu : function() {
            if ( ! this.isDetached() ){
                this.m_contextMenu.removeAll();
                var cmds = skel.Command.CommandAll.getInstance();
                var vals = cmds.getValue();
                var emptyFunc = function(){};
                for ( var i = 0; i < vals.length; i++ ){
                    var cmdType = vals[i].getType();
                    if ( cmdType === skel.Command.Command.TYPE_COMPOSITE  || 
                            cmdType === skel.Command.Command.TYPE_GROUP ){
                        //Only add top-level commands specific to this window.
                        var supported = this.isCmdSupported( vals[i] );
                        if ( supported ){
                            var menu = skel.widgets.Util.makeMenu( vals[i]);
                            var menuButton = new qx.ui.menu.Button( vals[i].getLabel() );
                            this.m_contextMenu.add( menuButton );
                            menuButton.setMenu( menu);
                        }
                    }
                    else if ( cmdType === skel.Command.Command.TYPE_BUTTON ){
                        var button = skel.widgets.Util.makeButton( vals[i], emptyFunc, false, true );
                        this.m_contextMenu.add( button );
                    }
                    else {
                        console.log( "Menu unsupported top level command type="+ cmdType );
                    }
                }
            }
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
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVarLink = this.m_connector.getSharedVar( this.m_identifier + path.SEP + "links");
            this.m_sharedVarLink.addCB( this._sharedVarLinkCB.bind( this));
            this._sharedVarLinkCB( this.m_sharedVarLink.get());
        },
        
        /**
         * Initialize the list of commands supported by a generic window.
         */
        _initSupportedCommands : function(){
            var windowCmd = skel.Command.Window.CommandWindow.getInstance();
            this.m_supportedCmds.push( windowCmd.getLabel() );
            var viewsCmd = skel.Command.View.CommandViews.getInstance();
            this.m_supportedCmds.push( viewsCmd.getLabel() );
            var linksCmd = skel.Command.Link.CommandLink.getInstance();
            this.m_supportedCmds.push( linksCmd.getLabel() );
        },
        
        
        /**
         * Returns true if this window supports the passed in command.
         * @param cmd {String} an identifier for a command.
         * @return {boolean} true if the command is appropriate for this window; false otherwise.
         */
        isCmdSupported : function(cmd){
            var supported = false;
            if ( this.m_supportedCmds !== null ){
                var cmdName = cmd.getLabel();
                for ( var i = 0; i < this.m_supportedCmds.length; i++ ){
                    if ( this.m_supportedCmds[i] == cmdName ){
                        supported = true;
                        break;
                    }
                }
            }
            return supported;
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
         * Callback for removing a link that was incorrectly established.
         * @param anObject {skel.widgets.Window.DisplayWindow} this window.
         * @param sourceWinId {String} an identifier for the link source window.
         */
        //Written for the case of the histogram, which currently supports linking to
        //only one controller.  If the user draws in a link to a second controller,
        //we remove it and post an error message.
        _linkUndoCmd : function( anObject, sourceWinId ){
            return function( msg ){
                if ( msg !== null && msg.length > 0 ){
                    var linkIndex = anObject.m_links.indexOf( sourceWinId );
                    if ( linkIndex >= 0 ){
                        anObject.m_links.splice(linkIndex);
                        var linkCanvas = skel.widgets.Link.LinkCanvas.getInstance();
                        linkCanvas.removeLink( sourceWinId, anObject.m_identifier );
                    }
                }
            };
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
                        anObject.fireDataEvent( "registered", "" );
                    }
                }
            };
        },
        
        /**
         * Window specific action to be taken when a link is removed.
         * @param sourceWinId {String} server side id of the source window.
         * @param destWinId {String} server side id of the destination window.
         */
        removeLink : function( sourceWinId, destWinId ){
            //console.log( "Remove link not implemented for "+this.m_pluginId);
        },
        
        /**
         * Restores the window to its location in the main display.
         */
        restoreWindow : function() {
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
            //Send a command to link the source window this window.
            var linkCmd;
            var linkUndo = null;
            if ( addLink ){
                linkCmd = skel.Command.Link.CommandLinkAdd.getInstance();
                linkUndo = this._linkUndoCmd( this, sourceWinId );
            }
            else {
                linkCmd = skel.Command.Link.CommandLinkRemove.getInstance();
            }
            linkCmd.link( sourceWinId, this.m_identifier, linkUndo );
        },
        
        /**
         * Show the link window having this window as a source.
         */
        _showLinkWindow : function(){
            if ( this.m_identifier ){
                var linkData = {
                        "plugin" : this.m_pluginId,
                        "window" : this.m_identifier
                };
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                    "showLinks", linkData));
            }
        },
        
        
        /**
         * Callback for a state change for this window.
         */
        _sharedVarCB : function( ){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var winObj = JSON.parse( val );
                    this.windowSharedVarUpdate( winObj );
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
                }
            }
        },
        
        /**
         * Callback for a link state change for this window.
         */
        _sharedVarLinkCB : function( ){
            var val = this.m_sharedVarLink.get();
            if ( val ){
                try {
                    var winObj = JSON.parse( val );
                    var i = 0;
                    //Update the new links for this window.
                    var data = {
                       link: this.m_identifier
                    };
                    qx.event.message.Bus.dispatch( new qx.event.message.Message("clearLinks", data));
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
         * Update the location of this window.
         * @param row {Number} the grid row index.
         * @param col {Number} the grid column index.
         */
        setLocation : function ( locationId ){
            this.m_locationId = locationId;
        },

        /**
         * Set the appearance of this window based on whether or not it is selected.
         * @param selected {boolean} true if the window is selected; false otherwise.
         * @param multiple {boolean} true if multiple windows can be selected; false otherwise.
         */
        setSelected : function(selected, multiple) {
            if ( selected != this.isActive()){
                this.setActive( selected );
                if( selected) {
                    this.getChildControl("pane").addState( "winSel" );
                }
                else {
                	this.getChildControl("pane").removeState( "winSel");
                }
                
                //Notify window has been selected.
                if ( !multiple && selected ) {
                    qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "windowSelected", this));
                }
                
                //Reset the context menu based on functionality specific to this window.
                if ( selected ){
                    if ( !multiple ){
                        skel.Command.Command.clearActiveWindows();
                    }
                    skel.Command.Command.addActiveWindow( this );
                    this._initContextMenu();
                }
                else {
                    skel.Command.Command.clearActiveWindows();
                }
            }
        },
        
        /**
         * Place holder for subclasses to override for code to be executed once the shared
         * variable has been initialized.
         */
        windowIdInitialized : function(){
        },
        
        /**
         * Update window specific elements from the shared variable.
         * @param winObj {String} represents the server state of this window.
         */
        windowSharedVarUpdate : function( winObj ){
        },

        m_closed : false,
        m_contextMenu : null,
        m_scrollArea : null,
        m_content : null,
        
        m_links : null,
        m_supportedCmds : null,

        //Identifies the plugin we are displaying.
        m_pluginId : "",
        
        //Connected variables 
        m_connector : null,
        m_sharedVar : null,
        m_sharedVarLink : null,
        
        //Server side object id
        m_identifier : "",
        m_locationId : ""
    }

});
