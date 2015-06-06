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
        skel.widgets.TestID.addTestId( this.m_rowCountSpin, "customLayoutRows");
        this.m_rowCountSpin.set({
            maximum : this.m_GRID_MAX,
            minimum : this.m_GRID_MIN
        });
        this.m_rowListenId =this.m_rowCountSpin.addListener(skel.widgets.Path.CHANGE_VALUE, this._fireSizeEvent, this);

        var colLabel = new qx.ui.basic.Label("Column Count:");
        this.m_colCountSpin = new qx.ui.form.Spinner();
        skel.widgets.TestID.addTestId( this.m_colCountSpin, "customLayoutCols");
        this.m_colCountSpin.set({
            maximum : this.m_GRID_MAX,
            minimum : this.m_GRID_MIN
        });
        this.m_colListenId = this.m_colCountSpin.addListener(skel.widgets.Path.CHANGE_VALUE, this._fireSizeEvent, this);
        
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
        skel.widgets.TestID.addTestId( closeButton, "customLayoutClose");
        closeButton.addListener( "execute", function(){
            this.fireDataEvent("closeCustomLayout", "");
        }, this);
        butContainer.add( closeButton );
        this._add( butContainer);
        
       
    },

    events : {
        "closeCustomLayout" : "qx.event.type.Data",
        "layoutSizeChanged" : "qx.event.type.Data"
    },

    members : {
        
        /**
         * Notification that the layout size has changed.
         */
        _fireSizeEvent : function(){
            var data = {
                    rows : this.m_rowCountSpin.getValue(),
                    cols : this.m_colCountSpin.getValue()
            };
            this.fireDataEvent( "layoutSizeChanged", data );
        },
        
        
        /**
         * Set the current number of rows and columns in the layout.
         * @param rows {Number} the current layout row count.
         * @param cols {Number} the current layout column count.
         */
        setGridSize : function( rows, cols ){
            if ( rows !== null ){
                this.m_rowCountSpin.removeListenerById( this.m_rowListenId );
                this.m_rowCountSpin.setValue( rows );
                this.m_rowListenId = this.m_rowCountSpin.addListener(skel.widgets.Path.CHANGE_VALUE, this._fireSizeEvent, this);
            }
            if ( cols !== null ){
                this.m_colCountSpin.removeListenerById( this.m_colListenId );
                this.m_colCountSpin.setValue( cols );
                this.m_colListenId = this.m_colCountSpin.addListener(skel.widgets.Path.CHANGE_VALUE, this._fireSizeEvent, this);
            }
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
