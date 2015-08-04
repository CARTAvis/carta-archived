/**
 * Container for region drawing commands.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Region.CommandRegion", {
    extend : skel.Command.CommandGroup,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments, "Region" );
        this.m_global = false;
        this.setEnabled(false);
        this.m_cmds = [];
        this.setValue( this.m_cmds );
    }
});
