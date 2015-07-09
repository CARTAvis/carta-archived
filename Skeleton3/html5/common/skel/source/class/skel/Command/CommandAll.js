/**
 * Root command.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.CommandAll", {
    extend : skel.Command.CommandComposite,
    type : "singleton",
    
    /**
     * Constructor
     */
    construct : function( ) {
        this.base( arguments, "All");
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds = [];
        
        //First the globals
        
        this.m_cmds[0] = skel.Command.Session.CommandSession.getInstance();
        this.m_cmds[1] = skel.Command.Layout.CommandLayout.getInstance();
        this.m_cmds[2] = skel.Command.Preferences.CommandPreferences.getInstance();
        
        //Now the window specific ordered from most popular to least supported.
        this.m_cmds[3] = skel.Command.View.CommandViews.getInstance();
        this.m_cmds[4] = skel.Command.Link.CommandLink.getInstance();
        this.m_cmds[5] = skel.Command.Window.CommandWindow.getInstance();
        this.m_cmds[6] = skel.Command.Clip.CommandClip.getInstance();
        this.m_cmds[7] = skel.Command.Data.CommandData.getInstance();
        this.m_cmds[8] = skel.Command.Settings.SettingsGrid.getInstance();
        this.m_cmds[9] = skel.Command.Animate.CommandAnimations.getInstance();
        this.m_cmds[10] = skel.Command.Settings.SettingsHistogram.getInstance();
        this.m_cmds[11] = skel.Command.Settings.SettingsColor.getInstance();
        this.m_cmds[12] = skel.Command.Popup.CommandPopup.getInstance();
        
        //Except that global help always comes last
        this.m_cmds[13] = skel.Command.Help.CommandHelp.getInstance();
        this.setValue( this.m_cmds );
    },
    
    members : {
        _resetEnabled : function( ){
            //Just tell the children to reset their enabled status
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                this.m_cmds[i]._resetEnabled();
            }
        }
    }

});