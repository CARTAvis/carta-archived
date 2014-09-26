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
            this.m_height = bounds["height"];
            this.m_width = bounds["width"];

            this._resetLayoutCB();
            this.addListener("resize", function() {
                // this._resizeContent();
                var bounds = this.getBounds();
                this.m_height = bounds["height"];
                this.m_width = bounds["width"];
            }, this);
        }, this);

    },

    events : {
        "iconifyWindow" : "qx.event.type.Data",
        "addWindowMenu" : "qx.event.type.Data"
    },

    members : {

        /**
         * Load the data identified by the path.
         * @param path {String} an identifier for data to be displayed.
         */
        dataLoaded : function(path) {
            if (this.m_pane != null) {
                this.m_pane.dataLoaded(path);
            }
        },

        /**
         * Unload the data identified by the path.
         * @param path {String} an identifier for data to be removed.
         */
        dataUnloaded : function(path) {
            if (this.m_pane != null) {
                this.m_pane.dataUnloaded(path);
            }
        },
        
        _drawModeChanged : function(ev) {
            if (this.m_pane != null) {
                this.m_pane.setDrawMode(ev.getData());
            }
        },
        
        /**
         * Returns a list of screen locations where new windows
         * can be added.
         */
        getAddWindowLocations : function() {
            var locations = [];
            if (this.m_pane != null) {
                locations = this.m_pane.getAddWindowLocations();
            }
            return locations;
        },
        

        /**
         * Returns a list of information concerning windows that can be linked to
         * the given source window showing the indicated plug-in.
         * @param pluginId {String} the name of the plug-in.
         * @param sourceWinId {String} an identifier for the window displaying the
         *      plug-in that wants information about the links that can emanate frome it.
         */
        getLinkInfo : function(pluginId, sourceId) {
            var linkInfo = [];
            if (this.m_pane != null) {
                linkInfo = this.m_pane.getLinkInfo(pluginId, sourceId);
            }
            return linkInfo;
        },

        


        /**
         * Initialize the state variables.
         */
        _initSharedVariables : function() {
            this.m_connector = mImport("connector");

            // row & columns of layout
            var pathDict = skel.widgets.Path.getInstance();
            this.m_gridRowCount = this.m_connector.getSharedVar(pathDict.LAYOUT_ROWS);
            this.m_gridColCount = this.m_connector.getSharedVar(pathDict.LAYOUT_COLS);
            this.m_gridRowCount.addCB(this._resetLayoutCB.bind(this));
            this.m_gridColCount.addCB(this._resetLayoutCB.bind(this));
        },
        
        /**
         * Layout the screen real estate using a square grid
         * with the indicated number of rows and columns.
         * 
         * @param rows
         *                {Number} the number of rows in the
         *                grid.
         * @param cols
         *                {Number} the number of columns in the
         *                grid.
         */
        layout : function(rows, cols) {
            if (rows >= 1 && cols >= 1) {
                this._removeWindows();
                var splitterSize = 10;
                var splitterHeight = this.m_height - (rows - 1)* splitterSize;
                var splitterWidth = this.m_width - (cols - 1)* splitterSize;
                this.m_pane = new skel.widgets.DisplayArea(rows, cols, splitterHeight, splitterWidth, 0, 0, cols - 1);
                this.m_pane.addListener("iconifyWindow",
                        function(ev) {
                            this.fireDataEvent("iconifyWindow",ev.getData());
                        }, this);

                qx.event.message.Bus.subscribe("drawModeChanged", this._drawModeChanged, this);
                qx.event.message.Bus.subscribe(
                                "windowSelected",
                                function(message) {
                                    var selectedWindow = message.getData();
                                    this.m_pane.windowSelected(selectedWindow);
                                    var menuButtons = selectedWindow.getWindowMenu();
                                    this.fireDataEvent("addWindowMenu",menuButtons);
                                }, this);

                var displayArea = this.m_pane.getDisplayArea();

                this.add(displayArea);
            }
        },

        /**
         * Display a single image on the screen.
         */
        layoutImage : function() {
            this.m_connector.sendCommand("/clearLayout", "", function(){});
            
            
            this.m_gridRowCount.set(2);
            this.m_gridColCount.set(1);
            this._setSharedVariablesLayoutPlugin( skel.widgets.Path.getInstance().CASA_LOADER, skel.widgets.DisplayWindow.EXCLUDED );
        },
        
        /**
         * Layout the display area using a grid containing a
         * large image area, two (analysis) plugin windows, and
         * an animation area.
         */
        layoutImageAnalysisAnimator : function() {
            
            
            this.m_connector.sendCommand("/clearLayout", "", function(){}); 
            
            this.m_gridRowCount.set(3);
            this.m_gridColCount.set(2);
            this._setSharedVariablesLayoutPlugin( 
                    skel.widgets.Path.getInstance().CASA_LOADER, "plugins",
                    skel.widgets.DisplayWindow.EXCLUDED, "statistics",
                    skel.widgets.DisplayWindow.EXCLUDED, "animator" );
     
            /*this.layout(3, 2);

            var imagePercent = .6;
            this.m_pane.setView(skel.widgets.Path.getInstance().CASA_LOADER,0, 0);
            this.m_pane.setAreaWidth(Math.floor(this.m_width* imagePercent), 0, 0);
            this.m_pane.setAreaHeight(this.m_height, 0, 0);

            this.m_pane.excludeArea(1, 0);
            this.m_pane.excludeArea(2, 0);

            this.m_pane.setView("plugins", 0, 1);
            this.m_pane.setAreaWidth(Math.floor(this.m_width* (1 - imagePercent)), 0, 1);

            this.m_pane.setView("statistics", 1, 1);
            this.m_pane.setAreaWidth(Math.floor(this.m_width* (1 - imagePercent)), 1, 1);
            // this.m_pane.link( 0, 0, 1, 1);

            this.m_pane.setView("animator", 2, 1);
            this.m_pane.setAreaWidth(Math.floor(this.m_width* (1 - imagePercent)), 2, 1);
            */
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
            if (this.m_pane != null) {
                this.m_pane.changeLink(sourceWinId, destWinId,addLink);
            }
        },



        /**
         * Restore the window at the given layout position to its original location.
         * @param row {Number} the row index of the window to be restored.
         * @param col {Number} the column index of the window to be restored.
         */
        restoreWindow : function(row, col) {
            this.m_pane.restoreWindow(row, col);
        },

        /**
         * Clears out the existing windows.
         */
        _removeWindows : function() {
            if (this.m_pane != null) {
                this.m_pane.removeWindows();
                this.removeAll();
            }
            skel.widgets.DisplayWindow.windowCounter = 0;
        },

        /**
         * Resets which plugins are displayed in each window.
         */
        _resetDisplayedPlugins : function( rowCount, colCount ) {
            var index = 0;
            var pathDict = skel.widgets.Path.getInstance();
            var basePath = pathDict.LAYOUT_PLUGIN;
            for (var row = 0; row < rowCount; row++) {
                for (var col = 0; col < colCount; col++) {
                    var pluginPath = basePath + pathDict.SEP + this.m_PLUGIN_PREFIX+index;
                    var name = this.m_connector.getSharedVar( pluginPath).get();
                    if ( name ){
                        if ( name != skel.widgets.DisplayWindow.EXCLUDED ){
                            this.m_pane.setView(name, row, col);
                        }
                        else {
                            this.m_pane.excludeArea( row, col );
                            skel.widgets.DisplayWindow.windowCounter++;
                        }
                    }
                    else {
                        this.m_pane.setView( null, row, col );
                    }
                    index++;
                }
            }
        },
        
        /**
         * Reset the layout based on changed row and column
         * counts.
         */
        _resetLayoutCB : function() {
            var rowCount = parseInt(this.m_gridRowCount.get());
            var colCount = parseInt(this.m_gridColCount.get());
            this.layout(rowCount, colCount);
            this._resetDisplayedPlugins( rowCount, colCount );
        },
        
        /**
         * Update the number of rows in the current layout.
         * @param gridRows {Number} the number of rows in the layout.
         */
        setRowCount : function(gridRows) {
            this.m_connector.sendCommand("/clearLayout", "", function(){});
            this.m_gridRowCount.set(gridRows);
        },

        /**
         * Update the number of columns in the current layout.
         * @param gridCols {Number} the number of columns in the layout.
         */
        setColCount : function(gridCols) {
            this.m_connector.sendCommand("/clearLayout", "", function(){});
            this.m_gridColCount.set(gridCols);
        },


        
        /**
         * Set the shared variables that store the plugins that will be displayed
         * in each cell.
         */
        
        _setSharedVariablesLayoutPlugin : function(){
            
            var pathDict = skel.widgets.Path.getInstance();
            var basePath = pathDict.LAYOUT_PLUGIN;
            for( var i = 0; i < arguments.length; i++ ){
                var pluginPath = basePath + pathDict.SEP + this.m_PLUGIN_PREFIX+i;
                var layoutPlugin = this.m_connector.getSharedVar(pluginPath);
                layoutPlugin.set( arguments[i]);
            }
        },

        m_PLUGIN_PREFIX : "win",
        
        m_pane : null,
        m_height : 0,
        m_width : 0,

        //State variables
        m_gridRowCount : null,
        m_gridColCount : null,
        m_connector : null
    },

    properties : {
        appearance : {
            refine : true,
            init : "display-main"
        }
    }

});
