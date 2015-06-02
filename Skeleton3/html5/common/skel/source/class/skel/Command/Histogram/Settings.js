/**
 * Container for all commands which show/hide histogram configuration settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Histogram.Settings", {
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
        
        this.m_cmds.push( new skel.Command.Histogram.SettingsBinCount());
        this.m_cmds.push( new skel.Command.Histogram.SettingsCube());
        this.m_cmds.push( new skel.Command.Histogram.SettingsDisplay());
        this.m_cmds.push( new skel.Command.Histogram.SettingsRange());
        this.m_cmds.push( new skel.Command.Histogram.SettingsTwoD());
        this.m_cmds.push( new skel.Command.Histogram.SettingsClips());
        var cmdAll = new skel.Command.Settings.SettingsAll("Histogram Show All Settings", this.m_cmds);
        this.m_cmds.splice( 0, 0, cmdAll );
        this.setValue( this.m_cmds );
        
    },
    
    members : {
        
        _resetEnabled : function( ){
            arguments.callee.base.apply( this, arguments );
            //Update UI based on server side values.
            var enabled = this.resetPrefs( this.m_cmds );
            if ( this.isEnabled() != enabled ){
                this.setEnabled( enabled );
                qx.event.message.Bus.dispatch( new qx.event.message.Message("commandsChanged", null));
            }
        },
        
        getParent : function(){
            return skel.Command.Histogram.Histogram.getInstance();
        }
    }
});