/**
 * Grid layout that supports drag and drop.
 */


qx.Class.define("skel.widgets.Statistics.DragDropGrid", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( label ) {
        this.base(arguments);
        this.m_type = label;
        this._init( );
    },
    
    events: {
        "orderChange" : "qx.event.type.Data",
        "statMoved" : "qx.event.type.Data"
    },
    

    members : {
        
        /**
         * Move the widget at the source row and column to the destination row and
         * column.
         * @param sourceRow {Number} - current row index of the widget.
         * @param sourceCol {Number} - current column index of the widget.
         * @param destRow {Number} - row index where the widget should be moved.
         * @param destCol {Number} - column index where the widget should be moved.
         */
        _changeWidgetOrder : function( sourceRow, sourceCol, destRow, destCol ){
            var origIndex = this._getIndex(sourceRow, sourceCol);
            var destIndex = this._getIndex(destRow, destCol );
            var target = this.m_widgets[origIndex];
            var count = this.m_widgets.length;
            
            //Move all the widgets up to the target index up by one.
            if ( origIndex != destIndex ){
                var data = {
                    type : this.m_type,
                    originalIndex : origIndex,
                    moveIndex : destIndex
                };
                this.fireDataEvent( "statMoved", data );
            }
           
        },
        
        /**
         * Update the drag indicator location based on the drag event.
         * @param ev {Object} - information about the current position of the
         *      mouse cursor.
         */
        _drag : function( ev ){
            var data = ev.getData();
            this.m_dragItem.setDomPosition( data.posX, data.posY );
        },
        
        /**
         * Hide the drag indicator at the end of a drag as well as
         * deselecting all widgets.
         */
        _dragEnd : function(){
            this.m_dragItem.setDomPosition( -1000, -1000 );
            this.m_dragItem.setValue( "");
            var itemCount = this.m_widgets.length;
            for ( var i = 0; i < itemCount; i++ ){
                this.m_widgets[i].setSelected( false );
            }
        },
        
        
        /**
         * A drag has finished; collate information to change the widget
         * order, if appropriate.
         */
        _dragFinish : function(){
            if ( this.m_dragItem.getValue() != ""){
                for ( var i = 0; i < this.m_widgets.length; i++ ){
                    var mouseOver= this.m_widgets[i].isMouseOver();
                    if ( mouseOver ){
                        var destRow = this.m_widgets[i].getRow();
                        var destCol = this.m_widgets[i].getCol();
                        this._changeWidgetOrder( this.m_dragSourceRow, this.m_dragSourceCol,
                                destRow, destCol);
                        this._dragEnd();
                        break;
                    }
                    
                }
            }
        },
        
        /**
         * Update the drag indicator with information about the widget
         * being dragged.
         * @param ev {Object} - information about the object being dragged.
         */
        _dragStart : function( ev ){
            var data = ev.getData();
            this.m_dragSourceRow = data.row;
            this.m_dragSourceCol = data.col;
            this.m_dragItem.setValue( data.title);
        },
        
        
        /**
         * Compute the list index of the widget at the given row and column.
         * @param row {Number} - a row index.
         * @param column {Number} - a column index.
         */
        _getIndex : function( row, column ){
            return row * this.m_colCount + column;
        },
        
        
        /*
         * Initializes the UI.
         */
        _init : function( width ) {
            this.setPadding( 5, 5, 5, 5 );
            this.setBackgroundColor( "white");
            var gridLayout = new qx.ui.layout.Grid(5,5);
            this._setLayout( gridLayout );
            
            this.m_dragItem = new qx.ui.basic.Label("");
            this.m_dragItem.setZIndex( 500 );
            this.m_dragItem.setLayoutProperties( {left:-1000, top:-1000});
            qx.core.Init.getApplication().getRoot().add( this.m_dragItem );
        },
        
        
        /**
         * Update the layout.
         */
        _layout : function(){
            this._removeAll();
            var itemCount = this.m_widgets.length;
            var rowIndex = 0;
            var colIndex = 0;
            for ( var i = 0; i < itemCount; i++ ){
                this.m_widgets[i].setRow( rowIndex );
                this.m_widgets[i].setCol( colIndex );
                this._add( this.m_widgets[i], {row:rowIndex, column:colIndex});
                colIndex++;
                if ( colIndex == this.m_colCount ){
                    rowIndex++;
                    colIndex = 0;
                }
            }
        },
        
        
        /**
         * Generate a new UI widget.
         * @param label {String} - the string the widget should display.
         * @param visible {boolean} - true if the widget should be displayed; false
         *      otherwise.
         */
        _makeCheckableWidget : function( label, visible ){
            var checkWidget = new skel.widgets.Statistics.CheckableWidget();
            checkWidget.setLabel( label );
            checkWidget.setStatType( this.m_type );
            checkWidget.setValue( visible );
            checkWidget.addListener( "dragStart", this._dragStart, this );
            checkWidget.addListener( "dragging", this._drag, this );
            checkWidget.addListener( "dragEnd", this._dragFinish, this );
            checkWidget.setId( this.m_id );
            return checkWidget;
        },
        
        
        /**
         * Set the number of columns in the grid.
         * @param count {Number} - the number of columns in the grid.
         */
        setColCount : function( count ){
            this.m_colCount = count;
        },
        
        /**
         * Set the grid items enabled/disabled.
         * @param enable {boolean} - true to enable all the widgets;
         *      false otherwise.
         */
        setGridEnabled : function( enable ){
            this.m_enabled = enable;
            if ( this.m_widgets !== null ){
                for ( var i = 0; i < this.m_widgets.length; i++ ){
                    this.m_widgets[i].setCheckEnabled( enable );
                }
            }
        },
        
        
        /**
         * Set the server-side id of the statistics object.
         * @param id {String} - the id of the server-side id of the statistics object.
         */
        setId : function( id ){
            this.m_id = id;
            if ( this.m_widgets !== null ){
                for ( var i = 0; i < this.m_widgets.length; i++ ){
                    this.m_widgets[i].setId( this.m_id );
                }
            }
        },
        
        
        /**
         * Update the UI with a new list of labels to display.
         * @param labels {Array} - the new list of labels to display.
         */
        setLabels : function( labels ){
            this.m_widgets = [];
            var itemCount = labels.length;
            var nameLabel = "Name";
            //The items that are coming in are ordered.  We first make UI elements
            //out of those that are visible and add the invisible ones at the end.
            var j = 0;
            for ( var i = 0; i < itemCount; i++ ){
                if ( labels[i].label !== nameLabel && labels[i].visible ){
                    this.m_widgets[j] = this._makeCheckableWidget( labels[i].label, labels[i].visible );
                    j++;
                }
            }
            var baseIndex = this.m_widgets.length;
            j = 0;
            //Now we make UI items out of those that are not visible.
            for ( var i = 0; i < itemCount; i++ ){
                if ( labels[i].label !== nameLabel && !labels[i].visible ){
                    var widgetIndex = baseIndex + j;
                    this.m_widgets[widgetIndex] = this._makeCheckableWidget( labels[i].label, labels[i].visible );
                    j++;
                    this.m_widgets[widgetIndex].setCheckEnabled( this.m_enabled );
                }
            }
            this._layout();
        },
       
        
        m_colCount : 3,
        m_enabled : true,
        m_dragItem : null,
        m_dragSourceRow : null,
        m_dragSourceCol : null,
        m_id : null,
        m_type : null,
        m_widgets : null

    }
});