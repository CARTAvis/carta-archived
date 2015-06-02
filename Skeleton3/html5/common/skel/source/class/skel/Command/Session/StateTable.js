/**
 * Table for displaying state.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/
qx.Class.define("skel.Command.Session.StateTable", {
    extend : qx.ui.table.Table,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        
        //Data Model
        var tableModel = new qx.ui.table.model.Simple();
        var path = skel.widgets.Path.getInstance();
        var colArray = ["Name", "Date Created", path.STATE_LAYOUT, path.STATE_PREFERENCES, 
                        path.STATE_SESSION];
        tableModel.setColumns( colArray);
        var i = 0;
        for ( i = 0; i < colArray.length; i++ ){
            tableModel.setColumnEditable(i, false );
        }
        this.setTableModel( tableModel );
        
        
        //Selection Model
        var selectModel = this.getSelectionModel();
        selectModel.setSelectionMode( qx.ui.table.selection.Model.SINGLE_SELECTION);
        selectModel.addListener( "changeSelection", function(){
            this.fireDataEvent( "selectionChanged", "");
        }, this );
        
        //Display checkboxes in the last column
        var tcm = this.getTableColumnModel();
        for ( i = 2; i < colArray.length; i++ ){
            tcm.setDataCellRenderer(i, new qx.ui.table.cellrenderer.Boolean());
        }
    },
    
    events : {
        "selectionChanged" : "qx.event.type.Data"
    },
    
    members : {
        
        /**
         * Add a snapshot to the display.
         * @param name {String} the name of the snapshot.
         * @param layout {boolean} true if the layout state was saved; false otherwise.
         * @param preferences {boolean} true if the preference state was saved; false otherwise.
         * @param session {boolean} true if everything was saved; false otherwise.
         */
        addSnapshot : function( name, createDate, layout, preferences, session ){
            var tableRow=[];
            tableRow[0] = [];
            tableRow[0].push( name );
            tableRow[0].push( createDate);
            tableRow[0].push( layout );
            tableRow[0].push( preferences );
            tableRow[0].push( session );
            var tableModel = this.getTableModel();
            tableModel.addRows( tableRow );
            if ( this.m_selectedName === null || this.m_selectedName === name ){
                var selectModel = this.getSelectionModel();
                var rowCount = tableModel.getRowCount();
                selectModel.setSelectionInterval( rowCount - 1, rowCount - 1);
            }
        },
        
        /**
         * Remove all snapshots from the table.
         */
        clear : function(){
            this.m_selectedName = this.getSnapshotName();
            var tableModel = this.getTableModel();
            var rowCount = tableModel.getRowCount();
            tableModel.removeRows( 0, rowCount, true );
        },
        
        /**
         * Return the name of the selected snapshot.
         * @return {String} the name of the selected snapshot.
         */
        getSnapshotName : function(){
            var selectionModel = this.getSelectionModel();
            var selectionRanges = selectionModel.getSelectedRanges();
            var name = null;
            if ( selectionRanges.length > 0 ){
                var selectionIndex = selectionRanges[0].minIndex;
                var dataModel = this.getTableModel();
                var selectedData = dataModel.getRowData( selectionIndex );
                name = selectedData[0];
            }
            return name;
        },
        
        /**
         * Set the name of the snapshot to be selected.
         */
        setSelectedName : function( name ){
            if ( name.length > 0 ){
                this.m_selectedName = name;
            }
        },
        
        m_selectedName : null
    }
});
