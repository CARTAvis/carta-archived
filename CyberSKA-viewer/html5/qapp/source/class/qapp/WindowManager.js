/**
 * Purpose of this class:
 *
 * To manage all windows:
 *   create, open, hide, reposition, apply layouts
 *
 * Listens to messages:
 *
 *   toui.windowManager.show { id: window-id }
 *   toui.windowManager.hide { id: window-id }
 *   toui.windowManager.toggle { id: window-id }
 *   toui.windowManager.applyLayout { name: layout-name }
 */

/**

 @ignore(fv.hub)
 @ignore(fv.console.error)
 @ignore(fv.assert)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(fv.makeGlobalVariable)

 */

qx.Class.define(
    "qapp.WindowManager",
    {
        extend: qx.core.Object,

        /**
         * @param hub {Hub} The hub on which it will communicate
         * @param root {Widget}where to add windows (this is basically a IWindowManager)
         * @param boundsWidget {Widget} invisible widget for applying layouts
         */
        construct: function (hub, root, boundsWidget) {

            this.base( arguments);
            this.m_hub = hub;
            this.m_root = root;
            this.m_boundsWidget = boundsWidget;
            this.m_windows = {};

            this.m_profileController = new qapp.ProfileController( this.m_hub);

            this.m_hub.subscribe( "toui.windowManager.applyLayout", this._doLayout, this);
            this.m_hub.subscribe( "toui.windowManager.toggle", this._toggle, this);

            // re-apply last layout when browser is resized
            this.m_lastLayout = "basic";
            qx.core.Init.getApplication().getRoot().addListener("resize",function(){
                this._doLayout( this.m_lastLayout);
            }, this);

            // listen for image type changes
            this.m_gVars = {};
            this.m_gVars.loadedImageType = fv.makeGlobalVariable(
                "/loadedImageType", this._loadedImageTypeCB.bind(this));
        },

        members: {

            _loadedImageTypeCB: function( val) {
                if( val === "xy1" || val === "xy") {
                    this._hide( "moviePlayer");
                    this._hide( "profile-iz");
                    this._hide( "profile-qz");
                    this._hide( "profile-uz");
                    this._hide( "profile-pi");
                    this._hide( "profile-pa");
                    this._hide( "profile-qu");
                    this._hide( "rs-nan");
                    this._hide( "rs-min");
                    this._hide( "rs-max");
                    this._hide( "rs-avg");
                    this._hide( "rs-sum");
                    this._hide( "rs-std");
                    this._hide( "rs-bkg");
                    this._hide( "rs-sumbkg");
                    this._hide( "rs-maxbkg");
                    if( this.m_lastLayout === "all" || this.m_lastLayout === "polarization") {
                        this._doLayout( "profiles");
                    }
                }
                if( val === "xyz") {
                    this._hide( "profile-qz");
                    this._hide( "profile-uz");
                    this._hide( "profile-pi");
                    this._hide( "profile-pa");
                    this._hide( "profile-qu");
                    if( this.m_lastLayout === "all" || this.m_lastLayout === "polarization") {
                        this._doLayout( "profiles");
                    }
                }
            },

            _createWindow: function(id) {
                var win = null;
                if( id === "main") {
                    win = new qapp.MainWindow( this.m_hub);
                }
                else if( id === "profile-ix") {
                    win = this.m_profileController.getProfileWindow( "ix");
                }
                else if( id === "profile-iy") {
                    win = this.m_profileController.getProfileWindow( "iy");
                }
                else if( id === "profile-iz") {
                    win = this.m_profileController.getProfileWindow( "iz");
                }
                else if( id === "profile-qz") {
                    win = this.m_profileController.getProfileWindow( "qz");
                }
                else if( id === "profile-uz") {
                    win = this.m_profileController.getProfileWindow( "uz");
                }
                else if( id === "profile-pi") {
                    win = this.m_profileController.getProfileWindow( "pi");
                }
                else if( id === "profile-pa") {
                    win = this.m_profileController.getProfileWindow( "pa");
                }
                else if( id === "profile-qu") {
                    win = this.m_profileController.getProfileWindow( "qu");
                }
                else if( id === "header") {
                    win = new qapp.HeaderWindow( this.m_hub);
                }
                else if( id === "fileShortcuts") {
                    win = new qapp.widgets.FileShortcutsWindow( this.m_hub);
                }
                else if( id === "moviePlayer") {
                    win = new qapp.widgets.MoviePlayerWindow( this.m_hub);
                }
                else if( id === "g2dFit") {
                    win = new qapp.G2dFitWindow( this.m_hub);
                }
                else if( id === "regionStats") {
                    win = new qapp.RegionStatsWindow( this.m_hub);
                }
                else if( id === "colormap") {
                    win = new qapp.ColormapWindow( this.m_hub);
                }
                else if( id === "histogram") {
                    win = new qapp.widgets.HistogramWindow( this.m_hub);
                }
                else if( id === "threeDsurfacePlot") {
                    win = new qapp.widgets.ThreeDsurfacePlotWindow( this.m_hub);
                }
                else if( id === "rs-nan") {
                    win = this.m_profileController.getProfileWindow( id);
                }
                else if( id === "rs-min") {
                    win = this.m_profileController.getProfileWindow( id);
                }
                else if( id === "rs-max") {
                    win = this.m_profileController.getProfileWindow( id);
                }
                else if( id === "rs-avg") {
                    win = this.m_profileController.getProfileWindow( id);
                }
                else if( id === "rs-sum") {
                    win = this.m_profileController.getProfileWindow( id);
                }
                else if( id === "rs-std") {
                    win = this.m_profileController.getProfileWindow( id);
                }
                else if( id === "rs-bkg") {
                    win = this.m_profileController.getProfileWindow( id);
                }
                else if( id === "rs-sumbkg") {
                    win = this.m_profileController.getProfileWindow( id);
                }
                else if( id === "rs-maxbkg") {
                    win = this.m_profileController.getProfileWindow( id);
                }
                else if( id === "aboutWindow") {
                    win = new qapp.widgets.AboutWindow( this.m_hub);
                }
                fv.assert( win != null, "Unknown window " + id.toString());
                win.setLayoutRequestId( id);
                function visibilityCB( scope, id, e) {
                    scope._emit( "ui.windowManager.visibilityChanged", {
                        id: id,
                        visible: e.getData()
                    });
                }
                win.addListener( "changeVisibility", visibilityCB.bind(null, this, id));
                this.m_root.add( win);
                return win;
            },

            _createOnce: function( id) {
                if( this.m_windows[id] === undefined) {
                    this.m_windows[id] = this._createWindow(id);
                }
            },

            _isVisible: function(id) {
                if( this.m_windows[id] === undefined) return false;
                return this.m_windows[id].isVisible();
            },

            _toggle: function( id) {
                if( this._isVisible(id))
                    this._hide( id);
                else
                    this._show( id);
            },

            _show: function( id) {
                if(this._isVisible(id)) return;
                this._createOnce( id);
                this.m_windows[id].toggle();
            },

            _hide: function( id) {
                if(! this._isVisible(id)) return;
                this.m_windows[id].toggle();
            },

            _doLayout: function (val) {
                if (val === "basic") {
                    this._doLayoutBasic();
                }
                else if (val === "profiles") {
                    this._doLayoutProfiles();
                }
                else if (val === "polarization") {
                    this._doLayoutPolarization();
                }
                else if (val === "all") {
                    this._doLayoutAll();
                }
                else {
                    fv.console.error( "Layout " + val + " unknown");
                    return;
                }
                this.m_lastLayout = val;
            },

            _doLayoutBasic: function () {

                var bounds = this.m_boundsWidget.getBounds();
                var rw = bounds.width;
                var rh = bounds.height;
                var margin = 5;

                this._show( "main");
                this._hide( "profile-ix");
                this._hide( "profile-iy");
                this._hide( "profile-iz");
                this._hide( "profile-qz");
                this._hide( "profile-uz");
                this._hide( "profile-pi");
                this._hide( "profile-pa");
                this._hide( "profile-qu");

                this.m_hub.emit("toui.windowMove", {
                    id: "main",
                    left: margin,
                    top: margin,
                    width: rw - 2 * margin,
                    height: rh - 2 * margin
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "colormap",
                    left: 2 * margin,
                    top: 2 * margin,
                    width: null,
                    height: null
                });

                this.m_hub.emit("toui.windowMove", {
                    id: "histogram",
                    left: 2 * margin,
                    top: 200,
                    width: null,
                    height: null
                });
            },

            _doLayoutProfiles: function () {

                var bounds = this.m_boundsWidget.getBounds();
                var rw = bounds.width;
                var rh = bounds.height -5;
                var x0 = 0;
                var x1 = rw - 400;
                if( x1 < 100) x1 = 100;
                var x2 = rw;
                var ph = rh / 3;


                this._show( "main");
                this._show( "profile-ix");
                this._show( "profile-iy");
                if( this.m_gVars.loadedImageType.get() === "xy" ||
                    this.m_gVars.loadedImageType.get() === "xy1")
                {
                    this._hide( "profile-iz");
                } else {
                    this._show( "profile-iz");
                }
                this._hide( "profile-qz");
                this._hide( "profile-uz");
                this._hide( "profile-pi");
                this._hide( "profile-pa");
                this._hide( "profile-qu");

                this.m_hub.emit("toui.windowMove", {
                    id: "main",
                    left: 0,
                    top: 0,
                    width: x1 - x0,
                    height: rh
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "colormap",
                    left: 10,
                    top: 10,
                    width: null,
                    height: null
                });

                this.m_hub.emit("toui.windowMove", {
                    id: "histogram",
                    left: 10,
                    top: 200,
                    width: null,
                    height: null
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-ix",
                    left: x1,
                    top: Math.round( 0 * ph),
                    width: x2-x1,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-iy",
                    left: x1,
                    top: Math.round( 1 * ph),
                    width: x2-x1,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-iz",
                    left: x1,
                    top: Math.round( 2 * ph),
                    width: x2-x1,
                    height: Math.round( ph)
                });
            },

            _doLayoutPolarization: function () {

                var bounds = this.m_boundsWidget.getBounds();
                var rw = bounds.width;
                var rh = bounds.height - 5;
                var x0 = 0;
                var x1 = Math.round(rw / 2);
                var x2 = Math.round(rw * 3 / 4);
                var x3 = rw;
                var ph = rh / 3;

                this._show( "main");
                this._hide( "profile-ix");
                this._hide( "profile-iy");
                this._hide( "profile-iz");
                this._show( "profile-qz");
                this._show( "profile-uz");
                this._show( "profile-pi");
                this._show( "profile-pa");
                this._show( "profile-qu");

                this.m_hub.emit("toui.windowMove", {
                    id: "main",
                    left: 0,
                    top: 0,
                    width: x1,
                    height: rh
                });

                this.m_hub.emit("toui.windowMove", {
                    id: "profile-qz",
                    left: x1,
                    top: Math.round( 0 * ph),
                    width: x2 - x1,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-uz",
                    left: x2,
                    top: Math.round( 0 * ph),
                    width: x3 - x2,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-pi",
                    left: x1,
                    top: Math.round( 1 * ph),
                    width: x2 - x1,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-pa",
                    left: x2,
                    top: Math.round( 1 * ph),
                    width: x3 - x2,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-qu",
                    left: x1,
                    top: Math.round( 2 * ph),
                    width: x3 - x1,
                    height: Math.round( ph)
                });
            },

            _doLayoutAll: function () {

                var bounds = this.m_boundsWidget.getBounds();
                var rw = bounds.width;
                var rh = bounds.height - 5;
                var x0 = 0;
                var x1 = Math.round(rw / 2);
                var x2 = Math.round(rw * 3 / 4);
                var x3 = rw;
                var ph = rh / 4;

                this._show( "main");
                this._show( "profile-ix");
                this._show( "profile-iy");
                this._show( "profile-iz");
                this._show( "profile-qz");
                this._show( "profile-uz");
                this._show( "profile-pi");
                this._show( "profile-pa");
                this._show( "profile-qu");

                this.m_hub.emit("toui.windowMove", {
                    id: "main",
                    left: 0,
                    top: 0,
                    width: x1,
                    height: rh
                });

                this.m_hub.emit("toui.windowMove", {
                    id: "profile-ix",
                    left: x1,
                    top: Math.round( 0 * ph),
                    width: x2 - x1,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-iy",
                    left: x1,
                    top: Math.round( 1 * ph),
                    width: x2 - x1,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-qz",
                    left: x1,
                    top: Math.round( 2 * ph),
                    width: x2 - x1,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-pi",
                    left: x1,
                    top: Math.round( 3 * ph),
                    width: x2 - x1,
                    height: Math.round( ph)
                });

                this.m_hub.emit("toui.windowMove", {
                    id: "profile-iz",
                    left: x2,
                    top: Math.round( 0 * ph),
                    width: x3 - x2,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-qu",
                    left: x2,
                    top: Math.round( 1 * ph),
                    width: x3 - x2,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-uz",
                    left: x2,
                    top: Math.round( 2 * ph),
                    width: x3 - x2,
                    height: Math.round( ph)
                });
                this.m_hub.emit("toui.windowMove", {
                    id: "profile-pa",
                    left: x2,
                    top: Math.round( 3 * ph),
                    width: x3 - x2,
                    height: Math.round( ph)
                });
            },

            _emit: function( path, data) {
                this.m_hub.emit( path, data);
            }

        }
    });
