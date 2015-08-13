/**
 * Displays an editable list of contour levels.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Contour.LevelTable", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this._init( );
    },
    
    events : {
        "levelsChanged" : "qx.event.type.Data",
        "levelSelected" : "qx.event.type.Data"
    },

    members : {
        
        /**
         * Returns a list of user selected contour levels.
         * @return {Array} a list of contour levels the user has selected.
         */
        getSelectedLevels : function(){
            var levels = [];
            var selectModel = this.m_table.getSelectionModel();
            var map = selectModel.getSelectedRanges();
            for ( var i = 0; i < map.length; i++ ){
                var minValue = map[i].minIndex;
                var maxValue = map[i].maxIndex;
                for ( var j = minValue; j <= maxValue; j++ ){
                    var levelValue = this.m_tableModel.getValue( 0, j );
                    levels.push( levelValue );
                }
            }
            return levels;
        },
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            
            this._setLayout( new qx.ui.layout.VBox(2));
            
            this.m_table = new qx.ui.table.Table();
            this.m_table.setColumnVisibilityButtonVisible( false );
            this.m_table.setStatusBarVisible( false );
            this.m_table.set( {width:25, height:175});
            this.m_table.addListener( "dataEdited", this._levelsChanged, this);
            
            this.m_tableModel = new qx.ui.table.model.Simple();
            this.m_tableModel.setColumns(["Level"]);
            this.m_tableModel.setColumnEditable( 0, true );
            
            var selectModel = this.m_table.getSelectionModel();
            selectModel.addListener( "changeSelection", function(evt){
                var selectModel = this.m_table.getSelectionModel();
                var index = selectModel.getLeadSelectionIndex();
                var levelValue = this.m_tableModel.getValue( 0, index );
                var data = {
                        level: levelValue
                };
                this.fireDataEvent("levelSelected", data );
            }, this );
            selectModel.setSelectionMode( qx.ui.table.selection.Model.SINGLE_SELECTION );
            
            
            this.m_table.setTableModel( this.m_tableModel );
            
            
            var butContainer = new qx.ui.container.Composite();
            butContainer.setLayout( new qx.ui.layout.HBox(2));
            var addButton = new qx.ui.form.Button( "Add");
            addButton.addListener( "execute", function(evt){
                var newRow = [[""]];
                this.m_tableModel.addRows( newRow );
            }, this );
            var deleteButton = new qx.ui.form.Button( "Delete");
            deleteButton.addListener( "execute", function(evt){
                var selectionModel = this.m_table.getSelectionModel();
                var index = selectionModel.getLeadSelectionIndex();
                this.m_tableModel.removeRows( index, 1 );
                this._levelsChanged();
            }, this );
            butContainer.add( addButton );
            butContainer.add( deleteButton );
            
            this._add( this.m_table );
            this._add( butContainer );
        },
        
        _levelsChanged : function(){
            /*var tableArray = this.m_tableModel.getData();
            var dataArray = [];
            for ( var i = 0; i < tableArray.length; i++ ){
                dataArray = dataArray.concat( tableArray[i] );
            }
            var data = {
                levels : dataArray
            };
            console.log( "levels changed");*/
            this.fireDataEvent( "levelsChanged", null );
        },
        
        /**
         * Update the UI based on server-side information about the contour levels
         * that are available in the contour set.
         * @param levels {Array} - a list of contour levels.
         */
        setLevels : function( levels ){
            
            this.m_levels = levels;
            
            var rowArray = [];
            for ( var i = 0; i < this.m_levels.length; i++ ){
                var row = [];
                row.push( this.m_levels[i]);
                rowArray.push( row );
            }
            this.m_tableModel.setData( rowArray );
            var selectModel = this.m_table.getSelectionModel();
            selectModel.setSelectionInterval( 0, 0 );
        },

        m_levels : null,
        m_table : null,
        m_tableModel : null
    }
});