/**
 * Base class for Windows displaying plugins.
 */
/*global mImport */
/**
 @ignore( mImport)
 ************************************************************************ */


qx.Class.define("skel.widgets.DisplayWindow",
    {
        extend: qx.ui.window.Window,
        /**
         * Constructor.
         * @param pluginId {String} the name of the plugin that will be displayed.
         * @param row {Number} a row in the screen grid.
         * @param col {Number} a column in the screeen grid.
         */
        construct: function ( pluginId, row, col) {
            this.base(arguments);
            this.m_pluginId = pluginId;
            this.m_identifier = "win"+skel.widgets.DisplayWindow.windowCounter;
        	skel.widgets.DisplayWindow.windowCounter++;
        	var paramMap = "pluginId:"+this.m_pluginId+",winId:"+this.m_identifier;	
            var connector = mImport( "connector");
            connector.sendCommand( "registerView", paramMap, function(){});	
            var id = this.addListener( "appear", function(){	
            	var container = this.getContentElement().getDomElement();
            	container.id= this.m_identifier;
            	this.removeListenerById( id );
            }, this );
            
            
            this.m_row = row;
            this.m_col = col;
                  
            this._init();
           
        },

        events:
        {
          "maximizeWindow" : "qx.event.type.Data",
          "restoreWindow" : "qx.event.type.Data",
          "closeWindow" : "qx.event.type.Data",
          "windowSelected" : "qx.event.type.Data",
          "setView" : "qx.event.type.Data"
        	  
        }, 
        
        statics : {
        	//Used for generating unique window ids.
        	windowCounter : 0
        },
        
        members: {
        	/**
        	 * Set the appearance of this window based on whether or not it is selected.
        	 * @param selected {boolean} true if the window is selected; false otherwise.
        	 * @param multiple {boolean} true if multiple windows can be selected; false otherwise.
        	 */
        	setSelected : function ( selected, multiple ){
        		if (selected ){
        			this.setAppearance("display-window-selected");
        			if ( !multiple ){
        				this.fireDataEvent( "windowSelected", this);
        			}
        		}
        		else {
        			this.setAppearance("display-window");
        		}
        	},
 
        	/**
             * Implemented by subclasses having context menu items that should be displayed
             * on the main menu when they are selected.
             */
            getWindowMenu: function(){
        	},
        	
              /**
               * Set the identifier for the plugin that will be displayed.
               * @param label {String} an identifier for the plugin.
               */
        	  setPlugin: function( label ){
               
              	//Right now the pluginId is the title, but this will change.
              	this.setTitle( label );
              	this._initContextMenu();
              },
              
              /**
               * Return the window title.
               */
              getIdentifier: function(){
              	return this.m_identifier;
              },
              
              
              setLinkId : function( id ){
            	  return true;
              },
              
              /**
               * Set an (optional) title for the window.
               * @param label {String} a title for the window.
               */
              setTitle: function( label ){
              	if ( this.m_title == null ){
              		this.m_title = new skel.boundWidgets.Label(label,"",""); 
              		this.m_content.add( this.m_title );
              	}            	
              	this.m_title.setValue( label );           	
              },
        	
            _init : function(){
            	//Make the window decorations invisible.
                this.getChildControl("captionbar").setVisibility("excluded"); 
            	this.setShowMinimize(false);
                this.setShowMaximize(false);
                this.setShowClose(false);
                this.setUseResizeFrame(false);
                this.setContentPadding(0, 0, 0, 0);
                this.setAllowGrowX( true );
                this.setAllowGrowY( true );
                
             
                this.setLayout(new qx.ui.layout.VBox(0));
                this.m_scrollArea = new qx.ui.container.Scroll();
                this.m_content = new qx.ui.container.Composite();
                this.m_content.setLayout( new qx.ui.layout.VBox(0));
                this.m_scrollArea.add( this.m_content );
                this.add( this.m_scrollArea, {flex:1});
                this.m_contextMenu = new qx.ui.menu.Menu;
                
                this.addListener( "mousedown", function(ev){           	
                	this.setSelected( true, ev.isCtrlPressed());
                });
            },
            
            /**
             * Initializes a generic window context menu.
             */
            _initContextMenu: function(){
            	this.m_linkButton = new qx.ui.menu.Button( "Link");
            	this.m_linkButton.setMenu( this._initMenuLink());
            	this.m_contextMenu.add( this.m_linkButton );
            	
                this.m_windowMenu = new qx.ui.menu.Menu;
        	    this.m_minimizeButton = new qx.ui.menu.Button( "Minimize");
        	    this.m_minimizeButton.addListener( "execute", function(){
        	    	this.hide();
        	    }, this );
        	    this.m_maximizeButton = new qx.ui.menu.Button( "Maximize");
        	    this.m_maximizeButton.addListener( "execute", function(){
        	    	this._maximize();
        	    }, this );
        	    this.m_restoreButton = new qx.ui.menu.Button( "Restore");
        	    this.m_restoreButton.addListener( "execute", function(){
        	    	this._restore();
        	    }, this );
        	    this.m_closeButton = new qx.ui.menu.Button( "Close");
        	    this.m_closeButton.addListener( "execute", function(){
        	    	this._close();
        	    }, this );
        	   
        	    var windowButton = new qx.ui.menu.Button( "Window" );
        	    windowButton.setMenu( this.m_windowMenu );
        	    this.m_contextMenu.add( windowButton );
        	    this.m_windowMenu.add( this.m_maximizeButton );
        	    this.m_windowMenu.add( this.m_minimizeButton );
        	    this.m_windowMenu.add( this.m_closeButton );
        	    
        	    var pluginMenu = new qx.ui.menu.Menu;
        	    var imageButton = new qx.ui.menu.Button( "Image");
        	    imageButton.addListener( "execute", function(){
        	    	this.fireDataEvent("setView","image");
        	    }, this);
        	    pluginMenu.add( imageButton );
        	    var profileButton = new qx.ui.menu.Button( "Profile");
        	    profileButton.addListener( "execute", function(){
        	    	this.fireDataEvent( "setView","profile");
        	    }, this);
        	    pluginMenu.add( profileButton );
        	    var histogramButton = new qx.ui.menu.Button( "Histogram");
        	    histogramButton.addListener( "execute", function(){
        	    	this.fireDataEvent( "setView", "histogram");
        	    }, this);
        	    pluginMenu.add( histogramButton );
        	    var statButton = new qx.ui.menu.Button( "Statistics");
        	    statButton.addListener( "execute", function(){
        	    	this.fireDataEvent("setView", "statistics");
        	    }, this);
        	    pluginMenu.add( statButton );
        	    var animButton = new qx.ui.menu.Button( "Animator");
        	    animButton.addListener( "execute", function(){
        	    	this.fireDataEvent( "setView", "animator");
        	    }, this);
        	    pluginMenu.add( animButton );
        	    var pluginButton = new qx.ui.menu.Button( "View" );
        	    this.m_contextMenu.add( pluginButton );
        	    pluginButton.setMenu( pluginMenu );
        	    
        	   
                this.setContextMenu( this.m_contextMenu );
            },
            
            _initMenuLink : function(){
        	    var linkMenu = new qx.ui.menu.Menu;
        		linkMenu.add( new qx.ui.menu.Button( "Animator..."));       			       		
        		return linkMenu;
            },
            
            /**
             * Maximizes the window
             */
            _maximize: function () {
            	var maxIndex = this.m_windowMenu.indexOf(this.m_maximizeButton);
            	if ( maxIndex >= 0 ){
            		this.m_windowMenu.remove( this.m_maximizeButton );
            		this.m_windowMenu.addAt( this.m_restoreButton, maxIndex );
            	}
            	this.fireDataEvent( "maximizeWindow", this);
            	this.maximize();
            },
            
            /**
             * Closes the window
             */
            _close: function () {
            	//Send signal to other windows that they can reclaim space.
            	this.m_closed = true;
            	this.fireDataEvent( "closeWindow", this );
            },
            
            /**
             * Returns whether or not this window is closed.
             */
            isClosed: function(){
            	return this.m_closed;
            },
        
            
            /**
             * Restores the window to its location in the main display.
             */
            _restore: function(){
            	var restoreIndex = this.m_windowMenu.indexOf( this.m_restoreButton );
            	var maxIndex = this.m_windowMenu.indexOf( this.m_maximizeButton );
            	if ( maxIndex == -1 ){
            		this.m_windowMenu.addAt( this.m_maximizeButton, restoreIndex );
            	}
            	if ( restoreIndex >= 0 ){
            		this.m_windowMenu.remove( this.m_restoreButton );
            	}
            	this.open();
            	this.m_closed = false;
            	this.fireDataEvent( "restoreWindow", this);
            	this.restore();
            },
            
            /**
             * Implemented by subclasses that display particular types of data.
             */
            dataLoaded: function( path ){
          
            },
            
            /**
             * Implemented by subclasses that display particular types of data.
             */
            dataUnloaded: function( path ){
           	
            },

        
            m_closed: false,
            m_contextMenu: null,
            m_windowMenu: null,
            m_scrollArea: null,
            m_content: null,
            
            
            //Identifies the plugin we are displaying.
            m_pluginId: "",
            //Unique identifier generated by the server.
            m_identifier: "",
            //For now a display friendly title.
            m_title: null,
            m_row: 0,
            m_col: 0
        },

        properties: {
            appearance: {
                refine: true,
                init: "display-window"
            }
            
        }

    });

