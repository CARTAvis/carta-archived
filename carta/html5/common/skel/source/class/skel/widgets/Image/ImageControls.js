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
        this._init();
    },
    
    events : {
        "gridControlsChanged" : "qx.event.type.Data"
    },
   
    members : {
        

        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            this.setPadding( 0, 0, 0, 0 );

            this._setLayout( new qx.ui.layout.VBox(1));
            this.m_tabView = new qx.ui.tabview.TabView();
            this.m_tabView.setContentPadding( 2, 2, 2, 2 );
            this._add( this.m_tabView );
            
            this.m_gridControls = new skel.widgets.Image.Grid.GridControls();
            this.m_gridControls.addListener( "gridControlsChanged", function(ev){
                this.fireDataEvent( "gridControlsChanged", ev.getData() );
            }, this );
            this.m_tabView.add( this.m_gridControls );
            
            this.m_contourControls = new skel.widgets.Image.Contour.ContourControls();
            this.m_tabView.add( this.m_contourControls );
        },
        
        
        /**
         * Send a command to the server to get the grid control id.
         * @param imageId {String} the server side id of the image object.
         */
        setId : function( imageId ){
           this.m_gridControls.setId( imageId );
           this.m_contourControls.setId( imageId );
        },
        
        m_id : null,
        
        m_tabView : null,
        
        m_gridControls : null,
        m_contourControls : null
    }
});