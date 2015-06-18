/**
 * Container for all commands which show/hide grid configuration settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Grid.Settings", {
    extend : skel.Command.CommandComposite,
    include : skel.Command.Settings.SettingsMixin,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Grid Settings", null );
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds = [];
        
        this.m_cmds.push( new skel.Command.Grid.SettingsGrid());
        this.m_cmds.push( new skel.Command.Grid.SettingsGridColor());
        this.m_cmds.push( new skel.Command.Grid.SettingsGridFont());
        
        var cmdAll = new skel.Command.Settings.SettingsAll("Grid Show All Settings", this.m_cmds);
        this.m_cmds.splice( 0, 0, cmdAll );
        this.setValue( this.m_cmds );
        
    },
    
    members : {
        getParent : function(){
            return skel.Command.Grid.GridControls.getInstance();
        },
        
        _resetEnabled : function( ){
            arguments.callee.base.apply( this, arguments );
            //Update UI based on server side values.
            var enabled = this.resetPrefs( this.m_cmds );
            if ( this.isEnabled() != enabled ){
                this.setEnabled( enabled );
                qx.event.message.Bus.dispatch( new qx.event.message.Message("commandsChanged", null));
            }
        }
        
       
    }
});