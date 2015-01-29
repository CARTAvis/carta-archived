/**
 * Author: Pavol Feder ( federl@gmail.com )
 *
 * Entry point to experimental functionality.
 */

/*global qx, mImport, skel */

/**
 @ignore( mImport)
 */

qx.Class.define("skel.hacks.Hacks", {

    extend: qx.ui.window.Window,

    /**
     * Constructor
     *
     */
    construct: function( app)
    {
        this.base( arguments, "Experimental...");

        this.m_app = app;
        this.m_connector = mImport( "connector" );

        this.setWidth( 300 );
        this.setHeight( 200 );
        this.setShowMinimize( false );
        this.setLayout( new qx.ui.layout.VBox( 5));


        this.add(new skel.boundWidgets.Toggle( "Cursor", "/hacks/cursorVisible"));
        this.add(new skel.boundWidgets.Toggle( "Colormap", "/hacks/cm-windowVisible"));

        if( this.m_connector.getConnectionStatus() != this.m_connector.CONNECTION_STATUS.CONNECTED ) {
            console.error( "Create me only once connection is active!!!" );
            return;
        }

        var win = new qx.ui.window.Window( "Hack view" );
        win.setWidth( 300 );
        win.setHeight( 200 );
        win.setShowMinimize( false );
        win.setUseResizeFrame( false);
        win.setContentPadding( 5, 5, 5, 5 );
        win.setLayout( new qx.ui.layout.Grow() );
        win.add( new skel.hacks.HackView( "hackView"));
        this.m_app.getRoot().add( win, {left: 200, top: 220} );
        win.open();

        // newer hack view
        var win2 = new qx.ui.window.Window( "Hack view new" );
        win2.setWidth( 300 );
        win2.setHeight( 200 );
        win2.setShowMinimize( false );
        win2.setUseResizeFrame( false);
        win2.setContentPadding( 5, 5, 5, 5 );
        win2.setLayout( new qx.ui.layout.Grow() );
        win2.add( new skel.hacks.HackView( "IVC7"));
        this.m_app.getRoot().add( win2, {left: 220, top: 420} );
        win2.open();

        /*
                var mmcb = function(ev) {
                    //console.log("mm", ev.getDocumentLeft());

                    var box = this.m_viewWithInput.overlayWidget().getContentLocation( "box" );
                    var pt = {
                        x: ev.getDocumentLeft() - box.left,
                        y: ev.getDocumentTop() - box.top
                    };
                    console.log( "mm", pt.x, pt.y);

                };
                mmcb = mmcb.bind(this);

                this.m_viewWithInput.overlayWidget().addListener( "mousemove", mmcb);
        */

        // hacks for temporary functionality
        this.m_cursorWindow = new skel.boundWidgets.CursorWindow();
        this.m_colormapWindow = new skel.boundWidgets.ColormapWindow();
    },

    members: {
        m_connector: null,
        m_app: null,
        m_viewWithInput: null

    }

});
