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
            // create first hack view
            // ==================================================================================
            //var win = new qx.ui.window.Window( "Hack view" );
            //win.setWidth( 300 );
            //win.setHeight( 200 );
            //win.setShowMinimize( false );
            //win.setUseResizeFrame( false);
            //win.setContentPadding( 5, 5, 5, 5 );
            //win.setLayout( new qx.ui.layout.Grow() );
            //win.add( new skel.hacks.HackView( "hackView"));
            //this.m_app.getRoot().add( win, {left: 200, top: 220} );
            //win.open();

            // ==================================================================================
            // newer hack window
            // ==================================================================================
            var newViewName = "IVC7";
            var win2 = new qx.ui.window.Window( "Hack view new" );
            win2.setWidth( 300 );
            win2.setHeight( 200 );
            win2.setShowMinimize( false );
            win2.setUseResizeFrame( false);
            win2.setContentPadding( 5, 5, 5, 5 );
            win2.setLayout( new qx.ui.layout.VBox(5) );
            win2.add( new skel.hacks.HackView( newViewName), { flex: 1 });
            // add mini movie player
            var mp = {};
            mp.prefix = "/hacks/views/" + newViewName;
            mp.slider = new qx.ui.form.Slider();
            mp.slider.set({minimum: 0, maximum: 10000, pageStep: 1000 });
            win2.add( mp.slider);
            mp.container = new qx.ui.container.Composite( new qx.ui.layout.HBox(5 ));
            mp.container.getLayout().setAlignY( "middle");
            mp.playButton = new skel.boundWidgets.Toggle( "Play", mp.prefix + "/playToggle");
            mp.container.add( mp.playButton);
            mp.container.add( new qx.ui.basic.Label( "Delay:"));
            mp.delayTF = new skel.boundWidgets.TextField( mp.prefix + "/delay");
            mp.container.add( mp.delayTF);
            mp.container.add( new skel.boundWidgets.Label( "Frame:", "", mp.prefix + "/frame"));
            //mp.container.add( (new qx.ui.core.Spacer()).set({ allowStretchX: true }));
            mp.container.add( new qx.ui.core.Spacer(), { flex: 1 });
            mp.gridTB = new skel.boundWidgets.Toggle( "Grid", mp.prefix + "/gridToggle");
            mp.container.add( mp.gridTB);
            win2.add( mp.container);
            mp.slider.addListener( "changeValue", function(mp, ev) {
                var v = ev.getData() / mp.slider.getMaximum();
                console.log( "slider->", mp.prefix, ev.getData(), v);
                this.m_connector.sendCommand( mp.prefix + "/setFrame", v);
            }.bind( this, mp));
            // pop up the window
            this.m_app.getRoot().add( win2, {left: 220, top: 420} );
            win2.open();

            // create cursor window
            this.m_cursorWindow = new skel.boundWidgets.CursorWindow();

            // create colormap window
            this.m_colormapWindow = new skel.boundWidgets.ColormapWindow();

            // create grid controls window
            this.m_gridControlsWindow = new skel.hacks.GridControlsWindow( "/hacks/gridControls/c1/");

        },

        m_connector: null,
        m_app: null,
        m_viewWithInput: null,
        m_hacksEnabledVar: null,
        m_hackMainWindow: null
    }

});
