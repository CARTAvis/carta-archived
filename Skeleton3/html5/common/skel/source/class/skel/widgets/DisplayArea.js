/**
 * A Composite containing a split pane that manages two child areas of screen
 * real estate space.
 */

/*******************************************************************************
 * 
 * 
 * 
 * 
 * 
 ******************************************************************************/

qx.Class
        .define(
                "skel.widgets.DisplayArea",
                {
                    extend : qx.core.Object,

                    /**
                     * Constructor; recursively builds up the screen.
                     * 
                     * @param rows
                     *                {Number} the number of layout rows still
                     *                being constructed.
                     * @param cols
                     *                {Number} the number of layout column still
                     *                being constructed.
                     * @param height
                     *                {Number} the number of pixels still
                     *                available in the vertical direction.
                     * @param width
                     *                {Number} the number of pixels still
                     *                available in the horizontal direction.
                     * @param rowIndex
                     *                {Number} the smallest row index available.
                     * @param colIndex
                     *                {Number} the smallest column index
                     *                available.
                     * @param lastColIndex
                     *                {Number} index of the last column in the
                     *                layout.
                     * @param secondArea
                     *                {DisplayArea} used when splitting an
                     *                existing display area; the second area is
                     *                provided rather than constructed.
                     * @param splitType
                     *                {String} used when splitting an exsiting
                     *                area; whether to split horizontally or
                     *                vertically.
                     */
                    construct : function(rows, cols, height, width, rowIndex,
                            colIndex, lastColIndex, secondArea, splitType) {
                        this.base(arguments);
                        if (typeof secondArea == 'undefined') {
                            this._constructOne(rows, cols, height, width,
                                    rowIndex, colIndex, lastColIndex);
                        } else {
                            this._constructTwo(height, width, rowIndex,
                                    colIndex, secondArea, splitType);
                        }
                    },

                    events : {
                        "iconifyWindow" : "qx.event.type.Data"
                    },

                    members : {

                        /**
                         * Constructor for making a new display area. Please see
                         * the constructor for parameter documentation.
                         */
                        _constructOne : function(rows, cols, height, width,
                                rowIndex, colIndex, lastColIndex) {
                            var rowHeight = Math.floor(height / rows);
                            var colWidth = Math.floor(width / cols);

                            // No more horizontal splits so make this one a
                            // vertical one.
                            if (cols == 1) {
                                this.m_pane = new qx.ui.splitpane.Pane(
                                        "vertical").set({
                                    allowGrowX : true,
                                    allowGrowY : true
                                });

                                // Create container with fixed dimensions for
                                // the top:
                                this.m_areaFirst = this._makeArea(colWidth,
                                        rowHeight, rowIndex, colIndex);

                                // We don't need another split pane so just add
                                // another container.
                                if (rows <= 2) {
                                    this.m_areaSecond = this._makeArea(
                                            colWidth, rowHeight, rowIndex + 1,
                                            colIndex);
                                }
                                // Make a child node with one less row and add
                                // it.
                                else if (rows > 2) {
                                    this.m_areaSecond = this._makeChild(
                                            rows - 1, cols, rowHeight,
                                            colWidth, rowIndex + 1, colIndex,
                                            lastColIndex);
                                }

                            }
                            // Make another horizontal split.
                            else {
                                this.m_pane = new qx.ui.splitpane.Pane(
                                        "horizontal").set({
                                    allowGrowX : true,
                                    allowGrowY : true
                                });
                                // We can handle it with a single split pane.
                                if (rows == 1) {
                                    // Create container with fixed dimensions
                                    // for the top:
                                    this.m_areaFirst = this._makeArea(colWidth,
                                            rowHeight, rowIndex, colIndex);
                                    this.m_areaSecond = this._makeArea(
                                            colWidth, rowHeight, rowIndex,
                                            colIndex + 1);
                                }
                                // Each of our sides needs to be a split pane.
                                else {
                                    this.m_areaFirst = this._makeChild(rows, 1,
                                            rowHeight, colWidth, rowIndex,
                                            colIndex);
                                    this.m_areaSecond = this._makeChild(rows,
                                            cols - 1, rowHeight, colWidth,
                                            rowIndex, colIndex + 1,
                                            lastColIndex);

                                }
                            }
                        },

                        /**
                         * Constructor for splitting an existing display area in
                         * two pieces.
                         */
                        _constructTwo : function(rowHeight, colWidth, rowIndex,
                                colIndex, secondArea, splitType) {
                            this.m_pane = new qx.ui.splitpane.Pane(splitType)
                                    .set({
                                        allowGrowX : true,
                                        allowGrowY : true
                                    });

                            this.m_areaFirst = this._makeArea(rowHeight,
                                    colWidth, rowIndex, colIndex, true);
                            this.m_areaSecond = secondArea;
                            this.m_pane.add(this.m_areaSecond.getDisplayArea(),
                                    1);
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
                            var linkSet = this.m_areaFirst.changeLink(
                                    sourceWinId, destWinId, addLink);
                            if (!linkSet) {
                                linkSet = this.m_areaSecond.changeLink(
                                        sourceWinId, destWinId, addLink);
                            }
                            return linkSet;
                        },
                        

                        /**
                         * Notifies children that data has been loaded.
                         */
                        dataLoaded : function(path) {
                            if (this.m_areaFirst !== null) {
                                this.m_areaFirst.dataLoaded(path);
                            }
                            if (this.m_areaSecond !== null) {
                                this.m_areaSecond.dataLoaded(path);
                            }
                        },

                        /**
                         * Notifies children that data has been unloaded.
                         */
                        dataUnloaded : function(path) {
                            if (this.m_areaFirst !== null) {
                                this.m_areaFirst.dataUnloaded(path);
                            }
                            if (this.m_areaSecond !== null) {
                                this.m_areaSecond.dataUnloaded(path);
                            }
                        },


                        /**
                         * Returns whether or not a desktop at the given row and
                         * column of the layout was successfully removed; used
                         * for creating asymmetrical layouts.
                         * 
                         * @param row
                         *                {Number} an index of a row in the
                         *                grid.
                         * @param col
                         *                {Number} an index of a column in the
                         *                grid.
                         */
                        excludeArea : function(row, col) {
                            var excluded = this.m_areaFirst.excludeArea(row,
                                    col);
                            if (!excluded) {
                                excluded = this.m_areaSecond.excludeArea(row,
                                        col);
                            }
                            return excluded;
                        },


                        /**
                         * Returns an array of points (pixel coordinates) where
                         * new display areas can be added.
                         */
                        getAddWindowLocations : function() {
                            var locations = [];
                            var i = 0;
                            if (this.m_pane !== null) {
                                var loc = this._getSplitterLocation();
                                if (loc.length > 0) {
                                    loc.push(this);
                                    locations.push(loc);
                                    if (this.m_areaFirst !== null) {
                                        var areaFirstLocs = this.m_areaFirst
                                                .getAddWindowLocations();
                                        for ( i = 0; i < areaFirstLocs.length; i++) {
                                            locations.push(areaFirstLocs[i]);
                                        }
                                    }
                                    if (this.m_areaSecond !== null) {
                                        var areaSecondLocs = this.m_areaSecond.getAddWindowLocations(locations);
                                        for ( i = 0; i < areaSecondLocs.length; i++) {
                                            var areaLoc = areaSecondLocs[i];
                                            if (this.m_pane.getOrientation() == "horizontal") {
                                                areaLoc[0] = areaLoc[0] + loc[0];
                                            } 
                                            else {
                                                areaLoc[1] = areaLoc[1] + loc[1];
                                            }
                                            locations.push(areaLoc);
                                        }
                                    }
                                }
                            }
                            return locations;
                        },
                        
                        /**
                         * Returns the row and column location of the second
                         * area.
                         */
                        _getLastIndices : function() {
                            return this.m_areaSecond._getLastIndices();
                        },

                        /**
                         * Returns the approximate width and height occupied by
                         * both sides of the split pane.
                         */
                        _getDimensions : function() {
                            var firstSize = this.m_areaFirst._getDimensions();
                            var secondSize = this.m_areaSecond._getDimensions();
                            var overallSize = firstSize;
                            if (this.m_pane.getOrientation() == "vertical") {
                                overallSize[1] = overallSize[1] + secondSize[1];
                            } else {
                                overallSize[0] = overallSize[0] + secondSize[0];
                            }
                            return overallSize;
                        },
                        

                        /**
                         * Returns the split pane.
                         */
                        getDisplayArea : function() {
                            return this.m_pane;
                        },
                        

                        /**
                         * Returns a list of information concerning windows that can be linked to
                         * the given source window showing the indicated plug-in.
                         * @param pluginId {String} the name of the plug-in.
                         * @param sourceWinId {String} an identifier for the window displaying the
                         *      plug-in that wants information about the links that can emanate frome it.
                         */
                        getLinkInfo : function(pluginId, sourceWinId) {
                            var linkInfo = [];
                            var i = 0;
                            if (this.m_areaFirst !== null) {
                                var firstInfo = this.m_areaFirst.getLinkInfo( pluginId, sourceWinId);
                                for ( i = 0; i < firstInfo.length; i++) {
                                    linkInfo.push(firstInfo[i]);
                                }
                            }
                            if (this.m_areaSecond !== null) {
                                var secondInfo = this.m_areaSecond.getLinkInfo( pluginId, sourceWinId);
                                for ( i = 0; i < secondInfo.length; i++) {
                                    linkInfo.push(secondInfo[i]);
                                }
                            }
                            return linkInfo;
                        },
                        
                        /**
                         * Returns the pixel location of the midpoint of the
                         * splitter.
                         */
                        _getSplitterLocation : function() {
                            var loc = [];
                            var splitter = this.m_pane
                                    .getChildControl("splitter");
                            var splitterBottom = skel.widgets.Util
                                    .getBottom(splitter);
                            var splitterRight = skel.widgets.Util
                                    .getRight(splitter);
                            var splitterLeft = skel.widgets.Util
                                    .getLeft(splitter);
                            var splitterTop = skel.widgets.Util
                                    .getTop(splitter);
                            var splitterWidth = splitterRight - splitterLeft;
                            var splitterHeight = splitterBottom - splitterTop;
                            if (splitterWidth > 0 && splitterHeight > 0) {
                                var xPos = Math.floor(splitterLeft + splitterWidth / 2);
                                var yPos = Math.floor(splitterTop + splitterHeight / 2);
                                loc = [ xPos, yPos ];
                            }
                            return loc;
                        },

                        /**
                         * Returns the window identifier for the window at the
                         * given row and column.
                         * 
                         * @param sourceRow
                         *                {number} a row in the screen grid.
                         * @param sourceCol
                         *                {number} a column in the screen grid.
                         */
                        getWinId : function(sourceRow, sourceCol) {
                            var winId = this.m_areaFirst.getWinId(sourceRow,
                                    sourceCol);
                            if (winId.length === 0) {
                                winId = this.m_areaSecond.getWinId(sourceRow, sourceCol);
                            }
                            return winId;
                        },


                        /**
                         * Links the window located at the source row and column
                         * to the window located at the destination row and
                         * column.
                         * 
                         * @param sourceRow
                         *                {Number} a row in the screen grid.
                         * @param souceCol
                         *                {Number} a column in the screen grid.
                         * @param destRow
                         *                {Number} a row in the screen grid.
                         * @param destCol
                         *                {Number} a column in the screen grid.
                         */
                        link : function(sourceRow, sourceCol, destRow, destCol) {
                            var winId = this.getWinId(sourceRow, sourceCol);
                            if (winId.length > 0) {
                                this.setLinkId(destRow, destCol, winId);
                            }
                        },
                        


                        /**
                         * Constructs a child leaf node of this composite.
                         */
                        _makeArea : function(colWidth, rowHeight, rowIndex,
                                colIndex, includeView) {

                            var area = new skel.widgets.Window.DisplayDesktop(
                                    rowIndex, colIndex).set({
                                width : colWidth,
                                height : rowHeight,
                                decorator : "main"
                            });
                            if (includeView) {
                                area.setView("", -1, rowIndex, colIndex);
                            }

                            area.addListener("iconifyWindow", function(ev) {
                                var data = ev.getData();
                                area.exclude();
                                this.fireDataEvent("iconifyWindow", data);
                            }, this);

                            this.m_pane.add(area, 1);
                            return area;
                        },
                        


                        /**
                         * Constructs a composite child node of this composite.
                         */
                        _makeChild : function(rows, cols, rowHeight, colWidth,
                                rowIndex, colIndex, lastColIndex, secondArea,
                                splitType) {
                            var child = new skel.widgets.DisplayArea(rows,
                                    cols, rows * rowHeight, cols * colWidth,
                                    rowIndex, colIndex, lastColIndex,
                                    secondArea, splitType);
                            child.addListener("iconifyWindow", function(ev) {
                                this.fireDataEvent("iconifyWindow", ev
                                        .getData());
                            }, this);

                            var flex = 0;
                            if (lastColIndex == colIndex) {
                                flex = 1;
                            }
                            this.m_pane.add(child.getDisplayArea(), flex);
                            return child;
                        },

                        /**
                         * Returns whether or not the window with the given id
                         * was restored.
                         * 
                         * @param row {Number} the layout row of the window
                         *                to be restored.
                         * @param col {Number} the layout column of the
                         *                window to be restored.
                         */
                        restoreWindow : function(row, col) {
                            var restored = this.m_areaFirst.restoreWindow(row,
                                    col);
                            if (!restored) {
                                restored = this.m_areaSecond.restoreWindow(row,
                                        col);
                            }
                            return restored;
                        },

                        /**
                         * Remove all windows.
                         */
                        removeWindows : function() {
                            this.m_areaFirst.removeWindows();
                            this.m_areaSecond.removeWindows();
                        },

                        /**
                         * Divides this display area into two, creating a new
                         * blank window.
                         */
                        split : function() {
                            var indices = this.m_areaFirst._getLastIndices();
                            var rowIndex = indices[0];
                            var colIndex = indices[1];
                            var sizeSecond = this.m_areaSecond._getDimensions();
                            var splitType = "horizontal";
                            var rowHeight = sizeSecond[1];
                            var colWidth = sizeSecond[0];
                            var decreaseHeight = false;
                            var decreaseWidth = false;
                            if (this.m_pane.getOrientation() == "horizontal") {
                                colWidth = Math.floor(colWidth / 2);
                                colIndex = colIndex + 1;
                                decreaseWidth = true;
                            } else {

                                rowHeight = Math.floor(rowHeight / 2);
                                rowIndex = rowIndex + 1;
                                splitType = "vertical";
                                decreaseHeight = true;
                            }

                            this.m_pane.add(this.m_areaFirst.getDisplayArea(),
                                    1);
                            this.m_areaSecond._setDimensions(colWidth,
                                    rowHeight, decreaseWidth, decreaseHeight);
                            this.m_areaSecond = this._makeChild(1, 1, colWidth,
                                    rowHeight, rowIndex, colIndex,
                                    colIndex + 1, this.m_areaSecond, splitType);
                        },



                        /**
                         * Resets the width and height of the two sides of the
                         * split pane, decreasing them as appropriate to make
                         * space for a new area.
                         * 
                         * @param width
                         *                {Number} the base width.
                         * @param height
                         *                {Number} the base height.
                         * @param decreaseWidth
                         *                {Boolean} whether or not the width
                         *                should be halved.
                         * @param decreaseHeight
                         *                {Boolean} whether or not the height
                         *                should be halved.
                         */
                        _setDimensions : function(width, height, decreaseWidth,
                                decreaseHeight) {
                            if (decreaseHeight) {
                                height = Math.floor(height / 2);
                            }
                            if (decreaseWidth) {
                                width = Math.floor(width / 2);
                            }
                            this.m_areaFirst._setDimensions(width, height,
                                    decreaseWidth, decreaseHeight);
                            this.m_areaSecond._setDimensions(width, height,
                                    decreaseWidth, decreaseHeight);
                        },

                        setDrawMode : function(drawInfo) {
                            if (this.m_areaFirst !== null) {
                                this.m_areaFirst.setDrawMode(drawInfo);
                            }
                            if (this.m_areaSecond !== null) {
                                this.m_areaSecond.setDrawMode(drawInfo);
                            }
                        },


                        /**
                         * Returns whether or not a different plug-in was reassigned to this DisplayArea
                         * based on whether its location matches the rowIndex and colIndex passed in.
                         * @param pluginId {String} a new plug-in identifier.
                         * @param rowIndex {Number} a row index in the layout.
                         * @param colIndex {Number} a column index in the layout.
                         */
                        setView : function(pluginId, index, rowIndex, colIndex) {
                            var pluginAssigned = false;
                            if (this.m_areaFirst !== null) {
                                pluginAssigned = this.m_areaFirst.setView(
                                        pluginId, index, rowIndex, colIndex);
                            }
                            if (!pluginAssigned) {
                                if (this.m_areaSecond !== null) {
                                    pluginAssigned = this.m_areaSecond.setView(
                                            pluginId, index, rowIndex, colIndex);
                                }
                            }
                            return pluginAssigned;
                        },

                        /**
                         * Sets the height of the window located at the given row and column.
                         * @param height {Number} the new height in pixels.
                         * @param rowIndex {Number} the row location of the target window.
                         * @param colIndex {Number} the column location of the target window.
                         */
                        setAreaHeight : function(height, rowIndex, colIndex) {
                            var heightSet = false;
                            if (this.m_areaFirst !== null) {
                                heightSet = this.m_areaFirst.setAreaHeight(
                                        height, rowIndex, colIndex);
                            }
                            if (!heightSet) {
                                if (this.m_areaSecond !== null) {
                                    heightSet = this.m_areaSecond
                                            .setAreaHeight(height, rowIndex,
                                                    colIndex);
                                }
                            }
                            return heightSet;
                        },

                        /**
                         * Sets the width of the window located at the given row and column.
                         * @param width {Number} the new width in pixels.
                         * @param rowIndex {Number} the row location of the target window.
                         * @param colIndex {Number} the column location of the target window.
                         */

                        setAreaWidth : function(width, rowIndex, colIndex) {
                            var widthSet = false;
                            if (this.m_areaFirst !== null) {
                                widthSet = this.m_areaFirst.setAreaWidth(width,
                                        rowIndex, colIndex);
                            }
                            if (!widthSet) {
                                if (this.m_areaSecond !== null) {
                                    widthSet = this.m_areaSecond.setAreaWidth(
                                            width, rowIndex, colIndex);
                                }
                            }
                            return widthSet;
                        },
                        

                        /**
                         * Notifies children that the given window was selected.
                         * 
                         * @param win
                         *                {skel.widgets.Window.DisplayWindow} the
                         *                selected window.
                         */
                        windowSelected : function(win) {
                            this.m_areaFirst.windowSelected(win);
                            this.m_areaSecond.windowSelected(win);
                        },

                        m_pane : null,
                        m_areaFirst : null,
                        m_areaSecond : null
                    }

                });
