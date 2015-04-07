/**
 * Command to add a window to the display.
 */

qx.Class.define("skel.Command.Window.CommandWindowAdd", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "addWindow";
        this.base( arguments, "Add", cmd );
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                var path = skel.widgets.Path.getInstance();
                for ( var i = 0; i < skel.Command.Command.m_activeWins.length; i++ ){
                    var window = skel.Command.Command.m_activeWins[i];
                    var row = window.getRow();
                    var col = window.getCol();
                    var params = "row:"+row+",col:"+col;
                    this.sendCommand( path.LAYOUT, params, undoCB );
                }
            }
        },
        
        getToolTip : function(){
            return "Add an additional window.";
        }
    }
});