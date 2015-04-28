/**
 * Container for commands to close specific images.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Data.CommandDataClose", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments, "Close" );
        this.m_cmds = [];
        this.setEnabled( false );
        this.m_global = false;
        this.setToolTipText("Close data...");
        this.setValue( this.m_cmds );
    },
    
    members : {
        /**
         * The commands to close individual images have changed.
         */
        // Needed so that if data is added to an image that is already selected, i.e.,
        // enabled status has not changed, but data count has, the close image commands
        // will be updated.
        closeChanged : function(){
            this._resetEnabled();
        },
        
        _resetEnabled : function( ){
            arguments.callee.base.apply( this, arguments );
            //Dynamically create close image commands based on the active windows.
            this.m_cmds = [];
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                //Use the first one in the list that supports this cmd.
                var k = 0;
                var dataCmd = skel.Command.Data.CommandData.getInstance();
                for ( var i = 0; i < activeWins.length; i++ ){
                    if ( activeWins[i].isCmdSupported( dataCmd ) ){
                        var closes = activeWins[i].getCloses();
                        for ( var j = 0; j < closes.length; j++ ){
                            this.m_cmds[k] = new skel.Command.Data.CommandDataCloseImage( closes[j]);
                            k++;
                        }
                    }
                }
            }
            if ( this.m_cmds.length > 0 ){
                this.setEnabled( true );
            }
            else {
                this.setEnabled( false );
            }
            this.setValue( this.m_cmds );
            qx.event.message.Bus.dispatch(new qx.event.message.Message(
                    "commandsChanged", null));
        }
    }

});