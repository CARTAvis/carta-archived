/**
 * Listens for errors on the server and displays them according to user preferences.
 *
 * @ignore( mImport)
 *
 */

qx.Class.define("skel.widgets.ErrorHandler", {
    extend : qx.core.Object,
    type : "singleton",

    /**
     * Constructor
     */
    construct : function() {
        this.base(arguments);
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport( "connector");
            var path = skel.widgets.Path.getInstance();
            this.m_sharedVar = this.m_connector.getSharedVar( path.ERROR_HANDLER );
            this.m_sharedVar.addCB( this._errorStatusCB.bind( this ));
            this._errorStatusCB();
        }
    },

    members : {
        /**
         * Clear the status bar of errors
         */
        clearErrors : function(){
            if ( this.m_statusBar !== null ){
                this.m_statusBar.clearMessages();
            }
        },
        
        /**
         * Callback for an error status change on the server.
         */
        _errorStatusCB : function(){
            if ( this.m_sharedVar !== null ){
                var statusStr = this.m_sharedVar.get();
                var errors = JSON.parse( statusStr );
                if ( errors ){
                    //Clear out any old errors.
                    if ( this.m_statusBar !== null ){
                        this.m_statusBar.clearMessages();
                    }
                    if ( errors.errorsExist === true ){
                       
                        //Send a command to get the new errors.
                        var path = skel.widgets.Path.getInstance();
                        var cmd = path.ERROR_HANDLER + path.SEP_COMMAND + "getErrors";
                        var params = "";
                        this.m_connector.sendCommand( cmd, params, this._getErrorListCB(this));
                    }
                }
            }
        },
        
        /**
         * Callback for the list of errors on the server.
         * @param anObject {skel.widgets.ErrorHandler}.
         */
        _getErrorListCB : function( anObject ){
            return function( errorList ){
                anObject.updateErrors( errorList );
            };
        },
        
        /**
         * Stores the status area for posting messages.
         * @param bar {skel.widgets.Window.MenuBar}.
         */
        setStatusBar : function( bar ){
            this.m_statusBar = bar;
        },
        
        /**
         * Posts the errors to the user for viewing.
         * @param errors {String} a list of messages/errors for display to the user.
         */
        updateErrors : function( errors ){
            if ( this.m_statusBar !== null ){
                this.m_statusBar.showMessages( errors );
            }
        },
        m_statusBar : null,
        m_sharedVar : null,
        m_connector : null
    }

});
