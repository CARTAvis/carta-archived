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

    extend: qx.core.Object,

    /**
     * Constructor
     *
     */
    construct: function()
    {
        this.base( arguments);

        // this.m_app = app;
        this.m_app = qx.core.Init.getApplication();
        this.m_connector = mImport( "connector" );

        if( this.m_connector.getConnectionStatus() != this.m_connector.CONNECTION_STATUS.CONNECTED ) {
            console.error( "connection not yet active!!!" );
            return;
        }

        this.m_hacksEnabledVar = this.m_connector.getSharedVar( "/hacks/enabled" );
        this.m_hacksEnabledVar.addCB( this._activateHacks.bind( this ));
        this._activateHacks();
    },

    members: {

        _activateHacks: function() {

            // if hacks not enabled, do nothing...
            if( this.m_hacksEnabledVar.get() !== "1") return;

            // ==================================================================================
            // create the main hack window
            // ==================================================================================
            this.m_hackMainWindow = new skel.hacks.HackMainWindow();
            this.m_app.getRoot().add( this.m_hackMainWindow, {left: 20, top: 220} );
            this.m_hackMainWindow.open();

            // ==================================================================================
            // newer hack window
            // ==================================================================================
            var newViewName = "IVC7";
            var win2 = new qx.ui.window.Window( "Hack view new" );
            win2.setWidth( 600 );
            win2.setHeight( 400 );
            win2.setShowMinimize( false );
            win2.setUseResizeFrame( false);
            win2.setContentPadding( 5, 5, 5, 5 );
            win2.setLayout( new qx.ui.layout.VBox(5) );
            var newView = new skel.hacks.HackView( newViewName);
            win2.add( newView, { flex: 1 });
            // add mini movie player
            var mp = {};
            mp.prefix = "/hacks/views/" + newViewName;
            mp.slider = new skel.hacks.BoundSlider({
                sharedVar: this.m_connector.getSharedVar( mp.prefix + "/frameSlider"),
                maximum: 999999,
                pageStep: 50000,
                singleStep: 1
            });
            win2.add( mp.slider);
            mp.container = new qx.ui.container.Composite( new qx.ui.layout.HBox(5 ));
            mp.container.getLayout().setAlignY( "middle");
            mp.playButton = new skel.boundWidgets.Toggle( "Play", mp.prefix + "/playToggle");
            mp.container.add( mp.playButton);
            mp.container.add( new qx.ui.basic.Label( "Delay:"));
            mp.delayTF = new skel.boundWidgets.TextField( mp.prefix + "/delay");
            mp.container.add( mp.delayTF);
            mp.container.add( new skel.boundWidgets.Label( "Frame:", "", mp.prefix + "/frame"));
            mp.container.add( new qx.ui.core.Spacer(), { flex: 1 });
            mp.gridTB = new skel.boundWidgets.Toggle( "Grid", mp.prefix + "/gridToggle");
            mp.container.add( mp.gridTB);

            win2.add( mp.container);
            this.m_app.getRoot().add( win2, {left: 100, top: 100} );
            win2.open();

            // ==================================================================================
            // VGview hack window
            // ==================================================================================
            var vgWin = new qx.ui.window.Window( "Managed layers hack" );
            vgWin.setWidth( 600 );
            vgWin.setHeight( 400 );
            vgWin.setShowMinimize( false );
            vgWin.setUseResizeFrame( false);
            vgWin.setContentPadding( 5, 5, 5, 5 );
            vgWin.setLayout( new qx.ui.layout.VBox(5) );

            //var vgview =  new skel.hacks.VGView( "mlv1");
            var vgview =  new skel.boundWidgets.View.VGView( "mlv1");

            //vgview.installHandler( skel.hacks.inputHandler.Hover);
            //vgview.installHandler( skel.hacks.inputHandler.Tap);
            vgview.installHandler( skel.hacks.inputHandler.Drag);
            vgWin.add( vgview, { flex: 1 });
            this.m_app.getRoot().add( vgWin, {left: 150, top: 120} );
            vgWin.open();

            /*
            var vgWin2 = new qx.ui.window.Window( "VGhack2" );
            vgWin2.setWidth( 600 );
            vgWin2.setHeight( 400 );
            vgWin2.setShowMinimize( false );
            vgWin2.setUseResizeFrame( false);
            vgWin2.setContentPadding( 5, 5, 5, 5 );
            vgWin2.setLayout( new qx.ui.layout.VBox(5) );
            vgWin2.add( new skel.hacks.LayeredViewHack( "vgview2"), { flex: 1 });
            this.m_app.getRoot().add( vgWin2, {left: 170, top: 140} );
            vgWin2.open();
            */

            // create cursor window
            this.m_cursorWindow = new skel.boundWidgets.CursorWindow();

            // create colormap window
            this.m_colormapWindow = new skel.boundWidgets.ColormapWindow();

            // create grid controls window
            this.m_gridControlsWindow = new skel.hacks.GridControlsWindow( "/hacks/gridControls/c1/");

            // create contour editor window
            this.m_contourEditorWindow = new skel.hacks.ContourEditorWindow( "/hacks/contourEditor/ce1/");
        },

        m_connector: null,
        m_app: null,
        m_viewWithInput: null,
        m_hacksEnabledVar: null,
        m_hackMainWindow: null
    }

});
