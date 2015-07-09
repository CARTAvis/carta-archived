/**
 * Represents a user command (something a user can trigger.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/


qx.Class.define("skel.Command.Command", {
    extend : qx.ui.core.Command,

    /**
     * Constructor.
     * @param label {String} a user-friendly string for displaying.
     * @param cmd {String} a string identifying what should happen on the server-side
     *          when the command is executed.
     */
    construct : function( label, cmd ) {
        //No shortcut at this time
        this.base( arguments, "" );
        this.setLabel(label);
        this.m_cmd = cmd;
        this.m_menuVisible = true;
        this.m_toolBarVisible = false;
        this.m_connector = mImport("connector");
    },
    
    statics : {
        
        /**
         * Add the server-side id of a user selected window to the list of active windows.
         * @param win {skel.widgets.Window.DisplayWindow} the active window.
         */
        addActiveWindow : function( win ){
            if ( skel.Command.Command.m_activeWins.indexOf( win) < 0 ){
                skel.Command.Command.m_activeWins.push( win );
                skel.Command.Command._resetEnabledCmds();
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "commandsChanged", null));
            }
        },
        
        /**
         * Remove all windows for the selected window list.
         */
        clearActiveWindows : function(){
            if ( skel.Command.Command.m_activeWins.length > 0 ){
                skel.Command.Command.m_activeWins = [];
                skel.Command.Command._resetEnabledCmds();
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "commandsChanged", null));
            }
        },
        
        /**
         * Go through and set the enabled status of all children commands.
         */
        _resetEnabledCmds : function(){
            var allCmd = skel.Command.CommandAll.getInstance();
            allCmd._resetEnabled();
        },
        
        /**
         * Remove the server-side if of a window that is no longer active.
         * @param win {skel.widgets.Window.DisplayWindow} the window that is no longer active.
         */
        removePassiveWindow : function( win ){
            var winIndex = skel.Command.Command.m_activeWins.indexOf( win );
            if ( winIndex >= 0 ){
                skel.Command.Command.m_activeWins.splice( winIndex );
                skel.Command.Command._resetEnabledCmds();
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "commandsChanged", null));
            }
        },
        
        m_activeWins : [],
        MENU : "Menu",
        TOOLBAR : "Toolbar",
        TYPE_GROUP : "group",
        TYPE_COMPOSITE : "composite",
        TYPE_BOOL : "boolean",
        TYPE_BUTTON : "button"
    },
    
   
    members : {
        
        
        /**
         * Execute this command.
         * @param valMap {Object} parameters to pass to the server.
         * @param undoCB {Function} possible undo command, currently not implemented.
         */
        doAction : function( valMap, undoCB ){
            console.log( "doAction not implemented for "+this.getLabel());
        },
        
        /**
         * Returns this command if cmdName matches the name of this command or null
         * if there is no match.
         * @param cmdName {String} an identifier for a command.
         */
        getCommand : function( cmdName ){
            var cmd = null;
            if ( this.isMatch( cmdName )){
                cmd = this;
            }
            return cmd;
        },
        
        
        
        /**
         * Returns the type of the command; for example, some commands are composite in that they
         * are containers for other commands.
         * @return {String} the type of command.
         */
        getType : function(){
            return skel.Command.Command.TYPE_BUTTON;
        },
        
        /**
         * Returns whether or not any of the selected windows support this Command.
         * @return {boolean} - true if at least one active window supports the command;
         *      false otherwise.
         */
        _isCmdSupported : function(){
            var activeWins = skel.Command.Command.m_activeWins;
            var supported = false;
            if ( activeWins !== null && activeWins.length > 0 ){
                var activeWinCount = activeWins.length;
                for ( var i = 0; i < activeWinCount; i++ ){
                    if ( activeWins[i].isCmdSupported( this ) ){
                        supported = true;
                        break;
                    }
                }
            }
            return supported;
        },
        
        /**
         * Returns true if this command is not window specific; false if the command is only
         * applicable when particular windows are selected.
         * @return {boolean} true if the command applies globally; false if it only applies
         *      to particular selected windows.
         */
        isGlobal : function(){
            return this.m_global;
        },
       
        
        /**
         * Returns true if this command has the given name.
         * @param cmdName {String} the name of a command.
         * @return {boolean} true if this command has the given name; false otherwise.
         */
        isMatch : function( cmdName ){
            var match = false;
            if ( cmdName === this.getLabel() ){
                match = true;
            }
            return match;
        },
        
        /**
         * Returns true if the command is visible from the menu bar; false
         *      otherwise.
         * @return {boolean} true if this is a menubar visible command; false otherwise.
         */
        isVisibleMenu : function(){
            return this.m_menuVisible;
        },
        
        /**
         * Returns true if the command is visible from the tool bar; false
         *      otherwise.
         * @return {boolean} true if the command is tool bar visible; false otherwise.
         */
        isVisibleToolbar : function(){
            return this.m_toolBarVisible;
        },
        
        /**
         * Returns whether the command is visible from the object identified
         * by the id.
         * @param id {String} an identifier for an object where the command can be displayed.
         * @return {boolean} true if the command is visible from the object; false otherwise.
         */
        isVisible : function( id ){
            var visible = false;
            if ( id === skel.Command.Command.MENU ){
                visible = this.isVisibleMenu();
            }
            else if ( id === skel.Command.Command.TOOLBAR ){
                visible = this.isVisibleToolbar();
            }
            else {
                console.log( "Unrecognized vis getter id="+id);
            }
            return visible;
        },
        
        
        
        /**
         * Tell the server to perform the command.
         * @param id {String} the server-side id of the object that should receive the command.
         * @param params {String} the parameters to send with the command.
         * @param cbFunction {Function} a callback - not implemented.
         */
        sendCommand : function(  objectId, params, cbFunction ){
            if ( this.m_cmd !== null ){
                var fullCmd = objectId + this.m_cmd;
                if ( cbFunction === null ){
                    cbFunction = function(){};
                }
                this.m_connector.sendCommand( fullCmd, params, cbFunction);
            }
        },
        
        /**
         * Reset whether or not the command is enabled based on the windows that
         * are selected.
         */
        _resetEnabled : function( ){
            if ( ! this.m_global ){
                var enable = this._isCmdSupported();
                if ( this.isEnabled() != enable ){
                    this.setEnabled( enable );
                }
            }
        },
        
        setSettings : function( id, obj ){},
        
        /**
         * Sets the command visible/invisible for the particular object identified by the id.
         * @param id {String} the idea of an object that can display a command.
         * @param visible {boolean} true if the command should be visible; false otherwise.
         */
        setVisible : function( id, visible ){
            if ( id === skel.Command.Command.MENU ){
                this.setVisibleMenu( visible );
            }
            else if ( id === skel.Command.Command.TOOLBAR ){
                this.setVisibleToolbar( visible);
            }
            else {
                console.log( "Unrecognized vis setter id="+id);
            }
        },
        
        /**
         * Set the command visible/invisible on the menu bar.
         * @param visible {boolean} true if the command should be visible on the menu; false otherwise.
         */
        setVisibleMenu : function( visible){
            if ( this.m_menuVisible != visible ){
                this.m_menuVisible = visible;
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "commandVisibilityMenuChanged", ""));
            }
        },
        
        /**
         * Set the command visible/invisible on the tool bar.
         * @param visible {boolean} true if the command should be visible on the tool bar; false otherwise.
         */
        setVisibleToolbar : function( visible ){
            if ( this.m_toolBarVisible != visible ){
                this.m_toolBarVisible = visible;
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "commandVisibilityToolChanged", ""));
            }
        },
        
       
        
        m_connector : null,
        m_cmd : null,
        //Whether the commands acts globally across all windows or is window specific.
        m_global : true,
        
        m_menuVisible : null,
        m_toolBarVisible : null
    }
});