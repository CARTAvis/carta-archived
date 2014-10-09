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
     */
    construct : function() {
        this.m_connector = mImport("connector");
        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVar = this.m_connector
                .getSharedVar(pathDict.PLUGIN_LIST_STAMP);
        this.m_sharedVar.addCB(this._sharedVarCB.bind(this));
        this.base(arguments, "");

        // create the UI
        this._createUI();

        // manually invoke the callback so that the label is immediately updated
        // with the current value
        this._sharedVarCB(this.m_sharedVar.get());
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
            this.m_table.setAllowShrinkY( true);
            this.m_table.setAllowStretchY( true);
            this.m_table.setAllowGrowY( true);
            this.m_table.setMinHeight(100);
            this.setAllowShrinkY( true);
            this.setAllowStretchY( true);
            this.setAllowGrowY( true);
            this.setMinHeight(100);

            var colModel = this.m_table.getTableColumnModel();
            colModel.setDataCellRenderer(4,
                    new qx.ui.table.cellrenderer.Boolean());

            this.add(this.m_table);
        },

        /**
         * Shared var callback. Read in the contents of the plugin list and render it.
         * @param val {String} Number of plugins.
         * @private
         */
        _sharedVarCB : function(val) {
            var console = mImport("console");

            var newData = [];
            var n = parseInt(val);
            for (var i = 0; i < val; i++) {
                var base = "/carta/pluginList/"
                var pIndex = "p" + i;
                var pluginName = base + "name/" + pIndex;
                var name = this.m_connector.getSharedVar(pluginName).get();
                var pluginDescription = base + "description/" + pIndex;
                var description = this.m_connector.getSharedVar(
                        pluginDescription).get();
                var type = this.m_connector.getSharedVar(
                        base + "type/" + pIndex).get();
                var version = this.m_connector.getSharedVar(
                        base + "version/" + pIndex).get();
                var errors = this.m_connector.getSharedVar(
                        base + "loadErrors/" + pIndex).get();
                var loaded = (errors === "" || errors === null);
                newData.push([ name, description, type, version, loaded ]);
            }

            this.m_tableModel.setData(newData);
        }

    }

});
