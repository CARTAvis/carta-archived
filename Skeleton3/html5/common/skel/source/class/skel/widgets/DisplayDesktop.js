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
	construct: function ( row, col) {
		this.base( arguments);

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
		 * Returns an empty list as windows cannot be added in the middle of a desktop.
		 */
		getAddWindowLocations: function(){
			return [];
		},

		/**
		 * Returns this.
		 */
		getDisplayArea : function(){
			return this;
		},

		/**
		 * Factory method for making window specialized to correct type.
		 * @param pluginId {String} an identifier for the type of plugin the window will manage.
		 */
		_makeWindow : function( pluginId ){
			if ( pluginId == "casaLoader"){
				this.m_window = new skel.widgets.DisplayWindowImage(this.m_row, this.m_col);
			}
			else if ( pluginId == "animator"){
				this.m_window = new skel.widgets.DisplayWindowAnimation(this.m_row, this.m_col);
			}
			else {
				this.m_window = new skel.widgets.DisplayWindowGenericPlugin(this.m_row, this.m_col, pluginId);       			
			} 
		},

		/**
		 * Returns the identifier for the window this desktop manages if the passed in location
		 * matches the location of its window; otherwise, returns an empty string.
		 * @param sourceRow {Number} a row location on the screen.
		 * @param sourceCol {Number} a column location on the screen.
		 */
		getWinId : function (sourceRow, sourceCol ){
			var winId = "";
			if ( this.m_row == sourceRow && this.m_col == sourceCol ){
				if ( this.m_window != null ){
					winId = this.m_window.getIdentifier();
				}
			}
			return winId;
		},

		/**
		 * Links this display to the one identified by the link parameter provided the
		 * passed in location matches the location of its managed window.
		 * @param row {Number} a row location on the screen.
		 * @param col {Number} a column location on the screen.
		 * @param link {String} an identifier for another window.
		 */
		setLinkId : function( row, col, link ){
			var linkSet = false;
			if ( this.m_row == row && this.m_col == col ){
				if ( this.m_window != null ){
					linkSet = this.m_window.setLinkId( link );
				}
			}
			return linkSet;
		},

		/**
		 * Returns the row and column of its managed window.
		 */
		_getLastIndices : function(){
			var indices = [this.m_row, this.m_col];
			return indices;
		},

		/**
		 * Returns the width and height.
		 */
		_getDimensions : function(){
			var dims = [this.getWidth(), this.getHeight()];
			return dims;
		},

		/**
		 * Sets the width and height.
		 * @param width {Number} the new width.
		 * @param height {Number} the new height.
		 * @param decreaseWidth {Boolean} - currently ignored.
		 * @param decreaseHeight {Boolean} - currently ignored.
		 */
		_setDimensions : function( width, height, decreaseWidth, decreaseHeight ){
			this.setWidth( width );
			this.setHeight( height );
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


		 m_window: null,
		 m_row: null,
		 m_col: null
	}

		});

