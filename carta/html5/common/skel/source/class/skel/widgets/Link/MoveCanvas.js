/**
 * Displays possible window relocation positions.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
/*******************************************************************************/

qx.Class.define("skel.widgets.Link.MoveCanvas",{
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
             * Return help information for the move canvas.
             * @return {String} help information for the move canvas.
             */
            getHelp : function( ){
                return "Draw a line from the source window (blue) to its new location(black); escape to exit.";
            },
            
            /**
             * Show a dialog asking the user to confirm the move.
             * @param link {skel.widgets.Link} - information about the move source and destination.
             * @param pt {Object} - where the user clicked on the screen.
             */
            linkCompleted : function( link, pt ){
                this.m_link = link;
                if (this.m_confirmMoveDialog === null) {
                    this.m_confirmMoveDialog = new skel.widgets.Link.MoveConfirmDialog();
                    this.m_confirmMoveDialog.addListener( "moveConfirmed", function(){
                        this._sendMoveCmd( );
                        this._quit();
                    }, this );
                    this.m_confirmMoveDialog.addListener( "moveCanceled", function(){
                        this.update();
                    }, this );
                }
                this.m_confirmMoveDialog.placeToPoint(
                                {
                                    left : pt.x,
                                    top : pt.y
                                },
                                false);
                this.m_confirmMoveDialog.show();
            },
            
            /**
             * Send a command to the server indication the source and destination locations for
             * the window that is being moved.
             * @param link {skel.widgets.Link.Link} identification of the source and destination move
             *          windows.
             */
            _sendMoveCmd : function(){
                var connector = mImport( "connector");
                var path = skel.widgets.Path.getInstance();
                var cmd = path.getCommandMoveWindow();
                var destLinkIndex = -1;
                var destTarget = this.m_link.getDestination();
                console.log( "Looking for:"+ destTarget + ": dest count="+this.m_destLinks.length);
                for ( var i = 0; i < this.m_destLinks.length; i++ ){
                    console.log( "i="+i+" winId="+this.m_destLinks[i].winId+":" );
                    if ( destTarget == this.m_destLinks[i].winId ){
                        destLinkIndex = i;
                        console.log( "Set destLinkIndex="+i);
                        break;
                    }
                }
                if ( destLinkIndex >= 0 ){
                    var params = "sourcePlugin:"+this.m_sourceLink.pluginId+",sourceLocateId:"+this.m_sourceLink.locationId+
                        ",destPlugin:"+this.m_destLinks[destLinkIndex].pluginId+",destLocateId:"+this.m_destLinks[destLinkIndex].locationId;
                    console.log( "Sending cmd="+cmd);
                    console.log( "Params="+params);
                    connector.sendCommand( cmd , params, function(){});
                }
                else {
                    console.log( "Could not find destination for window");
                }
            },
       
            /**
             * Exit the move canvas.
             */
            _quit : function(){
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "moveFinished", ""));
            },
            m_link : null,
            m_confirmMoveDialog : null
        }
    });