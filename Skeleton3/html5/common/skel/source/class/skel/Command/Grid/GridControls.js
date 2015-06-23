/**
 * Container for colormap user preference commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Grid.GridControls", {
    extend : skel.Command.CommandComposite,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function() {
        this.base( arguments, "Grid Controls", null );
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds = [];
        this.m_cmds.push( new skel.Command.Grid.SettingsGrid());
        this.setValue( this.m_cmds );
    },
    
    members : {
        
       
        _resetEnabled : function( ){
            arguments.callee.base.apply(this, arguments);
            for ( var i = 0; i < this.m_cmds.length; i++ ){
                this.m_cmds[i]._resetEnabled( );
            }
        },
        
        
        /**
         * Update from the the server; shows or hides the setting.
         * @param obj {Object} the server side state.
         */
        setSettings : function( obj ){
            this.setCmdSettings( obj, this.m_cmds );
        }
    }
});