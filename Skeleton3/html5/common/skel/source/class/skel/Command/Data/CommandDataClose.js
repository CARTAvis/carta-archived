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
    },
    
    members : {
        //Overriden to dynamically create close image commands based on the
        //active window.
        getValue : function(){
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
            return this.m_cmds;
        },
        
        getToolTip : function(){
            return "Close data...";
        }
    }

});