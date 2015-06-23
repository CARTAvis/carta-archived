/**
 * Command to show/hide the image grid settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Grid.SettingsGrid", {
    extend : skel.Command.Settings.Setting,
    include : skel.Command.Settings.SettingsMixin,

    /**
     * Constructor.
     */
    construct : function( ) {
        var path = skel.widgets.Path.getInstance();
        var cmd = path.SEP_COMMAND + "setSettingsVisible";
        this.base( arguments, "Grid Settings", cmd);
        this.setToolTipText( "Show/hide grid settings.");
    },
    
    members : {
        
        _resetEnabled : function(){
            arguments.callee.base.apply(this, arguments);
            var cmds = [];
            cmds.push( this );
            var enabled = this.resetPrefs( cmds );
            if ( this.isEnabled() != enabled ){
                this.setEnabled( enabled );
            }
        },
        
        getParent : function(){
            return skel.Command.Grid.GridControls.getInstance();
        },
        
        /**
         * Update the visibility of the grid settings based on server state.
         * @param obj {Object} the server object containing visibility of individual
         *      user configuration settings.
         */
        resetValueFromServer : function( obj ){
            if ( this.getValue() != obj.grid ){
                this.setValue( obj.grid );
            }
        }
    }
});