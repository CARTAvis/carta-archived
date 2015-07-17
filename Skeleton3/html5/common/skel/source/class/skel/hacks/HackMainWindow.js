/**
 * Author: Pavol Feder ( federl@gmail.com )
 *
 * Entry point to experimental functionality.
 */

/*global qx, mImport, skel */

/**
 @ignore( mImport)
 */

qx.Class.define("skel.hacks.HackMainWindow", {

    extend: qx.ui.window.Window,

    /**
     * Constructor
     *
     */
    construct: function()
    {
        this.base( arguments, "Experimental...");

        this.m_app = qx.core.Init.getApplication();
        this.m_connector = mImport( "connector" );

        if( this.m_connector.getConnectionStatus() != this.m_connector.CONNECTION_STATUS.CONNECTED ) {
            console.error( "Create me only once connection is active!!!" );
            return;
        }

        //this.setWidth( 300 );
        //this.setHeight( 200 );
        this.setShowMinimize( false );
        this.setLayout( new qx.ui.layout.VBox( 5));
        this.setAlwaysOnTop( true);

        this.add(new skel.boundWidgets.Toggle( "Cursor", "/hacks/cursorVisible"));
        this.add(new skel.boundWidgets.Toggle( "Colormap", "/hacks/cm-windowVisible"));
        this.add(new skel.boundWidgets.Toggle( "Grid...", "/hacks/gridControls/c1/visible"));
        this.add(new skel.boundWidgets.Toggle( "Contours", "/hacks/contourEditor/ce1/visible"));
    },

    members: {
        m_connector: null,
        m_app: null
    }

});
