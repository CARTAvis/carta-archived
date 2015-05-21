/**
 * Handles functionality for containers that contain commands for 
 * showing/hiding user configuration settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Mixin.define("skel.Command.Settings.SettingsMixin", {
    
    
    members : {
        
        
        
        /**
         * Determines the active window (user show/hide settings) and notifies 
         * child commands.
         * @param cmds {Array} the list of child show/hide user settings cmds.
         */
        resetPrefs : function( cmds ){
            var enabled = false;
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                for ( var i = 0; i < activeWins.length; i++ ){
                    if ( activeWins[i].isCmdSupported( this.getParent() ) ){
                        var prefs = activeWins[i].getPreferences();
                        var prefId = activeWins[i].getPreferencesId();
                        try {
                            var setObj = JSON.parse( prefs );
                            this.setCmdSettings( prefId, setObj, cmds );
                        }
                        catch( err ){
                            console.log( "Could not parse histogram settings");
                        }
                        enabled = true;
                        break;
                    }
                }
            }
            return enabled;
        },
        
        
        /**
         * Update from the the server; shows or hides the setting.
         * @param obj {Object} the server side state.
         * @param id {String} the server side id of user preferences.
         * @param cmds {Array} the list of child user preference settings.
         */
        setCmdSettings : function( id, obj, cmds ){
            //Wait do do All Settings until the individual ones have been set.
            for ( var i = 1; i < cmds.length; i++ ){
                cmds[i].setSettings( id, obj );
            }
            cmds[0].setSettings( id, obj );
        }
    }
});