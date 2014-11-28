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

            _afterConnect: function()
            {

                var win = new qx.ui.window.Window( "Hack view" );
                win.setWidth( 300 );
                win.setHeight( 200 );
                win.setShowMinimize( false );
                win.setLayout( new qx.ui.layout.Grow() );
                win.setContentPadding( 5, 5, 5, 5 );
                win.add( new skel.boundWidgets.View( "hackView" ) );
                win.setUseResizeFrame( false);
                this.getRoot().add( win, {left: 20, top: 220} );
                win.open();

                // hacks for temporary functionality
                this.m_cursorWindow = new skel.boundWidgets.CursorWindow();
                this.m_colormapWindow = new skel.boundWidgets.ColormapWindow();

                var connector = mImport( "connector" );
                if( connector.getConnectionStatus() != connector.CONNECTION_STATUS.CONNECTED ) {
                    console.log( "Connection not established yet..." );
                    return;
                }

                this.m_mainContainer = new qx.ui.container.Composite( new qx.ui.layout.Canvas() );
                this.m_mainContainer.setAppearance( "display-main" );
                this.getRoot().add( this.m_mainContainer, {
                    left  : "0%",
                    right : "0%",
                    top   : "0%",
                    bottom: "0%"
                } );

                this.m_desktop = new skel.widgets.DisplayMain();
                this.m_mainContainer.add( this.m_desktop, {
                    top   : "0%",
                    bottom: "0%",
                    left  : "0%",
                    right : "0%"
                } );
                this.m_statusBar = new skel.widgets.StatusBar();
                this.m_mainContainer.add( this.m_statusBar, {
                    bottom: "0%",
                    left  : "0%",
                    right : "0%"
                } );

                this._initMenuBar();
                this.m_desktop.addListener( "iconifyWindow", this._iconifyWindow, this );
                this.m_menuBar.addListener( "appear", function( ev )
                {
                    this._repositionDesktop( true );
                }, this );
                this.m_desktop.addListener( "addWindowMenu", function( ev )
                {
                    this.m_menuBar.addWindowMenu( ev );
                }, this );

                qx.event.message.Bus.subscribe( "showLinks", function( message )
                {
                    this._showLinks( message );
                }, this );
                this.m_mainContainer.addListener( "mousemove", function( ev )
                {
                    this.m_statusBar.showHideStatus( ev );
                    this.m_menuBar.showHideMenu( ev );
                }, this );

                qx.event.message.Bus.subscribe( "showFileBrowser", function( message )
                {
                    if( this.m_fileBrowser == null ) {
                        this.m_fileBrowser = new skel.widgets.FileBrowser();
                    }
                    this.m_fileBrowser.setTarget( message.getData() );
                    if( this.m_mainContainer.indexOf( this.m_fileBrowser ) < 0 ) {
                        this.m_mainContainer.add( this.m_fileBrowser, {top: "15%", left: "15%"} );
                    }
                }, this );

                qx.event.message.Bus.subscribe( "closeFileBrowser", function( message )
                {
                    if( this.m_fileBrowser != null && this.m_mainContainer.indexOf( this.m_fileBrowser ) >= 0 ) {
                        this.m_mainContainer.remove( this.m_fileBrowser );
                    }
                }, this );
            },

            /**
             * Initialize the menu bar.
             */
            _initMenuBar: function()
            {
                this.m_menuBar = new skel.widgets.MenuBar();
                this.m_menuBar.addListener( "layoutImage", function()
                {
                    this._hideWindows();
                    this.m_desktop.layoutImage();
                }, this );

                this.m_menuBar.addListener( "layoutImageAnalysisAnimator", function()
                {
                    this._hideWindows();
                    this.m_desktop.layoutImageAnalysisAnimator();
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

                this.m_menuBar.addListener( "statusAlwaysVisible", function( ev )
                {
                    this.m_statusBar.setStatusAlwaysVisible( ev.getData() );
                    this._hideWindows();
                    this._repositionDesktop( ev.getData() );
                }, this );

                this.m_menuBar.addListener( "menuAlwaysVisible", function( ev )
                {
                    this._hideWindows();
                    this._repositionDesktop( ev.getData() );
                }, this );

                this.m_menuBar.addListener( "menuMoved", function( ev )
                {
                    this._hideWindows();
                    this._repositionDesktop();
                }, this );

                this.m_menuBar.addListener( "newWindow", function( ev )
                {
                    this._showWindowLocator();
                }, this );

                this.m_menuBar.addListener( "shareSession", function( ev )
                {
                    this.m_statusBar.updateSessionSharing( ev.getData() );
                }, this );

                this.m_mainContainer.add( this.m_menuBar );
                this.m_menuBar.reposition();
            },

            /**
             * Remove an overlay window.
             */
            _hideWindow: function( window )
            {
                if( window != null ) {
                    if( this.m_mainContainer.indexOf( window ) >= 0 ) {
                        this.m_mainContainer.remove( window );
                    }
                }
            },

            /**
             * Removes all overlay windows.
             */
            _hideWindows: function()
            {
                this._hideWindow( this.m_windowLocator );
                this._hideWindow( this.m_windowLink );
                if( this.m_windowLocator != null ) {
                    this.m_windowLocator.removeAll();
                }
            },

            /**
             * Iconify a window.
             */
            _iconifyWindow: function( ev )
            {
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
             * Adjust the size of the desktop based on a the visibility and location of
             * the menubar and status bar.
             */
            _repositionDesktop: function()
            {
                var permanentMenu = this.m_menuBar.isAlwaysVisible();
                var permanentStatus = this.m_statusBar.isAlwaysVisible();
                var topMenu = this.m_menuBar.isTop();
                var topValue = "0%";
                var leftValue = "0%";
                if( permanentMenu ) {
                    if( topMenu ) {
                        var bottomMenu = this.m_menuBar.getHeight();
                        topValue = bottomMenu;
                    }
                    //Left menu bar.
                    else {
                        var menuWidth = this.m_menuBar.getWidth();
                        leftValue = menuWidth;
                    }
                }
                var bottomValue = "0%";
                if( permanentStatus ) {
                    var topStatus = skel.widgets.Util.getTop( this.m_statusBar );
                    var topWin = skel.widgets.Util.getBottom( this.m_mainContainer );
                    var height = this.m_statusBar.getAnimationHeight();
                    bottomValue = Math.round( height / topWin * 100 ) + "%";
                }
                var desktopMap = {top: topValue, bottom: bottomValue, left: leftValue, right: "0%"};
                this.m_desktop.setLayoutProperties( desktopMap );
                return desktopMap;
            },

            /**
             * Restore the window at the given layout row and column to its original position.
             * @param row {Number} the layout row index of the window to be restored.
             * @param col {Number} the layout column index of the window to be restored.
             */
            restoreWindow: function( row, col )
            {
                this.m_desktop.restoreWindow( row, col );
            },

            /**
             * Show an overlay window displaying linkage between windows that allows
             * the user to edit links.
             */
            _showLinks: function( ev )
            {
                var linkSource = ev.getData();
                var pluginId = linkSource["plugin"];
                var winId = linkSource["window"];
                var linkInfo = this.m_desktop.getLinkInfo( pluginId, winId );
                if( this.m_windowLink == null ) {
                    this.m_windowLink = new skel.widgets.LinkCanvas();
                    var resizeLinkWindow = function( desktop, linkWindow )
                    {
                        var left = skel.widgets.Util.getLeft( desktop );
                        var top = skel.widgets.Util.getTop( desktop );
                        linkWindow.setLinkOffsets( left, top );
                    }
                    resizeLinkWindow( this.m_desktop, this.m_windowLink );
                    this.m_desktop.addListener( "resize", function()
                    {
                        resizeLinkWindow( this.m_desktop, this.m_windowLink );
                    }, this );

                    this.m_windowLink.addListener( "link", function( ev )
                    {
                        this._linksChanged( true, ev );
                    }, this );
                    this.m_windowLink.addListener( "linkRemove", function( ev )
                    {
                        this._linksChanged( false, ev );
                    }, this );
                    this.m_windowLink.addListener( "linkingFinished", function( ev )
                    {
                        this._hideWindows();
                    }, this );
                }
                this.m_windowLink.setDrawInfo( linkInfo );
                var desktopMap = this._repositionDesktop();
                this.m_mainContainer.add( this.m_windowLink, desktopMap );
            },

            /**
             * Add an overlay showing where new windows can be added.
             */
            _showWindowLocator: function()
            {
                var desktopMap = this._repositionDesktop();
                if( this.m_windowLocator == null ) {
                    this.m_windowLocator = new qx.ui.container.Composite();
                    this.m_windowLocator.setLayout( new qx.ui.layout.Basic() );
                    this.m_windowLocator.setBackgroundColor( "transparent" );

                }
                var locations = this.m_desktop.getAddWindowLocations();
                for( var i = 0 ; i < locations.length ; i ++ ) {
                    var loc = locations[i];
                    var windowButton = new qx.ui.form.Button( "", "skel/icons/blackCross.png" );
                    windowButton.setAppearance( "invisible-button" );
                    windowButton.splitPane = loc[2];
                    windowButton.addListener( "execute", function( ev )
                    {
                        this.splitPane.split();
                    }, windowButton );
                    windowButton.addListener( "execute", this._hideWindows(), this );
                    this.m_windowLocator.add( windowButton, {left: loc[0], top: loc[1]} );
                    windowButton.addListener( "appear", function()
                    {
                        var bounds = this.getBounds();
                        var offsetLeft = bounds["left"] - bounds["width"] / 2 + 5;
                        var offsetTop = bounds["top"] - bounds["height"] / 2;
                        this.setLayoutProperties( {left: offsetLeft, top: offsetTop} );
                    }, windowButton );
                }
                this.m_mainContainer.add( this.m_windowLocator, desktopMap );
            },

            m_desktop       : null,
            m_menuBar       : null,
            m_statusBar     : null,
            m_mainContainer : null,
            m_windowLocator : null,
            m_windowLink    : null,
            m_fileBrowser   : null,
            m_cursorWindow  : null,
            m_colormapWindow: null
        }
    } );


