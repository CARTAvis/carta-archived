/**
 * A display window for showing cursor and other statistics.
 */

/*******************************************************************************
 * 
 * 
 * 
 ******************************************************************************/

qx.Class.define("skel.widgets.Window.DisplayWindowStatistics",
        {
            extend : skel.widgets.Window.DisplayWindow,

            /**
             * Constructor.
             * @param row {Number} the row location of the window.
             * @param col {Number} the column location of the window.
             * @param pluginId {String} an identifier the plug-in to be displayed.
             * @param index {Number} index of the plug-in for the case where 
             *          multiple plug-ins of the same type are displayed.
             * @param detached {boolean} true for a dialog type window; 
             *          false for an in-line window.
             */
            construct : function(row, col, index, detached ) {
                var path = skel.widgets.Path.getInstance();
                this.base(arguments, path.STATISTICS, row, col, index, detached );
                this.m_links = [];
                qx.event.message.Bus.subscribe("addLink", function(ev){
                    var data = ev.getData();
                    this._updateStatSource( data.source, data.destination, true);
                }, this );
            },

            members : {
                

                /**
                 * Adds a window displaying cursor statistics if the sourceWinId matches
                 * the server-side id of this window.
                 * 
                 * @param sourceWinId {String} for the window displaying the statistics.
                 * @param destWinId {String} an identifier for the window generating the statistics.
                 * @param addLink {boolean} true if the statistics should be added;
                 *                false if cursor statistics should be removed.
                 */
                _updateStatSource : function(sourceWinId, destWinId, addLink) {
                    if ( this.m_identifier === null || sourceWinId !== this.m_identifier ){
                        return;
                    }
                    if (addLink) {
                        // Right now only generic support is statistics.
                        // Need to generalize.
                        var path = skel.widgets.Path.getInstance();
                        var viewPath = destWinId + path.SEP + path.VIEW;
                        var cursorLabel = new skel.boundWidgets.Label( "", "", viewPath, function( anObject){
                            return anObject.formattedCursorCoordinates;
                        });
                        cursorLabel.setRich( true );
                        this.m_content.add( cursorLabel );
                    } 
                    else if (!addLink) {
                        this.m_content.removeAll();
                    }
                }
            }

        });
