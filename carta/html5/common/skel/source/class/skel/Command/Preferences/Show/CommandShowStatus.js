/**
 * Command to show/hide the status bar.
 */

qx.Class.define("skel.Command.Preferences.Show.CommandShowStatus", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setStatusVisible";
        this.base( arguments, "Show Status", cmd );
        this.m_toolBarVisible = false;
        this.setValue( true );
        this.m_global = true;
        this.setToolTipText( "Show/hide the status bar.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var params = "statusVisible:"+vals;
            this.sendCommand( path.PREFERENCES, params, undoCB );
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
        setPrefs : function( prefObj ){
            var oldValue = this.getValue();
            if ( oldValue !== prefObj.statusVisible ){
                this.setValue( prefObj.statusVisible);
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "layoutChanged", null));
            }
        }
    }
});