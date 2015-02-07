/* ************************************************************************

 Copyright:

 License:

 Authors:
 @asset(skel/icons/blackCross2.png)
 ************************************************************************ */

/* global qx, console, mImport */
/* jshint strict: false */

/**
 * This is the main application class
 *
 * @asset(skel/*)
 * @ignore( mImport)
 */
qx.Class.define( "skel.Application",
    {
        extend: qx.application.Standalone,

        /*
         *****************************************************************************
         MEMBERS
         *****************************************************************************
         */

        members: {
            /**
             * This method contains the initial application code and gets called
             * during startup of the application
             *
             * @lint ignoreDeprecated(alert)
             */
            main: function()
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

                    console.warn( "You are running debug version" );
                }
                else {
                    console.log( "You are running build version" );
                }

                var connector = mImport( "connector" );

                // delay start of the application until we receive CONNECTED event...
                // only after we receive this event we can safely start modifying state, etc
                // otherwise some state changes/commands might get lost
                connector.setConnectionCB( this._afterConnect.bind( this ) );
                connector.connect();
            },

            _afterConnect: function(){
                
                /*var win = new qx.ui.window.Window( "Hack view" );
                win.setWidth( 300 );
                win.setHeight( 200 );
                win.setShowMinimize( false );
                win.setLayout( new qx.ui.layout.Grow() );
                win.setContentPadding( 5, 5, 5, 5 );
                win.add( new skel.boundWidgets.View( "hackView" ) );
                win.setUseResizeFrame( false);
                this.getRoot().add( win, {left: 20, top: 220} );
                win.open();*/
                var connector = mImport( "connector" );
                if( connector.getConnectionStatus() != connector.CONNECTION_STATUS.CONNECTED ) {
                    console.log( "Connection not established yet..." );
                    return;
                }

                this.m_mainContainer = new qx.ui.container.Composite( /*new qx.ui.layout.Canvas()*/new qx.ui.layout.VBox(0) );
                this.m_mainContainer.setAppearance( "display-main" );
                this.getRoot().add( this.m_mainContainer, {
                    left  : "0%",
                    right : "0%",
                    top   : "0%",
                    bottom: "0%"
                } );

                this.m_desktop = new skel.widgets.DisplayMain();
                this.m_statusBar = new skel.widgets.Menu.StatusBar();
                this._initMenuBar();
                this.m_toolBar = new skel.widgets.Menu.ToolBar();
                
                var errorHandler = skel.widgets.ErrorHandler.getInstance();
                errorHandler.setStatusBar( this.m_statusBar );
                
                this._initDesktop();

                this._initSubscriptions();
                
                var path = skel.widgets.Path.getInstance();
                this.m_sharedVarPreferences = connector.getSharedVar( path.PREFERENCES );
                this.m_sharedVarPreferences.addCB(this._preferencesCB.bind(this));
                this._preferencesCB();
        },
        
        /**
         * Initialize desktop callbacks.
         */
        _initDesktop: function(){
            this.m_desktop.addListener( "iconifyWindow", this._iconifyWindow, this );
            this.m_desktop.addListener( "addWindowMenu", function( ev )
            {
                this.m_menuBar.addWindowMenu( ev );
            }, this );
        },

        /**
         * Initialize the menu bar.
         */
            _initMenuBar: function(){
                this.m_menuBar = new skel.widgets.Menu.MenuBar();
                this.m_menuBar.addListener( "layoutImage", function()
                {
                    this._hideWindows();
                    this.m_desktop.layoutImage();
                }, this );

                this.m_menuBar.addListener( "layoutAnalysis", function()
                {
                    this._hideWindows();
                    this.m_desktop.layoutAnalysis();
                }, this );

                this.m_menuBar.addListener( "layoutRowCount", function( ev )
                {
                    this._hideWindows();
                    this.m_desktop.setRowCount( ev.getData() );
                }, this );

                this.m_menuBar.addListener( "layoutColCount", function( ev )
                {
                    this._hideWindows();
                    this.m_desktop.setColCount( ev.getData() );
                }, this );

                this.m_menuBar.addListener( "shareSession", function( ev )
                {
                    this.m_statusBar.updateSessionSharing( ev.getData() );
                }, this );
               
        },
        
        /**
         * Initialize message callbacks.
         */
        _initSubscriptions : function(){
            qx.event.message.Bus.subscribe( "showLinks", function( message )
                    {
                        this._showLinks( message );
                    }, this );
            
            qx.event.message.Bus.subscribe( "showPopupWindow", function( message ){
                        this._showPopup( message );
                    }, this );


            qx.event.message.Bus.subscribe( "showFileBrowser", function( message )
            {
                if( this.m_fileBrowser === null ) {
                    this.m_fileBrowser = new skel.widgets.FileBrowser();
                }
                this.m_fileBrowser.setTarget( message.getData() );
                if( this.m_mainContainer.indexOf( this.m_fileBrowser ) < 0 ) {
                    this.getRoot().add( this.m_fileBrowser, {
                        left  : "0%",
                        right : "50%",
                        top   : "0%",
                        bottom: "50%"
                    } );
                }
            }, this );

            qx.event.message.Bus.subscribe( "closeFileBrowser", function( message )
            {
                var root = this.getRoot();
                if( this.m_fileBrowser !== null && root.indexOf( this.m_fileBrowser ) >= 0 ) {
                    root.remove( this.m_fileBrowser );
                }
            }, this );
            
            qx.event.message.Bus.subscribe( "showCustomizeMenuDialog", function( message ){
                if( this.m_customizeMenuDialog === null ) {
                        this.m_customizeMenuDialog = new skel.widgets.Command.CustomizeDialog();
                    }
                    this._setPopupWinProperties( this.m_customizeMenuDialog );
                }, this );
        },
        
        /**
         * Update the layout of the main display area containing possible menu bars,
         * tool bars, status bars, etc.
         */
        _layout : function(){
            this._hideWindows();
            this.m_mainContainer.removeAll();
            var showMenuCmd = skel.widgets.Command.CommandShowMenu.getInstance();
            if ( showMenuCmd.getValue() ){
                this.m_mainContainer.add( this.m_menuBar );
            }
            var showToolCmd = skel.widgets.Command.CommandShowToolBar.getInstance();
            if ( showToolCmd.getValue( )){
                this.m_mainContainer.add( this.m_toolBar );
            }
            this.m_mainContainer.add( this.m_desktop, {flex : 1});
            var showStatusCmd = skel.widgets.Command.CommandShowStatus.getInstance();
            if ( showStatusCmd.getValue()){
                this.m_mainContainer.add( this.m_statusBar);
            }
        },
        
        /**
         * Callback for when user preferences change.
         */
        _preferencesCB : function(){
            var prefVal = this.m_sharedVarPreferences.get();
            if ( prefVal ){
                try {
                    var prefObj = JSON.parse( prefVal );
                    var showMenuCmd = skel.widgets.Command.CommandShowMenu.getInstance();
                    showMenuCmd.setValue( prefObj.menuVisible );
                    var showToolCmd = skel.widgets.Command.CommandShowToolBar.getInstance();
                    showToolCmd.setValue( prefObj.toolBarVisible );
                    var showStatusCmd = skel.widgets.Command.CommandShowStatus.getInstance();
                    showStatusCmd.setValue( prefObj.statusVisible );
                    this._layout();
                }
                catch( err ){
                    console.log( "Could not parse preferences: "+prefVal );
                }
            }
        },

        /**
         * Remove an overlay window.
         */
            _hideWindow: function( window )
            {
                if( window !== null ) {
                    if( this.getRoot().indexOf( window ) >= 0 ) {
                        this.getRoot().remove( window );
                    }
                }
        },

        /**
         * Removes all overlay windows.
         */
        _hideWindows: function(){
            this._hideWindow( this.m_windowLink );
        },

        /**
         * Iconify a window.
         */
        _iconifyWindow: function( ev ){
            this.m_statusBar.addIconifiedWindow( ev, this );
        },

        /**
         * A linkage between displays has either been added or removed.
         * @param addLink {boolean} whether the link is being added or removed.
         */
        _linksChanged: function( addLink, ev )
            {
                var data = ev.getData();
                var linkSource = data.source;
                var linkDest = data.destination;
                this.m_desktop.link( linkSource, linkDest, addLink );
        },



        /**
         * Restore the window at the given layout row and column to its original position.
         * @param row {Number} the layout row index of the window to be restored.
         * @param col {Number} the layout column index of the window to be restored.
         */
        restoreWindow : function( row, col){
            this.m_desktop.restoreWindow( row, col );
        },

        /**
         * Show an overlay window displaying linkage between windows that allows
         * the user to edit links.
         */
        _showLinks : function( ev ){
            var linkSource = ev.getData();
            var pluginId = linkSource.plugin;
            var winId = linkSource.window;
            var linkInfo = this.m_desktop.getLinkInfo( pluginId, winId );
            if ( this.m_windowLink === null ){
                this.m_windowLink = new skel.widgets.Link.LinkCanvas();
                var resizeLinkWindow = function( anObject, linkWindow ){
                    var left = 0;
                    var top = anObject._getLinkTopOffset();
                    linkWindow.setLinkOffsets( left, top );
                };
                resizeLinkWindow( this, this.m_windowLink );
                this.m_desktop.addListener( "resize", function(){
                    resizeLinkWindow( this, this.m_windowLink );
                }, this );

                this.m_windowLink.addListener( "link", function( ev ){
                    this._linksChanged( true, ev );
                }, this );
                this.m_windowLink.addListener( "linkRemove", function( ev ){
                    this._linksChanged( false, ev );
                }, this );
                this.m_windowLink.addListener( "linkingFinished", function( ev ){
                    this._hideWindows();
                }, this );
                }
                this.m_windowLink.setDrawInfo( linkInfo );
               
                var topPos = this._getLinkTopOffset();
                var bottomPos = this._getLinkBottomOffset();
                this.getRoot().add( this.m_windowLink, {left:0, top:topPos, bottom:bottomPos, right:0});
            },
            
            /**
             * Return the total height in pixels of display elements like a possible menu or toolbar
             * bar at the top of the main window.
             * @return {Number} an offset from the top of the window where the main
             *          display begins.
             */
            _getLinkTopOffset : function(){
                var topPos = 0;
                var menuCmd = skel.widgets.Command.CommandShowMenu.getInstance();
                if ( menuCmd.getValue() ){
                    var menuBounds = this.m_menuBar.getBounds();
                    var height = menuBounds.height;
                    topPos = topPos + height;
                }
                var toolCmd = skel.widgets.Command.CommandShowToolBar.getInstance();
                if ( toolCmd.getValue()){
                    var toolBounds = this.m_toolBar.getBounds();
                    var toolHeight = toolBounds.height;
                    topPos = topPos + toolHeight;
                }
                return topPos;
            },
            
            /**
             * Return the total height in pixels of display elements like a possible status
             * bar at the bottom of the main window.
             * @return {Number} an offset from the bottom of the window where the main
             *          display begins.
             */
            _getLinkBottomOffset : function(){
                var bottomPos = 0;
                var statusCmd = skel.widgets.Command.CommandShowStatus.getInstance();
                if ( statusCmd.getValue()){
                    var statusBounds = this.m_statusBar.getBounds();
                    var height = statusBounds.height;
                    bottomPos = bottomPos + height;
                }
                return bottomPos;
            },
            
            /**
             * Show a window as a popup dialog.
             * @param message {Object} information about thee window that will be shown.
             */
            _showPopup : function( message ){
                var data = message.getData();
                var win = skel.widgets.Window.WindowFactory.makeWindow( data.pluginId, -1, -1, -1, true );
                this._setPopupWinProperties( win );
            },
            
            /**
             * Set uniform look and feel for a popup window.
             * @param win {skel.widgets.Window.MoveResizeWindow}.
             */
            _setPopupWinProperties : function( win ){
                win.setWidth( 300 );
                win.setHeight( 200 );
                win.setLayout( new qx.ui.layout.Grow() );
                win.setContentPadding( 5, 5, 5, 5 );
                win.setShowClose( true );
                win.setMovable( true );
                win.setResizable( true );
                win.setAlwaysOnTop( true );
                var center = skel.widgets.Util.getCenter( this.m_mainContainer );
                var halfWidth = 250;
                var halfHeight = 100;
                var leftPt = center[0] - halfWidth;
                var topPt = center[1] - halfHeight;
                var widthVal = 2 * halfWidth;
                var heightVal = 2 * halfHeight;
                this.getRoot().add(win);
                win.open();
                win.setUserBounds( leftPt, topPt, widthVal, heightVal );
            },


            m_desktop       : null,
            m_menuBar       : null,
            m_toolBar : null,
            m_statusBar     : null,
            m_mainContainer : null,
            m_windowLink    : null,
            m_fileBrowser   : null,
            m_customizeMenuDialog : null,
            m_sharedVarPreferences : null
        }
    } );


