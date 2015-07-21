/**
 * Either shows/hides ALL children user settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Settings.SettingsAll", {
    extend : skel.Command.Settings.Setting,

    /**
     * Constructor.
     */
    construct : function(title, cmds) {
        this.base( arguments, title, null );
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds = [];
        for ( var i = 0; i < cmds.length; i++ ){
            this.m_cmds.push( cmds[i]);
        }
        this.setValue( false );
    },
    
    members : {
        
        /**
         * Update from the UI; sends information to the server.
         */
        doAction : function( vals, undoCB ){
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                var cmdValue = this.m_cmds[i].getValue();
                if ( vals != cmdValue ){
                    this.m_cmds[i].setValue( vals );
                    this.m_cmds[i].doAction( vals, undoCB );
                }
            }
        },
        
        /**
         * Update from the the server; shows or hides all child settings.
         * @param obj {Object} the server side state.
         */
        setSettings : function( obj ){
            var allChecked = true;
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                if ( !this.m_cmds[i].getValue() ){
                    allChecked = false;
                    break;
                }
            }
            this.setValue( allChecked );
        },
        
        m_cmds : null
        
    }
});