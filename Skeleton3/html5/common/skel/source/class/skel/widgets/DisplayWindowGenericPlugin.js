/**
 * A display window for a generic plug-in.
 */

/**

 

 ************************************************************************ */


qx.Class.define("skel.widgets.DisplayWindowGenericPlugin",
    {
        extend: skel.widgets.DisplayWindow,
        
        /**
         * Constructor.
         */
        construct: function ( row, col, pluginId) {
            this.base(arguments, pluginId, row, col);
        },

        members: {
        	/**
        	 * Implemented to initialize a context menu.
        	 */
        	setPlugin : function( label ){
        		this._initDisplaySpecific();
        		arguments.callee.base.apply(this, arguments);
        		
        		
        	},
        	
        	/**
        	 * Returns plug-in context menu items that should be displayed on the main
        	 * menu when this window is selected.
        	 */
        	getWindowMenu: function(){
        		var windowMenuList = []
        		return windowMenuList;
        	},
       
            _initDisplaySpecific: function(){
            	 if ( this.m_pluginId == "plugins"){
             		  var pluginList = new skel.boundWidgets.PluginList();
             		  this.m_content.add( pluginList );
             	 }
            },
            
            /**
             * Links this window to another with the given id.
             */
            //TODO:: Make generic.
            setLinkId : function( id ){
            	
          	  if ( this.m_title.getValue() == "statistics"){
    			var mouseXPath = "/carta/mouse/x/"+id;
        		var labelx = new skel.boundWidgets.Label( "MouseX:", "pix", mouseXPath );
                this.m_content.add( labelx);
                var mouseYPath = "/carta/mouse/y/"+ id;
                var labely = new skel.boundWidgets.Label( "MouseY:", "pix", mouseYPath);
                this.m_content.add( labely);
          	  }
          	 
          	  return true;
          	  
            }
            
        }

       
    });

