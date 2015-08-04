/**
 * A widget that allows user to manipulate settings related to WCS grids.
 *
 * @ignore( mImport)
 */

/*global mImport,qx */

qx.Class.define( "skel.hacks.BoundSlider", {
    extend: qx.ui.form.Slider,

    construct: function( pconfig) {
        // figure out options
        var defaultConfig = {
            orientation: "horizontal",
            sharedVar: null,
            maximum: 1000,
            pageStep: 100,
            singleStep: 1,
            val2slider: function(x) { return x; },
            slider2val: function(x) { return x; }
        };
        this.m_config = qx.lang.Object.mergeWith( defaultConfig, pconfig, true);
        // call old constructor
        this.base( arguments, this.m_config.orientation);

        // set some other parameters
        this.setMaximum( this.m_config.maximum);
        this.setPageStep( this.m_config.pageStep);
        this.setSingleStep( this.m_config.singleStep);

        // listen and react to state changes
        this.m_config.sharedVar.addCB( this._sharedVarCB.bind(this));
        this._sharedVarCB( this.m_config.sharedVar.get());

        // listen to UI events
        this.addListener( "changeValue", this._changeValueCB.bind(this));

    },

    members: {

        _sharedVarCB: function( val) {
            console.log( "BoundSlider::_sharedVarCB", val);
            if( this.m_ignoreSharedVarCB) return;
            var newValue = this.m_config.val2slider( Number(val));
            this.m_ignoreChangeValueCB = true;
            try {
                this.setValue( newValue);
            } catch ( err) {
                console.warn( err);
            }
            this.m_ignoreChangeValueCB = false;
        },

        _changeValueCB: function( ev) {
            if( this.m_ignoreChangeValueCB) return;
            var newVal = this.m_config.slider2val( Number(ev.getData()));
            this.m_ignoreSharedVarCB = true;
            this.m_config.sharedVar.set( newVal);
            this.m_ignoreSharedVarCB = false;
        },

        m_ignoreChangeValueCB: false,
        m_ignoreSharedVarCB: false

    }

});