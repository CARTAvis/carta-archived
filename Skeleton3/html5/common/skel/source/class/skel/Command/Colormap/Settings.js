/**
 * Container for commands that colormap show/hide user configuration settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Colormap.Settings", {
    extend : skel.Command.CommandComposite,
    include : skel.Command.Settings.SettingsMixin,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Settings", null );
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds = [];
        this.m_cmds.push( new skel.Command.Colormap.SettingsMix());
        this.m_cmds.push( new skel.Command.Colormap.SettingsModel());
        this.m_cmds.push( new skel.Command.Colormap.SettingsScale());
        this.m_cmds.push( new skel.Command.Colormap.SettingsTransform());
        this.m_cmds.splice( 0, 0, new skel.Command.Settings.SettingsAll("Colormap Show All Settings", this.m_cmds));
        this.setValue( this.m_cmds );
        
    },
    
    members : {
        
        _resetEnabled : function( ){
            arguments.callee.base.apply( this, arguments );
            
            //Update UI based on server side values.
            var enabled = this.resetPrefs( this.m_cmds );
            if ( this.isEnabled() != enabled ){
                this.setEnabled( enabled );
                qx.event.message.Bus.dispatch( 
                        new qx.event.message.Message("commandsChanged", null));
            }
        },
        
        getParent : function(){
            return skel.Command.Colormap.Colormap.getInstance();
        }
      
    }
});