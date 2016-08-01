/**
 * Base class for layout cells that can either contain other layout cells or
 * contain a window displaying a plug-in.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Layout.LayoutNode",{
    extend : qx.core.Object,

    
    construct : function(id) {
        this.base(arguments);
        this.m_id = id;
        this.m_connector = mImport("connector");
    },

    events : {
        "iconifyWindow" : "qx.event.type.Data",
        "resizeNode" : "qx.event.type.Data"
    },

    members : {

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
         */
        changeLink : function(sourceWinId, destWinId, addLink) {
           
        },

        /**
         * Notifies children that data has been loaded.
         * @param path {String} an identifier for the data to load.
         */
        dataLoaded : function(path) {
        },

        /**
         * Notifies children that data has been unloaded.
         * @param path {String} an identifier for the data to remove.
         */
        dataUnloaded : function(path) {
        },
        
        exclude : function(){},
        
        /**
         * Returns the identifier for this layout cell.
         * @return {String} an identifier for the layout cell.
         */
        getId : function(){
            return this.m_id;
        },

        
        /**
         * Returns a location identifier for the second area.
         */
        _getLastIndices : function() {
            return this.m_areaSecond._getLastIndices();
        },

        /**
         * Returns the approximate width and height occupied by
         * both sides of the split pane.
         * @return {Array} containing the [width,height] of the pane.
         */
        _getDimensions : function() {
            return [];
        },
        

        /**
         * Returns the display area.
         * @return {null} the display area is subclass dependent.
         */
        getDisplayArea : function() {
            return null;
        },
        

        /**
         * Returns a list of information concerning windows that can be linked to
         * the given source window showing the indicated plug-in.
         * @param pluginId {String} the name of the plug-in.
         * @param sourceWinId {String} an identifier for the window displaying the
         *      plug-in that wants information about the links that can originate from it.
         * @param linkInfos {Array} - list of information about windows that are currently
         *      linked to the source or have potential to be linked to the source.
         */
        getLinkInfo : function(pluginId, sourceWinId, linkInfos) {
        },
        
        
        /**
         * Returns a list of information concerning windows that can be replaced by 
         * the given source window showing the indicated plug-in.
         * @param sourceWinId {String} an identifier for the window displaying the
         *      plug-in that wants information about where it can be moved.
         * @return {String} information about the locations where the source window
         *      can be moved.
         */
        getMoveInfo : function(sourceWinId) {
            return [];
        },
        
        /**
         * Returns the pixel location of the midpoint of the
         * splitter.
         */
        _getSplitterLocation : function() {
            return [];
        },
        
        
        /**
         * Initialize the shared variable that contains layout information
         * from the server.
         */
        initSharedVar : function(){
           
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id );
            this.m_sharedVar.addCB( this._layoutChangedCB.bind( this ));
            this._layoutChangedCB( this.m_sharedVar.get());
        },
        
        /**
         * Update the view based on new layout information from the server.
         * @param val {String} - layout information from the server.
         */
        _layoutChangedCB : function( val ){
            if ( val ){
                try {
                    var layoutObj = JSON.parse( val );
                    this.serverUpdate( layoutObj );
                }
                catch( err ){
                    console.log( "Could not parse layout node: "+val );
                    console.log( "Err: "+err);
                }
            }
        },

        
        /**
         * Returns whether or not the window with the given id
         * was restored.
         * @param locationId {String} an identifier for the location where the window
         *      should be restored.
         * @return {boolean} true if the window was restored; false otherwise.
         */
        restoreWindow : function(locationId) {
            return false;
        },
        
        /**
         * Remove all windows.
         */
        removeWindows : function() {
        },
        
        
        /**
         * Update the UI based on server information.
         * @param obj {Object} - server layout information.
         */
        serverUpdate : function (obj ){
            
        },
        
        /**
         * Returns this layout cell if it has matching id; otherwise, returns null.
         * @param id {String} - an identifier for a layout cell.
         */
        getNode : function( id ){
            var target = null;
            if ( this.m_id == id ){
                target = this;
            }
            return target;
        },
        
        
        
        /**
         * Returns a list of windows displayed by this area and its children.
         * @return {Array} a list of windows displayed by the area and its children.
         */
        getWindows : function ( ){
            return [];
        },
        
        /**
         * Returns the window at the given location if one exists; otherwise returns null.
         * @param row {Number} the desired grid row.
         * @param col {Number} the desired grid column.
         * @return {skel.widgets.Window.DisplayWindow} window at the given location contained
         *      in a child of this object or null if there is no such window.
         */
        getWindow : function( locationId ){
            return null;
        },

        /**
         * Update the server with the new size of the node.
         * @param width {Number} - the new width of the node.
         * @param height {Number} - the new height of the node.
         */
        sendSizeCmd : function( width, height ){
            if ( this.m_connector !== null && this.m_id != null){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND +"setSize";
                var params = "width:"+width+",height:"+height;
                this.m_connector.sendCommand( cmd, params, null);
            }
        },

        setDrawMode : function(drawInfo) {
        },
        
        /**
         * Returns whether or not the identified child is replaced with the
         * new node.
         * @param sourceId {String} - an identifier for a layout cell; replacement is
         *      considered only if the identifier matches this cells identifier.
         * @param childId {String} - indicates whether a first or second child 
         *      should be replaced.
         * @param node {skel.widgets.Layout.LayoutNode} the replacement node.
         * @return {boolean} true if the node was replaced; false otherwise.
         */
        setNode : function( sourceId, childId, node ){
            return false;
        },

        /**
         * Notifies children that the given window was selected.
         * 
         * @param win {skel.widgets.Window.DisplayWindow} the
         *                selected window.
         */
        windowSelected : function(win) {
        },
        
        m_connector : null,
        m_id : null,
        m_sharedVar : null
     
    }

});
