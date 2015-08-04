/**
 * Container for rendering images in different formats.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.Command.Render.CommandRender", {
    extend : skel.Command.CommandGroup,
    type : "singleton",

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base( arguments, "Render" );
        this.m_global = false;
        this.setEnabled( false );
        this.m_cmds = [];
        this.setValue( this.m_cmds );
    }
});