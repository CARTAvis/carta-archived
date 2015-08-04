/**
 * Handles registration and updates of the shared variable containing information about
 * state that is saved.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/
qx.Mixin.define("skel.Command.Session.SnapRegistration", {

    members : {
        
        /**
         * Send the command to get the shared variable containing snapshot information.
         * @param callback {Function} the function to be called when the shared variable changes.
         */
        registerSnapshot : function( callback ){
            this.m_connector = mImport( "connector");
            var params = "";
            this.m_callback = callback;
            var pathDict = skel.widgets.Path.getInstance();
            var regCmd = pathDict.getCommandRegisterSnapshots();
            this.m_connector.sendCommand( regCmd, params, this._registrationCallback(this));
        },
        
        /**
         * Initializes the shared variable containing snapshot information.
         * @param anObject {Object} the class needing a shared variable containing snapshot information.
         */
        _registrationCallback : function( anObject ){
            return function( id ){
                if ( id && id.length > 0 ){
                    if ( id != anObject.m_identifier ){
                        anObject.m_identifier = id;
                        anObject.m_sharedVar = anObject.m_connector.getSharedVar( anObject.m_identifier );
                        if ( anObject.m_callback !== null ){
                            anObject.m_sharedVar.addCB( anObject.m_callback.bind( anObject ));
                            anObject.m_callback();
                        }
                    }
                }
            };
        },
       
        m_callback : function(){},
        m_connector : null,
        m_sharedVar : null,
        m_identifier : null
        
    }

});
