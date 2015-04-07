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
        this.m_value = true;
        this.m_global = true;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var params = "menuVisible:"+vals;
            this.sendCommand( path.PREFERENCES, params, undoCB );
        },
        
        setValue : function ( prefObj ){
            var oldValue = this.getValue();
            if ( oldValue !== prefObj.menuVisible ){
                this.m_value = prefObj.menuVisible;
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "layoutChanged", null));
            }
        },
        
        getToolTip : function(){
            return "Show/hide the menu bar.";
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        }
    }
});