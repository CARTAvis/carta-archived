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
        
        var tableModel = new qx.ui.table.model.Simple();
        var path = skel.widgets.Path.getInstance();
        var colArray = ["Name", "Date Created", path.STATE_LAYOUT, path.STATE_PREFERENCES, path.STATE_SESSION];
        tableModel.setColumns( colArray);
        var i = 1;
        for ( i = 1; i <= colArray.length; i++ ){
            tableModel.setColumnEditable(i, false );
        }
        
        this.setTableModel( tableModel );
        this.set({
            width: 300,
            height: 400
        });
        this.getSelectionModel().setSelectionMode( qx.ui.table.selection.Model.SINGLE_SELECTION);
        
        //Display checkboxes in the last column
        var tcm = this.getTableColumnModel();
        for ( i = 3; i <= colArray.length; i++ ){
            tcm.setDataCellRenderer(3, new qx.ui.table.cellrenderer.Boolean());
        }
        
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
            this.getTableModel().addRows( tableRow );
        },
        
        /**
         * Remove all snapshots from the table.
         */
        clear : function(){
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
            var selectionIndex = selectionRanges[0].minIndex;
            var dataModel = this.getTableModel();
            var selectedData = dataModel.getRowData( selectionIndex );
            var name = selectedData[0];
            return name;
        }
    }
});
