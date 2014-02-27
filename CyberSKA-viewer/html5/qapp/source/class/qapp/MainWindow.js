/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 08/04/13
 * Time: 1:46 PM
 * To change this template use File | Settings | File Templates.
 */

/* global app,fv */

/**

 @ignore(fv.console.log)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(fv.makeGlobalVariable)

 @asset(qapp/icons/swheel12.png)
 @asset(qapp/icons/colormap16.png)
 @asset(qapp/icons/histogram16.png)
 @asset(qapp/icons/movie16.png)

 ************************************************************************ */

qx.Class.define("qapp.MainWindow",
    {
        extend: qapp.BetterWindow,

        construct: function (hub) {
            this.base( arguments, hub);

//            this.m_hub = hub;

            this.setShowMinimize(false);
            this.setShowMaximize(true);
            this.setShowClose(true);
            this.setUseResizeFrame(false);
            this.setMinHeight(135);
            this.setCaption("FITS viewer");
            this.setContentPadding(0, 0, 0, 0);
//            this.setResizeSensitivity( 10);

//            this.setLayout(new qx.ui.layout.Canvas());
            this.setLayout(new qx.ui.layout.VBox(0));
            this.getChildrenContainer().setBackgroundColor("#000000");

            // setup commands
            function cmd(shortcut, type, cb, scope) {
                var c = new qx.ui.core.Command(shortcut);
                c.addListener(type, cb, scope);
                return c;
            }

            this.m_commands = {
                regionStats: cmd("Alt+R", "execute", this._regionStatsCB, this),
                g2dfit: cmd("Alt+G", "execute", this._g2dfitCB, this),
                showHeader: cmd(null, "execute", this._showHeaderCB, this),
                colormap: cmd("Alt+C", "execute", this._colormapCB, this),
                histogram: cmd("Alt+H", "execute", this._histogramCB, this),
                fileShortcuts: cmd("Alt+F", "execute", this._fileShortcutsCB, this),
                moviePlayer: cmd("Alt+M", "execute", this._moviePlayerCB, this),
                threeDsurfacePlot: cmd("Alt+T", "execute", this._threeDsurfacePlotCB, this),
                showAbout: cmd(null, "execute", this._showAboutWindowCB, this)
            };

            this.m_commands.showHeader.setToolTipText("Shows the FITS header window");
            this.m_commands.colormap.setToolTipText("Toggles colormap window");
            this.m_commands.histogram.setToolTipText("Toggles histogram window");
            this.m_commands.threeDsurfacePlot.setToolTipText("Toggles 3d surface plot window");

            // create a menu
            var menuBar = this.createMenuBar();
//            this.add(menuBar, { left: "0%", right: "0%", top: "0%" });
            this.add(menuBar);

            // create the lower container
            var lowerContainer = new qx.ui.container.Composite( new qx.ui.layout.Canvas());
            this.add(lowerContainer, { flex: 1 });

            // create the pureweb widget
            this.m_pwView = new qapp.PureWebView("mainPWview", "MainView", qapp.PureWebView.JPEG);
            this.m_pwView.set({ minHeight: 10, minWidth: 10});
            lowerContainer.add(this.m_pwView,
                { left: "0%", right: "0%", top: "0%", bottom: "0%"});

            this.m_sb = new qapp.widgets.MainStatusBar();
            lowerContainer.add( this.m_sb, { left: "0%", right: "0%", bottom: "0%" });

            // create the overlay canvas
            this.m_overlayCanvas = new qapp.widgets.SelectionCanvas(this.m_hub);
            lowerContainer.add(this.m_overlayCanvas,
                { left: "0%", right: "0%", top: "0%", bottom: "0%"});

            // setup toggles in the title bar
            // ------------------------------
            this.m_advancedSettingsButton = this.addToolButton(null, "qapp/icons/swheel12.png").set({
                show: "icon",
                toolTipText: "Adjust image quality."
            });
            this.m_advancedSettingsButton.addListener("execute", function () {
                if (this.m_moreSettingsGroup.isVisible()) {
                    this.m_moreSettingsGroup.exclude();
                } else {
                    this.m_moreSettingsGroup.show();
                }
            }, this);
            this.m_gridToggle = this.addToolToggle("Grid");
            this.m_gridToggle.addListener("changeValue", function () {
                this._emit("ui.mainWindow.grid.visible", this.m_gridToggle.getValue());
                if (this.m_gridToggle.getValue()) {
                    this.m_pwView.setLayoutProperties({ bottom: this.SB_HEIGHT});
                }
                else {
                    this.m_pwView.setLayoutProperties({ bottom: "0%"});
                }
            }, this);
            this.m_gauss2dToggle = this.addToolToggle("Gauss.fit");
            this.m_gauss2dToggle.setCommand( this.m_commands.g2dfit);
            this.m_regionStatsToggle = this.addToolToggle("Region");
            this.m_regionStatsToggle.setCommand( this.m_commands.regionStats);
            this.m_threeDsurfacePlotToggle = this.addToolToggle("3d");
            this.m_threeDsurfacePlotToggle.setCommand( this.m_commands.threeDsurfacePlot);

            // create popup for setting image quality
            this._createImageQualityPopup();
            lowerContainer.add(this.m_moreSettingsGroup, {
                right: 5,
                top: 5
            });

            // set the initial quality - JPEG 90
            this.setQuality( 3);

            // register hub listeners
            this.registerHubListeners(this.m_hub);
        },

        members: {

            m_pwView: null,
            m_gridToggle: null,
            m_gauss2dToggle: null,
            m_regionStatsToggle: null,
            m_sb: null,
            m_overlayCanvas: null,
            m_commands: null,
            m_hub: null,
            m_colorPopupWindow: null,
            m_colorPopupWidget: null,
            m_colorPopupCallback: null,

            /**
             * Create the image quality popup
             */
            _createImageQualityPopup: function() {
                this.m_moreSettingsGroup = new qx.ui.groupbox.GroupBox("More settings").set({
                    layout: new qx.ui.layout.VBox(0),
                    padding: 3,
                    backgroundColor: "rgba(0,0,0,0.2)",
                    textColor: "black",
                    appearance: "mwMoreSettings",
                    visibility: "excluded"
                });
                this.m_moreSettingsGroup.add( new qx.ui.basic.Label("Image quality:"));
                this.m_imageQualityLabel = new qx.ui.basic.Label( "");
                this.m_moreSettingsGroup.add( this.m_imageQualityLabel);
                this.m_imageQualitySlider = new qx.ui.form.Slider().set({
                    minimum: 0,
                    maximum: 11,
                    pageStep: 1,
                    minWidth: 200
                });
                this.m_imageQualitySlider.addListener( "changeValue", function(e) {
                    this.setQuality( this.m_imageQualitySlider.getValue());
                }, this);
                this.m_moreSettingsGroup.add( this.m_imageQualitySlider);

                // close button
                this.m_moreSettingsGroup.add( new qx.ui.core.Spacer(1, 10), {});
                var hbox = new qx.ui.container.Composite( new qx.ui.layout.HBox(3));
                hbox.add( new qx.ui.core.Spacer, {flex: 1});
                var closeButton = new qx.ui.form.Button( "Close").set({
                    padding: [1,1,1,1]
                });
                hbox.add( closeButton);
                closeButton.addListener( "execute", function() {
                    this.m_moreSettingsGroup.exclude();
                }, this);

                this.m_moreSettingsGroup.add( hbox);
            },

            createMenuBar: function () {
                var toolBar = new qx.ui.toolbar.ToolBar();
                toolBar.setOverflowHandling(true);
                toolBar.setHeight(28);
                toolBar.setShow("both");

                var menuPart = toolBar;

                // grid menu
                {
                    var gridMenuButton = new qx.ui.toolbar.MenuButton(
                        "Coordinates", null, new qx.ui.menu.Menu());
                    menuPart.add(gridMenuButton);

                    this.m_wcsRadioGroup = new qx.ui.form.RadioGroup()
                        .set({ allowEmptySelection: false });
                    this.m_wcsRadioGroup.addListener("changeSelection", function(e){
                        var sel = e.getData();
                            fv.GLOBAL_DEBUG && fv.console.log("sel = %O", sel);
                        if(sel.length !== 1) return;
                        sel = sel[0];
                        this._emit("ui.mainWindow.setWCS", sel.getUserData("wcs"));
                    }, this);
                    var coords = [
                        [ "FK5", "FK5"],
                        [ "FK4", "FK4"],
                        [ "FK4 (no E)", "FK4_NO_E"],
                        [ "J2000", "J2000"],
                        [ "ICRS", "ICRS"],
                        [ "Galactic", "GALACTIC"],
                        [ "Ecliptic", "ECLIPTIC"],
                        [ "Geocentric", "GAPPT"],
                        [ "Azel", "AZEL"],
                        [ "HelioEcliptic", "HELIOECLIPTIC"],
                        [ "SuperGalactic", "SUPERGALACTIC"]
                    ];
                    this.m_wcs2radio = {};
                    coords.forEach( function( e){
                        var radio = new qx.ui.menu.RadioButton( e[0]);
                        this.m_wcsRadioGroup.add( radio);
//                        radio.set({ group: this.m_wcsRadioGroup });
                        radio.setUserData( "wcs", e[1]);
                        gridMenuButton.getMenu().add( radio);
                        this.m_wcs2radio[e[1]] = radio;
                    }, this);
                    gridMenuButton.getMenu().add(new qx.ui.menu.Separator());

                    var gridColor1Button = new qx.ui.menu.Button(
                        "Line color...", null, null);
                    gridColor1Button.addListener("execute", function () {
                        this._colorPopup("Grid line color", "#ffff00", function (val) {
//                            fv.GLOBAL_DEBUG && fv.console.log("gridcolor1cb %O", val);
                            this._emit("ui.mainWindow.grid.lineColor", val);
                        });
                    }, this);
                    gridMenuButton.getMenu().add(gridColor1Button);
                    var gridColor2Button = new qx.ui.menu.Button(
                        "Label color...", null, null);
                    gridColor2Button.addListener("execute", function () {
                        this._colorPopup("Grid label color", "#ffffff", function (val) {
//                            fv.GLOBAL_DEBUG && fv.console.log("gridcolor2cb %O", val);
                            this._emit("ui.mainWindow.grid.textColor", val);
                        });
                    }, this);
                    gridMenuButton.getMenu().add(gridColor2Button);
                }

                // region tools menu
                {
                    var regionMenuButton = new qx.ui.toolbar.MenuButton(
                        "Region tools", null, new qx.ui.menu.Menu());
                    menuPart.add(regionMenuButton);

                    var regionStatsButton = new qx.ui.menu.Button(
                        "Region statistics", null, this.m_commands.regionStats);
                    regionMenuButton.getMenu().add(regionStatsButton);
                    var g2dfitButton = new qx.ui.menu.Button(
                        "Gaussian 2d fit", null, this.m_commands.g2dfit);
                    regionMenuButton.getMenu().add(g2dfitButton);
                }

                {
                    var miscMenuButton = new qx.ui.toolbar.MenuButton(
                        "Misc.", null, new qx.ui.menu.Menu());
                    menuPart.add(miscMenuButton);
                    var fitsHeaderButton = new qx.ui.menu.Button(
                        "Show header...", null, this.m_commands.showHeader);
                    miscMenuButton.getMenu().add(fitsHeaderButton);
                    var fileShortcutsButton = new qx.ui.menu.Button(
                        "File shortcuts...", null, this.m_commands.fileShortcuts);
                    miscMenuButton.getMenu().add(fileShortcutsButton);
                }


                var colormapButton = new qx.ui.toolbar.Button(
                    "Colormap", "qapp/icons/colormap16.png", this.m_commands.colormap);
                menuPart.add(colormapButton);

                var histogramButton = new qx.ui.toolbar.Button(
                    "Histogram", "qapp/icons/histogram16.png", this.m_commands.histogram);
                menuPart.add(histogramButton);

                this.m_moviePlayerButton = new qx.ui.toolbar.Button(
                    "Movie player", "qapp/icons/movie16.png", this.m_commands.moviePlayer);
                menuPart.add(this.m_moviePlayerButton);

                {
                    var zoomPart = new qx.ui.toolbar.Part();
                    toolBar.add( zoomPart);
                    // add preset buttons
                    var presetsContainer = new qx.ui.container.Composite(
                        new qx.ui.layout.HBox(0).set({
                            alignX: "center"}));
                    var radioGroup = new qx.ui.form.RadioGroup();
                    radioGroup.setAllowEmptySelection(true);
                    var presets = [ ["Fit",-1], ["1/4",0.25],["1/2",0.5],["1x",1],
                        ["2x", 2], ["3x", 3], ["4x", 4], ["8x", 8], ["12x", 12], ["16x", 16]];
                    this.m_zoomButtons = [];
                    presets.forEach(function (e, ind) {
                        var button = new qx.ui.toolbar.RadioButton(e[0]);
                        button.addListener("execute", function () {
                            this._emit("ui.mainWindow.zoomPreset", { zoom: e[1] });
                        }, this);
                        radioGroup.add(button);
                        button.setFocusable(false);
                        presetsContainer.add(button);
                        this.m_zoomButtons.push(button);
                    }, this);
                    zoomPart.add(presetsContainer);
                }

                // add a widget which signals that something has been removed
                {
                    var overflow = new qx.ui.toolbar.MenuButton(
                        "More...", null, new qx.ui.menu.Menu());
                    toolBar.add(overflow);
                    toolBar.setOverflowIndicator(overflow);
                    overflow.addListener( "changeVisibility", function(){
                        fv.console.log( "vis = ", overflow.getVisibility());
                    });
                    this.m_moviePlayerButton2 = new qx.ui.menu.Button(
                        "Movie player2", "qapp/icons/movie16.png", this.m_commands.moviePlayer);
                    overflow.getMenu().add( this.m_moviePlayerButton2);
                }

                // help menu
                menuPart.add( new qx.ui.core.Spacer, { flex: 1 });
                {
                    var helpMenuButton = new qx.ui.toolbar.MenuButton(
                        "Help", null, new qx.ui.menu.Menu());
                    menuPart.add(helpMenuButton);
                    var aboutButton = new qx.ui.menu.Button(
                        "About...", null, this.m_commands.showAbout);
                    helpMenuButton.getMenu().add(aboutButton);
                }

                return toolBar;
            },

            _regionStatsCB: function () {
/*
                if (this.m_regionStatsWindow === null) {
                    this.m_regionStatsWindow = new qapp.RegionStatsWindow(this.m_hub);
                }
                this.m_regionStatsWindow.toggle();
*/
                this._emit( "toui.windowManager.toggle", "regionStats");
            },

            _g2dfitCB: function () {
/*
                if (this.m_g2dFitWindow === null) {
                    this.m_g2dFitWindow = new qapp.G2dFitWindow(this.m_hub);
                }
                this.m_g2dFitWindow.toggle();
*/
                this._emit( "toui.windowManager.toggle", "g2dFit");
            },

            _threeDsurfacePlotCB: function() {
                this._emit( "toui.windowManager.toggle", "threeDsurfacePlot");
            },

            _showAboutWindowCB: function() {
                this._emit( "toui.windowManager.toggle", "aboutWindow");
            },

            _showHeaderCB: function () {
                this._emit( "toui.windowManager.toggle", "header");
            },

            _fileShortcutsCB: function () {
                this._emit( "toui.windowManager.toggle", "fileShortcuts");
            },

            _moviePlayerCB: function () {
                this._emit( "toui.windowManager.toggle", "moviePlayer");
            },

            /**
             * Displays a color popup widget next to widget, and sets its value to value
             * If the user changes value, callback will be called with the value.
             * If user cancels action callback will be called with null.
             *
             //             * @param title
             //             * @param value
             //             * @param callback
             */
            _colorPopup: function (title, value, callback) {
                if (this.m_colorPopupWindow == null) {

                    this.m_colorPopupWindow = new qx.ui.window.Window().set({
                        resizable: true,
                        modal: false,
                        alwaysOnTop: true,
                        allowMaximize: false,
                        allowMinimize: false,
                        showMaximize: false,
                        showMinimize: false,
                        showStatusbar: false,
                        contentPadding: 2
                    });
                    var win = this.m_colorPopupWindow;
                    win.setLayout(new qx.ui.layout.VBox(0));

                    this.m_colorPopupWidget = new qx.ui.control.ColorSelector;
                    win.add(this.m_colorPopupWidget);

                    var buttonBar = new qx.ui.container.Composite(new qx.ui.layout.HBox(8, "right"));
                    win.add(buttonBar);

                    this.m_colorPopupWidget.addListener("changeValue", function () {
                        var col = this.m_colorPopupWidget.getValue();
                        var rgb = qx.util.ColorUtil.stringToRgb(col);
                        col = qx.util.ColorUtil.rgbToHexString(rgb);
                        if (this.m_colorPopupCallback)
                            this.m_colorPopupCallback.call(this, col);
                    }, this);
                    this.m_colorPopupWindow.addListener("changeVisibility", function () {
                        if (this.m_colorPopupWindow.getVisibility() == "visible") return;
                        this.m_colorPopupCallback = null;
                    }, this);
                }
                this.m_colorPopupCallback = null;
                this.m_colorPopupWindow.setCaption(title);
                this.m_colorPopupWindow.center();
                this.m_colorPopupWindow.show();
                this.m_colorPopupWidget.setValue(value);
                this.m_colorPopupCallback = callback;
            },


            _colormapCB: function () {
/*
                if (this.m_colormapWindow === null) {
                    this.m_colormapWindow = new qapp.ColormapWindow(this.m_hub);
                    this.m_colormapWindow.setLayoutRequestId( "colormapWindow");
                }
                this.m_colormapWindow.toggle();
*/
                this._emit( "toui.windowManager.toggle", "colormap");
            },


            _histogramCB: function () {
                this._emit( "toui.windowManager.toggle", "histogram");
            },

            /**
             *
             */
            registerHubListeners: function (hub) {

                var that = this;

                // shared variable to listen to WCS changes
                this.m_wcsVar = fv.makeGlobalVariable( "/Status/WCS", function(val) {
                    var selwcs = that.m_wcs2radio[val];
                    that.m_wcsRadioGroup.setEnabled( selwcs != null);
                    if( selwcs != null) selwcs.setValue( true);
                }, false);

                // shared variable to listen to frozen cursor changes
                this.m_frozenCursorVar = fv.makeGlobalVariable( "/Status/frozenCursor", function(val) {
                    if( typeof val !== "string") {
                        val = "0";
                    }
                    val = val.split(" ");
                    if( val[0] === "0" || val.length < 3) {
                        that.m_overlayCanvas.setFrozenCursor( null);
                    }
                    else {
                        that.m_overlayCanvas.setFrozenCursor( parseFloat(val[1]), parseFloat(val[2]));
                    }
                }, false);

                this.m_gVars = {};
                this.m_gVars.loadedImageType = fv.makeGlobalVariable(
                    "/loadedImageType", function(val){
                        if( val === "xy1" || val === "xy") {
                            this.m_moviePlayerButton.setEnabled( false);
                            this.m_moviePlayerButton2.setEnabled( false);
                        }
                        else {

                            this.m_moviePlayerButton.setEnabled( true);
                            this.m_moviePlayerButton2.setEnabled( true);
                        }
                    }.bind(this));
/*
                this.m_hub.subscribe("toui.mainWindow.cursor", function (val) {
                    fv.console.log( "status", val);
                    // set cursor text
                    var txt = val.part1 + " " + val.part2 + "<br/>" + val.value + " " + val.frameString;
                    txt += " " + val.wcs + " " + val.isOutside;
                    txt += " " + ( val.cursorFrozen ? "Cursor is frozen" : "Not frozeN");
                    this.m_sb.setValue(txt);
                    this.m_sb.setOpacity( val.isOutside ? 0.5 : 1.0);

                    // set wcs
                    var selwcs = this.m_wcs2radio[val.wcs];
                    this.m_wcsRadioGroup.setEnabled( selwcs !== undefined);
                    if( selwcs !== undefined) selwcs.setValue( true);

                    if( val.cursorFrozen)
                        this.m_overlayCanvas.setFrozenCursor( val.mouseX, val.mouseY);
                    else
                        this.m_overlayCanvas.setFrozenCursor( null);
                }, this);
                */

                // listen for title changes
                this.m_titleVar = fv.makeGlobalVariable(
                    "/title", this.setTitle.bind(this));

                this.m_hub.subscribe( "ui.windowManager.visibilityChanged", function(e){
                    if(e.id === "g2dFit" ) {
                        this.m_gauss2dToggle.setValue(e.visible !== "hidden");
                    }
                    else if(e.id === "regionStats" ) {
                        this.m_regionStatsToggle.setValue(e.visible !== "hidden");
                    }
                    else if(e.id === "threeDsurfacePlot" ) {
                        this.m_threeDsurfacePlotToggle.setValue(e.visible !== "hidden");
                    }
                }, this);
            },

            setTitle: function( title) {
                if( title != null) {
                    this.setCaption( title);
                }
                else {
                    this.setCaption( "CyberSKA Image Viewer");
                }
            },

            setQuality: function( quality) {
                var txt;
                switch( quality) {
                    case 1: txt = "JPG 100"; break;
                    case 2: txt = "JPG 95"; break;
                    case 3: txt = "JPG 90"; break;
                    case 4: txt = "JPG 80"; break;
                    case 5: txt = "JPG 70"; break;
                    case 6: txt = "JPG 60"; break;
                    case 7: txt = "JPG 50"; break;
                    case 8: txt = "JPG 40"; break;
                    case 9: txt = "JPG 30"; break;
                    case 10: txt = "JPG 20"; break;
                    case 11: txt = "JPG 10 (worst quality)"; break;
                    case 0:
                    default:
                        txt = "PNG (best quality)"; quality = 0; break;
                }
                this.m_imageQualityLabel.setValue( txt);
                this.m_imageQualitySlider.setValue( quality);
                this._emit("ui.mainWindow.setQuality", quality);
            },

            SB_HEIGHT: 62

        },

        properties: {

        }

    });

