/**
 * A display window for a generic plug-in.
 */

/**

 @ignore(fv.assert)
 @ignore(fv.console.*)

 

 ************************************************************************ */


qx.Class.define("skel.widgets.DisplayWindowGenericPlugin",
    {
        extend: skel.widgets.DisplayWindow,
        
        /**
         * Constructor.
         */
        construct: function ( /*hub*/) {
            this.base(arguments);
        },

        members: {
        	/**
        	 * Implemented to initialize a context menu.
        	 */
        	setPlugin : function( label ){
        		this._initDisplaySpecific();
        		arguments.callee.base.apply(this, arguments);
        		
        		if ( label == "statistics"){
            		var labelx = new skel.boundWidgets.Label( "MouseX:", "pix", "/mouse/x");
                    this.add( labelx);
                    var labely = new skel.boundWidgets.Label( "MouseY:", "pix", "/mouse/y");
                    this.add( labely);
            
            	}
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
            	
            },
            
            _emit: function (path, data) {
                //fv.assert(this.m_hub !== null, "hub is NULL");
                //this.m_hub.emit(path, data);
            },

            m_hub: null
    		
        }

       
    });

