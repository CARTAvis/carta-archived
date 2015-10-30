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
        "listReordered" : "qx.event.type.Data"
    },

    members : {
        
        /**
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
            this.m_list.setMinWidth( width );
            this.m_list.setSelectionMode("multi");
            this.m_list.setDraggable(true);
            this.m_list.setDroppable( true );
            this.m_listContainer.add( this.m_list, {left:0,top:0});

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
         * Reorder the list.
         * @param orig {Object} - the list item where things should be dropped
         *      or the drag marker if we are dropping at the end of the list.
         */
        _reorderList : function( orig ){
            // Only continue if the target is a list item.
            var listItem = false;
            if (orig instanceof qx.ui.form.ListItem) {
                listItem = true;
            }
           
            var sel = this.m_list.getSortedSelection();
            var lastChild = null;
            //If we are not at a list item, we need to find the end
            //of the list.
            if ( !listItem ){
                var children = this.m_list.getChildren();
                if ( children.length > 0 ){
                    lastChild = children[children.length - 1];
                }
            }
            for (var i = 0; i < sel.length; i++) {
                if ( listItem ){
                    //At a list item.
                    this.m_list.addBefore(sel[i], orig);
                }
                else {
                    //Add at the end of the list
                    this.m_list.addAfter( sel[i], lastChild );
                }
            }
            
            var names = [];
            var children = this.m_list.getChildren();
            for ( var j = 0; j < children.length; j++ ){
                names[j] = children[j].getLabel();
            }
            var data = {
                "listItems" : names
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
            for ( var i = 0; i < dataCount; i++ ){
                var listItem = new qx.ui.form.ListItem( items[i].layer );
                this.m_list.add( listItem );
                var visible = items[i].visible;
                var contextMenu = new qx.ui.menu.Menu();
                
                //Close button
                var closeCmd = new skel.Command.Data.CommandDataCloseImage( items[i].layer );
                var closeButton = new qx.ui.menu.Button( "Close");
                closeButton.addListener( "execute", function(){
                    this.doAction( true, function(){} );
                }, closeCmd );
                contextMenu.add( closeButton );
                
                if ( visible ){
                    //Hide button
                    var hideCmd = new skel.Command.Data.CommandDataHideImage( items[i].layer );
                    var hideButton = new qx.ui.menu.Button( "Hide");
                    hideButton.addListener( "execute", function(){
                        this.doAction( true, function(){});
                    }, hideCmd );
                    contextMenu.add( hideButton );
                }
                else {
                    //Show button
                    var showCmd = new skel.Command.Data.CommandDataShowImage( items[i].layer );
                    var showButton = new qx.ui.menu.Button( "Show");
                    showButton.addListener( "execute", function(){
                        this.doAction( true, function(){});
                    }, showCmd );
                    contextMenu.add( showButton );
                }
                listItem.setContextMenu(contextMenu);
            }
        },

        m_list : null,
        m_dragMarker : null,
        m_drag : false,
        m_listContainer : null
    }
});