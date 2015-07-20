/**
 * Displays and manages links from a source window to a destination window.
 */

qx.Class.define("skel.widgets.Link.LinkCanvas",{
        extend : skel.widgets.Link.DrawCanvas,
        type : "singleton",
        
        /**
         * Constructor.
         */
        construct : function() {
            this.base(arguments);
        },

        events : {
            "link" : "qx.event.type.Data",
            "linkRemove" : "qx.event.type.Data"
        },

        members : {

            /**
             * Add/remove items from the context menu based on whether or not the user clicked
             * near a line.
             * @param lineMatch {boolean} - true if the user clicked a link; false otherwise.
             */
            _adjustContextMenu : function( lineMatch ){
                if (lineMatch !== null) {
                    if ( this.m_contextMenu.indexOf( this.m_removeLinkButton) >= 0 ){
                        this.m_contextMenu.remove( this.m_removeLinkButton );
                    }
                    this.m_removeLinkButton = new qx.ui.menu.Button("Remove Link");
                    this.m_removeLinkButton.addListener("execute", function(){
                        this._removeLink( lineMatch );
                    }, this );
                   
                    if ( this.m_contextMenu.indexOf( this.m_removeLinkButton) < 0 ){
                        this.m_contextMenu.add(this.m_removeLinkButton);
                    }
               }
                else {
                    //Not a line match
                    if ( this.m_removeLinkButton !== null ){
                        if ( this.m_contextMenu.indexOf( this.m_removeLinkButton) >= 0 ){
                            this.m_contextMenu.remove( this.m_removeLinkButton );
                        }
                    }
                }
            },

            /**
             * Exit the link canvas.
             */
            _quit : function(){
                var linkCmd = skel.Command.Link.CommandLink.getInstance();
                linkCmd.doAction( false, function(){});
            },
            
            /**
             * Return help information for the link canvas.
             * @return {String} help information for the link canvas.
             */
            getHelp : function( ){
                return "Add a link by drawing a line from a source (blue) to a destination(black); right-click a link to remove it; escape to exit.";
            },

            /**
             * Notify users about the new link.
             * @param link {skel.widgets.Link} - information about the source and destinations of the link.
             * @param pt {Object} - user click location.
             */
            linkCompleted : function( link, pt ){
                this.fireDataEvent( "link", link );
            },

            /**
             * Remove the passed in link.
             * @param lineMatch {skel.widgets.LinkInfo} information about the link to be removed.
             */
            _removeLink : function( lineMatch ){
                var sourceId = this.m_sourceLink.winId;
                var destId = lineMatch.winId;
                this.removeLink( sourceId, destId );
            },

            
            /**
             * Remove the link from the indicated source to the destination.
             * @param sourceId {String} an identifier for the source of the link.
             * @param destId {String} an identifier for the destination of the link.
             */
            removeLink : function( sourceId, destId ){
                var link = new skel.widgets.Link.Link( sourceId, destId );
                var removeIndex = -1;
                for ( var i = 0; i < this.m_destLinks.length; i++ ){
                    var drawInfo = this.m_destLinks[i];
                    if ( drawInfo.winId === destId ){
                      removeIndex = i;
                      break;
                    }
                }
                if ( removeIndex >= 0 ){
                    this.m_destLinks[removeIndex].linked = false;
                }
                this.update();
                this.fireDataEvent("linkRemove",link);
            },
            
            m_removeLinkButton : null

        }
    });