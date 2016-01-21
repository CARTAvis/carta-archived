/**
 * List that supports drag and drop.
 */


qx.Class.define("skel.widgets.Image.Stack.DragDropList", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( width ) {
        this.base(arguments);
        this._init( width );
    },
    
    events : {
        "listReordered" : "qx.event.type.Data",
        "listSelection" : "qx.event.type.Data"
    },

    members : {
        
        /**
         * Return a list of the indices of selected list items.
         * @return {Array} - a list of indices of list items that have
         *      been selected.
         */
        getSelectedIndices : function(){
            var indices = [];
            var children = this.m_list.getChildren();
            for ( var i = 0; i < children.length; i++ ){
                if ( this.m_list.isSelected( children[i] ) ){
                    indices.push( i );
                }
            }
            return indices;
        },
        /*
         * Initializes the UI.
         */
        _init : function( width ) {
            this._setLayout( new qx.ui.layout.VBox(1) );
            this.m_listContainer = new qx.ui.container.Composite();
            this.m_listContainer.setLayout( new qx.ui.layout.Basic() );
            this._add( this.m_listContainer );
            
            this._initList( width );
            this._initDragIndicator( width );
        },
        
        /**
         * Initialize the drag indicator.
         * @param width {Number} - how wide to make the drag marker.
         */
        _initDragIndicator : function( width ){
            this.m_dragMarker = new qx.ui.core.Widget();
            this.m_dragMarker.setDecorator(new qx.ui.decoration.Decorator().set({
                widthTop: 2,
                styleTop: "solid",
                colorTop: "gray"
            }));
            this.m_dragMarker.setHeight(2);
            this.m_dragMarker.setWidth( width );
            this.m_dragMarker.setOpacity(0.5);
            this.m_dragMarker.setZIndex(100);
            this.m_dragMarker.setDroppable(true);
            this.m_listContainer.add(this.m_dragMarker, {left:0,top:-5});
            
            this.m_dragMarker.addListener( "drop", function(e){
                this._reorderList( this.m_dragMarker );
                this.m_drag = false;
            }, this );
        },
        
        /**
         * Initialize the list.
         * @param width {Number} - how wide to make the list.
         */
        _initList : function( width ){
            this.m_list = new qx.ui.form.List();
            skel.widgets.TestID.addTestId( this.m_list, "imageStackList" ); 
            this.m_list.setMinWidth( width );
            this.m_list.setSelectionMode("multi");
            this.m_list.setDraggable(true);
            this.m_list.setDroppable( true );
            this.m_listContainer.add( this.m_list, {left:0,top:0});
            this.m_list.addListener( "changeSelection", function(){
                var data = {};
                this.fireDataEvent( "listSelection", data );
            }, this );
            //Support moves.
            this.m_list.addListener("dragstart", function(e) {
                e.addAction("move");
                this.m_drag = true;
            }, this);

            this.m_list.addListener("dragend", function(e){
                if ( this.m_dragMarker !== null ){
                    //Hide the drag marker
                    if ( this.m_dragMarker !== null ){
                        this.m_dragMarker.setLayoutProperties( {left:0, top:-5} );
                    }
                    this.m_drag = false;
                }
            }, this );

            this.m_list.addListener("drag", function(e){
                this._doDrag(e);
            }, this );

            this.m_list.addListener("dragover", function(e){
                // Stop when the dragging comes from outside
                if (e.getRelatedTarget()) {
                    e.preventDefault();
                }
            }, this );

            this.m_list.addListener("drop", function(e) {
                var orig = e.getOriginalTarget();
                this._reorderList(orig);
                this.m_drag = false;
            }, this);
        },
        
        /**
         * Update the drag marker while the drag is in progress.
         * @param e {qx.event.Event} - the drag event.
         */
        _doDrag : function( e ){
            if ( !this.m_drag ){
                return;
            }
            var orig = e.getOriginalTarget();

            //Determine if we are at a list item or at the end of
            //the list.
            var listItem =false;
            if (orig instanceof qx.ui.form.ListItem) {
                listItem = true;
            }

            var origCoords = orig.getContentLocation();
            var listCoords = this.m_list.getContentLocation();
            var origTop = 0;
            if ( listItem ){
                //Between list items.
                origTop = origCoords.top - listCoords.top;
            }
            else {
                //At the end of the list.
                var childs = this.m_list.getChildren();
                var lastItem = childs[childs.length - 1];
                var lastItemBottom = this.m_list.getItemBottom( lastItem );
                origTop = lastItemBottom + 4;
            }
            
            //Position the drag marker
            if ( this.m_dragMarker !== null ){
                this.m_dragMarker.setLayoutProperties( {left:0, top:origTop} );
            }
        },
        
        /**
         * Return the index of the item in the list.
         * @param item {qx.ui.core.Widget} - a list item.
         * @return {Number} - the index of the item in the list.
         */
        _getIndex : function( item ){
            var children = this.m_list.getChildren();
            var index = -1;
            for ( var i = 0; i < children.length; i++ ){
                if ( children[i] == item ){
                    index = i;
                    break;
                }
            }
            return index;
        },
        
        /**
         * Returns the permuted order of the list indices after an insertion at the
         * end of the list.
         * @param insertIndices {Array} - a list of the indices of list items that
         *      were inserted.
         * @return {Array} - a list of permuted indices.
         */
        _getListIndicesAfter : function( insertIndices ){
            //Go through the list and add any index that is not being
            //inserter.
            var indices = [];
            var listCount = this.m_list.getChildren().length;
            for ( var j = 0; j < listCount; j++ ){
                if ( insertIndices.indexOf( j) < 0 ){
                    indices.push( j );
                }
            }
            //Now go through and add the insert indices
            for ( j = 0; j < insertIndices.length; j++ ){
                indices.push( insertIndices[j] );
            }
            return indices;
        },
        
        /**
         * Returns the permuted order of the list indices after an insertion before an
         * item in the list.
         * @param insertIndices {Array} - a list of the indices of list items that
         *      were inserted.
         * @param origIndex {Number} - the index of the list item just after where
         *      the insertion took place.
         * @return {Array} - a list of permuted indices.
         */
        _getListIndicesBefore : function( insertIndices, origIndex ){
            //First add in all the non-selected items, up to
            //the origIndex.
            var indices = [];
            for ( var j = 0; j < origIndex; j++ ){
                var insertIndex = insertIndices.indexOf( j );
                if ( insertIndices.indexOf(j) < 0 ){
                    indices.push( j );
                }
            }
            //Now add in all the selectedItems
            for ( j = 0; j < insertIndices.length; j++ ){
                indices.push( insertIndices[j]);
            }
            //Finally add in from the original index to the end,
            //leaving off the insert indices.
            var listCount = this.m_list.getChildren().length;
            for ( j = origIndex; j < listCount; j++ ){
                var insertIndex = insertIndices.indexOf( j );
                if ( insertIndex < 0 ){
                    indices.push( j );
                }
            }
            return indices;
        },

        /**
         * Reorder the list.
         * @param orig {Object} - the list item where things should be dropped
         *      or the drag marker if we are dropping at the end of the list.
         */
        _reorderList : function( orig ){
            // Only continue if the target is a list item.
            var listItem = false;
            var origIndex = -1;
            if (orig instanceof qx.ui.form.ListItem) {
                origIndex = this._getIndex( orig );
                listItem = true;
            }
           
            var sel = this.m_list.getSortedSelection();
            var lastChild = null;
            //If we are not at a list item, we need to find the end
            //of the list.
            if ( !listItem ){
                var children = this.m_list.getChildren();
                if ( children.length > 0 ){
                    origIndex = children.length - 1;
                    lastChild = children[ origIndex ];
                }
            }
            
            var insertIndices = [];
            for (var i = 0; i < sel.length; i++) {
                if ( listItem ){
                    //At a list item.
                    var beforeIndex = this._getIndex( sel[i] );
                    insertIndices.push( beforeIndex );
                    this.m_list.addBefore(sel[i], orig);
                }
                else {
                    //Add at the end of the list
                    var afterIndex = this._getIndex( sel[i] );
                    insertIndices.push( afterIndex );
                    this.m_list.addAfter( sel[i], lastChild );
                }
            }
            
            var indices = [];
            if ( listItem ){
                indices = this._getListIndicesBefore( insertIndices, origIndex );
            }
            else {
                indices = this._getListIndicesAfter( insertIndices );
            }
            
            var data = {
                "listItems" : indices
            };
            this.fireDataEvent( "listReordered", data );
        },
       
        
        /**
         * Update the items in the list.
         * @param items {Array} - a list of strings.
         */
        setListItems : function( items ){
            this.m_list.removeAll();
            var dataCount = items.length;
            var selectedItems = [];
            for ( var i = 0; i < dataCount; i++ ){
                var listItem = new qx.ui.form.ListItem( items[i].file );
                this.m_list.add( listItem );
                var visible = items[i].visible;
                if ( items[i].selected ){
                    selectedItems.push( i );
                }
                var contextMenu = new qx.ui.menu.Menu();
                
                //Close button
                var closeCmd = new skel.Command.Data.CommandDataCloseImage( items[i].file );
                var closeButton = new qx.ui.menu.Button( "Close");
                closeButton.addListener( "execute", function(){
                    this.doAction( true, function(){} );
                }, closeCmd );
                contextMenu.add( closeButton );
                
                if ( visible ){
                    //Hide button
                    var hideCmd = new skel.Command.Data.CommandDataHideImage( items[i].file, i );
                    var hideButton = new qx.ui.menu.Button( "Hide");
                    hideButton.addListener( "execute", function(){
                        this.doAction( true, function(){});
                    }, hideCmd );
                    contextMenu.add( hideButton );
                }
                else {
                    //Show button
                    var showCmd = new skel.Command.Data.CommandDataShowImage( items[i].file, i );
                    var showButton = new qx.ui.menu.Button( "Show");
                    showButton.addListener( "execute", function(){
                        this.doAction( true, function(){});
                    }, showCmd );
                    contextMenu.add( showButton );
                }
                listItem.setContextMenu(contextMenu);
            }
            var children = this.m_list.getChildren();
            var selectedChildren = [];
            for ( var i = 0; i < selectedItems.length; i++ ){
                selectedChildren.push( children[selectedItems[i]] );
            }
            this.m_list.setSelection( selectedChildren );
        },

        m_list : null,
        m_dragMarker : null,
        m_drag : false,
        m_listContainer : null
    }
});