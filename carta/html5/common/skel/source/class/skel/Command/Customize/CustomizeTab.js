/**
 * A page containing a command listing and the ability to change there visibility
 * on a particular display object.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Customize.CustomizeTab", {
    extend : qx.ui.tabview.Page,

    /**
     * Constructor.
     */
    construct : function( title ) {
        this.base( arguments, title, "");
        this._init();
    },

    events : {
        cmdVisibilityChanged : 'qx.event.type.Data'
    },

    members : {


        /**
         * Initializes the UI.
         */
        _init : function() {
            this.m_commandMap = {};
            this.setLayout( new qx.ui.layout.VBox());
            this.m_tree = new qx.ui.tree.Tree();
            this.m_tree.setWidth(300);
            this.m_tree.setHeight(300);
            
            this._add( this.m_tree );
            this.m_treeController = new qx.data.controller.Tree(null,
                    this.m_tree, "value", "name");
            this.m_treeController.setDelegate( this );
        },
        
        /**
         * Recursive build of tree.
         * @param tree {Object} the current node of the tree.
         * @param cmds {Object} the current command.
         */
        _buildTreeData : function( tree, cmds ){
            var i = 0;
            if ( typeof cmds === 'object'){
                var label;
                if( Object.prototype.toString.call( cmds.value ) === '[object Array]' ) {
                    tree.name = "";
                    if ( cmds.name ){
                        tree.name = cmds.name;
                    }
                    tree.checked = false;
                    tree.value = [];
                    label = tree.name;
                    
                    for ( i = 0; i < cmds.value.length; i++ ){
                        tree.value[i] = {};
                        this._buildTreeData( tree.value[i], cmds.value[i]);
                    }
                }
                else {
                    label = cmds.getLabel();
                    tree.name = label;
                    var visible = cmds.isVisible(this.getLabel());
                    tree.checked = visible;
                    if ( cmds.getType() == skel.Command.Command.TYPE_COMPOSITE ||
                            cmds.getType() == skel.Command.Command.TYPE_GROUP ){
                        var valArray = cmds.getValue();
                        tree.value = [];
                        for ( i = 0; i < valArray.length; i++ ){
                            tree.value[i] = {};
                            this._buildTreeData( tree.value[i], valArray[i]);
                        }
                    }
                }
            }
            else {
                console.log( "Unrecognized cmd="+ (typeof cmds));
            }
            return tree;
        },
        
     
        /**
         * Constructs a file tree from a hierarchical JSON string.
         * 
         * @param dataTree
         *                {String} representing available data files in a
         *                hierarchical JSON format
         */
        updateTree : function(cmds) {
            var node = {};
            var treeData = this._buildTreeData( node, cmds );
            var jsonModel = qx.data.marshal.Json.createModel(treeData);
            this.m_treeController.setModel( jsonModel );
            this.m_tree.getRoot().setOpen(true);
        },
        
        /**
         * Binds view item actions to methods.
         */
        bindItem : function( controller,item, id){
            controller.bindDefaultProperties( item, id );
            controller.bindProperty( "checked", "checked", null, item, id );
        },
        
        /**
         * Creates a view item (delegate implementation).
         */
        createItem : function() {
            var tabCol = new skel.Command.Customize.CustomizeTabColumn();
            tabCol.addListener( "cmdVisibilityChanged", function(ev){
                var data = ev.getData();
                var cmdName = data.name;
                var visible = data.checked;
                var cmdFactory = skel.Command.CommandFactory.getInstance();
                var cmd = cmdFactory.getCommand( cmdName );
                if ( cmd !== null ){
                    cmd.setVisible( this.getLabel(), visible);
                    this.fireDataEvent( "cmdVisibilityChanged", "");
                }
                
            }, this );
            return tabCol;
        },
        
        m_tree : null,
        m_treeController : null,
        m_commandMap : null
    }

});