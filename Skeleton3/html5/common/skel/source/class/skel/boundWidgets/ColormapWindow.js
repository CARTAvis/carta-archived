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

    construct: function() {
        this.base( arguments, "Colormap");
        this.setLayout( new qx.ui.layout.Flow(5,5));
        this.setMinWidth( 100);
        this.setMinHeight( 100);
        this.setWidth( 300);
        this.setAlwaysOnTop( true);

        this.addListener( "minimize", this._userHidWindow.bind(this));
        this.addListener( "close", this._userHidWindow.bind(this));

        this.m_connector = mImport( "connector");
        this.m_windowVisible = this.m_connector.getSharedVar( "/hacks/cm-windowVisible");
        this.m_windowVisible.addCB( this._visibleCB.bind(this));

        this.m_currentColormap = this.m_connector.getSharedVar( "/hacks/cm-current");
        this.m_currentColormap.addCB( this._currentColormapCB.bind(this));

        this.m_colormapCount = this.m_connector.getSharedVar( "/hacks/cm-count");
        this.m_colormapCount.addCB( this._countCB.bind(this));

        // manually invoke callbacks in case the values have been already set
        this._visibleCB( this.m_windowVisible.get());
        this._countCB( this.m_colormapCount.get());
        this._currentColormapCB( this.m_currentColormap.get());
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
            this.m_windowVisible.set( "0");
        },

        _countCB: function(val) {
            if(val == null) return;
            var count = parseInt( val);
            this.removeAll();
            this.m_cmaps = [];
            for( var i = 0 ; i < count ; i ++ ) {
                var name = this.m_connector.getSharedVar("/hacks/cm-names-" + i ).get();
                var toggle = new skel.widgets.FancyToggle( name);
                toggle.addListener( "execute", this._toggleCB.bind(this, i, name));
                this.m_cmaps[i] = { toggle: toggle };
                this.add( toggle);
            }
        },

        _currentColormapCB: function(val) {
            if(val == null) return;
            var ival = parseInt(val);
            for( var ind in this.m_cmaps) {
                this.m_cmaps[ind].toggle.setValue( ival == ind);
            }
        },

        _toggleCB : function (ind, name) {
            console.log( "toggle cb", arguments);
            this.m_currentColormap.set(ind);
        },

        m_connector: null,
        m_cursorText: null,
        m_windowVisible: null
    }

});