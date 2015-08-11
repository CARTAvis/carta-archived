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
         */
        resetPrefs : function(){
            var enabled = false;
            var activeWins = skel.Command.Command.m_activeWins;
            if ( activeWins !== null && activeWins.length > 0 ){
                for ( var i = 0; i < activeWins.length; i++ ){
                    var cmdSupported = activeWins[i].isCmdSupported( this);
                    if ( cmdSupported  ){
                        var prefs = activeWins[i].getPreferences();
                        var prefId = activeWins[i].getPreferencesId();
                        try {
                            var setObj = JSON.parse( prefs );
                            this.setCmdSettings( prefId, setObj/*, cmds*/ );
                        }
                        catch( err ){
                            console.log( "Could not parse settings");
                            console.log( "Error: "+err);
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
            this.setSettings( id, obj );
        }
    }
});