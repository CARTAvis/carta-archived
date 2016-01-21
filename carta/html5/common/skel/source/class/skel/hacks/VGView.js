/**
 * Created by pfederl on 04/01/15.
 *
 * Widget capable of displaying raster images overlayed with vector graphics.
 *
 * Note: we may need different implementation for desktop/server environments to get
 * the most performance, e.g. for rendering VG.
 *
 * @ignore( mImport)
 */

qx.Class.define( "skel.hacks.VGView", {

    extend: qx.ui.container.Composite,

    statics:
    {
        INPUT_ALL_BUILTINS: "all",
        INPUT_TAP: "tap",
        INPUT_DRAG: "drag",
        INPUT_HOVER: "hover"
    },

    events:
    {
        "viewRefreshed" : "qx.event.type.Data"
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
            padding: 5
        } );
        this.add( this.m_layerManager, { bottom: 10, right: 10 });

        this.m_overlayWidget.addListener( "keydown", function( e )
        {
            console.log( "keydown", e );
            console.log( "keyid", e.getKeyIdentifier(), e.getModifiers() );
            if( e.isCtrlPressed() && e.isShiftPressed() && e.getKeyIdentifier() === 'S') {
                if( this.m_settingsLayer.getVisibility() == "visible") {
                    this.m_settingsLayer.setVisibility( "excluded");
                } else {
                    this.m_settingsLayer.setVisibility( "visible");
                }
            }
            if( e.isCtrlPressed() && e.isShiftPressed() && e.getKeyIdentifier() === 'L') {
                if( this.m_layerManager.getVisibility() == "visible") {
                    this.m_layerManager.setVisibility( "excluded");
                } else {
                    this.m_layerManager.setVisibility( "visible");
                }
            }
            }.bind(this) );
        this.m_overlayWidget.addListener( "keypress", function()
        {
            console.log( "keypress", arguments );
        } );

        this.m_qualityValue = this.m_connector.supportsRasterViewQuality() ? 90 : 101;

        // once the View widget is up, we can finally
        //this.m_viewWidget.addListener( "appear", function(){
        //    this.setQuality( this.m_qualityValue);
        //}.bind(this));
        this.setQuality( this.m_qualityValue);

        // listen for the raster view refreshes
        this.m_viewWidget.addListener( "viewRefreshed", this._rasterViewRefreshCB.bind(this));
    },

    members: {

        m_overlayWidget: null,
        m_viewWidget   : null,
        m_settingsLayer: null,
        m_qualitySlider: null,
        m_fpsLabel: null,
        m_qualityLabel: null,
        m_qualityValue: 90,
        m_fpsAvg: 0,
        m_lastRefresh: null,
        m_connector: null,

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

        setQuality: function( quality) {
            this.m_viewWidget.setQuality( quality);
            var qtext = "" + quality;
            if( quality === 0) {
                qtext += " (lowest JPG)";
            }
            if( quality === 100) {
                qtext += " (highest JPG)";
            }
            if( quality === 101) {
                qtext += " (PNG)";
            }
            if( quality === 102) {
                qtext += " (MPEG*)";
            }
            this.m_qualityLabel.setValue( "Quality: " + qtext);
        },

        /**
         * Installs built in handler[s].
         * @param which which handlers to install
         *
         * Example for use: installDefaultInputHandler([ INPUT_TAP, INPUT_HOVER ])
         */
        installDefaultInputHandler: function( list)
        {
            if( ! qx.lang.Type.isArray( which ) ) {
                list = [list];
            }
            for( var which in list ) {
                which = list[which];
                var handled = false;
                if( which === this.INPUT_TAP || which === this.INPUT_ALL_BUILTINS) {
                    console.log( "Installing tap handler on view", this.m_viewWidget.viewName())
                    handled = true;
                    this.overlayWidget().addListener( "click", function( e )
                    {
                        var box = this.overlayWidget().getContentLocation();
                        var mouseX = e.getDocumentLeft() - box.left
                        var mouseY = e.getDocumentTop() - box.top
                        this.sendInputEvent( {type: "tap", x: mouseX, y: mouseY} );
                    }.bind( this ) );
                }
                if( ! handled) {
                    console.warn( "Don't understand built in event", which);
                }
            }
        },

        /**
         * Send an input event to the server side.
         * @param e
         */
        sendInputEvent: function(e) {
            console.log( "Sending input event", e);
            var params = JSON.stringify( e);
            this.m_connector.sendCommand( "vgview/inputEvent/" + this.m_viewWidget.viewName(), params);
        },

        _rasterViewRefreshCB: function()
        {

            // update FPS
            var currRefresh = window.performance.now();
            if( this.m_lastRefresh === null) {
                this.m_lastRefresh = currRefresh - 1000;
            }
            var fps = 1000 / (currRefresh - this.m_lastRefresh);
            this.m_lastRefresh = currRefresh;
            this.m_fpsAvg = 0.9 * this.m_fpsAvg + 0.1 * fps;

            this.m_fpsLabel.setValue("FPS:"+ this.m_fpsAvg.toPrecision(3) + "(" + fps.toPrecision(3) + ")");

            // also refire the event
            this.fireDataEvent( "viewRefreshed");
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
                minWidth: 200
            } );
            var fpsLabel = new qx.ui.basic.Label( "FPS:" );
            settings.add( fpsLabel );
            var qualityLabel = new qx.ui.basic.Label( "Quality:" );
            settings.add( qualityLabel );
            var qualitySlider = new qx.ui.form.Slider();
            qualitySlider.set({
                maximum: 102,
                value: this.m_qualityValue
            });
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
            dec.set({
                color: "rgba(0,0,0,0.5)",
                startColor: "rgba(255,255,255,0.5)",
                endColor: "rgba(225,225,255,1)",
                radius: [10, 0, 10, 10],
                width: 2
            });
            settings.setDecorator( dec);

            this.m_qualityLabel = qualityLabel;
            this.m_fpsLabel = fpsLabel;
            this.m_qualitySlider = qualitySlider;

            // hook up quality slider callback
            this.m_qualitySlider.addListener( "changeValue", this._qualitySliderCB.bind(this));

            // disable quality slider if view does not support it
            if( ! this.m_connector.supportsRasterViewQuality()) {
                this.m_qualitySlider.setEnabled( false);
            }

            return settings;
        },

        _qualitySliderCB: function(e) {
            this.setQuality( e.getData());
        }
    }

} );

