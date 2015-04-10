/**
 * Container for commands that determine whether the menubar, toolbar, statusbar,
 * etc should be shown..
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Preferences.Show.CommandShow", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    construct : function( ) {
        this.base( arguments, "Show" );
        this.m_global = true;
        this.m_cmds = [];
        this.m_cmds[0] = skel.Command.Preferences.Show.CommandShowMenu.getInstance();
        this.m_cmds[1] = skel.Command.Preferences.Show.CommandShowStatus.getInstance();
        this.m_cmds[2] = skel.Command.Preferences.Show.CommandShowToolBar.getInstance();
        this.setValue( this.m_cmds );
    },
    
    members : {
    
        setPrefs : function( val ){
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                this.m_cmds[i].setPrefs( val );
            }
        }
    }
});