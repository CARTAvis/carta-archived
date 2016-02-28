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
        _addChildren : function( root, itemArray, selections ){
            var id = root.getId();
            var path = skel.widgets.Path.getInstance();
            for ( var i = 0; i < itemArray.length; i++ ){
                var itemId = itemArray[i].id;
                var treeItem = null;
                if ( itemArray[i].type == "LayerData"){
                    var colorStr = "";
                    if ( itemArray[i].mask.colorSupport){
                        var colorArray = [];
                        colorArray[0] = itemArray[i].mask.red;
                        colorArray[1] = itemArray[i].mask.green;
                        colorArray[2] = itemArray[i].mask.blue;
                        colorStr = qx.util.ColorUtil.rgbToHexString( colorArray );
                    }
                    treeItem = new skel.widgets.Image.Stack.TreeItem( itemArray[i].name, colorStr, itemId );
                    treeItem.setSettings( itemArray[i].mask);
                    root.add( treeItem );
                }
                else if ( itemArray[i].type == "LayerGroup"){
                    
                    treeItem = new skel.widgets.Image.Stack.TreeGroup( itemArray[i].name, itemId);
                    treeItem.setSettings( itemArray[i].mode );
                    root.add( treeItem );
                    this._addChildren( treeItem, itemArray[i].layerGroup, selections );
                }
               
                if ( itemArray[i].selected ){
                    selections.push( treeItem );
                }
            }
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
         * Notify listeners that the selected tree node(s) have changed.
         */
        _notifySelection : function(){
            var data = {};
            this.fireDataEvent( "treeSelection", data );
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
            
            //Add everything.
            this._setLayout( new qx.ui.layout.VBox(1));
            this._add( this.m_tree, {flex:1} );
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
            if ( widgets.length == 1 ){
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
                var level = -1;
                for ( var i = 0; i < widgets.length; i++ ){
                    var treeItem = this.m_tree.getTreeItem( widgets[i] );
                    if ( !treeItem.hasChildrenContainer() ){
                        if ( level == -1 ){
                            level = treeItem.getLevel();
                        }
                        else {
                            if ( level != treeItem.getLevel() ){
                                siblings = false;
                                break;
                            }
                        }
                    }
                }
            }
            return siblings;
        },
        
        
        /**
         * Update the UI with new directory information from the server.
         */
        _resetModel : function( ){
            var root = new skel.widgets.Image.Stack.TreeGroup( "", "");
            this.m_tree.setRoot( root );
            var selections = [];
            this._addChildren( root, this.m_treeData, selections );
            root.setOpen( true );
            var selectCount = selections.length;
            var oldSelects = this.m_tree.getSelection();
            this.m_tree.removeListenerById( this.m_treeSelectId );
            this.m_tree.setSelection( selections );
            this.m_treeSelectId = this.m_tree.addListener( "changeSelection", this._notifySelection, this );
        },

        
        /**
         * Updates the file tree based on directory information from the server.
         * @param dataTree {String} representing available data files in a
         *                hierarchical JSON format
         */
        _updateTree : function(dataTree) {
            this.m_treeData = dataTree;
            this._resetModel();
            var errorMan = skel.widgets.ErrorHandler.getInstance();
            errorMan.clearErrors();
        },
        
        m_connector : null,
        m_controller : null,
        m_jsonObj : null,
        m_tree : null,
        m_treeSelectId : null,
        m_treeData : null
    }

});
