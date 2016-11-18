/**
 * Displays an editable list of text items.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.CustomUI.ItemTable", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( columnName, columnWidth ) {
        this.base(arguments);
        this._init( columnName, columnWidth );
    },
    
    events : {
        "itemsChanged" : "qx.event.type.Data",
        "itemsSelected" : "qx.event.type.Data"
    },

    members : {
        
        /**
         * Returns a list of all text items.
         * @return {Array} a list of all text items.
         */
        getAll : function(){
            var items = [];
            var rowCount = this.m_tableModel.getRowCount();
            for ( var i = 0; i < rowCount; i++ ){
                var itemValue = this.m_tableModel.getValue( 0, i );
                items.push( itemValue );
            }
            return items;
        },
        
        /**
         * Return the index of the first item the user selected in the list.
         * @return {Number} - the index of the first selected item.
         */
        getSelectedIndex : function(){
            var index = 0;
            var selectModel = this.m_table.getSelectionModel();
            var map = selectModel.getSelectedRanges();
            if ( map.length > 0 ){
                index = map[0].minIndex;
            }
            return index;
        },
        
        /**
         * Returns the indices of the items that have been selected.
         * @return {Array} - a list of indices of selected items.
         */
        getSelectedIndices : function(){
        	var indices = [];
        	 var index = 0;
             var selectModel = this.m_table.getSelectionModel();
             var map = selectModel.getSelectedRanges();
             for ( var i = 0; i < map.length; i++ ){
                 var min = map[i].minIndex;
                 var max = map[i].maxIndex;
                 for ( var j = min; j<=max; j++ ){
                	 indices.push( j );
                 }
             }
        	return indices;
        },
        
        /**
         * Returns a list of user selected text items.
         * @return {Array} a list of text items the user has selected.
         */
        getSelected : function(){
            var items = [];
            var selectModel = this.m_table.getSelectionModel();
            var map = selectModel.getSelectedRanges();
            for ( var i = 0; i < map.length; i++ ){
                var minValue = map[i].minIndex;
                var maxValue = map[i].maxIndex;
                for ( var j = minValue; j <= maxValue; j++ ){
                    var itemValue = this.m_tableModel.getValue( 0, j );
                    items.push( itemValue );
                }
            }
            return items;
        },
        
        /**
         * Initializes the UI.
         */
        _init : function( columnName, columnWidth ) {
            
            this._setLayout( new qx.ui.layout.VBox(2));
            
            this.m_tableModel = new qx.ui.table.model.Simple();
            this.m_tableModel.setColumns( [columnName] );
            this.m_tableModel.setColumnEditable( 0, true );
            
            this.m_table = new qx.ui.table.Table( this.m_tableModel );
            this.m_table.setColumnVisibilityButtonVisible( false );
            this.m_table.setColumnWidth( 0, columnWidth );
            this.m_table.setHeight( 150 );
            this.m_table.setStatusBarVisible( false );
            this.m_table.addListener( "dataEdited", this._itemsChanged, this);
            
            var selectModel = this.m_table.getSelectionModel();
            selectModel.addListener( "changeSelection", function(evt){
                var selectModel = this.m_table.getSelectionModel();
                var index = selectModel.getLeadSelectionIndex();
                var itemValue = this.m_tableModel.getValue( 0, index );
                var data = {
                        item: itemValue
                };
                this.fireDataEvent("itemsSelected", data );
            }, this );
            selectModel.setSelectionMode( qx.ui.table.selection.Model.MULTIPLE_INTERVAL_SELECTION );
            this._add( this.m_table );
        },
        
        
        /**
         * Notify listeners that the list of text items in the set has changed.
         */
        _itemsChanged : function(){
            this.fireDataEvent( "itemsChanged", null );
        },
        
        /**
         * Update the UI based on server-side information about the text items
         * that are available in the contour set.
         * @param items {Array} - a list of text items.
         */
        setItems : function( items ){
            //Only reset the items if they have changed.
            var itemsChanged = false;
            if ( this.m_items === null || (items.length != this.m_items.length) ){
                itemsChanged = true;
            }
            else {
                for ( var i = 0; i < items.length; i++ ){
                    if ( items[i] != this.m_items[i]){
                        itemsChanged = true;
                        break;
                    }
                }
            }
            if ( itemsChanged ){
                this.m_items = items;
                
                var rowArray = [];
                for ( var i = 0; i < this.m_items.length; i++ ){
                    var row = [];
                    row.push( this.m_items[i].toString());
                    rowArray.push( row );
                }
                this.m_tableModel.setData( rowArray );
                this.m_table.setTableModel( this.m_tableModel );
                var selectModel = this.m_table.getSelectionModel();
                /*if ( rowArray.length > 0 ){
                	console.log( "Setting 0 selected");
                    selectModel.setSelectionInterval( 0, 0 );
                }*/
            }
        },
        
        /**
         * Set the indices of the table rows that should be selected.
         * @param selectedIndices {Array} - a list of table rows that should be
         *      selected.
         */
        setSelected : function( selectedIndices ){
            var selectedCount = selectedIndices.length;
            var selectModel = this.m_table.getSelectionModel();
            var firstInterval = true;
            if ( selectedCount > 0 ){
                var start = selectedIndices[0];
                var end = start;
                for ( var index = 0; index < selectedCount; index++ ){
                    while ( index + 1 < selectedCount ){
                        if ( selectedIndices[index+1] - 1 == end ){
                            end = selectedIndices[index+1];
                            index++;
                        }
                        else {
                            //Add the selection interval we have
                            if ( firstInterval ){
                                selectModel.setSelectionInterval( start, end );
                                firstInterval = false;
                            }
                            else {
                                selectModel.addSelectionInterval( start, end );
                            }
                            start = selectedIndices[index+1];
                            end = start;
                            break;
                        }
                    }
                }
                //Add the last selection interval
                if ( firstInterval ){
                    selectModel.setSelectionInterval( start, end );
                }
                else {
                    selectModel.addSelectionInterval( start, end );
                }
            }
        },
        
        setTestId : function( id ){
            skel.widgets.TestID.addTestId( this.m_table, id ); 
        },
        
        
        /**
         * Set a test name for the item table based on the name of the parent contour set.
         * @param name {String} - the name of the parent application.
         */
        setName : function( name ){
            var testName = "itemList"+name;
            skel.widgets.TestID.addTestId( this.m_table, testName );
        },
        
        m_items : null,
        m_table : null,
        m_tableModel : null
    }
});