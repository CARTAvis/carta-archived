/**
 * Command to show/hide the customizable toolbar.
 */

qx.Class.define("skel.Command.Preferences.Show.CommandShowToolBar", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setToolBarVisible";
        this.base( arguments, "Show Toolbar", cmd);
        this.m_value = true;
        this.m_global = true;
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var params = "toolBarVisible:"+vals;
            this.sendCommand( path.PREFERENCES, params, undoCB );
        },
        
        getToolTip : function(){
            return "Show/hide the toolbar.";
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
        setValue : function ( prefObj ){
            var oldValue = this.getValue();
            if ( oldValue !== prefObj.toolBarVisible ){
                this.m_value = prefObj.toolBarVisible;
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "layoutChanged", null));
            }
        }
    }
});