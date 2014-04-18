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
          // everything inside the curlies gets compiled out

        // support native logging capabilities, e.g. Firebug for Firefox
//        qx.log.appender.Native;
        // support additional cross-browser console. Press F7 to toggle visibility
//        qx.log.appender.Console;

          console.log( "xyz1");
      }

        if( false) {
            console.log( "xyz2");
        }

        var xxx = false;
        if( xxx) {
            console.log( "xyz3");
        }

        console.log( "xyzw");

        var connector = mImport("connector");
        connector.setConnectionCB( function( s )
        {
            console.log( "connectionCB", connector, connector.getConnectionStatus() )
        } );
        connector.connect();



        var win = new qx.ui.window.Window("First Window");
        win.setWidth(300);
        win.setHeight(200);
        win.setShowMinimize(false);
        win.setLayout( new qx.ui.layout.Grow());
        win.setContentPadding( 5, 5, 5, 5);
        win.add( new skel.boundWidgets.View( "view1"));

        this.getRoot().add(win, {left:20, top:20});
        win.open();

      /*
      -------------------------------------------------------------------------
        Below is your actual application code...
      -------------------------------------------------------------------------
      */

      // Create a button
      var button1 = new qx.ui.form.Button("First Button", "skel/test.png");

      // Document is the application root
      var doc = this.getRoot();

      // Add button to document at fixed coordinates
      doc.add(button1, {left: 100, top: 50});

      // Add an event listener
      button1.addListener("execute", function(e) {
        alert("Hello World!");
      });
    }
  }
});
