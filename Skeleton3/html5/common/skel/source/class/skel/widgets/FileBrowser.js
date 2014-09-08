/**
 * Displays a file browser.
 */
/*global mImport */
/**
 @ignore( mImport)
 ************************************************************************ */

qx.Class.define("skel.widgets.FileBrowser",
 {
	extend: qx.ui.core.Widget,
	
	construct : function(){
		this.base(arguments);
			
		this._init();
		
		//file system shared variable
		var connector = mImport( "connector");
		var path = skel.widgets.Path.getInstance();
		var filePath = path.FILE_SYSTEM;
        this.m_files = connector.getSharedVar( filePath );
        this.m_files.addCB( this._updateTree.bind(this) );
		
		var paramMap = "";
	    connector.sendCommand( "getData", paramMap, function(){} );	
	},
	
	members:{
		
		/**
		 * Initializes the UI.
		 */
		_init : function(){
			var widgetLayout = new qx.ui.layout.VBox(2);
			this._setLayout( widgetLayout );
			
			this.m_tree = new qx.ui.tree.Tree();
			this.m_tree.setWidth( 300 );
			this.m_tree.setHeight( 300 );
						
			var closeButton = new qx.ui.form.Button( "Close");
			closeButton.addListener( "execute", function(){
				qx.event.message.Bus.dispatch( 
					new qx.event.message.Message("closeFileBrowser", ""));
			});
			var loadButton = new qx.ui.form.Button( "Load");
			loadButton.addListener( "execute", function(){
				var selectArray = this.m_tree.getSelection();
				var pathDict = skel.widgets.Path.getInstance();
				for ( var i = 0; i < selectArray.length; i++ ){
					var selectedItem = selectArray[i];
					var target = this.m_tree.getRoot();
					var path = [];
					while ( selectedItem != target ){
						var pathPart = selectedItem.getLabel();
						path.push( pathPart );
						selectedItem = selectedItem.getParent();
					}
					var filePath = pathDict.SEP + target.getLabel();
					for ( var j = path.length-1; j>=0; j-- ){
						filePath = filePath + pathDict.SEP + path[j];
					}
				}
				if ( this.m_target != null ){
					this.m_target.dataLoaded( filePath );
				}
				
			}, this );
			var buttonComposite = new qx.ui.container.Composite();
			var buttonLayout = new qx.ui.layout.HBox(2);
			buttonComposite.setLayout( buttonLayout );
			buttonComposite.add( new qx.ui.core.Spacer(), {flex:1});
			buttonComposite.add( loadButton );
			buttonComposite.add( closeButton );
			
			this._add( this.m_tree );
			this._add( buttonComposite );
		},
		
		/**
		 * Stores the window that wants to add data.
		 */
		setTarget : function( source ){
			this.m_target = source;
		},
		
		/**
		 * Constructs a file tree from a hierarchical JSON string.
		 * @param dataTree {String} representing available data files in a hierarchical JSON format
		 */
		_updateTree : function( dataTree ){
			var jsonObj = qx.lang.Json.parse( dataTree );
			var jsonModel = qx.data.marshal.Json.createModel( jsonObj );
			this.m_controller = new qx.data.controller.Tree( jsonModel, this.m_tree, "dir", "name");
		    this.m_tree.getRoot().setOpen( true);
		},
		
		m_tree : null,
		m_controller : null,
		m_files : null,
		m_target : null
    
	},
	properties: {
       appearance: {
           refine: true,
           init: "popup-dialog"
       }

	}

});