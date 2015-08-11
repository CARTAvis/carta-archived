/**
 * Displays a file browser that allows the user to specify a save location.
 */


qx.Class.define("skel.widgets.IO.SaveBrowser", {
    extend : qx.ui.core.Widget,
    include : skel.widgets.IO.FileTreeMixin,

    construct : function() {
        this.base(arguments);
        this._initTree();
        this._init();
        this._initData();
        this._setSaveFile( true );
    },
    

    members : {
        
        /**
         * Close this dialog.
         */
        _close : function(){
            var errorMan = skel.widgets.ErrorHandler.getInstance();
            errorMan.clearErrors();
            qx.event.message.Bus.dispatch(new qx.event.message.Message(
                    "cancelSaveBrowser", ""));
        },

        /**
         * Initializes the UI.
         */
        _init : function() {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
           
            var cancelButton = new qx.ui.form.Button("Close");
            cancelButton.setToolTipText( "Close this dialog");
            skel.widgets.TestID.addTestId( cancelButton, "cancelSaveBrowserButton");
            cancelButton.addListener("execute", this._close, this);
            var okButton = new qx.ui.form.Button("OK");
            okButton.setToolTipText( "Save to the specified file and directory");
            skel.widgets.TestID.addTestId( okButton, "okSaveBrowserButton");
            okButton.addListener("execute", function() {
                //Notify that a file has been selected
                var fullPath = this.getSelectedPath();
                this.m_target.fileChanged( fullPath );
                //Close this directory browser
                this._close();
            }, this);
            var buttonComposite = new qx.ui.container.Composite();
            var buttonLayout = new qx.ui.layout.HBox(2);
            buttonComposite.setLayout(buttonLayout);
            buttonComposite.add(new qx.ui.core.Spacer(), {
                flex : 1
            });
            buttonComposite.add(okButton);
            buttonComposite.add(cancelButton);

            this._add(this.m_treeDisplay);
            this._add( buttonComposite );
        },
        


        /**
         * Stores the window that wants to add data.
         * @param source {String} an identifier for the window that needs to load data.
         */
        setTarget : function(source) {
            this.m_target = source;
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