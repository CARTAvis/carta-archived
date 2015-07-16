/**
 * Command to save an image in an active window.
 */

qx.Class.define("skel.Command.Save.CommandSaveImage", {
    extend : skel.Command.Command,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Save...", null );
        this.setEnabled( false );
        this.m_global = false;
        this.setToolTipText( "Save the window image.");
        
        var path = skel.widgets.Path.getInstance();
        var cmd = path.BASE_PATH + path.DATA_LOADER + path.SEP_COMMAND + "isSecurityRestricted";
        var params = "";
        this.m_connector.sendCommand( cmd, params, this._updateSaveAccess);
    },
    
    members : {
        
        doAction : function( vals, undoCB ){
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                //Use the first one in the list that supports this cmd.
                for ( var i = 0; i < activeWins.length; i++ ){
                    if ( activeWins[i].isCmdSupported( this ) ){
                        qx.event.message.Bus.dispatch(new qx.event.message.Message(
                                "showFileSaver", activeWins[i]));
                        break;
                    }
                }
            }
        },
        
        /**
         * Returns whether or not the user is allowed to save images.
         * @return {boolean} - true if images can be saved; false otherwise.
         */
        isSaveAvailable : function(){
            return this.m_saveAvailable;
        },
        
        /**
         * Reset whether save functionality should be available to the
         * user based on information from the server.
         * @param val {String} - a string representation for a boolean; true if
         *      save is available and false otherwise.
         */
        _updateSaveAccess : function( val ){
            if ( val === "true"){
                this.m_saveAvailable = true;
            }
            else {
                this.m_saveAvailable = false;
            }
        },
        
        m_saveAvailable : false
    
    }
});