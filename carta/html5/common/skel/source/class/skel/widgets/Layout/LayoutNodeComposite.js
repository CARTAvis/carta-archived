/**
 * A Composite containing a split pane that manages two child areas of screen
 * real estate space.
 */

/*******************************************************************************
 * 
 * 
 * 
 * 
 * 
 ******************************************************************************/

qx.Class.define("skel.widgets.Layout.LayoutNodeComposite",{
    extend : skel.widgets.Layout.LayoutNode,

    /**
     * Constructor; recursively builds up the screen.
     */
    construct : function(id) {
        this.base(arguments, id );

        this.m_pane = new qx.ui.splitpane.Pane( this.m_HORIZONTAL ).set({
            allowGrowX : true,
            allowGrowY : true
        });
        qx.event.message.Bus.subscribe( "nodeFound", function( message ){
            var data = message.getData();
            this.setNode( data.nodeId, data.childId, data.child );
        }, this );
    },

    events : {
        "findChild" : "qx.event.type.Data",
        "iconifyWindow" : "qx.event.type.Data",
        "leafResize" : "qx.event.type.Data",
        "iconify" : "qx.event.type.Data"
    },

    members : {
        
        /**
         * Create a child layout cell with the given id and type.
         * @param childId {String} - an identifier for the child layout cell.
         * @param compositeType {String} - an identifier for the type of layout cell to create.
         */
        _initializeChild : function( childId, compositeType ){
            var child = null;
            //If the type is a composite, then we make a layoutNode composite
            //with the id.  if the type is a leaf, we make a leaf node with the id.
            if ( compositeType ){
                child = new skel.widgets.Layout.LayoutNodeComposite( childId );
            }
            else {
                child = new skel.widgets.Layout.LayoutNodeLeaf( childId );
            }
            child.addListener("iconifyWindow", function(ev) {
                    var data = ev.getData();
                    child.exclude();
                    this.fireDataEvent("iconifyWindow", data);
            }, this);
            child.addListener( "findChild", function(ev){
                this.fireDataEvent( "findChild", ev.getData());
            }, this );
            child.addListener( "leafResize", function(ev){
                var dimFirst = [0, 0];
                if ( this.m_areaFirst !== null ){
                    dimFirst = this.m_areaFirst._getDimensions();
                }
                var dimSecond = [0,0];
                if ( this.m_areaSecond !== null ){
                    dimSecond = this.m_areaSecond._getDimensions();
                }
                var width = dimFirst[0]+dimSecond[0];
                var height = dimFirst[1]+dimSecond[1];
                this.sendSizeCmd( width, height );
                this.fireDataEvent("resizeNode", null);
            }, this );
            child.initSharedVar();
            return child;
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
         */
        changeLink : function(sourceWinId, destWinId, addLink) {
            this.m_areaFirst.changeLink(sourceWinId, destWinId, addLink);
            this.m_areaSecond.changeLink(sourceWinId, destWinId, addLink);
        },
        

        /**
         * Notifies children that data has been loaded.
         * @param path {String} an identifier for the data to load.
         */
        dataLoaded : function(path) {
            if (this.m_areaFirst !== null) {
                this.m_areaFirst.dataLoaded(path);
            }
            if (this.m_areaSecond !== null) {
                this.m_areaSecond.dataLoaded(path);
            }
        },

        /**
         * Notifies children that data has been unloaded.
         * @param path {String} an identifier for the data to remove.
         */
        dataUnloaded : function(path) {
            if (this.m_areaFirst !== null) {
                this.m_areaFirst.dataUnloaded(path);
            }
            if (this.m_areaSecond !== null) {
                this.m_areaSecond.dataUnloaded(path);
            }
        },

        /**
         * Removes the pased in display area from the split pane.
         * @param displayArea {qx.ui.core.Widget} - the content to remove.
         */
        _removeChild : function( displayArea ){
            var children = this.m_pane.getChildren();
            for ( var i = 0; i < children.length; i++ ){
                if ( children[i] == displayArea ){
                    this.m_pane.remove( displayArea );
                }
            }
        },
        
        /**
         * Returns the child of the passed in node with the given identifier, if there is one;
         * otherwise, returns null.
         * @param node {skel.widgets.Layout.Node} the parent node.
         * @param id {String} an identifier for a node.
         * @return {skel.widgets.Layout.Node} the child of the parent node with matching id, if there is one; otherwise null.
         */
        getNode : function( id ){
            //See if the children are matches
            var target = null;
            if ( this.m_areaFirst !== null ){
                target = this.m_areaFirst.getNode( id );
                if ( target !== null && this.m_areaFirst.getId() == id ){
                    //Someone is kidnapping this child for their own display so
                    //remove it from ours.
                    var displayArea= this.m_areaFirst.getDisplayArea();
                    this._removeChild( displayArea );
                   
                }
            }
            if ( target === null ){
                if ( this.m_areaSecond !== null ){
                    target = this.m_areaSecond.getNode( id );
                    if ( target !== null && this.m_areaSecond.getId() == id ){
                        //Child is being kidnapped to bee displayed elsewhere so remove
                        //it from our display.
                        var displayArea2 = this.m_areaSecond.getDisplayArea();
                        this._removeChild( displayArea2 );
                    }
                }
            }
            if ( target === null ){
                target = arguments.callee.base.apply(this, arguments, id);
            }
            return target;
        },


        
        /**
         * Returns the row and column location of the second
         * area.
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
            var firstSize = this.m_areaFirst._getDimensions();
            var secondSize = this.m_areaSecond._getDimensions();
            var overallSize = firstSize;
            if (this.m_pane.getOrientation() == "vertical") {
                overallSize[1] = overallSize[1] + secondSize[1];
            } else {
                overallSize[0] = overallSize[0] + secondSize[0];
            }
            return overallSize;
        },
        

        /**
         * Returns the split pane.
         * @return {qx.ui.splitpane.Pane} this display area's divider.
         */
        getDisplayArea : function() {
            return this.m_pane;
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
        getLinkInfo : function(pluginId, sourceWinId, linkInfos ) {
            if (this.m_areaFirst !== null) {
                this.m_areaFirst.getLinkInfo( pluginId, sourceWinId, linkInfos );
            }
            if (this.m_areaSecond !== null) {
                this.m_areaSecond.getLinkInfo( pluginId, sourceWinId, linkInfos );
            }
        },
        
        /**
         * Returns a list of information about possible move destinations for
         * the given source window.
         * @param sourceWinId {String} an identifier for the window 
         *      that wants information about move locations.
         * @return {String} information about move locations for the source window.
         */
        getMoveInfo : function( sourceWinId) {
            var moveInfo = [];
            var i = 0;
            if (this.m_areaFirst !== null) {
                var firstInfo = this.m_areaFirst.getMoveInfo( sourceWinId);
                for ( i = 0; i < firstInfo.length; i++) {
                    moveInfo.push(firstInfo[i]);
                }
            }
            if (this.m_areaSecond !== null) {
                var secondInfo = this.m_areaSecond.getMoveInfo( sourceWinId);
                for ( i = 0; i < secondInfo.length; i++) {
                    moveInfo.push(secondInfo[i]);
                }
            }
            return moveInfo;
        },
        
        /**
         * Returns whether or not the window with the given id
         * was restored.
         * @param locationId {String} an identifier for the layout cell that should be restored.
         * @return {boolean} true if the window was restored; false otherwise.
         */
        restoreWindow : function(locationId) {
            var restored = this.m_areaFirst.restoreWindow(locationId);
            if (!restored) {
                restored = this.m_areaSecond.restoreWindow(locationId);
            }
            return restored;
        },
        
        /**
         * Remove all windows.
         */
        removeWindows : function() {
            this.m_areaFirst.removeWindows();
            this.m_areaSecond.removeWindows();
        },
        
        /**
         * Remove the two split pane content areas if they exist.
         */
        _clearContentAreas : function(){
            var clearedAreas = false;
            try {
                //Remove the children from the pane since we may replace them.
                var oldChildren = this.m_pane.getChildren();
                //Note::order must be backward since the oldChildren array is dynamically
                //updated as children are removed.
                for ( var i = oldChildren.length-1; i>= 0; i-- ){
                    this.m_pane.remove( oldChildren[i]);
                }
                clearedAreas = true;
            }
            catch( error ){
                console.log( "Could not clear content areas error" );
                console.log( error);
            }
            return clearedAreas;
        },
        
        /**
         * Update the UI based on information from the server.
         * @param obj {Object} - server layout information.
         */
        serverUpdate : function( obj ){
            //Reset the orientation
            if ( obj.horizontal ){
                if ( this.m_pane.getOrientation() != this.m_HORIZONTAL){
                    this.m_pane.setOrientation( this.m_HORIZONTAL );
                }
            }
            else {
                if ( this.m_pane.getOrientation() != this.m_VERTICAL){
                    this.m_pane.setOrientation( this.m_VERTICAL);
                }
            }

            var cleared = this._clearContentAreas();
            if ( !cleared ){
                return;
            }
            
            if ( this.m_areaFirst===null || obj.layoutLeft.id !== this.m_areaFirst.getId() ){
                //See if there happens to be an existing node with this id and make that the first child.
                var data = {
                        sourceId: this.m_id,
                        findId : obj.layoutLeft.id,
                        childId : this.m_FIRST
                };
                this.fireDataEvent( "findChild", data );
            }
            
            if ( this.m_areaSecond ===null || obj.layoutRight.id !== this.m_areaSecond.getId() ){
                //See if there happens to be an existing node with this id and make that the second child.
                var data2 = {
                        sourceId: this.m_id,
                        findId : obj.layoutRight.id,
                        childId : this.m_SECOND
                };
                this.fireDataEvent( "findChild", data2 );
            }
            
            //No matching first child so make a new one.
            if ( this.m_areaFirst === null || this.m_areaFirst.getId() != obj.layoutLeft.id ){
                this.m_areaFirst = this._initializeChild( obj.layoutLeft.id, obj.layoutLeft.composite );
            }
            
            //No matching second child so make a new one.
            if ( this.m_areaSecond === null || this.m_areaSecond.getId() != obj.layoutRight.id ){
                this.m_areaSecond = this._initializeChild( obj.layoutRight.id, obj.layoutRight.composite );
            }
            
            //Decide on the flex ratio based on the orientation of the pane
            //and the sizes of the children.
            var flexFirst = 1;
            var flexSecond = 1;
            if ( this.m_pane.getOrientation() == this.m_HORIZONTAL ){
                //Ratio will be for width.
                var leftWidth = obj.layoutLeft.width;
                var rightWidth = obj.layoutRight.width;
                var totalWidth = leftWidth + rightWidth;
                if ( totalWidth > 0 ){
                    flexFirst = leftWidth / totalWidth;
                    flexSecond = rightWidth / totalWidth;
                }
            }
            else {
                var topHeight = obj.layoutLeft.height;
                var bottomHeight = obj.layoutRight.height;
                var totalHeight = topHeight + bottomHeight;
                if ( totalHeight > 0 ){
                    flexFirst = topHeight / totalHeight;
                    flexSecond = bottomHeight / totalHeight;
                }
            }
            var firstArea = this.m_areaFirst.getDisplayArea();
            this.m_pane.add( firstArea, flexFirst);
            var secondArea = this.m_areaSecond.getDisplayArea();
            this.m_pane.add( secondArea, flexSecond);
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
            var nodeSet = false;
            if ( sourceId == this.m_id ){
                if ( childId == this.m_FIRST){
                    this.m_areaFirst = node;
                }
                else {
                    this.m_areaSecond = node;
                }
                nodeSet = true;
            }
            else {
                if ( this.m_areaFirst !== null ){
                    nodeSet = this.m_areaFirst.setNode( sourceId, childId, node );
                }
                if ( !nodeSet && this.m_areaSecond !== null ){
                    nodeSet = this.m_areaSecond.setNode( sourceId, childId, node );
                }
            }
            return nodeSet;
        },

        
        /**
         * Returns a list of windows displayed by this area and its children.
         * @return {Array} a list of windows displayed by the area and its children.
         */
        getWindows : function ( ){
            var firstWindows = this.m_areaFirst.getWindows();
            var secondWindows = this.m_areaSecond.getWindows();
            var windows = firstWindows.concat( secondWindows );
            return windows;
        },
        
        /**
         * Returns the window at the given location if one exists; otherwise returns null.
         * @param locationId {String} an identifier for a window location..
         * @return {skel.widgets.Window.DisplayWindow} window at the given location contained
         *      in a child of this object or null if there is no such window.
         */
        getWindow : function( locationId ){
            var win = null;
            if ( this.m_areaFirst !== null ){
                win = this.m_areaFirst.getWindow( locationId );
            }
            if ( win === null ){
                if ( this.m_areaSecond !== null ){
                    win = this.m_areaSecond.getWindow( locationId );
                }
            }
            return win;
        },

        setDrawMode : function(drawInfo) {
            if (this.m_areaFirst !== null) {
                this.m_areaFirst.setDrawMode(drawInfo);
            }
            if (this.m_areaSecond !== null) {
                this.m_areaSecond.setDrawMode(drawInfo);
            }
        },

        /**
         * Notifies children that the given window was selected.
         * 
         * @param win {skel.widgets.Window.DisplayWindow} the
         *                selected window.
         */
        windowSelected : function(win) {
            if ( this.m_areaFirst !== null ){
                this.m_areaFirst.windowSelected(win);
            }
            if ( this.m_areaSecond !== null ){
                this.m_areaSecond.windowSelected(win);
            }
        },
        m_FIRST : "first",
        m_SECOND : "second",
        m_HORIZONTAL : "horizontal",
        m_VERTICAL : "vertical",
        m_pane : null,
        m_areaFirst : null,
        m_areaSecond : null
     
    }

});
