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
        this.base( arguments, "Show Tool Bar", cmd);
        this.setValue( true );
        this.m_global = true;
        this.setToolTipText( "Show/hide the toolbar.");
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var params = "toolBarVisible:"+vals;
            this.sendCommand( path.PREFERENCES, params, undoCB );
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
        setPrefs : function ( prefObj ){
            if ( this.getValue() !== prefObj.toolBarVisible ){
                this.setValue( prefObj.toolBarVisible);
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "layoutChanged", null));
            }
        }
    }
});