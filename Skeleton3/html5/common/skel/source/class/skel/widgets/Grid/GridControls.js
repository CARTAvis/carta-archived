/**
 * Displays controls for customizing the grid.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Grid.GridControls", {
    extend : qx.ui.core.Widget, 

    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init();
    },
    

    members : {
        
        /**
         * Callback for a change in histogram preference settings.
         */
        _controlsChangedCB : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var controls = JSON.parse( val );
                    if ( this.m_grid !== null ){
                        this.m_grid.setControls( controls.grid );
                    }
                    if ( this.m_gridFont !== null ){
                        this.m_gridFont.setControls( controls.grid.font );
                    }
                    if ( this.m_gridColor !== null ){
                        this.m_gridColor.setControls( controls.grid);
                    }
                    
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
            this._setLayout(new qx.ui.layout.Grow());
            this.m_content = new qx.ui.container.Composite();
            this._add( this.m_content );
            this.m_content.setLayout(new qx.ui.layout.VBox());
            this.m_grid = new skel.widgets.Grid.Grid();
            this.m_gridFont = new skel.widgets.Grid.GridFont();
            this.m_gridColor = new skel.widgets.Grid.GridColor();
        },
        
        /**
         * Returns true if there are grid controls being displayed; false otherwise.
         * @return {boolean} true if there are grid controls visible; false otherwise.
         */
        isContent : function(){
            var childCount = this.m_content.getChildren().length;
            var content = false;
            if ( childCount > 0 ){
                content = true;
            }
            return content;
        },
        
        
        /**
         * Add/remove color map settings based on user preferences.
         * @param widget {qx.ui.form.Widget} - the widget to add or remove.
         * @param visible {boolean} - true if the widget should be added; false otherwise.
         */
        _layoutControls : function( widget, visible ){
            if ( visible ){
                //Add the widget if it is not already there.
                if ( this.m_content.indexOf( widget ) < 0 ){
                    this.m_content.add( widget );
                }
            }
            else {
                //Remove the widget from the container.
                if ( this.m_content.indexOf( widget ) >= 0 ){
                    this.m_content.remove( widget );
                }
            }
        },
        
        /**
         * Register to get updates on histogram settings from the server.
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
                this.m_grid.setId( gridId );
                this.m_gridFont.setId( gridId );
                this.m_gridColor.setId( gridId );
                this._registerControls();
            }
        },
       
        /**
         * Show/hide the grid controls.
         * @param visible {boolean} - true if the grid controls should be visible; false otherwise.
         */
        showHideGrid : function( visible ){
            this._layoutControls( this.m_grid, visible );
        },
        
        /**
         * Show/hide the grid color controls.
         * @param visible {boolean} - true if the grid color controls should be visible; false otherwise.
         */
        showHideGridColor : function( visible ){
            this._layoutControls( this.m_gridColor, visible );
        },
        
        /**
         * Show/hide the grid font controls.
         * @param visible {boolean} - true if the grid font controls should be visible; false otherwise.
         */
        showHideGridFont : function( visible ){
            this._layoutControls( this.m_gridFont, visible );
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        m_sharedVar : null,
        
        m_grid : null,
        m_gridColor : null,
        m_gridFont : null
    }
});