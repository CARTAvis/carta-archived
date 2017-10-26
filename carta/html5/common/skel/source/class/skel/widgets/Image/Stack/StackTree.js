/**
 * Manage a tree displaying directories/files and display areas for the selected
 * directory and file.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Stack.StackTree", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);
        if ( this.m_connector === null ){
            this.m_connector = mImport("connector");
        }
        this._init();
    },

    events : {
        "treeReordered" : "qx.event.type.Data",
        "treeSelection" : "qx.event.type.Data"
    },

    members : {


        /**
         * Add child nodes to the passed in root folder.
         * @param root {skel.widgets.Image.Stack.TreeGroup} - the parent node.
         * @param itemArray {Array} - a list of children to add.
         * @param selections {Array} - a running record of the tree nodes that are selected.
         */
        _addChildren : function( root, itemArray, selections, nodes ){
            var id = root.getId();
            var path = skel.widgets.Path.getInstance();
            for ( var i = 0; i < itemArray.length; i++ ){
                var itemId = itemArray[i].id;
                var treeItem = null;
                if ( itemArray[i].type == this.m_LAYER_DATA){
                    treeItem = new skel.widgets.Image.Stack.TreeItem( itemArray[i].name, itemId, itemArray[i].visible );
                    treeItem.setSettings( itemArray[i].mask);
                    root.add( treeItem );
                }
                else if ( itemArray[i].type == this.m_LAYER_GROUP){
                    treeItem = new skel.widgets.Image.Stack.TreeGroup( itemArray[i].name, itemId, itemArray[i].visible );
                    treeItem.setSettings( itemArray[i].mode );
                    root.add( treeItem );
                    this._addChildren( treeItem, itemArray[i].layers, selections, nodes );
                    treeItem.setOpen( true );
                }

                if ( itemArray[i].selected ){
                    selections.push( treeItem );
                }
                nodes.push( itemArray[i].name );
            }
        },

        /**
         * Returns a list of tree nodes that should be selected.
         * @param root {Object} - the current tree node.
         * @param selections {Array} - list of names for selected nodes.
         * @return {Array} - the children of the root with names in the selections list.
         */
        _getNodesSelected : function( root, selections, nodeSelections ){
            var children = root.getChildren();
            for ( var i = 0; i < children.length; i++ ){
                this._getNodesSelected( children[i], selections, nodeSelections );
            }
            if ( selections.contains( root.getLabel() ) ){
                nodeSelections.push( root );
            }
        },

        /**
         * Returns the id of the first selected tree item.
         * @return {String} - the id of the first selected tree item.
         */
        getSelectedId : function(){
            var widgets = this.m_tree.getSelection();
            var id = "";
            if ( widgets.length > 0 ){
                id = widgets[0].getId();
            }
            return id;
        },

        /**
         * Returns the name of the first selected tree node or an empty string.
         * @return {String} - the name of the first selected tree node.
         */
        getSelectedName : function(){
            var widgets = this.m_tree.getSelection();
            var name = "";
            if ( widgets.length > 0 ){
                name = widgets[0].getLabel();
            }
            return name;
        },


        /**
         * Returns the identifiers of the selected tree nodes.
         * @return {Array} - a list containing the identifiers of the selected tree nodes.
         */
        getSelectedPaths : function(){
            var widgets = this.m_tree.getSelection();
            var paths = [];
            var path = skel.widgets.Path.getInstance();
            for ( var i = 0; i < widgets.length; i++ ){
                paths[i] = widgets[i].getId();
            }
            return paths;
        },

        /**
         * Returns the settings of the first selected tree node.
         * @return {Object} - the settings of the first selected tree node.
         */
        getSelectedSettings : function(){
            var widgets = this.m_tree.getSelection();
            var settings = null;
            if ( widgets.length > 0 ){
                settings = widgets[0].getSettings();
            }
            return settings;
        },

        /**
         * Return a list of the names of tree nodes.
         * @param itemArray {Object} - an object containing tree nodes.
         * @param newNodes {Array} - list of names of tree nodes.
         */
        _getTreeNodes : function( itemArray, newNodes){
            for ( var i = 0; i < itemArray.length; i++ ){
                if ( itemArray[i].type == this.m_LAYER_GROUP ){
                    this._getTreeNodes( itemArray[i].layers, newNodes );
                }
                newNodes.push( itemArray[i].name );
            }
        },

        /**
         * Return tree nodes with matching names that are children of the passed
         * in node.
         * @param node {Object} - the root tree nodes.
         * @paran newNodes {Array} - children with matching names.
         * @param names {Array} - names of nodes to be selected.
         */
        _getTreeNodesMatching : function( node, newNodes, names){
            if ( node !== null ){
                var children = node.getChildren();
                for ( var i = 0; i < children.length; i++ ){
                    this._getTreeNodesMatching( children[i], newNodes, names );
                }
                if ( names.indexOf( node.getNodeLabel() ) >= 0 ){
                    newNodes.push( node);
                }
            }
        },

        /**
         * Return the list of tree nodes that should be selected.
         * @param itemArray {Array} - a list of labels for the nodes that should be
         *      selected.
         * @param newNodes {Array} - a list of tree nodes with labels matching those
         *      in the itemArray.
         */
        _getTreeNodeSelections : function( itemArray, newNodes ){
            for ( var i = 0; i < itemArray.length; i++ ){
                if ( itemArray[i].type == this.m_LAYER_GROUP ){
                    this._getTreeNodeSelections( itemArray[i].layers, newNodes );
                }
                if ( itemArray[i].selected ){
                    newNodes.push( itemArray[i].name );
                }
            }
        },



        /**
         * Initialize the UI for the file tree and the directory/file text
         * fields.
         */
        _init : function(){
            //Initialize the tree.
            this.m_tree = new qx.ui.tree.Tree();
            this.m_treeSelectId = this.m_tree.addListener( "changeSelection", this._notifySelection, this );
            this.m_tree.setSelectionMode( "multi");
            this.m_tree.setWidth(250);

            //Initialize the tree item controls
            this.m_treeItemControls = new skel.widgets.Image.Stack.TreeItemControls();

            //Add everything.
            this._setLayout( new qx.ui.layout.VBox(1));
            this._add( this.m_tree, {flex:1} );
            this._add( this.m_treeItemControls );
        },

        /**
         * Returns true if all selected tree nodes support RGB.
         * @return {boolean} - true if all selected tree nodes support RGB;
         *      false otherwise.
         */
        isColorNodeSelected : function(){
            var widgets = this.m_tree.getSelection();
            var colorSupport = false;
            if ( widgets.length > 0 ){
                colorSupport = true;
                for ( var i = 0; i < widgets.length; i++ ){
                    if ( !widgets[i].isColorSupport() ){
                        colorSupport = false;
                        break;
                    }
                }
            }
            return colorSupport;
        },

        /**
         * Returns true if all selected tree nodes are groups.
         * @return {boolean} - true if all selected tree nodes are groups;
         *      false otherwise.
         */
        isGroupSelected : function(){
            var widgets = this.m_tree.getSelection();
            var group = false;
            if ( widgets.length > 0 ){
                group = true;
                for ( var i = 0; i < widgets.length; i++ ){
                    var widgetGroup = widgets[i].isGroup();
                    if ( !widgetGroup ){
                        group = false;
                        break;
                    }
                }
            }
            return group;
        },

        /**
         * Returns true if the ability to move nodes up and down in the stack should be
         * enabled; false otherwise.
         * @return {boolean} - true to enable up/down the stack buttons; false otherwise.
         */
        isUpDownEnabled : function(){
            var stackNode = false;
            if ( this.m_tree.getRoot() ){
                var children = this.m_tree.getRoot().getChildren();
                if ( children.length > 1 ){
                    var widgets = this.m_tree.getSelection();
                    for ( var i = 0; i < widgets.length; i++ ){
                        var treeItem = this.m_tree.getTreeItem( widgets[i] );
                        if ( 0 == treeItem.getLevel() ){
                            stackNode = true;
                            break;
                        }
                    }
                }
            }
            return stackNode;
        },

        /**
         * Returns true if the selected tree nodes are siblings.
         * @return {boolean} - true if the selected tree nodes are siblings;
         *      false otherwise.
         */
        isSiblingsSelected : function(){
            var widgets = this.m_tree.getSelection();
            var siblings = false;
            //Need at least 2 siblings.
            if ( widgets.length >= 2 ){
                siblings = true;
                var level = 0;
                for ( var i = 0; i < widgets.length; i++ ){
                    var treeItem = this.m_tree.getTreeItem( widgets[i] );
                    if ( !treeItem.hasChildrenContainer() ){
                        if ( level != treeItem.getLevel() ){
                            siblings = false;
                            break;
                        }
                    }
                }
            }
            return siblings;
        },

        /**
         * Returns true if there is a difference in labels of the tree nodes between
         * the current tree and the ones passed in from the server.
         * @param dataTree {Object} - a list of tree nodes from the server.
         */
        _isTreeNodesChanged : function( dataTree ){
            //get the new nodes based on server data.
            var newNodes = [];
            this._getTreeNodes( dataTree, newNodes );

            //get the old nodes displayed by the tree.
            var oldNodes = [];
            if ( this.m_treeData != null ){
                this._getTreeNodes( this.m_treeData, oldNodes );
            }

            //Decide if the lists of nodes are the same.
            var changed = true;
            if ( oldNodes.length == newNodes.length ){
                var j = 0;
                for ( j = 0; j < oldNodes.length; j++ ){
                    if ( oldNodes[j] != newNodes[j] ){
                        break;
                    }
                }
                if ( j == oldNodes.length ){
                    changed = false;
                }
            }
            return changed;
        },

        /**
         * Notify listeners that the selected tree node(s) have changed.
         */
        _notifySelection : function(){
            var data = {
                send : true
            };
            this.fireDataEvent( "treeSelection", data );
        },


        /**
         * Update the UI with new tree information from the server.
         */
        _resetModel : function( ){
            var root = new skel.widgets.Image.Stack.TreeGroup( "", "");
            this.m_tree.setRoot( root );
            var selections = [];
            this.m_nodes = [];
            this._addChildren( root, this.m_treeData, selections, this.m_nodes );
            root.setOpen( true );
            var selectCount = selections.length;
            var oldSelects = this.m_tree.getSelection();
            this._setTreeSelections( selections );
        },

        /**
         * Set whether or not the nodes in the tree should be automatically selected or not.
         * @param auto {boolean} - true if they are automatically selected; false if the user
         *      can select them.
         */
        setAutoSelect : function( auto ){
            this.m_tree.setEnabled( !auto );
            this._updateGroupControls();
        },

        /**
         * Set the server-side id of the object managing the stack.
         * @param id {String} - the server-side id of the object managing the stack.
         */
        setId : function( id ){
            this.m_treeItemControls.setId( id );
        },

        /**
         * Update the UI with a list of the tree nodes (widgets) that should be
         * selected.
         * @param selections {Array} - list of tree nodes that should be selected.
         */
        _setTreeSelections : function( selections ){
            this.m_tree.setSelection( selections );
            this._updateGroupControls();
        },

        /**
         * Update the controls that determine stack movement and grouping of layers.
         */
        _updateGroupControls : function(){
            var treeEnabled = this.m_tree.isEnabled();
            var group = false;
            var siblings = false;
            var stackNodeSelected = false;
            if ( treeEnabled ){
                group = this.isGroupSelected();
                siblings = this.isSiblingsSelected();
                stackNodeSelected = this.isUpDownEnabled();
            }
            this.m_treeItemControls._treeItemSelected( group, siblings, stackNodeSelected );
        },

        /**
         * Update the nodes of the tree that are selected based on server-side
         * information.
         * @param dataTree {Object} - server-side information about the selected tree nodes.
         */
        _updateSelections : function( dataTree ){
            var selections = [];
            this._getTreeNodeSelections( dataTree, selections );
            var oldSelections = [];
            if ( this.m_treeData !== null ){
                this._getTreeNodeSelections( this.m_treeData, oldSelections );
            }
            var selectionsChanged = false;
            if ( oldSelections.length == selections.length ){
                var i = 0;
                for ( i = 0; i < selections.length; i++ ){
                    if ( selections[i] != oldSelections[i] ){
                        break;
                    }
                }
                if ( i != selections.length ){
                    selectionsChanged = true;
                }
            }
            else {
                selectionsChanged = true;
            }

            if ( selectionsChanged ){
                var nodeSelections = [];
                this._getTreeNodesMatching( this.m_tree.getRoot(), nodeSelections, selections );
                this._setTreeSelections( nodeSelections );
                var data = {
                    send : false
                };
                this._updateGroupControls();
                this.fireDataEvent( "treeSelection", data );
            }
        },



        /**
         * Update tree node settings (such as color) based on information from
         * the server.
         * @param dataTree {Object} - server-side information about note properties.
         * @param treeItems {Array} - a list of tree nodes.
         */
        _updateSettings : function( dataTree, treeItems ){
            for ( var i = 0; i < dataTree.length; i++ ){
                if ( dataTree[i].type == this.m_LAYER_GROUP ){
                    this._updateSettings( dataTree[i].layers, treeItems );
                }
                //see if any of the treeItems have the same id.
                for ( var j = 0; j < treeItems.length; j++ ){

                    if ( treeItems[j].getId() == dataTree[i].id ){
                       //Update the treeItems.
                       treeItems[j].setVisible( dataTree[i].visible );
                       treeItems[j].setSettings( dataTree[i].mask );
                       break;
                    }
                }

            }
        },


        /**
         * Updates the file tree based on directory information from the server.
         * @param dataTree {String} representing available data files in a
         *                hierarchical JSON format
         */
        _updateTree : function(dataTree) {
            var treeNodeChanged = this._isTreeNodesChanged( dataTree );
            if ( treeNodeChanged ){
                this.m_treeData = dataTree;
                this._resetModel();
            }
            else {
                var treeItems = this.m_tree.getItems( true, true );
                this._updateSettings( dataTree, treeItems );
                this._updateSelections( dataTree );
                this.m_treeData = dataTree;
            }
            var errorMan = skel.widgets.ErrorHandler.getInstance();
            errorMan.clearErrors();
        },

        m_connector : null,
        m_controller : null,
        m_jsonObj : null,
        m_tree : null,
        m_treeSelectId : null,
        m_treeData : null,
        m_treeItemControls : null,
        //List of names of current tree nodes
        m_nodes : null,
        m_LAYER_GROUP : "LayerGroup",
        m_LAYER_DATA : "LayerData"
    }

});
