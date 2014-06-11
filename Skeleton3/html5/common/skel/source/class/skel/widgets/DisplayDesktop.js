/**
 * Manages a single window for displaying a plugin.
 */

/**

 @ignore(fv.assert)
 @ignore(fv.console.*)

 

 ************************************************************************ */


qx.Class.define("skel.widgets.DisplayDesktop",
    {
        extend: qx.ui.window.Desktop,

        /**
         * Constructor.
         * @param row {Number} the layout row index of this desktop.
         * @param col {Number} the layout col index of this desktop.
         */
        construct: function ( /*hub,*/ row, col) {
        	this.base( arguments);
            /*hub = null;
            if( hub == null)
                this.m_hub = null;
            else
                this.m_hub = hub;
            */
            this.m_row = row;
            this.m_col = col;
           
            this.set({decorator:"desktop"});
        
        },

        events: {
            "iconifyWindow": "qx.event.type.Data",
            "windowSelected": "qx.event.type.Data"
        },

        members: {
        	
        	/**
        	 * Factory method for making window specialized to correct type.
        	 * @param pluginId {String} an identifier for the type of plugin the window will manage.
        	 */
        	_makeWindow : function( pluginId ){
        		if ( pluginId == "image"){
        			this.m_window = new skel.widgets.DisplayWindowImage();
        		}
        		else if ( pluginId == "animator"){
        			this.m_window = new skel.widgets.DisplayWindowAnimation();
        		}
        		else {
        			this.m_window = new skel.widgets.DisplayWindowGenericPlugin();       			
        		} 
        	},
        	
        	/**
        	 * Returns whether or not a different plug-in was reassigned to this DisplayArea
        	 * based on whether its location matches the rowIndex and colIndex passed in.
        	 * @param pluginId {String} a new plug-in identifier.
        	 * @param rowIndex {Number} a row index in the layout.
        	 * @param colIndex {Number} a column index in the layout.
        	 */
        	 setView : function( pluginId, rowIndex, colIndex ){
        		 //If this is not the target, return.
        		 if ( rowIndex != this.m_row || colIndex != this.m_col ){
        			 return false;
        		 }
        		 
        		 if ( this.m_window != null ){
        			 this.removeWindows();
        			 this.m_window = null;
        		 }
        		
        		 this._makeWindow( pluginId );
        		
        		 this.m_window.addListener("disappear", function () {  
                 	if ( ! this.m_window.isClosed()){
                 		var bounds = this.m_window.getBounds();
                 		var data ={
                 				row: this.m_row,
                 				col: this.m_col,
                 				title: this.m_window.getIdentifier()
                 			};
                 		this.fireDataEvent( "iconifyWindow", data);
                 	};
         		}, this );
                 
                 this.m_window.addListener( "windowSelected", function(ev){
                 	this.fireDataEvent( "windowSelected", ev.getData());
                 },this );
                             	 	
            	this.m_window.addListener("maximizeWindow", function(){
            		var appRoot = this.getApplicationRoot();
               	 	appRoot.add( this.m_window );
            	 }, this );
     	       	 	
     	        this.m_window.addListener("restoreWindow", function(){
     	    	 		this.restoreWindow(this.m_window.getIdentifier());
     	    	 }, this );
     	       	 
     	       	this.m_window.addListener("closeWindow", function(){
     		 		this.exclude();
     		 	}, this );
     	       	 	
            	 	
            	this.addListener( "resize", function(){
            	 	var bounds = this.getBounds();
            	 	this.m_window.setWidth( bounds["width"]);
            	 	this.m_window.setHeight( bounds["height"]);
            	});
            	
            	this.m_window.addListener( "setView", function(ev){
            		this.setView( ev.getData );
            	}, this);
            	
            	this.m_window.setPlugin( pluginId);
         		this.add( this.m_window );
            	
            	this.m_window.open();
            	return true;
        	 },
        	
        	 /**
        	  * Resets selected status.
        	  * @param win {DisplayWindow} the display window that has been selected.
        	  */
        	 windowSelected : function( win ){
        		 if ( this.m_window != null && this.m_window != win ){
        			 this.m_window.setSelected( false, false );
        		 }
        	 },
        	 
        	 /**
        	  * Returns whether or not the window was restored based on whether or not
        	  * the location matches this one.
        	  * @param row {Number} the layout row of the window to be restored.
        	  * @param col {Number} the layout column of the window to be restored.
        	  */
        	 restoreWindow : function( row, col ){
             	var restored = false;
             	if ( this.m_window != null && this.m_row==row && this.m_col == col ){
             		restored = true;
             		var appRoot = this.getApplicationRoot();
             		if ( appRoot.indexOf( this.m_window) != -1 ){
             			appRoot.remove( this.m_window);
             			this.add( this.m_window);
             		}
             		this.m_window.open();
             		this.show();
             	}
             	return restored;
             },
             
             /**
              * Remove all DisplayWindows.
              */
             removeWindows : function(){
            	if ( this.m_window != null ){
            		qx.event.Registration.removeAllListeners(this.m_window);
            		this.removeAll();
            	}
         	},
         	 
         
         	/**
       	  * Returns whether or not the width was set based on the location of this desktop
       	  * compared to the layout row and column index passed in.
       	  * @param width {Number} horizontal of the display area.
       	  * @param rowIndex {Number} a layout row index identifying the area.
       	  * @param colIndex {Number} a layout column index identifying the area.
       	  */
        	 setAreaWidth : function( width, rowIndex, colIndex ){
        		 var target = true;
        		 if ( rowIndex != this.m_row || colIndex != this.m_col ){
        			 target = false;
        		 }
        		 else {
        			 this.setWidth( width );
        		 }
        		 return target;
        	 },
         	
        	 /**
        	  * Returns whether or not the height was set based on the location of this desktop
        	  * compared to the layout row and column index passed in.
        	  * @param height {Number} vertical height of the display area.
        	  * @param rowIndex {Number} a layout row index identifying the area.
        	  * @param colIndex {Number} a layout column index identifying the area.
        	  */
        	 setAreaHeight : function( height, rowIndex, colIndex ){
        		 var target = true;
        		 if ( rowIndex != this.m_row || colIndex != this.m_col ){
        			 target = false;
        		 }
        		 else {
        			 this.setHeight( height );
        		 }
        		 return target;
        	 },
         	
             
             /**
              * Removes this DisplayDesktop from the layout if this row and column index
              * match the passed in row and column index.  Returns whether or not this DisplayDesktop
              * was removed from the display.
              * @param row {Number} a row index in the layout.
              * @param col {Number} a col index in the layout.
              */
             excludeArea: function( row, col ){
            	 var excluded = false;
            	 if ( row == this.m_row && col == this.m_col ){
            		 this.exclude();
            		 excluded = true;
            	 }
            	 return excluded;
             },
             
             /**
              * Loads the data.
              * @param path {String} the location or lookup for the data.
              */
             dataLoaded: function( path ){
            	 if ( this.m_window != null ){
            		 this.m_window.dataLoaded( path );
            	 }
             },
             
             /**
              * Removes the data.
              * @param path {String} the location or identifier for the data.
              */
             dataUnloaded: function( path ){
            	 if ( this.m_window != null ){
            		 this.m_window.dataUnloaded( path );
            	 }
             },
             
            m_hub: null,
            m_window: null,
            m_row: null,
            m_col: null
        }
                
    });

