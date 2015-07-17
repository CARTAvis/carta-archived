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
             * @param pluginId {String} an identifier the plug-in to be displayed.
             * @param index {Number} index of the plug-in for the case where 
             *          multiple plug-ins of the same type are displayed.
             * @param detached {boolean} true for a dialog type window; 
             *          false for an in-line window.
             */
            construct : function( index, detached ) {
                var path = skel.widgets.Path.getInstance();
                this.base(arguments, path.STATISTICS, index, detached );
                this.m_links = [];
            },

            members : {
                
                
                /**
                 * Remove a link.
                 * @param link {skel.widgets.Link.Link} the link to remove.
                 */
                removeLink : function( sourceWinId, destWinId ){
                    if ( this.m_identifier === null || sourceWinId !== this.m_identifier ){
                        return;
                    }
                    //Note:  need to be more sophisticated for multiple images.
                    this.m_content.removeAll();
                },

                /**
                 * Adds a window displaying cursor statistics if the sourceWinId matches
                 * the server-side id of this window.
                 * 
                 * @param sourceWinId {String} for the window displaying the statistics.
                 * @param destWinId {String} an identifier for the window generating the statistics.
                 */
                _updateStatSource : function(sourceWinId, destWinId ) {
                    if ( this.m_identifier === null || sourceWinId !== this.m_identifier ){
                        return;
                    }
                    if ( this.m_cursorLabel === null ){
                        // Right now only generic support is statistics.
                        // Need to generalize.
                        var path = skel.widgets.Path.getInstance();
                        var viewPath = destWinId + path.SEP + path.VIEW;
                        this.m_cursorLabel = new skel.boundWidgets.Label( "", "", viewPath, function( anObject){
                            return anObject.formattedCursorCoordinates;
                        });
                        this.m_cursorLabel.setRich( true );
                    }
                    if ( this.m_content.indexOf( this.m_cursorLabel) < 0){
                        this.m_content.add( this.m_cursorLabel );
                    }
                },
                
                m_cursorLabel : null
            }

        });
