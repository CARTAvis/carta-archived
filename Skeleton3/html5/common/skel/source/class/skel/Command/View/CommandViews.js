/**
 * Container for commands that allow users to switch the plug-ins they are viewing
 * in a particular window.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.View.CommandViews", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        var path = skel.widgets.Path.getInstance();
        var cmd = null;
        this.base( arguments, "View", null );
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds = [];
        this.setValue( this.m_cmds );
        
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
            var pathDict = skel.widgets.Path.getInstance();
            
            var paramMap = "pluginId:" + pathDict.PLUGINS +",index:0";
            var regViewCmd = pathDict.getCommandRegisterView();
            this.m_connector.sendCommand( regViewCmd, paramMap, this._viewPluginsCB( this ) );
        }
    },
    
    members : {
        
        /**
         * Callback for when the shared variable that represents loaded plug-ins changes;
         * updates the view menu.
         * @param anObject {Object} an object representing the plug-in view selections that
         *      are available.
         */
        _viewPluginsCB : function( anObject ){
            return function( id ){
                anObject.m_sharedVarPlugin = anObject.m_connector.getSharedVar(id);
                anObject.m_sharedVarPlugin.addCB(anObject._initPlugins.bind(anObject));
                anObject._initPlugins();
            };
        },
        
        /**
         * Initializes the available commands for switching views.
         */
        _initPlugins : function(){
            var val = this.m_sharedVarPlugin.get();
            if ( val ){
                try {
                    this.m_cmds = [];
                    var pluginNames = JSON.parse( val );
                    var pluginCount = pluginNames.pluginCount;
                    for ( var i = 0; i < pluginCount; i++ ){
                        var name = pluginNames.pluginList[i].name;
                        var errors = pluginNames.pluginList[i].loadErrors;
                        var loaded = (errors === "");
                        if ( loaded ){
                            var cmd = new skel.Command.View.CommandView( name );
                            this.m_cmds[i] = cmd;
                        }
                    }
                    this.setValue( this.m_cmds );
                    qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "commandsChanged", null));
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
                }
            }
        },
       
        m_sharedVarPlugin : null
    }
});