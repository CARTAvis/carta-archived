/**
 * Base class for showing/hiding user settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Settings.Setting", {
    extend : skel.Command.Command,

    /**
     * Constructor.
     * @param label {String} the name of the plug-in that can be viewed.
     */
    construct : function( label, cmd) {
        this.base( arguments, label, cmd);
        this.setValue( false );
    },
    
    members : {
        /**
         * Update from the UI; sends information to the server.
         */
        doAction : function( vals, undoCB ){
            var path = skel.widgets.Path.getInstance();
            var params = "visible:"+vals;
            this.sendCommand( this.m_id, params, undoCB );
        },
        
        getType : function(){
            return skel.Command.Command.TYPE_BOOL;
        },
        
       
        
        /**
         * Update from the the server; shows or hides the setting.
         * @param obj {Object} the server side state.
         */
        setSettings : function( id, obj ){
            this.m_id = id;
            try {
                this.resetValueFromServer( obj );
            }
            catch( err ){
                console.log( err);
                console.log( obj );
            }
            
        },
        
        /**
         * Update the value based on server information.
         * @param obj {Object} the server-side object containing setting information.
         */
        resetValueFromServer : function( obj ){
        },
        
        m_id : null
    }
});