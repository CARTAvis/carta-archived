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
                
                var connector = mImport( "connector" );
                if( connector.getConnectionStatus() != connector.CONNECTION_STATUS.CONNECTED ) {
                    console.log( "Connection not established yet..." );
                    return;
                }

                // activate experimental code
                var hacksVar = connector.getSharedVar( "/hacks/enabled" );
                var initHacks = function() {
                    if( hacksVar.get() !== "1") return;
                    this.m_hacks = new skel.hacks.Hacks( this);
                    this.getRoot().add( this.m_hacks, {left: 20, top: 220} );
                    this.m_hacks.open();
                }.bind(this);
                hacksVar.addCB( initHacks);
                initHacks();

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
                this.m_menuBar = new skel.widgets.Menu.MenuBar();
                this.m_toolBar = new skel.widgets.Menu.ToolBar();
                
                var errorHandler = skel.widgets.ErrorHandler.getInstance();
                errorHandler.setStatusBar( this.m_statusBar );
                
                this._initDesktop();
                this._initSubscriptions();
                this._layout();
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
        
        _showCustomizeDialog : function( message ){
            if( this.m_customizeDialog === null ) {
                this.m_customizeDialog = new skel.Command.Customize.CustomizeDialog();
                this.m_customizeDialog.addListener("closeCustomizeDialog", function(ev){
                    this._hideWidget( this.m_customizeDialog );
                }, this );
            }
            var data = message.getData();
            this.m_customizeDialog.setMenuPage( data.menu );
            var layoutObj = {
                    left  : "0%",
                    right : "70%",
                    top   : "10%",
                    bottom: "10%"
            };
            this._showWidget( this.m_customizeDialog, layoutObj );
        },
        
        _showLayoutPopup : function( message ){
            if( this.m_layoutPopup === null ) {
                this.m_layoutPopup = new skel.widgets.CustomLayoutPopup();
                this.m_layoutPopup.addListener( "layoutRowCount", function( message )
                        {
                            this._hideWindows();
                            this.m_desktop.setRowCount( message.getData() );
                        }, this );
                this.m_layoutPopup.addListener( "layoutColCount", function( message )
                        {
                            this._hideWindows();
                            this.m_desktop.setColCount( message.getData() );
                        }, this );
                this.m_layoutPopup.addListener("closeCustomLayout", function(ev){
                    this._hideWidget( this.m_layoutPopup );
                }, this );
            }
            //Only show the layout popup if we are not restoring a session.  Restoring might
            //trigger the pop-up to incorrectly show if the layout changes.
            if ( this.m_sessionRestoreDialog ===null || !this.m_sessionRestoreDialog.isVisible()){
                this.m_layoutPopup.setGridSize( this.m_desktop.getRowCount(), this.m_desktop.getColCount());
                var layoutObj = {
                        left  : "0%",
                        right : "90%",
                        top   : "0%",
                        bottom: "90%"
                };
                this._showWidget( this.m_layoutPopup, layoutObj );
            }
        },
        
        _showFileBrowser : function( message ){
            if( this.m_fileBrowser === null ) {
                this.m_fileBrowser = new skel.widgets.FileBrowser();
            }
            this.m_fileBrowser.setTarget( message.getData() );
            var layoutObj = {
                    left  : "0%",
                    right : "50%",
                    top   : "0%",
                    bottom: "50%"
            };
            this._showWidget( this.m_fileBrowser, layoutObj );
        },
        
        _showSessionRestore : function( message ){
            if ( this.m_sessionRestoreDialog === null ){
                this.m_sessionRestoreDialog = new skel.Command.Session.RestoreDialog();
                this.m_sessionRestoreDialog.addListener("closeSessionRestore",function(evt){
                    this._hideWidget( this.m_sessionRestoreDialog );
                }, this );
            }
            var layoutObj = {
                    left : "0%",
                    right : "25%",
                    top : "0%",
                    bottom : "75%"
            };
            this._showWidget( this.m_sessionRestoreDialog, layoutObj );
        },
        
        _showSessionSave : function( message ){
            if ( this.m_sessionSaveDialog === null ){
                this.m_sessionSaveDialog = new skel.Command.Session.SaveDialog();
                this.m_sessionSaveDialog.addListener("closeSessionSave",function(evt){
                    this._hideWidget( this.m_sessionSaveDialog );
                }, this );
            }
            var layoutObj = {
                    left : "0%",
                    right : "50%",
                    top : "0%",
                    bottom : "65%"
            };
            this._showWidget( this.m_sessionSaveDialog, layoutObj );
        },
        
        _showHistogramPreferences : function( message ){
            if( this.m_histogramPreferences === null ) {
                this.m_histogramPreferences = new skel.widgets.Histogram.Preferences( message.getData());
                this.m_histogramPreferences.addListener("closeHistogramPreferences", function(ev){
                    this._hideWidget( this.m_histogramPreferences );
                }, this );
            }
            var layoutObj = {
                    left  : "0%",
                    right : "75%",
                    top   : "0%",
                    bottom: "75%"
            };
            this._showWidget( this.m_histogramPreferences, layoutObj );
        },
        
        _showWidget : function( widget, layoutObj ){
            var root = this.getRoot();
            if( root.indexOf( widget ) < 0 ) {
                root.add( widget, layoutObj );
            }
        },
        
        /**
         * Remove the widget from the main display.
         * @param widget {qx.ui.core.Widget} the widget to remove.
         */
        _hideWidget : function( widget ){
            var root = this.getRoot();
            if( widget !== null && root.indexOf( widget ) >= 0 ) {
                root.remove( widget );
            }
        },
        
        /**
         * Initialize message callbacks.
         */
        _initSubscriptions : function(){
            qx.event.message.Bus.subscribe( "showLinks", function( message ){
                this._showLinks( message );
            }, this );
            qx.event.message.Bus.subscribe( "linkingFinished", function( ev ){
                this._hideWindows();
            }, this );
            qx.event.message.Bus.subscribe( "showPopupWindow", function(message){
                this._showPopup( message );
            }, this );
            qx.event.message.Bus.subscribe( "showCustomizeDialog", function(message){
                this._showCustomizeDialog( message );
            }, this );
            qx.event.message.Bus.subscribe( "showHistogramPreferences", function(message){
                this._showHistogramPreferences( message );
            }, this );
            qx.event.message.Bus.subscribe( "showLayoutPopup", function(message){
                this._showLayoutPopup( message);
            }, this );
            qx.event.message.Bus.subscribe( "showFileBrowser", function(message){
                this._showFileBrowser(message);
            }, this );
            qx.event.message.Bus.subscribe( "closeFileBrowser", function( message ){
                this._hideWidget( this.m_fileBrowser );
            }, this );
            qx.event.message.Bus.subscribe( "shareSession", function( ev ){
                this.m_statusBar.updateSessionSharing( ev.getData() );
            }, this );
            qx.event.message.Bus.subscribe( "showSessionRestoreDialog", function(message){
                this._showSessionRestore( message );
            }, this );
            qx.event.message.Bus.subscribe( "showSessionSaveDialog", function(message){
                this._showSessionSave( message);
            }, this );
            
            qx.event.message.Bus.subscribe( "layoutChanged", function( ev ){
                this._layout();
            }, this );
        },
        
        /**
         * Update the layout of the main display area containing possible menu bars,
         * tool bars, status bars, etc.
         */
        _layout : function(){
            this._hideWindows();
            this.m_mainContainer.removeAll();
            var showMenuCmd = skel.Command.Preferences.Show.CommandShowMenu.getInstance();
            if ( showMenuCmd.getValue() ){
                this.m_mainContainer.add( this.m_menuBar );
            }
            var showToolCmd = skel.Command.Preferences.Show.CommandShowToolBar.getInstance();
            if ( showToolCmd.getValue( )){
                this.m_mainContainer.add( this.m_toolBar );
            }
            this.m_mainContainer.add( this.m_desktop, {flex : 1});
            var showStatusCmd = skel.Command.Preferences.Show.CommandShowStatus.getInstance();
            if ( showStatusCmd.getValue()){
                this.m_mainContainer.add( this.m_statusBar);
            }
        },
        
        

        /**
         * Remove an overlay window.
         */
        _hideWindow: function( window ){
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
            this.m_statusBar.clearMessages();
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
                this.m_windowLink = skel.widgets.Link.LinkCanvas.getInstance();
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
               
            }
           
            this.m_windowLink.setDrawInfo( linkInfo );
            this.m_statusBar.showInformation( this.m_windowLink.getHelp());
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
            var menuCmd = skel.Command.Preferences.Show.CommandShowMenu.getInstance();
            if ( menuCmd.getValue() ){
                var menuBounds = this.m_menuBar.getBounds();
                var height = menuBounds.height;
                topPos = topPos + height;
            }
            var toolCmd = skel.Command.Preferences.Show.CommandShowToolBar.getInstance();
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
            var statusCmd = skel.Command.Preferences.Show.CommandShowStatus.getInstance();
            if ( statusCmd.getValue()){
                var statusBounds = this.m_statusBar.getBounds();
                var height = statusBounds.height;
                bottomPos = bottomPos + height;
            }
            return bottomPos;
        },
        
        /**
         * Show a window as a pop-up dialog.
         * @param message {Object} information about the window that will be shown.
         */
        _showPopup : function( message ){
            var data = message.getData();
            var win = skel.widgets.Window.WindowFactory.makeWindow( data.pluginId, -1, -1, -1, true );
            win.addListener( "registered", function(){
                var sourceId = win.getIdentifier();
                var addLinkCmd = skel.Command.Link.CommandLinkAdd.getInstance();
                addLinkCmd.link( sourceId, data.winId, null );
            }, this);
            this._setPopupWinProperties( win );
        },
            
        /**
         * Set uniform look and feel for a pop-up window.
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
            var halfWidth = 200;
            var halfHeight = 200;
            var leftPt = center[0] - halfWidth;
            var topPt = center[1] - halfHeight;
            var widthVal = 2 * halfWidth;
            var heightVal = 2 * halfHeight;
            this.getRoot().add(win);
            win.open();
            win.setUserBounds( leftPt, topPt, widthVal, heightVal );
        },

        m_desktop       : null,
        m_histogramPreferences : null,
        m_menuBar       : null,
        m_toolBar : null,
        m_statusBar     : null,
        m_mainContainer : null,
        m_windowLink    : null,
        m_fileBrowser   : null,
        m_customizeDialog : null,
        m_layoutPopup : null,
        m_sharedVarPreferences : null,
        m_sessionRestoreDialog : null,
        m_sessionSaveDialog : null
    }
} );


