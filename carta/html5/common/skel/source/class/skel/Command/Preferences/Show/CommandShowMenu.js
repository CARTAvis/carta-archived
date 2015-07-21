/**
 * Command to show/hide the menu bar.
 */

qx.Class.define("skel.Command.Preferences.Show.CommandShowMenu", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setMenuVisible";
        this.base( arguments, "Show Menu", cmd );
        this.setValue( true );
        this.m_global = true;
        this.setToolTipText( "Show/hide the menu bar.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var params = "menuVisible:"+vals;
            this.sendCommand( path.PREFERENCES, params, undoCB );
        },
        
        setPrefs : function ( prefObj ){
            var oldValue = this.getValue();
            if ( oldValue !== prefObj.menuVisible ){
                this.setValue( prefObj.menuVisible );
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "layoutChanged", null));
            }
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        }
    }
});