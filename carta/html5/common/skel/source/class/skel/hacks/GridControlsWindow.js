/**
 * A window that displays grid controls.
 *
 * @ignore( mImport)
 */

/*global mImport,qx */

qx.Class.define( "skel.hacks.GridControlsWindow", {
    extend: qx.ui.window.Window,

    construct: function( statePrefix) {
        this.base( arguments, "Grid controls");
        this.m_statePrefix = statePrefix;
        if( ! qx.lang.String.endsWith( this.m_statePrefix, "/")) {
            this.m_statePrefix += "/";
        }

        this.addListener( "minimize", this._userHidWindow.bind(this));
        this.addListener( "close", this._userHidWindow.bind(this));

        this.m_connector = mImport( "connector");
        this.m_visibleVar = this.m_connector.getSharedVar(
            this.m_statePrefix + "visible");

        this.setLayout( new qx.ui.layout.Grow());
        this.m_gridControlsWidget = new skel.hacks.GridControlsWidget( this.m_statePrefix);
        this.add( this.m_gridControlsWidget);

        this.setMinWidth( 100);
        this.setMinHeight( 100);
        this.setAlwaysOnTop( true);

        this._visibleCB( this.m_visibleVar.get());
        this.m_visibleVar.addCB( this._visibleCB.bind(this));
    },

    members: {

        _visibleCB: function(val) {
            var visible = val === "1";
            if( visible) {
                this.show();
            }
            else{
                this.hide();
            }
        },

        _userHidWindow: function() {
            this.m_visibleVar.set( "0");
        },

        m_connector: null,
        m_statePrefix: "/",
        m_visibleVar: null,
        m_gridControlsWidget: null
    }

});