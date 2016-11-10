/**
 * Widget for controlling properties of a QPen.
 */

/* global qx, mImport */
/* jshint expr: true */

/**
 @ignore( mImport)
 */

qx.Class.define( "skel.hacks.BoundQPen", {
    extend: qx.ui.container.Composite,

    construct: function( pconfig )
    {
        // figure out options
        var defaultConfig = {
            sharedVar     : null,
            title         : "n/a",
            enablePenWidth: true
        };
        this.m_config = qx.lang.Object.mergeWith( defaultConfig, pconfig, true );
        // call old constructor
        this.base( arguments, new qx.ui.layout.HBox( 5 ).set( {alignY: "middle"} ) );

        this.setBackgroundColor( "#aaaaaa" );
        this.setPadding( 2 );

        // add a preview
        this.m_previewWidget = new qx.ui.core.Widget();
        this.m_previewWidget.set( {
            allowStretchX: false,
            allowStretchY: false,
            height       : 20,
            width        : 20
        } );
        this.add( this.m_previewWidget );

        // button
        this.m_button = new qx.ui.form.Button( this.m_config.title );
        this.add( this.m_button );

        // listen and react to state changes
        this.m_config.sharedVar.addCB( this._sharedVarCB.bind( this ) );
        this._sharedVarCB( this.m_config.sharedVar.get() );

        // listen to UI events
        this.m_button.addListener( "execute", this._buttonCB.bind( this ) );

    },

    members: {

        _sharedVarCB: function( val )
        {
            console.log( "BounQPen::_sharedVarCB", val );

            if( this.m_ignoreSharedVarCB ) {
                return;
            }

            this.m_parsedColor = null;
            this.m_ignoreChangeValueCB = true;

            try {
                var vals = val.split( " " );
                if( vals.length < 3 ) {
                    console.warn( "bad qpen, need 3 parts" );
                }
                else {
                    this.m_parsedColor = vals[0];
                    this.m_parsedAlpha = Number( vals[1] );
                    this.m_parsedWidth = Number( vals[2] );
                }
            }
            catch(e) {
                console.log( "qpen expected in state", this.m_config.sharedVar.path(),
                    "value=", val);
            }

            this._updateUIAfterParse();

            this.m_ignoreChangeValueCB = false;
        },

        _uiCB: function()
        {
            if( this.m_ignoreChangeValueCB ) {
                return;
            }

            var color = this.m_colorPicker.getValue();
            var alpha = this.m_alphaSlider.getValue();
            alpha = alpha / this.m_alphaSlider.getMaximum()
            var width = this.m_widthSlider.getValue();
            width = 20 * width / this.m_alphaSlider.getMaximum();
            var newVal = "" + color + " " + alpha + " " + width;
            this.m_previewWidget.setBackgroundColor( color);

            this.m_ignoreSharedVarCB = true;

            this.m_config.sharedVar.set( newVal );

            this.m_ignoreSharedVarCB = false;
        },

        _buttonCB: function( ev )
        {
            this._createPopup();
            //this.m_popup.placeToWidget( this.m_button );
            this.m_popup.show();
        },

        _createPopup: function()
        {
            if( this.m_popup != null ) {
                return;
            }
            //this.m_popup = new qx.ui.popup.Popup( new qx.ui.layout.VBox() );
            this.m_popup = new qx.ui.window.Window( "Pen options" );
            this.m_popup.setLayout( new qx.ui.layout.VBox());
            this.m_popup.setAlwaysOnTop( true);
            this.m_popup.exclude();

            this.m_colorPicker = new qx.ui.control.ColorSelector();
            this.m_colorPicker.addListener( "changeValue", this._uiCB.bind( this ) );
            this.m_popup.add( this.m_colorPicker );

            var box;

            box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
            box.add( new qx.ui.basic.Label( "Alpha:"));
            this.m_alphaSlider = new qx.ui.form.Slider();
            this.m_alphaSlider.addListener( "changeValue", this._uiCB.bind( this ) );
            this.m_alphaSlider.set( {maximum: 10000, pageStep: 500} );
            box.add( this.m_alphaSlider, { flex: 1} );
            this.m_popup.add( box );

            box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
            box.add( new qx.ui.basic.Label( "Width:"));
            this.m_widthSlider = new qx.ui.form.Slider();
            this.m_widthSlider.set( {maximum: 10000, pageStep: 500} );
            this.m_widthSlider.addListener( "changeValue", this._uiCB.bind( this ) );
            box.add( this.m_widthSlider, { flex: 1} );
            this.m_popup.add( box );

            this._updateUIAfterParse();
        },

        _updateUIAfterParse: function()
        {
            if( this.m_parsedColor == null) {
                this.m_button.setEnabled( false);
            } else {
                this.m_button.setEnabled( true );
            }

            if( this.m_popup != null ) {
                this.m_colorPicker.setValue( this.m_parsedColor );
                this.m_alphaSlider.setValue( this.m_parsedAlpha * this.m_alphaSlider.getMaximum() );
                this.m_widthSlider.setValue( this.m_parsedWidth / 20 * this.m_widthSlider.getMaximum() );
            }
            this.m_previewWidget.setBackgroundColor( this.m_parsedColor );
        },

        m_ignoreChangeValueCB: false,
        m_ignoreSharedVarCB  : false,
        m_popup              : null,
        m_colorPicker        : null,
        m_alphaSlider        : null,
        m_widthSlider        : null,
        m_parsedColor        : null,
        m_parsedWidth        : 1.0,
        m_parsedAlpha        : 1.0

    }

} );