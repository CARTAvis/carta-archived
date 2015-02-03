/**
 * Represents a user command (something a user can trigger.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/


qx.Class.define("skel.widgets.Command.Command", {
    extend : qx.core.Object,

    /**
     * Constructor.
     * @param label {String} a user-friendly string for displaying.
     * @param cmd {String} a string identifying what should happen on the server-side
     *          when the command is executed.
     */
    construct : function( label, cmd ) {
        this.base( arguments );
        this.m_title = label;
        this.m_cmd = cmd;
        this.m_menuVisible = true;
        this.m_toolBarVisible = false;
        this.m_connector = mImport("connector");
       
    },
    
    events : {
      "cmdValueChanged" : "qx.event.type.Data"
    },
    
    statics : {
        MENU : "Menu",
        TOOLBAR : "Toolbar",
        TYPE_GROUP : "group",
        TYPE_COMPOSITE : "composite",
        TYPE_BOOL : "boolean"
    },
    
   
    members : {
        
        /**
         * Execute this command.
         * @param valMap {Object} parameters to pass to the server.
         * @param objectIDs {Array} server-side ids of receivers of the command.
         * @param undoCB {Function} possible undo command, currently not implemented.
         */
        doAction : function( valMap, objectIDs, undoCB ){
            console.log( "doAction not implemented for "+this.m_title);
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
         * Returns the user-friendly label for the command.
         * @return {String} the user-friendly label for the command.
         */
        getLabel : function(){
            return this.m_title;
        },
        
        /**
         * Returns a tool tip for the command.
         * @return {String} a tool tip for the command.
         */
        getToolTip : function(){
            return "";
        },
        
        /**
         * Returns the type of the command; for example, some commands are composite in that they
         * are containers for other commands.
         * @return {String} the type of command.
         */
        getType : function(){
            return "undefined";
        },
        
        /**
         * Returns the current value of this command.
         * @return {Object} the current command value.
         */
        getValue : function(){
            return this.m_value;
        },
        
       
        
        /**
         * Returns true if this command has the given name.
         * @param cmdName {String} the name of a command.
         * @return {boolean} true if this command has the given name; false otherwise.
         */
        isMatch : function( cmdName ){
            var match = false;
            if ( cmdName === this.m_title ){
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
            if ( id === skel.widgets.Command.Command.MENU ){
                visible = this.isVisibleMenu();
            }
            else if ( id === skel.widgets.Command.Command.TOOLBAR ){
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
         * Sets the current command value and notifies listeners that the value
         * has changed.
         * @param value {Object} the new command value.
         */
        setValue : function( value ){
            if ( this.m_value != value ){
                this.m_value = value;
                var data = {value: this.m_value};
                this.fireDataEvent( "cmdValueChanged", data );
            }
        },
        
        /**
         * Sets the command visible/invisible for the particular object identified by the id.
         * @param id {String} the idea of an object that can display a command.
         * @param visible {boolean} true if the command should be visible; false otherwise.
         */
        setVisible : function( id, visible ){
            return function( visible ){
                if ( id === skel.widgets.Command.Command.MENU ){
                    this.setVisibleMenu( visible );
                }
                else if ( id === skel.widgets.Command.Command.TOOLBAR ){
                    this.setVisibleToolbar( visible);
                }
                else {
                    console.log( "Unrecognized vis setter id="+id);
                }
            };
        },
        
        /**
         * Set the command visible/invisible on the menu bar.
         * @param visible {boolean} true if the command should be visible on the menu; false otherwise.
         */
        setVisibleMenu : function( visible){
            if ( this.m_menuVisible != visible ){
                this.m_menuVisible = visible;
            }
        },
        
        /**
         * Set the command visible/invisible on the tool bar.
         * @param visible {boolean} true if the command should be visible on the tool bar; false otherwise.
         */
        setVisibleToolbar : function( visible ){
            if ( this.m_toolBarVisible != visible ){
                this.m_toolBarVisible = visible;
            }
        },
        
       
        
        m_connector : null,
        m_value : null,
        m_cmd : null,
        m_title : null,
        m_menuVisible : null,
        m_toolBarVisible : null
    }
});