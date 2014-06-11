/**
 * A Composite containing a split pane that manages two child areas of screen
 * real estate space.
 */

/**

 @ignore(fv.assert)
 @ignore(fv.console.*)

 

 ************************************************************************ */


qx.Class.define("skel.widgets.DisplayArea",
    {
        extend: qx.core.Object,
	
        /**
         * Constructor; recursively builds up the screen.
         * @param rows {Number} the number of layout rows still being constructed.
         * @param cols {Number} the number of layout column still being constructed.
         * @param height {Number} the number of pixels still available in the vertical direction.
         * @param width {Number} the number of pixels still available in the horizontal direction.
         * @param rowIndex {Number} the smallest row index available.
         * @param colIndex {Number} the smallest column index available.
         * @param lastColIndex {Number} index of the last column in the layout.
         * 
         */
        construct: function ( rows, cols, height, width, rowIndex, colIndex, lastColIndex ) {
            this.base(arguments);
            var rowHeight = Math.floor(height / rows);
            var colWidth = Math.floor( width / cols );
            
            //No more horizontal splits so make this one a vertical one.
            if ( cols == 1 ){
            	this.m_pane = new qx.ui.splitpane.Pane("vertical").set({
                    allowGrowX : true,
                    allowGrowY : true
                  });
            	
            	 // Create container with fixed dimensions for the top:   
            	this.m_areaFirst = this._makeArea( colWidth, rowHeight, rowIndex, colIndex );
           
            	//We don't need another split pane so just add another container.
            	if ( rows <=2 ){
            		this.m_areaSecond = this._makeArea( colWidth, rowHeight, rowIndex+1, colIndex );
            	}
            	//Make a child node with one less row and add it.
            	else if ( rows > 2){
            		//this.m_childSecond = this._makeChild( rows - 1, cols, rowHeight, colWidth );
            		this.m_areaSecond = this._makeChild( rows - 1, cols, rowHeight, colWidth, rowIndex + 1, colIndex, lastColIndex );
            	}
            	
            }
            //Make another horizontal split.
            else {
            	this.m_pane = new qx.ui.splitpane.Pane("horizontal").set({
                    allowGrowX : true,
                    allowGrowY : true
                  });
            	//We can handle it with a single split pane.
            	if ( rows == 1 ){
            		 // Create container with fixed dimensions for the top:
            		this.m_areaFirst = this._makeArea( colWidth, rowHeight, rowIndex, colIndex );                   
                    this.m_areaSecond = this._makeArea( colWidth, rowHeight, rowIndex, colIndex + 1 );
            	}
            	//Each of our sides needs to be a split pane.
            	else {
            		this.m_areaFirst = this._makeChild( rows, 1, rowHeight, colWidth, rowIndex, colIndex );	
            		this.m_areaSecond = this._makeChild( rows, cols - 1, rowHeight, colWidth, rowIndex, colIndex+1, lastColIndex );
            		
            	}
            }
        },
        
        events: {
            "iconifyWindow": "qx.event.type.Data",
            "windowSelected": "qx.event.type.Data"
        },
        
        members: {
        	/**
        	 * Returns the split pane.
        	 */
        	 getDisplayArea: function(){
             	return this.m_pane;
             },	
             
             /**
              * Notifies children that the given window was selected.
              * @param win {skel.widgets.DisplayWindow} the selected window. 
              */
             windowSelected: function( win ){
            	 this.m_areaFirst.windowSelected( win );
            	 this.m_areaSecond.windowSelected( win );
             },
             
             /**
              * Returns whether or not the window with the given id was
              * restored.
              * @param row {Number} the layout row of the window to be restored.
              * @param col {Number} the layout column of the window to be restored.
              */
             restoreWindow : function( row, col ){
            	 var restored = this.m_areaFirst.restoreWindow( row, col );
            	 if ( !restored ){
            		 restored = this.m_areaSecond.restoreWindow( row, col );
            	 }
            	 return restored;
             },
             
             /**
              * Remove all windows.
              */
             removeWindows : function(){
            	 this.m_areaFirst.removeWindows();
            	 this.m_areaSecond.removeWindows();
             },
                          
          
             /**
              * Returns whether or not a desktop at the given row and column of the layout
              * was successfully removed; used for creating asymmetrical layouts.
              * @param row {Number} an index of a row in the grid.
              * @param col {Number} an index of a column in the grid.
              */
             excludeArea: function( row, col ){
            	 var excluded = this.m_areaFirst.excludeArea( row, col );
            	 if ( !excluded ){
            		 excluded = this.m_areaSecond.excludeArea( row, col );
            	 }
            	 return excluded;
             },
                          
             /**
              * Constructs a child leaf node of this composite.
              */
             _makeArea: function( colWidth, rowHeight, rowIndex, colIndex ){
            	 var area = new skel.widgets.DisplayDesktop(rowIndex,colIndex).set({
                     width : colWidth,
                     height: rowHeight,
                     decorator : "main"
                   });
            	 area.addListener( "iconifyWindow", function(ev){
            		var data = ev.getData();
            		area.exclude();
            		this.fireDataEvent( "iconifyWindow",data); 	
            	 }, this);
            	 
            	 area.addListener( "windowSelected", function(ev){
            		 this.fireDataEvent( "windowSelected", ev.getData());
            	 }, this );
            	 this.m_pane.add( area, 1);
            	 return area;
             },
             
            
             /**
              * Constructs a composite child node of this composite.
              */
             _makeChild: function( rows, cols, rowHeight, colWidth, rowIndex, colIndex, lastColIndex ){
            	 var child  = new skel.widgets.DisplayArea( rows, cols, rows * rowHeight, cols * colWidth, rowIndex, colIndex ); 
            	 child.addListener( "iconifyWindow", function(ev){
            		 this.fireDataEvent( "iconifyWindow", ev.getData());
            	 }, this );
            	 child.addListener( "windowSelected", function(ev){
            		 this.fireDataEvent( "windowSelected", ev.getData());
            	 }, this );

            	 var flex = 0;
            	 if ( lastColIndex == colIndex ){
            		 flex = 1;
            	 }
            	 this.m_pane.add( child.getDisplayArea(), flex);
            	 return child;
             },
             
             /**
              * Notifies children that data has been loaded.
              */
             dataLoaded: function( path ){
            	 if ( this.m_areaFirst != null ){
            		 this.m_areaFirst.dataLoaded( path );
            	 }
            	 if ( this.m_areaSecond != null ){
            		 this.m_areaSecond.dataLoaded( path );
            	 }
             },
             
             /**
              * Notifies children that data has been unloaded.
              */
             dataUnloaded: function( path ){
            	 if ( this.m_areaFirst != null ){
            		 this.m_areaFirst.dataUnloaded( path );
            	 }
            	 if ( this.m_areaSecond != null ){
            		 this.m_areaSecond.dataUnloaded( path );
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
         		 var pluginAssigned = false;
         		 if ( this.m_areaFirst != null ){
            		 pluginAssigned = this.m_areaFirst.setView( pluginId, rowIndex, colIndex );
            	 }
         		 if ( !pluginAssigned ){
         			 if ( this.m_areaSecond != null ){
         				 pluginAssigned = this.m_areaSecond.setView( pluginId, rowIndex, colIndex );
         			 }
         		 }
         		 return pluginAssigned;
         	 },
         	 
         	setAreaHeight : function( height, rowIndex, colIndex ){
         		var heightSet = false;
        		 if ( this.m_areaFirst != null ){
        			 heightSet = this.m_areaFirst.setAreaHeight( height, rowIndex, colIndex );
        		 }
        		 if ( !heightSet ){
        			 if ( this.m_areaSecond != null ){
        				 heightSet = this.m_areaSecond.setAreaHeight( height, rowIndex, colIndex );
        			 }
        		 }
        		 return heightSet;
         	},
         	 
         	setAreaWidth : function( width, rowIndex, colIndex ){
         		var widthSet = false;
         		if ( this.m_areaFirst != null ){
         			widthSet = this.m_areaFirst.setAreaWidth( width, rowIndex, colIndex );
         		}
         		if ( !widthSet ){
         			if ( this.m_areaSecond != null ){
         				widthSet = this.m_areaSecond.setAreaWidth( width, rowIndex, colIndex );
         			}
         		}
         		return widthSet;
         	},
         	m_pane: null,
         	m_areaFirst: null,
         	m_areaSecond: null
        }

    });

