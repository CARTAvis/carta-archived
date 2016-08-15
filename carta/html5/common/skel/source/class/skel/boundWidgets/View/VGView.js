/**
 * Created by pfederl.
 *
 * Widget capable of displaying a raster image, overlayed with a vector graphics.
 *
 * The functionality is quite trivial, but it's possible to build more complex functionality
 * on top of this. For example, multiple raster images could be combined into a single one, thus
 * simulating layered views.
 *
 * The main purpose of having this this class is to provide an opportunity to implement client
 * side rendering of the vector graphics, which at the moment is not implemented.
 *
 * Note: we may need different implementation for desktop/server environments to get
 * the most performance, e.g. for rendering VG.
 *
 * The widget can send arbitrary input events to the server, via installable input handlers.
 *
 * The widget has a built in settings widget, invokable via ctrl-shift-S.
 *
 * The widget has also a layer settings widget, invokable via ctrl-shift-L, which brings
 * up a control widget for a managed layer hack. This will most likely be removed in the future.
 *
 * @ignore(mImport)
 */

/*global mImport,qx */


qx.Class.define( "skel.boundWidgets.View.VGView", {

    extend: qx.ui.container.Composite,

    events: {
        "viewRefreshed": "qx.event.type.Data"
    },

    /**
     * Constructor
     */
    construct: function( viewName )
    {
        this.base( arguments );
        this.m_connector = mImport( "connector" );
        this.setLayout( new qx.ui.layout.Canvas() );
        // create a basic view widget as the bottom layer
        this.m_viewWidget = new skel.boundWidgets.View.View( viewName );
        this.add( this.m_viewWidget, {edge: 0} );
        // create an input overlay widget (transparent, used only to capture
        // input events)
        this.m_overlayWidget = new qx.ui.core.Widget();
        this.add( this.m_overlayWidget, {edge: 0} );

        // create a settings UI layer
        this.m_settingsLayer = this._createSettingsUI();
        this.add( this.m_settingsLayer, {top: 10, right: 10} );

        this.m_layerManager = new skel.hacks.LayeredViewManager( viewName );
        this.m_layerManager.setVisibility( "excluded" );
        this.m_layerManager.set( {
            visibility     : "excluded",
            backgroundColor: "white",
            padding        : 5
        } );
        this.add( this.m_layerManager, {bottom: 10, right: 10} );

        this.m_overlayWidget.addListener( "keydown", function( e )
        {
            console.log( "keydown", e );
            console.log( "keyid", e.getKeyIdentifier(), e.getModifiers() );
            if( e.isCtrlPressed() && e.isShiftPressed() && e.getKeyIdentifier() === 'S' ) {
                if( this.m_settingsLayer.getVisibility() == "visible" ) {
                    this.m_settingsLayer.setVisibility( "excluded" );
                }
                else {
                    this.m_settingsLayer.setVisibility( "visible" );
                }
            }
            if( e.isCtrlPressed() && e.isShiftPressed() && e.getKeyIdentifier() === 'L' ) {
                if( this.m_layerManager.getVisibility() == "visible" ) {
                    this.m_layerManager.setVisibility( "excluded" );
                }
                else {
                    this.m_layerManager.setVisibility( "visible" );
                }
            }
        }.bind( this ) );
        this.m_overlayWidget.addListener( "keypress", function()
        {
            console.log( "keypress", arguments );
        } );

        // set initial quality
        this.m_qualityValue = this.m_connector.supportsRasterViewQuality() ? 90 : 101;
        this.setQuality( this.m_qualityValue );

        // listen for the raster view refreshes
        this.m_viewWidget.addListener( "viewRefreshed", this._rasterViewRefreshCB.bind( this ) );
    },

    members: {

        m_overlayWidget: null,
        m_viewWidget   : null,
        m_settingsLayer: null,
        m_qualitySlider: null,
        m_fpsLabel     : null,
        m_qualityLabel : null,
        m_qualityValue : 90,
        m_fpsAvg       : 0,
        m_lastRefresh  : null,
        m_connector    : null,

        /**
         * Get the overlay widget
         */
        overlayWidget: function()
        {
            return this.m_overlayWidget;
        },

        /**
         * Get the view widget
         */
        viewWidget: function()
        {
            return this.m_viewWidget;
        },

        setQuality: function( quality )
        {
            this.m_viewWidget.setQuality( quality );
            var qtext = "" + quality;
            if( quality === 0 ) {
                qtext += " (lowest JPG)";
            }
            if( quality === 100 ) {
                qtext += " (highest JPG)";
            }
            if( quality === 101 ) {
                qtext += " (PNG)";
            }
            if( quality === 102 ) {
                qtext += " (MPEG*)";
            }
            this.m_qualityLabel.setValue( "Quality: " + qtext );
        },

        /**
         * Install an input handler.
         * @param handlerType {class}
         *
         * For example: installHandler( skel.hacks.inputHandlers.Tap)
         */
        installHandler: function( handlerType )
        {
            if( ! this.m_inputHandlers ) {
                this.m_inputHandlers = {};
            }
            if( this.m_inputHandlers[handlerType] !== undefined ) {
                console.warn( "Double install of handler" );
                return;
            }
            var handler = new handlerType( this );
            this.m_inputHandlers[handlerType] = handler;
        },

        /**
         * Install an input handler.
         * @param handlerType {class}
         */
        uninstallHandler: function( handlerType )
        {
            if( ! this.m_inputHandlers ) {
                this.m_inputHandlers = {};
            }
            if( this.m_inputHandlers[handlerType] === undefined ) {
                console.warn( "Cannot uninstall handler" );
                return;
            }
            this.m_inputHandlers[handlerType].deactivate();
            delete this.m_inputHandlers[handlerType];
        },

        /**
         * Send an input event to the server side.
         * @param e {object}
         */
        sendInputEvent: function( e )
        {
            console.log( "Sending input event", e );
            var params = JSON.stringify( e );
            this.m_connector.sendCommand( "vgview/inputEvent/" + this.m_viewWidget.viewName(), params );
        },

        _rasterViewRefreshCB: function()
        {

            // update FPS
            var currRefresh = window.performance.now();
            if( this.m_lastRefresh === null ) {
                this.m_lastRefresh = currRefresh - 1000;
            }
            var fps = 1000 / (currRefresh - this.m_lastRefresh);
            this.m_lastRefresh = currRefresh;
            this.m_fpsAvg = 0.9 * this.m_fpsAvg + 0.1 * fps;

            this.m_fpsLabel.setValue( "FPS:" + this.m_fpsAvg.toPrecision( 3 ) + "(" + fps.toPrecision( 3 ) + ")" );

            // also refire the event
            this.fireDataEvent( "viewRefreshed" );
        },

        _createSettingsUI: function()
        {
            var settings = new qx.ui.container.Composite( new qx.ui.layout.VBox() );
            settings.set( {
                opacity        : 0.9,
                backgroundColor: "rgba(255,255,255,0.7)",
                padding        : 5,
                visibility     : "excluded",
                //keepActive: true,
                //keepFocus: true,
                minWidth       : 200
            } );
            var fpsLabel = new qx.ui.basic.Label( "FPS:" );
            settings.add( fpsLabel );
            var qualityLabel = new qx.ui.basic.Label( "Quality:" );
            settings.add( qualityLabel );
            var qualitySlider = new qx.ui.form.Slider();
            qualitySlider.set( {
                maximum: 102,
                value  : this.m_qualityValue
            } );
            settings.add( qualitySlider );
            var closeButton = new qx.ui.form.Button( "Close" );
            closeButton.set( {alignX: "right", allowStretchX: false} );
            settings.add( closeButton );
            closeButton.addListener( "execute", function()
            {
                console.log( "closeButton clicked" );
                settings.exclude();
            } );

            var dec = new qx.ui.decoration.Decorator();
            dec.set( {
                color     : "rgba(0,0,0,0.5)",
                startColor: "rgba(255,255,255,0.5)",
                endColor  : "rgba(225,225,255,1)",
                radius    : [10, 0, 10, 10],
                width     : 2
            } );
            settings.setDecorator( dec );

            this.m_qualityLabel = qualityLabel;
            this.m_fpsLabel = fpsLabel;
            this.m_qualitySlider = qualitySlider;

            // hook up quality slider callback
            this.m_qualitySlider.addListener( "changeValue", this._qualitySliderCB.bind( this ) );

            // disable quality slider if view does not support it
            if( ! this.m_connector.supportsRasterViewQuality() ) {
                this.m_qualitySlider.setEnabled( false );
            }

            return settings;
        },

        _qualitySliderCB: function( e )
        {
            this.setQuality( e.getData() );
        }
    }

});

//
//qx.Class.define( "skel.boundWidgets.View.VGView", {
//
//    extend: qx.ui.container.Composite,
//
//    /**
//     * Constructor
//     */
//    construct: function( viewName) {
//        this.base( arguments);
//        this.setLayout( new qx.ui.layout.Grow());
//        this.m_viewWidget = new skel.boundWidgets.View.View( viewName );
//        this.add( this.m_viewWidget);
//        this.m_overlayWidget = new qx.ui.core.Widget();
//        this.add( this.m_overlayWidget);
//        // this.m_overlayWidget.setBackgroundColor( "rgba(255,0,0,0.2)");
//    },
//
//    members: {
//
//        m_overlayWidget: null,
//        m_viewWidget: null,
//
//        /**
//         * Get the overlay widget
//         */
//        overlayWidget: function()
//        {
//            return this.m_overlayWidget;
//        },
//
//        /**
//         * Get the view widget
//         */
//        viewWidget: function()
//        {
//            return this.m_viewWidget;
//        }
//    }
//
//});
