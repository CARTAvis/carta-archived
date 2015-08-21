/**
 * Displays controls for customizing the grid.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.GridControls", { 
    extend : qx.ui.tabview.Page,
    
    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments, "Grid", "");
        this.m_connector = mImport("connector");
        this._init();
    },
    
    events : {
        "gridControlsChanged" : "qx.event.type.Data"
    },

    members : {
        
        /**
         * Callback for a change in grid control preference settings.
         */
        _controlsChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var controls = JSON.parse( val );
                    if ( this.m_canvas !== null ){
                        this.m_canvas.setControls(controls);
                    }
                    if ( this.m_grid !== null ){
                        this.m_grid.setControls( controls);
                    }
                    if ( this.m_axes !== null ){
                        this.m_axes.setControls( controls);
                    }
                    if ( this.m_labels !== null ){
                        this.m_labels.setControls( controls );
                    }
                    
                    if ( this.m_ticks !== null ){
                        this.m_ticks.setControls( controls );
                    }
                    var data = {
                            grid : controls
                    };
                    this.fireDataEvent( "gridControlsChanged", data );
                    var errorMan = skel.widgets.ErrorHandler.getInstance();
                    errorMan.clearErrors();

                }
                catch( err ){
                    console.log( "Grid controls could not parse: "+val+" error: "+err );
                }
            }
        },
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );

            this._setLayout( new qx.ui.layout.VBox(1));

            this.m_tabView = new qx.ui.tabview.TabView();
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this._add( this.m_tabView );
            this.m_canvas = new skel.widgets.Image.Grid.GridPage.GridPageCanvas();
            this.m_grid = new skel.widgets.Image.Grid.GridPage.GridPageGrid();
            this.m_axes = new skel.widgets.Image.Grid.GridPage.GridPageAxes();
            this.m_labels = new skel.widgets.Image.Grid.GridPage.GridPageLabel();
            this.m_ticks = new skel.widgets.Image.Grid.GridPage.GridPageTicks();
            this.m_tabView.add( this.m_canvas );
            this.m_tabView.add( this.m_grid );
            this.m_tabView.add( this.m_axes );
            this.m_tabView.add( this.m_labels );
            this.m_tabView.add( this.m_ticks );
        },
        
       
        
        /**
         * Register to get updates on grid settings from the server.
         */
        _registerControls : function(){
            this.m_sharedVar = this.m_connector.getSharedVar( this.m_id);
            this.m_sharedVar.addCB(this._controlsChangedCB.bind(this));
            this._controlsChangedCB();
        },
        
        /**
         * Callback for when the registration is complete and an id is available.
         * @param anObject {skel.widgets.Grid.GridControls}.
         */
        _registrationCallback : function( anObject ){
            return function( id ){
                anObject._setGridId( id );
            };
        },
        
        /**
         * Send a command to the server to get the grid control id.
         * @param imageId {String} the server side id of the image object.
         */
        setId : function( imageId ){
            var path = skel.widgets.Path.getInstance();
            var cmd = imageId + path.SEP_COMMAND + "registerGridControls";
            var params = "";
            this.m_connector.sendCommand( cmd, params, this._registrationCallback( this));
        },
        
        /**
         * Store the grid control id.
         * @param gridId {String} a server-side identifier for the grid controls.
         */
        _setGridId : function( gridId ){
            if ( gridId !== null && gridId.length > 0 ){
                this.m_id = gridId;
                this.m_canvas.setId( gridId );
                this.m_grid.setId( gridId );
                this.m_axes.setId( gridId );
                this.m_labels.setId( gridId );
                this.m_ticks.setId( gridId );
                this._registerControls();
            }
        },
        
        m_id : null,
        m_connector : null,
        m_sharedVar : null,
        
        m_tabView : null,
        
        m_canvas : null,
        m_grid : null,
        m_axes : null,
        m_labels : null,
        m_ticks : null
    }
});