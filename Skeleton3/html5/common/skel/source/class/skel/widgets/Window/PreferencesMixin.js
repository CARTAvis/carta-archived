/**
 * Handles functionality for windows that have the ability to show/hide
 * user preferences.
 */

/*******************************************************************************
 * 
 * 
 * 
 ******************************************************************************/

qx.Mixin.define("skel.widgets.Window.PreferencesMixin", {

        members : {
            
            /**
             * Returns the contents of the user preferences shared variable.
             * @return {String} containing user preferences.
             */
            getPreferences : function(){
                if ( this.m_sharedVarPrefs !== null ){
                    return this.m_sharedVarPrefs.get();
                }
            },
            
            /**
             * Returns the server-side id of the user preferences.
             * @return {String} containing the user preferences.
             */
            getPreferencesId : function(){
                return this.m_prefId;
            },
            
            
            /**
             * Initializes the callback for updating preferences for the object.
             * @param anObject {Object} the parent object for which this manages 
             *          user preferences.
             */
            _preferencesCallback : function( anObject ){
                return function( id ){
                    if ( id && id.length > 0 ){
                        anObject.m_prefId = id;
                        anObject.m_sharedVarPrefs = anObject.m_connector.getSharedVar( id );
                        anObject.m_sharedVarPrefs.addCB( anObject._preferencesCB.bind( anObject ));
                        anObject._preferencesCB();
                    }
                };
            },
            
           
            /**
             * Sends a command to the server to get the id of the preferences.
             */
            initializePrefs : function( ) {
                //Get the  preferences shared var.
                var paramMap = "";
                var path = skel.widgets.Path.getInstance();
                var regCmd = this.getIdentifier() + path.SEP_COMMAND + "registerPreferences";
                this.m_connector.sendCommand( regCmd, paramMap, this._preferencesCallback(this));
            },
            
            m_prefId : null,
            m_sharedVarPrefs : null,
            m_parent : null
        }
});
