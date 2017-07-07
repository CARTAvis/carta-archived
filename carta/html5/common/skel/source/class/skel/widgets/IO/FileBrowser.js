/**
 * Displays a file browser that allows the user to load a file.
 */

qx.Class.define("skel.widgets.IO.FileBrowser", {
    extend : qx.ui.core.Widget,
    include : skel.widgets.IO.FileTreeMixin,

    construct : function() {
        this.base(arguments);
        this._initTree();
        this._init();
        this._initData();
    },

    members : {


        /**
         * Initializes the UI.
         */
        _init : function() {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);

            var closeButton = new qx.ui.form.Button("Close");
            closeButton.setToolTipText( "Close this dialog.");
            skel.widgets.TestID.addTestId( closeButton, "closeFileLoadButton");
            closeButton.addListener("execute", function() {
                var errorMan = skel.widgets.ErrorHandler.getInstance();
                errorMan.clearErrors();
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "closeFileBrowser", ""));
            });
            var loadButton = new qx.ui.form.Button("Load");
            loadButton.setToolTipText( "Load the selected file.");
            skel.widgets.TestID.addTestId( loadButton, "loadFileButton");
            loadButton.addListener("execute", function() {
                
                var filePath = this.getSelectedPath();
                if (this.m_tree.m_target !== null) {
                    this.m_tree.m_target.dataLoaded(filePath);
                }

            }, this);
            var buttonComposite = new qx.ui.container.Composite();
            var buttonLayout = new qx.ui.layout.HBox(2);
            buttonComposite.setLayout(buttonLayout);
            buttonComposite.add(new qx.ui.core.Spacer(), {
                flex : 1
            });
            buttonComposite.add(loadButton);
            buttonComposite.add(closeButton);

            this._add(this.m_treeDisplay);
            this._add(buttonComposite);
        },



        /**
         * Stores the window that wants to add data.
         * @param source {String} an identifier for the window that needs to load data.
         */
        setTarget : function(source) {
            this.m_tree.m_target = source;
        },


        m_target : null

    },
    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }

    }

});
