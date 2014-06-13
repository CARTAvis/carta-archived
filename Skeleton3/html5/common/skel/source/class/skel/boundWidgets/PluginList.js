/**
 * Author: Pavol Feder ( federl@gmail.com )
 *
 * Shows all plugins in a table.
 */

/*global qx, mImport */

/**
 @ignore( mImport)
 */


qx.Class.define("skel.boundWidgets.PluginList",
    {
        extend: qx.ui.container.Composite,

        /**
         * Constructor
         */
        construct: function () {
            this.m_connector = mImport( "connector");
            this.m_sharedVar = this.m_connector.getSharedVar( "/pluginList/stamp");
            this.m_sharedVar.addCB( this._sharedVarCB.bind(this));
            this.base(arguments, "");

            // create the UI
            this._createUI();

            // manually invoke the callback so that the label is immediately updated
            // with the current value
            this._sharedVarCB( this.m_sharedVar.get());
        },

        members: {

            m_sharedVar: null,
            m_connector: null,

            _createUI: function() {
                this.setLayout( new qx.ui.layout.Grow());

                var bogusData = [[ "png", "loads png data", "c++", "1.0.0"]];

                this.m_tableModel = new qx.ui.table.model.Simple();
                this.m_tableModel.setColumns([ "Name", "Description", "Type", "Version"]);
                this.m_tableModel.setData( bogusData);
                this.m_tableModel.setColumnEditable(1, false);
                this.m_tableModel.setColumnEditable(2, false);
                this.m_table = new qx.ui.table.Table( this.m_tableModel);

                this.add( this.m_table);
            },

            /**
             *
             * @param val
             * @private
             */
            _sharedVarCB: function( val) {
                var console = mImport( "console");
                console.log( "PluginList sharedvar = ", val);

                var newData = [];
                var n = parseInt( val);
                for( var i = 0 ; i < val ; i ++ ) {
                    var pf = "/pluginList/p" + i + "/";
                    var name = this.m_connector.getSharedVar( pf + "name" ).get();
                    var description = this.m_connector.getSharedVar( pf + "description" ).get();
                    var type = this.m_connector.getSharedVar( pf + "type" ).get();
                    var version = this.m_connector.getSharedVar( pf + "version" ).get();
                    newData.push( [ name, description, type, version ]);
                }

                this.m_tableModel.setData( newData);
            }

        }

    });

