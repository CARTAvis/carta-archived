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

                this.m_mainContainer = new qx.ui.container.Composite( new qx.ui.layout.Canvas());
                this.m_mainContainer.setAppearance( "display-main");
                this.getRoot().add( this.m_mainContainer, {left: "0%", right: "0%", top: "0%", bottom: "0%"});
                
                this.m_desktop = new skel.widgets.DisplayMain();
                this.m_mainContainer.add( this.m_desktop, { top: "0%", bottom: "0%", left: "0%", right: "0%"});
                this.m_statusBar = new skel.widgets.StatusBar();
                this.m_mainContainer.add( this.m_statusBar, {bottom: "0%", left: "0%", right: "0%"} );
                
                this._initMenuBar();
                this.m_desktop.addListener("iconifyWindow", this._iconifyWindow, this );
                this.m_menuBar.addListener( "appear", function(ev){
                	this._repositionDesktop( true );
                }, this);
                this.m_desktop.addListener("addWindowMenu", function( ev ){
                	this.m_menuBar.addWindowMenu( ev );
                }, this);
                              
                this.m_mainContainer.addListener( "mousemove", function(ev){
                	this.m_statusBar.showHideStatus( ev );
                	this.m_menuBar.showHideMenu( ev );
                }, this);
               
            },
            
            _initMenuBar : function(){
            	this.m_menuBar = new skel.widgets.MenuBar();
            	this.m_menuBar.addListener("layoutImage",function(){
            		this.m_desktop.layoutImage();
            	}, this );
            	
            	this.m_menuBar.addListener("layoutImageAnalysisAnimator",function(){
            		this.m_desktop.layoutImageAnalysisAnimator();
            	}, this );
            	
            	this.m_menuBar.addListener("layoutRowCount",function( ev ){
            		this.m_desktop.setRowCount( ev.getData() );
            	}, this );
            	
            	this.m_menuBar.addListener("layoutColCount",function( ev ){
            		this.m_desktop.setColCount( ev.getData() );
            	}, this );
            	
            	this.m_menuBar.addListener("statusAlwaysVisible",function( ev ){         		
            		this.m_statusBar.setStatusAlwaysVisible( ev.getData());
            		this._repositionDesktop( ev.getData());
            	}, this );
            	
            	this.m_menuBar.addListener("menuAlwaysVisible",function( ev ){
            		this._repositionDesktop( ev.getData());
            	}, this );
            	
            	this.m_menuBar.addListener("menuMoved",function( ev ){
            		this._repositionDesktop();
            	}, this );
            	
            	this.m_menuBar.addListener("dataLoaded",function( ev ){
            		this.m_desktop.dataLoaded(ev.getData());
            	}, this );
            	
            	this.m_menuBar.addListener("dataUnloaded",function( ev ){
            		this.m_desktop.dataUnloaded(ev.getData());
            	}, this );
            	 
            	this.m_mainContainer.add( this.m_menuBar );
            	this.m_menuBar.reposition();
            },
            
          
            
            _repositionDesktop : function( ){
            	var permanentMenu = this.m_menuBar.isAlwaysVisible();
            	var permanentStatus = this.m_statusBar.isAlwaysVisible();
            	var topMenu = this.m_menuBar.isTop();
            	var topValue = "0%";
            	var leftValue = "0%";
            	if ( permanentMenu ){
            		if (topMenu){
            			var bottomMenu = this.m_menuBar.getHeight();
            			topValue=bottomMenu;
            		}
            		//Left menu bar.
            		else {
            			var menuWidth = this.m_menuBar.getWidth();
            			leftValue = menuWidth;
            		}
            	}
            	var bottomValue = "0%";
            	if ( permanentStatus ){
            		var topStatus = skel.widgets.Util.getTop( this.m_statusBar );
            		var topWin = skel.widgets.Util.getBottom( this.m_mainContainer );
            		var height = this.m_statusBar.getAnimationHeight();
            		bottomValue = Math.round( height /topWin * 100)+"%";
            	}
            	this.m_desktop.setLayoutProperties( {top: topValue, bottom: bottomValue, left: leftValue, right: "0%"} );
            },
     
            _iconifyWindow: function( ev ){
            	this.m_statusBar.addIconifiedWindow( ev, this );
            },
            
            /**
             * Restore the window at the given layout row and column to its original position.
             * @param row {Number} the layout row index of the window to be restored.
             * @param col {Number} the layout column index of the window to be restored.
             */
            restoreWindow : function( row, col){
            	this.m_desktop.restoreWindow( row, col );        	
            },
            
            m_desktop: null,
            m_menuBar: null,
            m_statusBar: null,
        	m_mainContainer: null   
        
        }
    });
