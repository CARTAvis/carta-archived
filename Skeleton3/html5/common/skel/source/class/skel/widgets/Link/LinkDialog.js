/**
 * Allows the user to edit a link, specifying specific properties as being
 * linked or not.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Link.LinkDialog", {
    extend : qx.ui.popup.Popup,

    construct : function() {
        this.base(arguments);

        this._init();

    },

    members : {
        /**
         * Initializes the UI.
         */
        _init : function() {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);

            var colorMapCheck = new qx.ui.form.CheckBox("Colormap");
            colorMapCheck.setValue(true);
            this._add(colorMapCheck);

            var regionCheck = new qx.ui.form.CheckBox("Regions");
            regionCheck.setValue(true);
            this._add(regionCheck);

            var cursorCheck = new qx.ui.form.CheckBox("Cursor");
            cursorCheck.setValue(true);
            this._add(cursorCheck);

            var axesCheck = new qx.ui.form.CheckBox("Axes");
            axesCheck.setValue(true);
            this._add(axesCheck);

            var closeButton = new qx.ui.form.Button("Close");
            closeButton.addListener("execute", function() {
                this.hide();
            }, this);
            var buttonComposite = new qx.ui.container.Composite();
            var buttonLayout = new qx.ui.layout.HBox(2);
            buttonComposite.setLayout(buttonLayout);
            buttonComposite.add(new qx.ui.core.Spacer(), {
                flex : 1
            });
            buttonComposite.add(closeButton);

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