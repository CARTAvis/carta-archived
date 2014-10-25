/**
 * UI for setting a square layout of rows x cols.
 */

/**





 ************************************************************************ */

qx.Class.define("skel.widgets.CustomLayoutPopup", {
    extend : qx.ui.popup.Popup,

    /**
     * Constructor
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

        this.setLayout(new qx.ui.layout.Grid());
        this.add(rowLabel, {
            row : 0,
            column : 0
        });
        this.add(this.m_rowCountSpin, {
            row : 0,
            column : 1
        });
        this.add(colLabel, {
            row : 1,
            column : 0
        });
        this.add(this.m_colCountSpin, {
            row : 1,
            column : 1
        });
        
        
    },

    events : {
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
