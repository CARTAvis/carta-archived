/**
 * Manages a single window for displaying a plugin.
 */

/*******************************************************************************
 * 
 ******************************************************************************/

qx.Class
        .define(
                "skel.widgets.Window.DisplayDesktop",
                {
                    extend : qx.ui.window.Desktop,

                    /**
                     * Constructor.
                     * 
                     * @param row
                     *                {Number} the layout row index of this
                     *                desktop.
                     * @param col
                     *                {Number} the layout col index of this
                     *                desktop.
                     */
                    construct : function(row, col) {
                        this.base(arguments);

                        this.m_row = row;
                        this.m_col = col;

                        this.set({
                            decorator : "desktop"
                        });
                    },

                    events : {
                        "iconifyWindow" : "qx.event.type.Data"
                    },

                    members : {

                        _addWindowListeners : function(){
                            this.m_window.addListener("disappear", function() {
                                if (!this.m_window.isClosed()) {
                                    var bounds = this.m_window.getBounds();
                                    var data = {
                                        row : this.m_row,
                                        col : this.m_col,
                                        title : this.m_window.getIdentifier()
                                    };
                                    this.fireDataEvent("iconifyWindow", data);
                                }
                            }, this);

                            this.m_window.addListener("maximizeWindow",
                                            function() {
                                                var appRoot = this
                                                        .getApplicationRoot();
                                                appRoot.add(this.m_window);
                                            }, this);

                            this.m_window.addListener("restoreWindow",
                                    function() {
                                        this.restoreWindow(this.m_window
                                                .getIdentifier());
                                    }, this);

                            this.m_window.addListener("closeWindow",
                                    function() {
                                        this.exclude();
                                    }, this);

                            this.addListener("resize", this._resetWindowSize,
                                    this);
                            
                        },
                        
                        /**
                         * Returns an empty list as windows cannot be added in
                         * the middle of a desktop.
                         */
                        getAddWindowLocations : function() {
                            return [];
                        },

                        /**
                         * Returns this.
                         */
                        getDisplayArea : function() {
                            return this;
                        },


                        
                        /**
                         * Returns a list of information concerning windows that
                         * can be linked to the given source window showing the
                         * indicated plug-in.
                         * 
                         * @param pluginId
                         *                {String} the name of the plug-in.
                         * @param sourceWinId
                         *                {String} an identifier for the window
                         *                displaying the plug-in that wants
                         *                information about the links that can
                         *                emanate frome it.
                         */
                        getLinkInfo : function(pluginId, sourceWinId) {
                            var linkInfo = [];
                            if (this.m_window !== null) {
                                linkInfo.push(this.m_window.getLinkInfo(
                                        pluginId, sourceWinId));
                            }
                            return linkInfo;
                        },

                        /**
                         * Returns the identifier for the window this desktop
                         * manages if the passed in location matches the
                         * location of its window; otherwise, returns an empty
                         * string.
                         * 
                         * @param sourceRow
                         *                {Number} a row location on the screen.
                         * @param sourceCol
                         *                {Number} a column location on the
                         *                screen.
                         */
                        getWinId : function(sourceRow, sourceCol) {
                            var winId = "";
                            if (this.m_row == sourceRow && this.m_col == sourceCol) {
                                if (this.m_window !== null) {
                                    winId = this.m_window.getIdentifier();
                                }
                            }
                            return winId;
                        },

                        /**
                         * Returns true if the link from the source window to
                         * the destination window was successfully added or
                         * removed; false otherwise.
                         * 
                         * @param sourceWinId
                         *                {String} an identifier for the link
                         *                source.
                         * @param destWinId
                         *                {String} an identifier for the link
                         *                destination.
                         * @param addLink
                         *                {boolean} true if the link should be
                         *                added; false if the link should be
                         *                removed.
                         */
                        changeLink : function(sourceWinId, destWinId, addLink) {
                            var linkSet = false;
                            if (this.m_window !== null) {
                                linkSet = this.m_window.changeLink(sourceWinId,
                                        destWinId, addLink);
                            }
                            return linkSet;
                        },

                        /**
                         * Returns the row and column of its managed window.
                         */
                        _getLastIndices : function() {
                            var indices = [ this.m_row, this.m_col ];
                            return indices;
                        },

                        /**
                         * Returns the width and height.
                         */
                        _getDimensions : function() {
                            var dims = [ this.getWidth(), this.getHeight() ];
                            return dims;
                        },
                        
                        /**
                         * Factory method for making window specialized to
                         * correct type.
                         * 
                         * @param pluginId
                         *                {String} an identifier for the type of
                         *                plugin the window will manage.
                         * @param index {Number} an index that will be positive when there is more than one window with the same pluginId.
                         */
                        _makeWindow : function(pluginId, index ) {
                            this.m_window = skel.widgets.Window.WindowFactory.makeWindow( pluginId, index, 
                                    this.m_row, this.m_col, false );
                            this._addWindowListeners();
                        },

                        /**
                         * Sets the width and height.
                         * 
                         * @param width
                         *                {Number} the new width.
                         * @param height
                         *                {Number} the new height.
                         * @param decreaseWidth
                         *                {Boolean} - currently ignored.
                         * @param decreaseHeight
                         *                {Boolean} - currently ignored.
                         */
                        _setDimensions : function(width, height, decreaseWidth,
                                decreaseHeight) {
                            this.setWidth(width);
                            this.setHeight(height);
                        },

                        /**
                         * Returns whether or not a different plug-in was
                         * reassigned to this DisplayArea based on whether its
                         * location matches the rowIndex and colIndex passed in.
                         * 
                         * @param pluginId {String} a new plug-in identifier.
                         * @param index {Number} an index to indicate which one in the case of views having the same plugin.
                         * @param rowIndex {Number} a row index in the layout.
                         * @param colIndex {Number} a column index in the layout.
                         */
                        setView : function(pluginId, index, rowIndex, colIndex) {
                            // If this is not the target, return.
                            if (rowIndex != this.m_row || colIndex != this.m_col) {
                                return false;
                            }
                            if ( index < 0 && pluginId ){
                                return false;
                            }
                            var existingWindow = false;
                            if (this.m_window !== null) {
                                //Not the same plugin so we will remake the window.
                                if( this.m_window.getPlugin() != pluginId ) {
                                    this.removeWindows();
                                    this.m_window = null;
                                    this._makeWindow( pluginId, index);
                                }
                                else {
                                    existingWindow = true;
                                }
                            }
                            else {
                                this._makeWindow(pluginId, index);
                            }

                            if ( this.m_window !== null ){
                                this.add(this.m_window);
                                this._resetWindowSize();

                                this.m_window.open();
                                if ( existingWindow ){
                                    this.m_window.initID( index );
                                }
                            }
                            return true;
                        },


                        /**
                         * Reset the size of the contained window based on the
                         * size of this Desktop.
                         */
                        _resetWindowSize : function() {
                            var bounds = this.getBounds();
                            if (bounds !== null) {
                                this.m_window.setWidth(bounds.width );
                                this.m_window.setHeight(bounds.height);
                            }
                        },

                        /**
                         * Returns whether or not the window was restored based
                         * on whether or not the location matches this one.
                         * 
                         * @param row
                         *                {Number} the layout row of the window
                         *                to be restored.
                         * @param col
                         *                {Number} the layout column of the
                         *                window to be restored.
                         */
                        restoreWindow : function(row, col) {
                            var restored = false;
                            if (this.m_window !== null && this.m_row == row && this.m_col == col) {
                                restored = true;
                                var appRoot = this.getApplicationRoot();
                                if (appRoot.indexOf(this.m_window) != -1) {
                                    appRoot.remove(this.m_window);
                                    this.add(this.m_window);
                                }
                                this.m_window.open();
                                this.show();
                            }
                            return restored;
                        },

                        /**
                         * Remove all DisplayWindows.
                         */
                        removeWindows : function() {
                            if (this.m_window !== null) {
                                qx.event.Registration
                                        .removeAllListeners(this.m_window);
                                this.removeAll();
                            }
                        },

                        /**
                         * Returns whether or not the width was set based on the
                         * location of this desktop compared to the layout row
                         * and column index passed in.
                         * 
                         * @param width
                         *                {Number} horizontal of the display
                         *                area.
                         * @param rowIndex
                         *                {Number} a layout row index
                         *                identifying the area.
                         * @param colIndex
                         *                {Number} a layout column index
                         *                identifying the area.
                         */
                        setAreaWidth : function(width, rowIndex, colIndex) {
                            var target = true;
                            if (rowIndex !== this.m_row || colIndex !== this.m_col) {
                                target = false;
                            } else {
                                this.setWidth(width);
                            }
                            return target;
                        },

                        /**
                         * Returns whether or not the height was set based on
                         * the location of this desktop compared to the layout
                         * row and column index passed in.
                         * 
                         * @param height
                         *                {Number} vertical height of the
                         *                display area.
                         * @param rowIndex
                         *                {Number} a layout row index
                         *                identifying the area.
                         * @param colIndex
                         *                {Number} a layout column index
                         *                identifying the area.
                         */
                        setAreaHeight : function(height, rowIndex, colIndex) {
                            var target = true;
                            if (rowIndex !== this.m_row || colIndex !== this.m_col) {
                                target = false;
                            } else {
                                this.setHeight(height);
                            }
                            return target;
                        },

                        /**
                         * Removes this DisplayDesktop from the layout if this
                         * row and column index match the passed in row and
                         * column index. Returns whether or not this
                         * DisplayDesktop was removed from the display.
                         * 
                         * @param row
                         *                {Number} a row index in the layout.
                         * @param col
                         *                {Number} a col index in the layout.
                         */
                        excludeArea : function(row, col) {
                            var excluded = false;
                            if (row == this.m_row && col == this.m_col) {
                                this.exclude();
                                excluded = true;
                            }
                            return excluded;
                        },

                        /**
                         * Loads the data.
                         * 
                         * @param path
                         *                {String} the location or lookup for
                         *                the data.
                         */
                        dataLoaded : function(path) {
                            if (this.m_window !== null) {
                                this.m_window.dataLoaded(path);
                            }
                        },

                        /**
                         * Removes the data.
                         * 
                         * @param path
                         *                {String} the location or identifier
                         *                for the data.
                         */
                        dataUnloaded : function(path) {
                            if (this.m_window !== null) {
                                this.m_window.dataUnloaded(path);
                            }
                        },

                        setDrawMode : function(drawInfo) {
                            if (this.m_window !== null) {
                                this.m_window.setDrawMode(drawInfo);
                            }
                        },


                        
                        /**
                         * Resets selected status.
                         * 
                         * @param win
                         *                {DisplayWindow} the display window
                         *                that has been selected.
                         */
                        windowSelected : function(win) {
                            if (this.m_window !== null && this.m_window !== win) {
                                this.m_window.setSelected(false, false);
                            }
                        },


                        m_window : null,
                        m_row : null,
                        m_col : null
                    }

                });
