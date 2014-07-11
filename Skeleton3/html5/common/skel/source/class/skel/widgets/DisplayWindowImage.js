/**
 * A display window specialized for viewing images.
 */

/**


 ************************************************************************ */


qx.Class.define("skel.widgets.DisplayWindowImage",
    {
        extend: skel.widgets.DisplayWindow,
        
        /**
         * Constructor.
         */
        construct: function ( row, col) {
            this.base(arguments, "casaLoader", row, col); 
        },
        
        members: {
        	/**
        	 * Implemented to initialize the context menu.
        	 */
        	setPlugin : function( label ){
        		this._initDisplaySpecific();
        		arguments.callee.base.apply(this, arguments);
        	},
        	
        	/**
        	 * Hard-coded right now to load a fixed image.
        	 */
        	  dataLoaded: function( path ){
              	if ( this.m_content.indexOf( this.m_title) >= 0 ){
      	    		this.m_content.remove( this.m_title );
      	    	}
              	this.m_content.add( new skel.boundWidgets.View( this.m_identifier), {flex:1});        
              },
              
              /**
               * Unloads the data identified by the path.
               */
              dataUnloaded: function( path ){
              	this.m_content.removeAll();
      	    	this.m_content.add( this.m_title);
              },
              
        	
        	/**
        	 * Returns context menu items that should be displayed on the main
        	 * window when this window is selected.
        	 */
        	getWindowMenu: function(){
        		var windowMenuList = []
        		var regionButton = new qx.ui.toolbar.MenuButton( "Region");
        		regionButton.setMenu( this._initMenuRegion());
        		
        		var renderButton = new qx.ui.toolbar.MenuButton( "Render");
        		renderButton.setMenu( this._initMenuRender());
        		
        		windowMenuList.push( regionButton );
        		windowMenuList.push( renderButton );
        		return windowMenuList;
        	},
        	
            _initDisplaySpecific: function(){
            	          	
            	this.m_regionButton = new qx.ui.menu.Button("Region");
            	this.m_regionButton.setMenu( this._initMenuRegion());
            	this.m_contextMenu.add( this.m_regionButton );
            	
            	this.m_renderButton = new qx.ui.menu.Button("Render");
            	this.m_renderButton.setMenu( this._initMenuRender());
            	this.m_contextMenu.add( this.m_renderButton );
            },
        	
            _initMenuRegion : function(){
        	    var regionMenu = new qx.ui.menu.Menu;
        		regionMenu.add( new qx.ui.menu.Button( "Rectangle"));
        		regionMenu.add( new qx.ui.menu.Button( "Point"));
        		regionMenu.add( new qx.ui.menu.Button( "Ellipse"));
        		regionMenu.add( new qx.ui.menu.Button( "Polygon"));
        		var multiRegionButton = new qx.ui.menu.Button( "Multi");
        		regionMenu.add( multiRegionButton );
        		var multiRegionMenu = new qx.ui.menu.Menu;
        		multiRegionMenu.add( new qx.ui.menu.Button( "Rectangle"));
        		multiRegionMenu.add( new qx.ui.menu.Button( "Point"));
        		multiRegionMenu.add( new qx.ui.menu.Button( "Ellipse"));
        		multiRegionMenu.add( new qx.ui.menu.Button( "Polygon"));
        		multiRegionButton.setMenu( multiRegionMenu );		       		
        		return regionMenu;
            },
            
            _initMenuRender : function(){
        		var renderMenu = new qx.ui.menu.Menu;
        		renderMenu.add( new qx.ui.menu.Button( "Raster"));
        		renderMenu.add( new qx.ui.menu.Button( "Contour"));
        		renderMenu.add( new qx.ui.menu.Button( "Field"));
        		renderMenu.add( new qx.ui.menu.Button( "Vector"));
        		return renderMenu;
            },       
          
    	    m_regionButton: null, 	
    	    m_renderButton: null
        }

       
    });

