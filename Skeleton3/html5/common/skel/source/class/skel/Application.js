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
                this.base(arguments);

                // Enable logging in debug variant
                if (qx.core.Environment.get("qx.debug"))
                {
                    // everything inside the curlies gets compiled out in release mode

                    // support native logging capabilities, e.g. Firebug for Firefox
//        qx.log.appender.Native;
                    // support additional cross-browser console. Press F7 to toggle visibility
//        qx.log.appender.Console;

                    console.log( "I will be compiled out");
                }

                if( false) {
                    console.log( "I should be compiled out.");
                }

                var xxx = false;
                if( xxx) {
                    console.log( "I wish I was compiled out, but the compiler is not that good :(");
                }

                console.log( "I will never be compiled out :(");

                var connector = mImport("connector");
                connector.setConnectionCB( function( s )
                {
                    console.log( "connectionCB", connector, connector.getConnectionStatus() )
                } );
                connector.connect();



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


                // Create a button
                var button1 = new qx.ui.form.Button("Share this session", "skel/test.png");

                // Document is the application root
                var doc = this.getRoot();

                // Add button to document at fixed coordinates
                doc.add(button1, {left: 100, top: 50});

                // Add an event listener
                button1.addListener("execute", function(e) {
                    var con=mImport("connector");
                    con.shareSession( function( url) {
                            tfield.setValue( url);
                            tfield.setVisibility( "visible");
                        }, "a", null,
                            60*60*1000);
                });

                var tfield = new qx.ui.form.TextField("").set({
                    readOnly: true,
                    minWidth: 400,
                    visibility: "hidden"
                });
                this.getRoot().add( tfield, { left: 100, top: 130 });

                // status bar
                var statusBar = new qx.ui.form.TextField("Status bar").set({
                    readOnly: true
                });
                this.getRoot().add( statusBar, { left: 10, right: 10, bottom: 10 });



                var labelx = new skel.boundWidgets.Label( "MouseX:", "pix", "/mouse/x");
                doc.add( labelx, { left: 100, top: 170 });
                var labely = new skel.boundWidgets.Label( "MouseY:", "pix", "/mouse/y");
                doc.add( labely, { left: 100, top: 190 });


            }
        }
    });
