/*************************************************************************

 Copyright:

 License:

 Authors:

 ************************************************************************ */

/**

 @ignore(fv.hub)
 @ignore(fv.console.log)
 @ignore(fv.GLOBAL_DEBUG)
 @ignore(fvVersion)

 @asset(qapp/icons/splash1.jpg)
 @asset(qapp/icons/splash1smaller.jpg)


 */


/**
 * This is the main application class of your custom application "qapp"
 */
qx.Class.define(
    "qapp.Application",
    {
        extend: qx.application.Standalone,

        members: {
            /**
             * This method contains the initial application code and gets called
             * during startup of the application
             *
             */
            main: function () {

                fv.console.log( "Application.main() running", arguments);
                fv.console.log( "qx.$$resources = ", qx.$$resources);

                // set the hub
                // TODO: this is using a global variable, it would be nice to figure
                //       out how to pass parameters to main() instead
                this.m_hub = fv.hub;
                // Call super class
                this.base(arguments);

/*
                // Enable logging in debug variant
                if (qx.core.Environment.get("qx.debug")) {
                    // support native logging capabilities, e.g. Firebug for Firefox
                    qx.log.appender.Native;
                    // support additional cross-browser console. Press F7 to toggle visibility
                    qx.log.appender.Console;
                }
*/

                // announce that UI has been bootstrapped
                this.m_hub.emit("ui.bootstrapped", this);

                // listen for 'run' message
                this.m_hub.subscribe("toui.run", this.run, this);

                // show the loading popup
                this.render();
                this.showSplash();
                this.render();

                if( qx.core.Environment.get("device.type") === "mobile") {
                    qx.theme.manager.Meta.getInstance().setTheme( qapp.theme2m.Theme);
                }

                fv.console.log("Application::main done.");

            },


            run: function () {

                fv.console.log( "Application.run() running");

                // crate the main container widget with a side bar and a bounds widget for
                // the window manager
                this.m_mainContainer = new qx.ui.container.Composite( new qx.ui.layout.HBox(0));
                this.getRoot().add( this.m_mainContainer, {
                    left: "0%", right: "0%", top: "0%", bottom: "0%"
                });
                this.m_boundsWidget = new qx.ui.core.Widget().set({
                    // backgroundColor: "rgba(255,0,0,0.2)"
                });
                this.m_mainContainer.add( this.m_boundsWidget, { flex: 1});
                this.m_sideBar = new qapp.widgets.MainSideBar(this.m_hub);
//                this.m_mainContainer.add(this.m_sideBar);
                var scroll = new qx.ui.container.Scroll();
                scroll.add( this.m_sideBar);
                this.m_mainContainer.add( scroll);


                this.render();

                // create a window manager
                this.m_windowManager = new qapp.WindowManager(
                    this.m_hub, this.getRoot(), this.m_boundsWidget);

//                this._mainWindow = new qapp.MainWindow(this.m_hub);
//                this._mainWindow.setLayoutRequestId("mainWindow");
//                this.getRoot().add(this._mainWindow);
//                this._mainWindow.open();

                // initialize errors/warnings module
                this.m_errorsAndWarnings = new qapp.ErrorsAndWarnings( this.m_hub);

                this.render();

                this.m_hub.emit( "toui.windowManager.applyLayout", "basic");


                this.m_hub.emit("ui.created");
            },

            showSplash: function () {

                var popup = new qx.ui.popup.Popup(new qx.ui.layout.Canvas()).set({
                    backgroundColor: "#000000",
                    padding: [0, 0],
//                    offset : 3,
//                    offsetBottom : 20,
                    autoHide: false
                });


                var imgPath = "qapp/icons/splash1.jpg";
                if( window.screen.height < 801 || window.screen.width < 801 || qx.core.Environment.get("device.type") === "mobile")
                    var imgPath = "qapp/icons/splash1smaller.jpg";
                var image = new qx.ui.basic.Image( imgPath);
                image.set({
                    allowGrowX: false,
                    allowShrinkX: false,
                    allowGrowY: false,
                    allowShrinkY: false,
                    scale: false
                });
                popup.add(image);
                var versionText = window.fvVersion;
                if( fv.GLOBAL_DEBUG) versionText = "Debug " + versionText;
                var label = new qx.ui.basic.Label(
                    versionText).set({
                        font: "splashText",
                        textColor: "yellow"
                    });
                popup.add(label, { right: 5, bottom: 5});
//                popup.placeToPoint({ left: 0, top: 0 });
                popup.show();
                popup.addListener("appear", function () {
                    var rBounds = this.getRoot().getBounds();
                    var pBounds = popup.getBounds();
                    popup.placeToPoint({
                        left: Math.round(rBounds.width / 2 - pBounds.width / 2),
                        top: Math.round(rBounds.height / 2 - pBounds.height / 2)
                    });

                }, this);

                setTimeout(function () {
                    popup.destroy();
                }, 3000);
            }

        }
    })
;