/**
 * A window that displays the colormap selection window
 *
 * This is a temporary widget just to test some colormap functionality.
 *
 * @ignore( mImport)
 */

/*global mImport,qx */

qx.Class.define( "skel.boundWidgets.ColormapWindow", {
    extend: qx.ui.window.Window,

    construct: function()
    {
        this.base( arguments, "Colormap" );
        this.m_connector = mImport( "connector" );

        this.setLayout( new qx.ui.layout.VBox( 5 ) );
        this.m_ui = {};
        this.m_ui.pane1 = new qx.ui.container.Composite( new qx.ui.layout.Flow( 2, 2 ) );
        this.m_ui.pane2 = new qx.ui.container.Composite(
            new qx.ui.layout.HBox( 5 ).set( {alignY: "middle"} ) );
        this.add( this.m_ui.pane1, {flex: 1} );
        this.add( new qx.ui.core.Widget().set( {backgroundColor: "black", height: 1} ) );
        this.add( this.m_ui.pane2 );
        this.m_ui.pane3 = new qx.ui.container.Composite(
            new qx.ui.layout.Grow() );
        this.add( this.m_ui.pane3 );
        this.m_ui.pane4 = new qx.ui.container.Composite(
            new qx.ui.layout.HBox());
        this.add( this.m_ui.pane4 );

        this.m_ui.cacheColormapToggle = new skel.boundWidgets.Toggle(
            "Cache", "/hacks/pixelCacheOn" );
        this.m_ui.pane2.add( this.m_ui.cacheColormapToggle );
        this.m_ui.interpolateToggle = new skel.boundWidgets.Toggle(
            "Interpolate", "/hacks/pixelCacheInterpolationOn" );
        this.m_ui.pane2.add( this.m_ui.interpolateToggle );
        this.m_ui.cacheSizeTF = new skel.boundWidgets.TextField(
            "/hacks/pixelCacheSize" );
        this.m_ui.pane2.add( this.m_ui.cacheSizeTF, {flex: 1} );

        this.m_ui.canvas = new qx.ui.embed.Canvas().set( {syncDimension: true, height: 10} );
        this.m_ui.pane3.add( this.m_ui.canvas );
        this.m_ui.canvas.addListener( "redraw", this._redrawCanvas.bind( this ) );
        this.setMinWidth( 100 );
        this.setMinHeight( 100 );
        this.setWidth( 300 );
        this.setAlwaysOnTop( true );

        this.addListener( "minimize", this._userHidWindow.bind( this ) );
        this.addListener( "close", this._userHidWindow.bind( this ) );

        this.m_windowVisible = this.m_connector.getSharedVar( "/hacks/cm-windowVisible" );
        this.m_windowVisible.addCB( this._visibleCB.bind( this ) );

        this.m_currentColormap = this.m_connector.getSharedVar( "/hacks/cm-current" );
        this.m_currentColormap.addCB( this._currentColormapCB.bind( this ) );

        this.m_colormapCount = this.m_connector.getSharedVar( "/hacks/cm-count" );
        this.m_colormapCount.addCB( this._countCB.bind( this ) );

        this.m_cmapPreview = this.m_connector.getSharedVar( "/hacks/cm-preview" );
        this.m_cmapPreview.addCB( this._cmapPreviewCB.bind( this ) );

        this.m_ui.pane4.add( new skel.boundWidgets.Toggle( "Reverse", "/hacks/cm-reverse"));
        this.m_ui.pane4.add( new skel.boundWidgets.Toggle( "Invert", "/hacks/cm-invert"));

        // manually invoke callbacks in case the values have been already set
        this._visibleCB( this.m_windowVisible.get() );
        this._countCB( this.m_colormapCount.get() );
        this._currentColormapCB( this.m_currentColormap.get() );
        this._cmapPreviewCB( this.m_cmapPreview.get() );
    },

    members: {

        _cmapPreviewCB: function( val )
        {
            if( typeof val !== "string" ) {
                this.m_cmapPreviewParsed = [];
                return;
            }
            this.m_cmapPreviewParsed = val.split( "," );
            this._redrawCanvas();
        },

        _redrawCanvas: function()
        {
            if( this.m_cmapPreviewParsed.length < 2 ) {
                return;
            }
            var canvas = this.m_ui.canvas;
            var ctx = canvas.getContext2d();
            var width = canvas.getCanvasWidth();
            var height = canvas.getCanvasHeight();
            var grd = ctx.createLinearGradient( 0, 0, width, 0 );
            var n1 = this.m_cmapPreviewParsed.length - 1;
            this.m_cmapPreviewParsed.forEach( function(e,i) {
                grd.addColorStop( i/n1, "#" + e);
            }.bind(this));
            //grd.addColorStop( 0, "black" );
            //grd.addColorStop( 1, "white" );

            ctx.fillStyle = grd;
            ctx.fillRect( 0, 0, width, height );
        },

        _visibleCB: function( val )
        {
            var visible = val === "1";
            if( visible ) {
                this.show();
            }
            else {
                this.hide();
            }
        },

        _userHidWindow: function()
        {
            this.m_windowVisible.set( "0" );
        },

        _countCB: function( val )
        {
            if( val == null ) {
                return;
            }
            var count = parseInt( val );
            this.m_ui.pane1.removeAll();
            this.m_cmaps = [];
            for( var i = 0 ; i < count ; i ++ ) {
                var name = this.m_connector.getSharedVar("/hacks/cm-names-" + i ).get();
                var toggle = new skel.widgets.CustomUI.FancyToggle( name);
                toggle.setPadding( 2);
                toggle.addListener( "execute", this._toggleCB.bind(this, i, name));
                this.m_cmaps[i] = { toggle: toggle };
                this.m_ui.pane1.add( toggle);
            }
        },

        _currentColormapCB: function( val )
        {
            if( val == null ) {
                return;
            }
            var ival = parseInt( val );
            for( var ind in this.m_cmaps ) {
                this.m_cmaps[ind].toggle.setValue( ival == ind );
            }
        },

        _toggleCB: function( ind, name )
        {
            console.log( "toggle cb", arguments );
            this.m_cmaps[ind].toggle.setValue( true );
            this.m_currentColormap.set( ind );
        },

        m_connector    : null,
        m_cursorText   : null,
        m_windowVisible: null
    }

} );