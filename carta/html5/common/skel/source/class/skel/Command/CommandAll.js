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
        var index = 8;
        this.m_cmds[index] = skel.Command.Region.CommandRegions.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Save.CommandSaveImage.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Data.CommandPanReset.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Data.CommandZoomReset.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Settings.SettingsImage.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Animate.CommandAnimations.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Settings.SettingsProfile.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Settings.SettingsHistogram.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Settings.SettingsColor.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Settings.SettingsStatistics.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Settings.SettingsContext.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Settings.SettingsZoom.getInstance();
        index++;
        this.m_cmds[index] = skel.Command.Popup.CommandPopup.getInstance();
        index++;
        
        //Except that global help always comes last
        this.m_cmds[index] = skel.Command.Help.CommandHelp.getInstance();
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