/*
 var row1 = cqoo( {
 type: "hbox",
 id  : "hb1",
 lopt: {
 flex: 1
 },
 lset: {
 spacing: 5
 },
 set: {
 backgroundColor: "#00ff00"
 },
 add : [
 {
 type : "label",
 value: "FPS...:",
 lopt : {flex: 0},
 set  : {background: "#ff0000"}
 },
 {
 type: "label",
 lopt: {flex: 1}
 }
 ]
 } );
 var allRows = cqoo( {
 type: "vbox",
 id  : "vb1",
 add : [row1]
 } );


function cqoo_hbox( obj) {
    return new qx.ui.container.Composite( new qx.ui.layout.HBox());
}

function cqoo( obj )
{

    if( obj.$_widget !== undefined ) {
        return obj;
    }

    var res = {
        $_widget: null,
        $_lopt  : obj.lopt || {},
        $_id    : null
    };

    // obj must have type
    var type = obj.type;
    if( ! type ) {
        throw "No type";
    }

    // construct widget
    if( type == "hbox" ) {
        res.$_widget = cqoo_hbox( obj );
    }
    else if( type == "vbox" ) {
        res.$_widget = cqoo_vbox( obj );
    }
    else if( type == "label" ) {
        res.$_widget = cqoo_label( obj );
    }
    else {
        throw "Bad type: " + type;
    }

    // if we have layout options, apply them
    if( obj.lset != null && res.$_widget.getLayout && res.$_widget.getLayout()) {
        res.$_widget.getLayout().set( obj.lset);
    }

    // apply set
    if( obj.set != null) {
        res.$_widget.set( obj.set);
    }

    // do we have kids?
    if( obj.add != null ) {
        for( kidObj in obj.add ) {
            var kid = cqoo( kidObj );
            res.$_widget.add( kid.$_widget, kid.$_lopt );
            if( kid.$_id != null) {
                res[kid.$_id] = kid;
            }
        }
    }

    // do we have id?
    var id = obj.id;
    if( id != null ) {
        res[id] = widget;
    }
}
*/
