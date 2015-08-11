/**
 * Container for commands that window manipulation in the display.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Window.CommandWindow", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Window", null );
        this.m_cmds = [];
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds[0] = skel.Command.Window.CommandWindowMaximize.getInstance();
        this.m_cmds[1] = skel.Command.Window.CommandWindowMinimize.getInstance();
        this.m_cmds[2] = skel.Command.Window.CommandWindowRemove.getInstance();
        this.m_cmds[3] = skel.Command.Window.WindowAdd.getInstance();
        this.m_cmds[4] = skel.Command.Window.WindowMove.getInstance();
        this.setValue( this.m_cmds );
        
        this.m_cmds[0].addListener( "windowMaximized", this._windowMaximized, this );
        var restoreCmd = skel.Command.Window.CommandWindowRestore.getInstance();
        restoreCmd.addListener( "windowRestored", this._windowRestored, this );
    },
    
    members : {
        /**
         * Update the available commands when a window is maximized.
         */
        _windowMaximized : function(){
            //Remove the maximize and minimize cmds
            var maxCmd = skel.Command.Window.CommandWindowMaximize.getInstance();
            var maxIndex = this.m_cmds.indexOf( maxCmd );
            if ( maxIndex >= 0 ){
                this.m_cmds.splice( maxIndex, 1 );
            }
            var minCmd = skel.Command.Window.CommandWindowMinimize.getInstance();
            var minIndex = this.m_cmds.indexOf( minCmd );
            if ( minIndex >= 0 ){
                this.m_cmds.splice( minIndex, 1 );
            }
            //Insert the restore cmd
            var restoreCmd = skel.Command.Window.CommandWindowRestore.getInstance();
            var restoreIndex = this.m_cmds.indexOf( restoreCmd );
            if ( restoreIndex < 0 ){
                this.m_cmds.splice( maxIndex, 0, restoreCmd);
            }
        },
        
        /**
         * Update the available commands when a window is restored.
         */
        _windowRestored : function(){
            //Remove the restore cmd
            var restoreCmd = skel.Command.Window.CommandWindowRestore.getInstance();
            var restoreIndex = this.m_cmds.indexOf( restoreCmd );
            if ( restoreIndex >= 0 ){
                this.m_cmds.splice( restoreIndex, 1 );
            }
            //Insert the maximize and minimize cmd
            var maxCmd = skel.Command.Window.CommandWindowMaximize.getInstance();
            var maxIndex = this.m_cmds.indexOf( maxCmd );
            if (maxIndex < 0) {
                this.m_cmds.splice(restoreIndex, 0, maxCmd );
            }
            var minCmd = skel.Command.Window.CommandWindowMinimize.getInstance();
            var minIndex = this.m_cmds.indexOf( minCmd );
            if ( minIndex < 0 ){
                this.m_cmds.splice( restoreIndex, 0, minCmd );
            }
        }
    }
});