/* *
 * Author: Pavol Feder ( federl@gmail.com )
 *
 * Shows all plugins in a table.
 */

/* global qx, mImport */

/**
 * Widget for displaying all plugins.
 *
 * @ignore( mImport)
 */

qx.Class.define("skel.boundWidgets.PluginList", {
    extend : qx.ui.container.Composite,

    /**
     * Constructor.
     * @param pluginId {String} an identifier indicating this widget displays all available plugins.
     */
    construct : function( pluginId ) {
        this.base(arguments, "");

        // create the UI
        this._createUI();

        // Invoke the callback so that the plugin list will be populated.
        this.m_connector = mImport( "connector");
        var paramMap = "pluginId:" + pluginId + ",index:0";
        var pathDict = skel.widgets.Path.getInstance();
        var regViewCmd = pathDict.getCommandRegisterView();
        this.m_connector.sendCommand( regViewCmd, paramMap, this._registrationCallback( this ) );
    },

    members : {

        m_sharedVar : null,
        m_connector : null,

        /**
         * Create the user interface.
         * @private
         */
        _createUI : function() {
            this.setLayout(new qx.ui.layout.Grow());

            this.m_tableModel = new qx.ui.table.model.Simple();
            this.m_tableModel.setColumns([ "Name", "Description", "Type",
                    "Version", "Loaded" ]);
            this.m_tableModel.setData([]);

            var custom = {
                tableColumnModel : function(obj) {
                    return new qx.ui.table.columnmodel.Resize(obj);
                }
            };

            this.m_table = new qx.ui.table.Table(this.m_tableModel, custom);

            var colModel = this.m_table.getTableColumnModel();
            colModel.setDataCellRenderer(4,
                    new qx.ui.table.cellrenderer.Boolean());

            this.add(this.m_table);
        },
        
        /**
         * Callback for when the id of the object containing information about the
         * plugins has been received; initialize the shared variable and add a CB to it.
         * @param anObject {PluginList}.
         * @return {Function} the registration callback.
         */
        _registrationCallback : function( anObject ){
            return function( id ){
                anObject.m_sharedVar = anObject.m_connector.getSharedVar( id );
                anObject.m_sharedVar.addCB( anObject._sharedVarCB.bind( this ));
                anObject._sharedVarCB( anObject.m_sharedVar.get());
            };
        },

        /**
         * Shared var callback. Read in the contents of the plugin list and render it.
         * @param val {String} Number of plugins.
         * @private
         */
        _sharedVarCB : function(val) {
            if ( val ){
                try {
                    var plugins = JSON.parse( val );
                    var n = plugins.pluginCount;
                    var newData=[];
                    for (var i = 0; i < n; i++) {
                        var name = plugins.pluginList[i].name;
                        var description = plugins.pluginList[i].description;
                        var type = plugins.pluginList[i].type;
                        var version = plugins.pluginList[i].version;
                        var errors = plugins.pluginList[i].loadErrors;
                        var loaded = (errors === "" || errors === null);
                        newData.push([ name, description, type, version, loaded ]);
                    }
                    this.m_tableModel.setData(newData);
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
                }
            }
        }

    }

});
