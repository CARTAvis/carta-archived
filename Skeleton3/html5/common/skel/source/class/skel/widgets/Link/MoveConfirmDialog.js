/**
 * Allows the user to confirm/cancel a window move.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Link.MoveConfirmDialog", {
    extend : qx.ui.popup.Popup,
    
    /**
     * Constructor.
     */
    construct : function() {
        this.base(arguments);

        this._init();

    },
    
    events : {
        "moveConfirmed" : "qx.event.type.Data",
        "moveCancelled" : "qx.event.type.Data"
    },

    members : {
        /**
         * Initializes the UI.
         */
        _init : function() {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
            
            var directionLabel = new qx.ui.basic.Label( "Replace the destination window (black) with the source window( blue)?");
            this._add( directionLabel );
            
            var cancelButton = new qx.ui.form.Button("Cancel");
            cancelButton.addListener("execute", function() {
                this.hide();
                this.fireDataEvent( "moveCancelled", null );
            }, this);
            
            var okButton = new qx.ui.form.Button( "OK");
            okButton.addListener( "execute", function(){
                this.hide();
                this.fireDataEvent( "moveConfirmed", null );
            }, this );
            
            var buttonComposite = new qx.ui.container.Composite();
            var buttonLayout = new qx.ui.layout.HBox(2);
            buttonComposite.setLayout(buttonLayout);
            buttonComposite.add(new qx.ui.core.Spacer(), {
                flex : 1
            });
            buttonComposite.add( okButton );
            buttonComposite.add(cancelButton);

            this._add(buttonComposite);
        }

    },
    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }

    }

});