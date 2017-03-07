/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 05/04/13
 * Time: 4:27 PM
 * To change this template use File | Settings | File Templates.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.DisplayMain",
{
    extend : qx.ui.container.Composite,

    construct : function() {
        this.base(arguments, new qx.ui.layout.Grow());
        this._initSharedVariables();

        this.addListener("appear", function() {
            var bounds = this.getBounds();
            this.m_height = bounds.height;
            this.m_width = bounds.width;

            this._resetLayoutCB();
            this.addListener("resize", function() {
                // this._resizeContent();
                var bounds = this.getBounds();
                this.m_height = bounds.height;

                this.m_width = bounds.width;
                var sizeData = {
                        "offsetX" : bounds.left,
                        "offsetY" : bounds.top
                    };
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "mainOffsetsChanged", sizeData));
            }, this);
        }, this);

        qx.event.message.Bus.subscribe("setView", function(
                message) {
            var data = message.getData();
            this._setView(data.plugin, data.location);
        }, this);
    },

    events : {
        "iconifyWindow" : "qx.event.type.Data",
        "addWindowMenu" : "qx.event.type.Data"
    },

    members : {


        /**
         * Send a command to the server to clear the layout.
         */
        _clearLayout : function(){
            this._removeWindows();
            var path = skel.widgets.Path.getInstance();
            var clearLayoutCmd = path.getCommandClearLayout();
            this.m_connector.sendCommand(clearLayoutCmd, "", function(){});
        },

        /**
         * Load the data identified by the path.
         * @param path {String} an identifier for data to be displayed.
         */
        dataLoaded : function(path) {
            if (this.m_pane !== null) {
                this.m_pane.dataLoaded(path);
            }
        },

        /**
         * Unload the data identified by the path.
         * @param path {String} an identifier for data to be removed.
         */
        dataUnloaded : function(path) {
            if (this.m_pane !== null) {
                this.m_pane.dataUnloaded(path);
            }
        },

        _drawModeChanged : function(ev) {
            if (this.m_pane !== null) {
                this.m_pane.setDrawMode(ev.getData());
            }
        },

        /**
         * Returns the number of columns in the grid.
         * @return {Number} the number of grid columns.
         */
        getColCount : function(){
            return this.m_gridColCount;
        },

        /**
         * Returns the number of rows in the grid.
         * @return {Number} the number of grid rows.
         */
        getRowCount : function(){
            return this.m_gridRowCount;
        },

        /**
         * Returns a list of information concerning windows that can be linked to
         * the given source window showing the indicated plug-in.
         * @param pluginId {String} the name of the plug-in.
         * @param sourceId {String} an identifier for the window displaying the
         *      plug-in that wants information about the links that can originate from it.
         * @param linkInfos {Array} - list of linked and potentially linkable window information
         *      for the given source.
         * @return {Array} information about links that can be established from the specified
         *      source window/plug-in.
         */
        getLinkInfo : function(pluginId, sourceId, linkInfos ) {
            var linkInfo = [];
            if (this.m_pane !== null) {
                linkInfo = this.m_pane.getLinkInfo(pluginId, sourceId, linkInfos );
            }
            return linkInfo;
        },

        /**
         * Returns a list of information concerning windows that can be replaced by
         * the given source window showing the indicated plug-in.
         * @param sourceId {String} an identifier for the window displaying the
         *      plug-in that wants information about where it can be moved.
         * @return {String} information about move locations that can be established from the specified
         *      window/plug-in.
         */
        getMoveInfo : function(sourceId) {
            var moveInfo = [];
            if (this.m_pane !== null) {
                moveInfo = this.m_pane.getMoveInfo(sourceId);
            }
            return moveInfo;
        },

        /**
         * Initialize the state variables.
         */
        _initSharedVariables : function() {
            this.m_connector = mImport("connector");
            //layout details
            var pathDict = skel.widgets.Path.getInstance();
            this.m_layout = this.m_connector.getSharedVar( pathDict.LAYOUT );
            this.m_layout.addCB( this._resetLayoutCB.bind(this));
        },



        /**
         * Layout the screen real estate using a root layout cell with nested
         * children.
         * @param id {String} an identifier for the root layout cell.
         */
        layout : function( id ) {
            if ( this.m_pane !== null ){
                var area = this.m_pane.getDisplayArea();
                if ( this.indexOf( area ) >= 0 ){
                    this.remove( area );
                }
            }
            //var splitterSize = 10;
            //var splitterHeight = this.m_height - (this.m_gridRowCount - 1)* splitterSize;
            //var splitterWidth = this.m_width - (this.m_gridColCount - 1)* splitterSize;
            //Reinitialize the pane if the id has changed.
            if ( this.m_pane === null || this.m_pane.getId() != id ){
                this.m_pane = new skel.widgets.Layout.LayoutNodeComposite( id  );

                this.m_pane.addListener("iconifyWindow",
                        function(ev) {
                            this.fireDataEvent("iconifyWindow",ev.getData());
                        }, this);
                this.m_pane.addListener( "findChild",function(ev){
                    var data = ev.getData();
                    var childNode = this.m_pane.getNode( data.findId );
                    if ( childNode !== null ){
                        var childInfo = {
                            nodeId : data.sourceId,
                            childId : data.childId,
                            child : childNode
                        };
                        qx.event.message.Bus.dispatch( new qx.event.message.Message( "nodeFound", childInfo));
                    }
                }, this );
                this.m_pane.initSharedVar();
                qx.event.message.Bus.subscribe("drawModeChanged", this._drawModeChanged, this);
                qx.event.message.Bus.subscribe("windowSelected",
                    function(message) {
                        var selectedWindow = message.getData();
                        this.m_pane.windowSelected(selectedWindow);
                    }, this);
            }
            var displayArea = this.m_pane.getDisplayArea();
            this.add(displayArea);
            //Store the windows that were created so if the layout changes on the
            //server, we can reuse existing windows.
            var windows = this.m_pane.getWindows();
            skel.widgets.Window.WindowFactory.setExistingWindows( windows );
        },

        /**
         * Adds or removes the link from the window identified
         * by the sourceWinId to the window identified by the
         * destWinId.
         *
         * @param sourceWinId
         *                {String} an identifier for the window
         *                representing the source of the link.
         * @param destWinId
         *                {String} an identifier for the window
         *                representing the destination of the
         *                link.
         * @param addLink
         *                {boolean} true if the link should be
         *                added; false otherwise.
         */
        link : function(sourceWinId, destWinId, addLink) {
            if (this.m_pane !== null) {
                this.m_pane.changeLink(sourceWinId, destWinId,addLink);
            }
        },

        /**
         * Restore the window to its original location.
         * @param locationId {String} an identifier for the location where the window should be restored.
         */
        restoreWindow : function( locationId ) {
            this.m_pane.restoreWindow( locationId );
        },

        /**
         * Clears out the existing windows.
         */
        _removeWindows : function() {
            if (this.m_pane !== null) {
                this.m_pane.removeWindows();
                this.removeAll();
            }
        },


        /**
         * Reset the layout based on changed layout information from the server.
         */
        _resetLayoutCB : function() {
            var layoutObjJSON = this.m_layout.get();
            if ( layoutObjJSON !== null ){
                try {
                    var layout = JSON.parse( layoutObjJSON );
                    var windows = [];
                    //Store the existing windows in the factory so they
                    //can (possibly) be recycled in the new layout.
                    if ( this.m_pane !== null ){
                        windows = this.m_pane.getWindows();
                        skel.widgets.Window.WindowFactory.setExistingWindows( windows );
                    }
                    this.layout( layout.layoutNode);
                    //Store the available windows in the Popup command of the image
                    //display so that it can decide whether or not it should be enabled
                    //based on existing links to other windows.
                    if ( this.m_pane !== null ){
                        var newWindows = this.m_pane.getWindows();
                        var popCmd = skel.Command.Popup.CommandPopup.getInstance();
                        popCmd.setWindows( newWindows );
                    }

                    var layoutType = layout.layoutType;
                    if(layoutType) {
                        skel.Command.Layout.CommandLayout.getInstance().setCheckedType(layoutType);
                    }
                }
                catch( err ){
                    console.log( "Could not parse: "+layoutObjJSON );
                }
            }
        },

        /**
         * Update the number of columns in the current layout.
         * @param gridRows {Number} the number of rows in the layout.
         * @param gridCols {Number} the number of columns in the layout.
         */
        setLayoutSize : function(gridRows, gridCols) {
            //this._clearLayout();
            var path = skel.widgets.Path.getInstance();
            var layoutSizeCmd = path.getCommandSetLayoutSize();
            var params = "rows:"+gridRows + ",cols:"+gridCols;
            this.m_connector.sendCommand( layoutSizeCmd, params, function(){});
        },


        /**
         * Sends a command to the server letting it know that the displayed plug-in
         * has changed.
         * @param plugin {String} the name of the new plug-in.
         * @param locationId {String} an identifier for the location of the window containing the plug-in.
         */
        _setView : function( plugin, locationId ){
            var path = skel.widgets.Path.getInstance();
            var layoutPath = path.LAYOUT;
            var layoutSharedVar = this.m_connector.getSharedVar(layoutPath);
            var val = layoutSharedVar.get();
            if ( val ){
                try {
                    var layoutObj = JSON.parse( val );
                    var win = this.m_pane.getWindow( locationId );
                    var winPlugin = "";
                    if ( win !== null ){
                        win.clean();
                        winPlugin = win.getPlugin();
                    }

                    var cmd = path.getCommandSetPlugin();
                    var params = "destPlugin:"+plugin+",sourceLocateId:"+locationId;
                    this.m_connector.sendCommand( cmd, params, function(){} );
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
                }
            }
        },

        m_PLUGIN_PREFIX : "win",

        m_pane : null,
        m_height : 0,
        m_width : 0,
        m_gridRowCount : null,
        m_gridColCount : null,

        //State variables
        m_layout : null,

        m_connector : null
    },

    properties : {
        appearance : {
            refine : true,
            init : "display-main"
        }
    }

});
