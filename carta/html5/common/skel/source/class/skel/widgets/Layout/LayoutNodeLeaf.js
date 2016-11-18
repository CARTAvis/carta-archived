/**
 * Manages a single layout cell for displaying a plugin.
 */

/*******************************************************************************
 * 
 ******************************************************************************/

qx.Class.define("skel.widgets.Layout.LayoutNodeLeaf",{
    extend : skel.widgets.Layout.LayoutNode,

    /**
     * Constructor.
     */
    construct : function(id ) {
        this.m_desktop = new qx.ui.window.Desktop();
        this.base(arguments, id);
        
        this.m_desktop.set({
            decorator : "desktop"
        });
        this.m_desktop.addListener("resize", this._resetWindowSize, this);
    },

    events : {
        "iconifyWindow" : "qx.event.type.Data",
        "findChild" : "qx.event.type.Data",
        "leafResize" : "qx.event.type.Data",
        "iconify" : "qx.event.type.Data"
    },

    members : {
        
       

        /**
         * Adds listeners.
         */
        _addWindowListeners : function(){
            this.m_iconifyListenerId = this.m_window.addListener("iconify", function() {
                if (this.m_window !== null && !this.m_window.isClosed()) {
                    var data = {
                        id : this.m_id,
                        title : this.m_window.getPlugin()
                    };
                    this.fireDataEvent("iconifyWindow", data);
                }
            }, this);
           this.m_maxListenerId = this.m_window.addListener("windowMaximized",
               function() {
                   var appRoot = this.m_desktop.getApplicationRoot();
                   appRoot.add(this.m_window);
               }, this);
           this.m_closeListenerId = this.m_window.addListener( "closeWindow",
               function(){
                   var appRoot = this.m_desktop.getApplicationRoot();
                   if ( appRoot.indexOf( this.m_window) >= 0 ){
                       appRoot.remove( this.m_window );
                   }
           }, this );
           this.m_restoreListenerId = this.m_window.addListener( "windowRestored",
               function(){
                   this.restoreWindow( this.m_id );
               }, this );
        },
        
        /**
         * Returns true if the link from the source window to
         * the destination window was successfully added or
         * removed; false otherwise.
         * 
         * @param sourceWinId {String} an identifier for the link
         *                source.
         * @param destWinId {String} an identifier for the link
         *                destination.
         * @param addLink {boolean} true if the link should be
         *                added; false if the link should be
         *                removed.
         * @return {boolean} true if the link status changed; false otherwise.
         */
        changeLink : function(sourceWinId, destWinId, addLink) {
            var linkSet = false;
            if (this.m_window !== null) {
                linkSet = this.m_window.changeLink(sourceWinId,
                        destWinId, addLink);
            }
            return linkSet;
        },
        
        /**
         * Loads the data.
         * 
         * @param path {String} the location or lookup for the data.
         */
        dataLoaded : function(path) {
            if (this.m_window !== null) {
                this.m_window.dataLoaded(path);
            }
        },

        /**
         * Removes the data.
         * 
         * @param path {String} the location or identifier for the data.
         */
        dataUnloaded : function(path) {
            if (this.m_window !== null) {
                this.m_window.dataUnloaded(path);
            }
        },
        
        exclude : function(){
            this.m_desktop.exclude();
        },
        
        /**
         * Returns an empty list as windows cannot be added in
         * the middle of a desktop.
         * @return {Array} an empty array.
         */
        getAddWindowLocations : function() {
            return [];
        },
        
        /**
         * Returns the width and height.
         * @return {Array} containing [width,height] information.
         */
        _getDimensions : function() {
            var dims = [ this.getWidth(), this.getHeight() ];
            return dims;
        },

        /**
         * Returns the view object associated with this leaf.
         * @return {qx.ui.window.Desktop}.
         */
        getDisplayArea : function() {
            return this.m_desktop;
        },
        
        /**
         * Returns the row and column of its managed window.
         * @return {Array} containing [row,column] location.
         */
        _getLastIndices : function() {
            var indices = [ this.m_id ];
            return indices;
        },
        
        /**
         * Returns a list of information concerning windows that
         * can be linked to the given source window showing the
         * indicated plug-in.
         * @param pluginId {String} the name of the plug-in.
         * @param sourceWinId {String} an identifier for the window
         *                displaying the plug-in that wants
         *                information about the links that can
         *                originate from it.
         * @param linkInfos {Array} - list of information about windows that are currently
         *      linked to the source or have potential to be linked to the source.
         */
        getLinkInfo : function(pluginId, sourceWinId, linkInfos ) {
            if (this.m_window !== null) {
                this.m_window.getLinkInfo( pluginId, sourceWinId, linkInfos );
            }
        },
        
        /**
         * Returns a list of information concerning windows that
         * can be linked to the given source window showing the
         * indicated plug-in.
         * @param sourceWinId {String} an identifier for the window
         *                that wants information about relocation positions.
         * @return {String} information about location information that can be established
         *      from the given plug-in and window.
         */
        getMoveInfo : function( sourceWinId) {
            var moveInfo = [];
            if (this.m_window !== null) {
                moveInfo.push(this.m_window.getMoveInfo(sourceWinId ));
            }
            return moveInfo;
        },
        
        /**
         * Returns an array containing the window displayed by this desktop
         * or an empty array if no window is being displayed.
         * @return {Array} an array containing zero or one windows displayed.
         */
        getWindows : function(){
            var winArray = [];
            if ( this.m_window !== null ){
                winArray.push( this.m_window);
            }
            return winArray;
        },
        
        /**
         * Returns this desktop window if the location matches; otherwise return null.
         * @param locationId {String} an identifier for the layout location..
         * @return {skel.widgets.Window.DisplayWindow} window managed by this desktop or
         *      null if the desktop is not located in the specified grid row and column.
         */
        getWindow : function( locationId ){
            var win = null;
            if ( this.m_id == locationId ){
                win = this.m_window;
            }
            return win;
        },

        
        /**
         * Factory method for making window specialized to
         * correct type.
         * 
         * @param pluginId {String} an identifier for the type of
         *                plugin the window will manage.
         */
        _makeWindow : function(pluginId) {
            if ( pluginId !== "Hidden"){
                this.m_window = skel.widgets.Window.WindowFactory.makeWindow( pluginId, this.m_index,  false );
                this._addWindowListeners();
            }
            else {
                this.exclude();
            }
        },
        
        
        /**
         * Remove all DisplayWindows.
         */
        _removeWindow : function() {
            if (this.m_window !== null) {
                this.m_window.removeListenerById( this.m_iconifyListenerId );
                this.m_window.removeListenerById( this.m_maxListenerId );
                this.m_window.removeListenerById( this.m_restoreListenerId );
                this.m_window.removeListenerById( this.m_closeListenerId );
                this.m_desktop.removeAll();
            }
        },
        
        /**
         * Reset the size of the contained window based on the
         * size of this Desktop.
         */
        _resetWindowSize : function() {
            var bounds = this.m_desktop.getBounds();
            if (bounds !== null && this.m_window !== null ) {
                this.m_window.setWidth(bounds.width );
                this.m_window.setHeight(bounds.height);
                this.fireDataEvent("resizeNode", null);
                this.sendSizeCmd( bounds.width, bounds.height );
            }
        },
        
        /**
         * Returns whether or not the window was restored based
         * on whether or not the location matches this one.
         * @param locationId {String} an identifier for the location of the window to be restored.
         * @return {boolean} true if the window was restored; false otherwise.
         */
        restoreWindow : function( locationId ) {
            var restored = false;
            if (this.m_window !== null ) {
                if ( this.m_id == locationId ){
                    restored = true;
                    var appRoot = this.m_desktop.getApplicationRoot();
                    if (appRoot.indexOf(this.m_window) != -1) {
                        appRoot.remove(this.m_window);
                        this.m_desktop.add(this.m_window);
                        this.m_window.restoreWindow( this.m_id );
                    }
                    this.m_window.open();
                    this.m_desktop.show();
                }
            }
            return restored;
        },
        
        /**
         * Update of layout leaf information from server.
         * @param obj {Object} information about the layout from the server.
         */
        serverUpdate : function( obj ){
            this.m_index = obj.index;
            this._setView( obj.plugin );
        },
        
        setDrawMode : function(drawInfo) {
            if (this.m_window !== null) {
                this.m_window.setDrawMode(drawInfo);
            }
        },

        /**
         * Returns whether or not a different plug-in was
         * reassigned to this DisplayArea based on whether its
         * location matches the rowIndex and colIndex passed in.
         * 
         * @param pluginId {String} a new plug-in identifier.
         * @return {boolean} true if the plugin was reassigned; false otherwise.
         */
        _setView : function(pluginId ) {
          
            if ( pluginId === null || pluginId === undefined || pluginId.length === 0 ){
                return false;
            }
            var existingWindow = false;
            
            if (this.m_window !== null ) {
                
                //Not the same plugin or the window has beeen removed because it is being used
                //elsewhere so we will remake the window.
                if( this.m_window.getPlugin() != pluginId) {
                    this._removeWindow();
                    //this.m_window = null;
                    this._makeWindow( pluginId);
                }
                else {
                    existingWindow = true;
                }
            }
            else {
                this._makeWindow(pluginId);
            }
       
            if ( this.m_window !== null ){
                this.m_desktop.add(this.m_window);
                this.m_window.setLocation( this.m_id );
                this.m_window.open();
                if ( existingWindow ){
                    this.m_window.initID( this.m_index );
                }
                //In case the window is excluded
                this.m_desktop.show();
                this._resetWindowSize();
            }
            return true;
        },
        

        /**
         * Resets selected status.
         * 
         * @param win
         *                {DisplayWindow} the display window
         *                that has been selected.
         */
        windowSelected : function(win) {
            if (this.m_window !== null && this.m_window !== win) {
                this.m_window.setSelected(false, false);
            }
        },

        m_desktop : null,
        m_window : null,
        m_index : 0,
        m_iconifyListenerId : null,
        m_maxListenerId : null,
        m_restoreListenerId : null,
        m_closeListenerId : null
    }

});
