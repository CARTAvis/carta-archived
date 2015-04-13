/**
 * UI for setting a square layout of rows x cols.
 */

/**





 ************************************************************************ */

qx.Class.define("skel.widgets.CustomLayoutPopup", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);

        var rowLabel = new qx.ui.basic.Label("Row Count:");
        this.m_rowCountSpin = new qx.ui.form.Spinner();
        this.m_rowCountSpin.set({
            maximum : this.m_GRID_MAX,
            minimum : this.m_GRID_MIN
        });
        this.m_rowListenId =this.m_rowCountSpin.addListener(skel.widgets.Path.CHANGE_VALUE, this._fireRowEvent, this);

        var colLabel = new qx.ui.basic.Label("Column Count:");
        this.m_colCountSpin = new qx.ui.form.Spinner();
        this.m_colCountSpin.set({
            maximum : this.m_GRID_MAX,
            minimum : this.m_GRID_MIN
        });
        this.m_colListenId = this.m_colCountSpin.addListener(skel.widgets.Path.CHANGE_VALUE, this._fireColEvent, this);
        
        var gridContainer = new qx.ui.container.Composite();
        gridContainer.setLayout(new qx.ui.layout.Grid());
        gridContainer.add(rowLabel, {
            row : 0,
            column : 0
        });
        gridContainer.add(this.m_rowCountSpin, {
            row : 0,
            column : 1
        });
        gridContainer.add(colLabel, {
            row : 1,
            column : 0
        });
        gridContainer.add(this.m_colCountSpin, {
            row : 1,
            column : 1
        });
        this._setLayout( new qx.ui.layout.VBox(2));
        this._add( gridContainer);
        
        var butContainer = new qx.ui.container.Composite();
        butContainer.setLayout( new qx.ui.layout.HBox());
        butContainer.add( new qx.ui.core.Spacer(1), {flex:1});
        var closeButton = new qx.ui.form.Button( "Close");
        closeButton.addListener( "execute", function(){
            this.fireDataEvent("closeCustomLayout", "");
        }, this);
        butContainer.add( closeButton );
        this._add( butContainer);
        
       
    },

    events : {
        "closeCustomLayout" : "qx.event.type.Data",
        "layoutRowCount" : "qx.event.type.Data",
        "layoutColCount" : "qx.event.type.Data"
    },

    members : {
        
        /**
         * Notification that the column count has changed.
         */
        _fireColEvent : function(){
            var data = this.m_colCountSpin.getValue();
            this.fireDataEvent( "layoutColCount", data );
        },
        
        /**
         * Notification that the row count has changed.
         */
        _fireRowEvent : function(){
            var data = this.m_rowCountSpin.getValue();
            this.fireDataEvent( "layoutRowCount", data );
        },
        
        /**
         * Set the current number of rows and columns in the layout.
         * @param rows {Number} the current layout row count.
         * @param cols {Number} the current layout column count.
         */
        setGridSize : function( rows, cols ){
            this.m_rowCountSpin.removeListenerById( this.m_rowListenId );
            this.m_colCountSpin.removeListenerById( this.m_colListenId );
            this.m_rowCountSpin.setValue( rows );
            this.m_colCountSpin.setValue( cols );
            this.m_colListenId = this.m_colCountSpin.addListener(skel.widgets.Path.CHANGE_VALUE, this._fireColEvent, this);
            this.m_rowListenId = this.m_rowCountSpin.addListener(skel.widgets.Path.CHANGE_VALUE, this._fireRowEvent, this);
        },
        
        m_GRID_MAX : 100,
        m_GRID_MIN : 1,
        m_rowCountSpin : null,
        m_colCountSpin : null,
        m_colListenId : null,
        m_rowListenId : null
    },

    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }

    }

});
