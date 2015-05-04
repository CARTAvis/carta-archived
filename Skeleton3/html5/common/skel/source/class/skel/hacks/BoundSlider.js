/**
 * A widget that allows user to manipulate settings related to WCS grids.
 *
 * @ignore( mImport)
 */

/*global mImport,qx */

qx.Class.define( "skel.hacks.BoundSlider", {
    extend: qx.ui.form.Slider,

    construct: function( pconfig) {
        var defaultConfig = {
            orientation: "horizontal",
            sharedVar: null,
            min: 0,
            max: 1
        };
        var config = qx.lang.Object.mergeWith( defaultConfig, pconfig, true);
        this.base( arguments, config.orientation);

        console.log( config);
        this.m_sharedVar = config.sharedVar;
        console.log( this.m_sharedVar);
        this.m_sharedVar.addCB( this._sharedVarCB.bind(this));
        this._sharedVarCB( this.m_sharedVar.get());

        this.addListener( "changeValue", this._changeValueCB.bind(this));

    },

    members: {

        _sharedVarCB: function( val) {
            if( this.m_ignoreSharedVarCB) return;
            var newValue = Number(val);
            this.m_ignoreChangeValueCB = true;
            this.setValue( newValue);
            this.m_ignoreChangeValueCB = false;
        },

        _changeValueCB: function( ev) {
            if( this.m_ignoreChangeValueCB) return;
            var newVal = Number(ev.getData());
            this.m_ignoreSharedVarCB = true;
            this.m_sharedVar.set( newVal);
            this.m_ignoreSharedVarCB = false;
        },

        m_ignoreChangeValueCB: false,
        m_ignoreSharedVarCB: false

    }

});