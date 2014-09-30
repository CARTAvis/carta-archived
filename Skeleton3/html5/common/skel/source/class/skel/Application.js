/* ************************************************************************

 Copyright:

 License:

 Authors:

 ************************************************************************ */

/*global qx */

/**
 * This is the main application class
 *
 * @asset(skel/*)
 * @ignore( mImport)
 */
qx.Class.define("skel.Application",
    {
        extend : qx.application.Standalone,



        /*
         *****************************************************************************
         MEMBERS
         *****************************************************************************
         */

        members :
        {
            /**
             * This method contains the initial application code and gets called
             * during startup of the application
             *
             * @lint ignoreDeprecated(alert)
             */
            main : function()
            {
                // Call super class
                this.base( arguments );

                // Enable logging in debug variant
                if( qx.core.Environment.get( "qx.debug" ) )
                {
                    // everything inside the curlies gets compiled out in release mode

                    // support native logging capabilities, e.g. Firebug for Firefox
//        qx.log.appender.Native;
                    // support additional cross-browser console. Press F7 to toggle visibility
//        qx.log.appender.Console;

                    console.log( "I will be compiled out" );
                }

                if( false ) {
                    console.log( "I should be compiled out." );
                }

                var xxx = false;
                if( xxx ) {
                    console.log( "I wish I was compiled out, but the compiler is not that good :(" );
                }

                console.log( "I will never be compiled out :(" );

                var connector = mImport( "connector" );

                // delay start of the application until we receive CONNECTED event...
                // only after we receive this event we can safely start modifying state, etc
                // otherwise some state changes/commands might get lost
                connector.setConnectionCB( this._afterConnect.bind( this));
/*
                connector.setConnectionCB( function( s )
                {
                    console.log( "connectionCB status=", connector.getConnectionStatus() );

                } );
*/
                connector.connect();
            },

            _afterConnect: function()
            {
                console.log( "_afterConnect running");

                var connector = mImport( "connector" );
                if( connector.getConnectionStatus() != connector.CONNECTION_STATUS.CONNECTED) {
                    console.log( "Connection not established yet...");
                    return;
                }

                var win = new qx.ui.window.Window("First view");
                win.setWidth(300);
                win.setHeight(200);
                win.setShowMinimize(false);
                win.setLayout( new qx.ui.layout.Grow());
                win.setContentPadding( 5, 5, 5, 5);
                win.add( new skel.boundWidgets.View( "view1"));
                this.getRoot().add(win, {left:20, top:220});
                win.open();

                win = new qx.ui.window.Window("Second view");
                win.setWidth(400);
                win.setHeight(100);
                win.setShowMinimize(false);
                win.setLayout( new qx.ui.layout.Grow());
                win.setContentPadding( 5, 5, 5, 5);
                win.add( new skel.boundWidgets.View( "view2"));
                this.getRoot().add(win, {left:370, top:220});
                win.open();

                win = new qx.ui.window.Window("Image view");
                win.setWidth(400);
                win.setHeight(100);
                win.setShowMinimize(false);
                win.setLayout( new qx.ui.layout.Grow());
                win.setContentPadding( 5, 5, 5, 5);
                win.add( new skel.boundWidgets.View( "view3"));
                this.getRoot().add(win, {left:200, top:120});
                win.open();


                // Document is the application root
                var doc = this.getRoot();

                // Create a button
                var button1 = new qx.ui.form.Button("Share this session", "skel/test.png");
                button1.addListener("execute", function(e) {
                    var con=mImport("connector");
                    con.shareSession( function( url) {
                            tfield.setValue( url);
                            tfield.setVisibility( "visible");
                        }, "a", null,
                            60*60*1000);
                });

                // Add button to document at fixed coordinates
                doc.add(button1, {left: 100, top: 50});
                doc.add(new skel.boundWidgets.Toggle( "Recompute clips on new frame", "/autoClip"),
                    {left: 0, top: 0});

                // add preset buttons
                {
                    this.histClipVar = connector.getSharedVar( "/autoClipValue");
                    var presetsContainer = new qx.ui.container.Composite(
                        new qx.ui.layout.HBox(5).set({
                            alignX: "center"}));
                    var radioGroup = new qx.ui.form.RadioGroup();
                    radioGroup.setAllowEmptySelection(true);
                    var presets = [ "95%", "98%", "99%", "99.5%", "99.9%", "99.99%", "100%" ];
                    this.m_presetButtons = [];
                    presets.forEach(function (e, ind) {
                        var button = new qx.ui.toolbar.RadioButton(e).set({
                            toolTipText: "Set histogram to show " + e + " of the data.<br>" +
                                "Right click also automatically zooms in to the data."
                        });
                        button.addListener("execute", function () {
                            this.histClipVar.set( e);
                            button.setValue(true);
                        }, this);
                        button.addListener("mouseup", function (event) {
                            this.histClipVar.set( e);
                            button.setValue(true);
                        }, this);
                        radioGroup.add(button);
                        button.setFocusable(false);
                        presetsContainer.add(button);
                        this.m_presetButtons.push(button);
                    }, this);
                    doc.add(presetsContainer, {right:0, top: 0});
                }


                // status bar
                var statusBar = new qx.ui.form.TextField("Status bar").set({
                    readOnly: true
                });
                this.getRoot().add( statusBar, { left: 10, right: 10, bottom: 10 });



                var labelx = new skel.boundWidgets.Label( "MouseX:", "pix", "/mouse/x");
                doc.add( labelx, { left: 100, top: 170 });
                var labely = new skel.boundWidgets.Label( "MouseY:", "pix", "/mouse/y");
                doc.add( labely, { left: 100, top: 190 });
                var cursor = new skel.boundWidgets.Label( "Cursor:", "", "/cursor");
                doc.add( cursor, { left: 100, top: 210 });


                // create plugin list window
                var pluginWindow = new qx.ui.window.Window("Plugins");
                pluginWindow.set( {minWidth: 300});
                pluginWindow.setLayout(new qx.ui.layout.VBox(10));
                pluginWindow.setShowStatusbar(true);
                pluginWindow.setStatus("Plugins loaded...");
                pluginWindow.open();
                var pluginListWidget = new skel.boundWidgets.PluginList();
                pluginWindow.add( pluginListWidget );
                doc.add(pluginWindow, {left:20, top:20});

            }
        }
    });
