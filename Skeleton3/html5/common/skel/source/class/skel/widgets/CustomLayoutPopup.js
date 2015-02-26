/**
 * UI for setting a square layout of rows x cols.
 */

/**





 ************************************************************************ */

qx.Class.define("skel.widgets.CustomLayoutPopup", {
    extend : qx.ui.popup.Popup,

    /**
     * Constructor
     * @param rows {Number} the current layout row count.
     * @param cols {Number} the current layout column count.
     */
    construct : function( rows, cols ) {
        this.base(arguments);

        var rowLabel = new qx.ui.basic.Label("Row Count:");
        this.m_rowCountSpin = new qx.ui.form.Spinner();
        this.m_rowCountSpin.set({
            maximum : this.m_GRID_MAX,
            minimum : this.m_GRID_MIN,
            value : rows
        });
        this.m_rowCountSpin.addListener("changeValue", function() {
            this.fireDataEvent("rowCount", this.m_rowCountSpin.getValue());
        }, this);

        var colLabel = new qx.ui.basic.Label("Column Count:");
        this.m_colCountSpin = new qx.ui.form.Spinner();
        this.m_colCountSpin.set({
            maximum : this.m_GRID_MAX,
            minimum : this.m_GRID_MIN,
            value : cols
        });
        this.m_colCountSpin.addListener("changeValue", function() {
            this.fireDataEvent("colCount", this.m_colCountSpin.getValue());
        }, this);
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
        this.setLayout( new qx.ui.layout.VBox(2));
        this.add( gridContainer);
        
        var butContainer = new qx.ui.container.Composite();
        butContainer.setLayout( new qx.ui.layout.HBox());
        butContainer.add( new qx.ui.core.Spacer(1), {flex:1});
        var closeButton = new qx.ui.form.Button( "Close");
        closeButton.addListener( "execute", function(){
            this.fireDataEvent("closeCustomLayout", "");
        }, this);
        butContainer.add( closeButton );
        this.add( butContainer);
    },

    events : {
        "closeCustomLayout" : "qx.event.type.Data",
        "rowCount" : "qx.event.type.Data",
        "colCount" : "qx.event.type,Data"
    },

    members : {
        m_GRID_MAX : 100,
        m_GRID_MIN : 1,
        m_rowCountSpin : null,
        m_colCountSpin : null
    },

    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }

    }

});
