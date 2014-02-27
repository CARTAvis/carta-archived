/**
 * This widget is a rectangle selector overlay over top of the main view area. It allows
 * the user to interactively select rectangles. It is tied to the system via the hub.
 *
 */

/* global qx,fv, fv.console */

/**

 @ignore(fv.console.*)
 @ignore(fv.lib.closure)
 @ignore(fv.assert)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(qx)
 @ignore(fv.makeGlobalVariable)



 ************************************************************************ */

qx.Class.define("qapp.widgets.MainSideBar",
    {
        extend: qx.ui.container.Composite,

        construct: function (hub) {
            this.base(arguments);
            this.setLayout(new qx.ui.layout.VBox(3));
            this.setPadding(3);
            this.setWidth(90);
            this.setBackgroundColor( "rgba(0,0,0,0.1)");
            this.m_gVars = {};
//            this.setAppearance( "MainSideBar");

            this._createChildren();
            this.m_hub = hub;

            this.m_gVars.loadedImageType = fv.makeGlobalVariable(
                "/loadedImageType", this._loadedImageTypeCB.bind(this));
        },

        members: {

            _loadedImageTypeCB: function( val) {
                if( val === "xy1" || val === "xy") {
                    this._profileButtons.ix.setLabel( "X");
                    this._profileButtons.iy.setLabel( "Y");
                    this._profileButtons.iz.exclude();
                    this._profileButtons.qz.exclude();
                    this._profileButtons.uz.exclude();
                    this._profileButtons.pi.exclude();
                    this._profileButtons.pa.exclude();
                    this._profileButtons.qu.exclude();
                    this._fractionalToggle.exclude();
                    this._layoutButtons.polarization.exclude();
                    this._layoutButtons.all.exclude();
                }
                else if( val === "xyz") {
                    this._profileButtons.ix.setLabel( "X");
                    this._profileButtons.iy.setLabel( "Y");
                    this._profileButtons.iz.setLabel( "Z");
                    this._profileButtons.iz.show();
                    this._profileButtons.qz.exclude();
                    this._profileButtons.uz.exclude();
                    this._profileButtons.pi.exclude();
                    this._profileButtons.pa.exclude();
                    this._profileButtons.qu.exclude();
                    this._fractionalToggle.exclude();
                    this._layoutButtons.polarization.exclude();
                    this._layoutButtons.all.exclude();
                }
                else if( val === "iqu-fake") {
                    this._profileButtons.ix.setLabel( "Ix");
                    this._profileButtons.iy.setLabel( "Iy");
                    this._profileButtons.iz.setLabel( "Iz");
                    this._profileButtons.iz.show();
                    this._profileButtons.qz.show();
                    this._profileButtons.uz.show();
                    this._profileButtons.pi.show();
                    this._profileButtons.pa.show();
                    this._profileButtons.qu.show();
                    this._fractionalToggle.show();
                    this._layoutButtons.polarization.show();
                    this._layoutButtons.all.show();
                }
            },

            _buttonPushed: function (id) {
                if( id === "ix-profile") {
                    this._emit( "toui.windowManager.toggle", "profile-ix");
                }
                else if( id === "iy-profile") {
                    this._emit( "toui.windowManager.toggle", "profile-iy");
                }
                else if( id === "iz-profile") {
                    this._emit( "toui.windowManager.toggle", "profile-iz");
                }
                else if( id === "qz-profile") {
                    this._emit( "toui.windowManager.toggle", "profile-qz");
                }
                else if( id === "uz-profile") {
                    this._emit( "toui.windowManager.toggle", "profile-uz");
                }
                else if( id === "pi-profile") {
                    this._emit( "toui.windowManager.toggle", "profile-pi");
                }
                else if( id === "pa-profile") {
                    this._emit( "toui.windowManager.toggle", "profile-pa");
                }
                else if( id === "qu-profile") {
                    this._emit( "toui.windowManager.toggle", "profile-qu");
                }
                else if( id === "basic-layout") {
                    this._emit( "toui.windowManager.applyLayout", "basic");
                }
                else if( id === "profiles-layout") {
                    this._emit( "toui.windowManager.applyLayout", "profiles");
                }
                else if( id === "polarization-layout") {
                    this._emit( "toui.windowManager.applyLayout", "polarization");
                }
                else if( id === "all-layout") {
                    this._emit( "toui.windowManager.applyLayout", "all");
                }
                else if( id === "share") {
                    this._emit( "ui.mainSideBar.shareRequest");
                }
                else {
                    fv.console.error( "handler for " + id + " not implemented yet.");
                }
            },

            _createChildren: function () {

                var bar = this;

                function butt(label, id, container, containerOptions) {
                    var button = new qx.ui.form.Button(label).set({
                        appearance: "MainSideBar-button",
                        focusable: false
                    });
                    button.addListener("execute", function () {
//                        fv.console.log("button + ", id, bar);
                        bar._buttonPushed(id);
                    }, this);
                    container.add(button, containerOptions);
                    return button;
                }

                this.add(new qx.ui.basic.Label("Profiles").set({
                    appearance: "MainSideBar-label"
                }));
                var profileContainer = new qx.ui.container.Composite(new qx.ui.layout.Flow(3, 3, "center"));
                this.add(profileContainer);
                this._profileButtons = {};
                this._profileButtons.ix = butt("Ix", "ix-profile", profileContainer, {});
                this._profileButtons.iy = butt("Iy", "iy-profile", profileContainer, {});
                this._profileButtons.iz = butt("Iz", "iz-profile", profileContainer, {});
                this._profileButtons.qz = butt("Qz", "qz-profile", profileContainer, {});
                this._profileButtons.uz = butt("Uz", "uz-profile", profileContainer, {});
                this._profileButtons.pi = butt("PI", "pi-profile", profileContainer, {});
                this._profileButtons.pa = butt("PA", "pa-profile", profileContainer, {});
                this._profileButtons.qu = butt("QU", "qu-profile", profileContainer, {});
//                this._profileButtons.qu.setEnabled( false);
                this._fractionalToggle = new qapp.boundWidgets.CheckBox("Fractional Pol.", "/fractionalPolarization").set({
                    toolTipText: "Toggle fractional polarization mode."
                });
                this.add( this._fractionalToggle);
//                this._fractionalToggle.addListener( "changeValue", function(e){
//                    this._emit( "ui.mainSideBar.fractionalPolarization", { val: e.getData()});
//                }, this);

                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});


                this.add(new qx.ui.basic.Label("Layouts").set({
//                    textColor: "white",
                    allowGrowX: true,
                    textAlign: "center",
                    appearance: "MainSideBar-label"
                }));
                this._layoutButtons = {};
                this._layoutButtons.basic = butt("Basic", "basic-layout", this, {});
                this._layoutButtons.profiles = butt("Profiles", "profiles-layout", this, {});
                this._layoutButtons.polarization = butt("Polarization", "polarization-layout", this, {});
                this._layoutButtons.all = butt("All", "all-layout", this, {});
//                this._layoutButtons.all.addListener( "execute", function(){
//                    fv.console.log( "selector = ", this._layoutButtons.all.__appearanceSelector);
//                }, this);

                this._fullScreenToggle = new qx.ui.form.CheckBox("Full screen");
                this._fullScreenToggle.addListener("changeValue", function () {
                    var el, rfs;
                    if (this._fullScreenToggle.getValue()) {
                        el = qx.core.Init.getApplication().getRoot().getContentElement().getDomElement();
                        rfs = el.requestFullscreen || el.requestFullScreen
                            || el.webkitRequestFullScreen
                            || el.mozRequestFullScreen;
                    } else {
                        el = document;
                        rfs = el.exitFullscreen || el.exitFullScreen
                            || el.webkitCancelFullScreen || el.webkitExitFullScreen
                            || el.mozCancelFullScreen || el.mozExitFullScreen;
                    }
                    rfs.call(el);
                }, this);
                this.add(this._fullScreenToggle);

                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});

                // share button
                var shareButton = new qx.ui.form.Button( "Share").set({
                    appearance: "MainSideBar-button",
                    focusable: false
                });
                shareButton.addListener("execute", function () {
                    bar._buttonPushed("share");
                }, this);
                this.add( shareButton);

                this.add(new qx.ui.core.Widget().set({
                    backgroundColor: "rgba(0,0,0,0.2)", height: 1
                }), {});

                this._themeToggle = new qx.ui.form.CheckBox("Bigger font");
                this._themeToggle.setValue( window.screen.height < 801 || window.screen.width < 801 || qx.core.Environment.get("device.type") === "mobile");
                this._themeToggle.addListener("changeValue", function () {
                    if (this._themeToggle.getValue()) {
                        qx.theme.manager.Meta.getInstance().setTheme( qapp.theme2m.Theme);
                    } else {
                        qx.theme.manager.Meta.getInstance().setTheme( qapp.theme.Theme);
                    }
                }, this);
                this.add(this._themeToggle);


            },
            m_hub: null,

            _emit: function (path, data) {
                this.m_hub.emit(path, data);
            }

        },

        properties: {

            appearance: {
                refine: true,
                init: "MainSideBar"
            }


        }

    });

