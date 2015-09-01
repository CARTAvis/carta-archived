/**
 * Displays controls for customizing the grid.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.ImageControls", {
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
                            grid : controls.grid
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
            console.log( "making grid controls");
            this.m_gridControls = new skel.widgets.Image.Grid.GridControls();
            this.m_tabView.add( this.m_gridControls );
            console.log( "making contour controls");
            this.m_contourControls = new skel.widgets.Image.Contour.ContourControls();
            this.m_tabView.add( this.m_contourControls );
            console.log( "Added contour");
        },
        
       
        
        /**
         * Register to get updates on grid settings from the server.
         */
        _registerControls : function(){
            /*this.m_sharedVar = this.m_connector.getSharedVar( this.m_id);
            this.m_sharedVar.addCB(this._controlsChangedCB.bind(this));
            this._controlsChangedCB();*/
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
            console.log( "Setting imageId="+imageId );
           this.m_gridControls.setId( imageId );
           this.m_contourControls.setId( imageId );
        },
        

        
        m_id : null,
        m_connector : null,
        m_sharedVar : null,
        
        m_tabView : null,
        
        m_gridControls : null,
        m_contourControls : null
    }
});