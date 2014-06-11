/**
 * Created with IntelliJ IDEA.
 * User: pfederl
 * Date: 05/04/13
 * Time: 4:27 PM
 * To change this template use File | Settings | File Templates.
 */

/**

 @ignore(fv.assert)
 @ignore(fv.console.*)

 

 ************************************************************************ */


qx.Class.define("skel.widgets.DisplayMain",
    {
		extend: qx.ui.container.Composite,

        construct: function ( /*hub windowManager*/) {
        	this.base( arguments, new qx.ui.layout.Grow() );
            /*hub = null;
            if( hub == null)
                this.m_hub = null;
            else
                this.m_hub = hub;
            */
        	
            this.addListener( "appear", function(){
            	var bounds = this.getBounds();
    			this.m_height = bounds["height"];
    			this.m_width = bounds["width"];
    			this.layoutImageAnalysisAnimator();
            	
            	this.addListener("resize",function(){
            		//this._resizeContent();
            		var bounds = this.getBounds();
        			this.m_height = bounds["height"];
        			this.m_width = bounds["width"]; 
                	}, this);
            }, this );	     
        },
        
        events: {
            "iconifyWindow": "qx.event.type.Data",
            "addWindowMenu": "qx.event.type.Data"
        },

        members: {  
        	
        	/**
        	 * Restore the window at the given layout position to its original location.
        	 * @param row {Number} the row index of the window to be restored.
        	 * @param col {Number} the column index of the window to be restored.
        	 */
        	restoreWindow : function( row, col ){
        		this.m_pane.restoreWindow( row, col );
        	},
        	
        	/**
        	 * Display a single image on the screen.
        	 */
        	layoutImage : function(){
        		this.layout( 1, 1 );
        		this.m_pane.setView( "image", 0, 0 );
        		this.m_pane.excludeArea( 1, 0 );
        	},
        	
        	/**
        	 * Layout the display area using a grid containing a large image area, two
        	 * (analysis) plugin windows, and an animation area.
        	 */
        	layoutImageAnalysisAnimator : function(){
        		this.layout( 3, 2 );
        		
    			var imagePercent = .6;
    			this.m_pane.setView( "image", 0, 0 );
    			this.m_pane.setAreaWidth( Math.floor( this.m_width * imagePercent), 0, 0);
    			this.m_pane.setAreaHeight( this.m_height, 0, 0 );
    		
    			this.m_pane.excludeArea( 1, 0 );
    			this.m_pane.excludeArea( 2, 0 );
    			
    			this.m_pane.setView( "profile", 0, 1);
    			this.m_pane.setAreaWidth( Math.floor( this.m_width * (1-imagePercent)), 0, 1);
    		
    			this.m_pane.setView( "statistics", 1, 1);
    			this.m_pane.setAreaWidth( Math.floor( this.m_width * (1-imagePercent)), 1, 1);
    		
    			this.m_pane.setView( "animator", 2, 1);
    			this.m_pane.setAreaWidth( Math.floor( this.m_width * (1-imagePercent)), 2, 1);
        	},
        	
        	/**
        	 * Layout the screen real estate using a square grid with the indicated number of 
        	 * rows and columns.
        	 * @param rows {Number} the number of rows in the grid.
        	 * @param cols {Number} the number of columns in the grid.
        	 */
            layout : function( rows, cols ){
        		if ( rows >= 1 && cols >= 1 ){
        			this._removeWindows();
        			var splitterSize = 10;
        			var splitterHeight = this.m_height - (rows-1)*splitterSize;
        			var splitterWidth = this.m_width - (cols-1)*splitterSize;
        			this.m_pane = new skel.widgets.DisplayArea(rows, cols, splitterHeight, splitterWidth, 0, 0, cols-1 );
        			this.m_pane.addListener( "iconifyWindow", function(ev){
        				this.fireDataEvent( "iconifyWindow", ev.getData()); 
        			}, this);
        			this.m_pane.addListener( "windowSelected", function(ev){
        				var selectedWindow = ev.getData();
        				this.m_pane.windowSelected( selectedWindow);
        				var menuButtons = selectedWindow.getWindowMenu();
        				this.fireDataEvent( "addWindowMenu", menuButtons );
        			}, this);
        				
        	
        			var displayArea = this.m_pane.getDisplayArea();
        			
        			this.add( displayArea );
        		}
        	},
        	
        	/**
        	 *  Assign plugins to a generic window display.
        	 */
        	initializeDisplays : function(){
        		
        		for ( var row = 0; row < this.m_gridRowCount; row++ ){
        			for ( var col = 0; col < this.m_gridColCount; col++ ){
        			
        				//Last column will be controls
        				if ( col > 0 && col == this.m_gridColCount - 1 ){
        					var controlIndex = row % 3;
        					if ( controlIndex == 0 ){
        						this.m_pane.setView( "profile", row, col);
        					}
        					else if ( controlIndex == 1){
        						this.m_pane.setView( "statistics", row, col );
        					}
        					else {
        						this.m_pane.setView( "histogram", row, col );
        					}
        				}
        			
        				//Last row will be animators with the exception of last col.
        				else if ( row > 0 && row == this.m_gridRowCount - 1 && col != this.m_gridColCount - 1){
        					this.m_pane.setView( "animator", row, col);
        				}
        				else {
        					this.m_pane.setView( "image", row, col );
        				}
        			}
        		}
        	},
        	
        	/**
        	 * Update the number of rows in the current layout.
        	 * @param gridRows {Number} the number of rows in the layout.
        	 */
        	setRowCount : function( gridRows ){
        		this.m_gridRowCount = gridRows;
        		this.layout( this.m_gridRowCount, this.m_gridColCount );
        		this.initializeDisplays();
        	},
        	
        	/**
        	 * Update the number of columns in the current layout.
        	 * @param gridCols {Number} the number of columns in the layout.
        	 */
        	setColCount : function( gridCols ){
        		this.m_gridColCount = gridCols;
        		this.layout( this.m_gridRowCount, this.m_gridColCount );
        		this.initializeDisplays();
        	},
        	
        	/**
        	 * Load the data identified by the path.
        	 * @param path {String} an identifier for data to be displayed.
        	 */
        	dataLoaded: function( path ){
           	 if ( this.m_pane != null ){
           		 this.m_pane.dataLoaded( path );
           	 }
            },
            
            /**
             * Unload the data identified by the path.
             * @param path {String} an identifier for data to be removed.
             */
            dataUnloaded: function( path ){
           	 if ( this.m_pane != null ){
           		 this.m_pane.dataUnloaded( path );
           	 }
            },
            
            _removeWindows : function(){
        		if ( this.m_pane != null ){
        			this.m_pane.removeWindows();
        			this.removeAll();
        		}   	
            },
        
        	m_pane : null,
        	m_height : 0,
        	m_width  : 0,
        	m_gridRowCount : 1,
        	m_gridColCount : 1
        },
        
        properties: {
            appearance: {
                refine: true,
                init: "display-main"
            }
        }

    });

