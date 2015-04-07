/**
 * Container for region drawing commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Data.CommandRegion", {
    extend : skel.Command.CommandGroup,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments, "Region" );
        this.m_global = false;
        this.m_enabled = false;
        this.m_cmds = [];
        
    }
});