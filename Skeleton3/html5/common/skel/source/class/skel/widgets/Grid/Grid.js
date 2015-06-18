/**
 * Displays controls for customizing the grid.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Grid.Grid", {
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
         * Initializes the UI.
         */
        _init : function( ) {
            this._setLayout(new qx.ui.layout.VBox(2));
           
            this.m_content = new qx.ui.groupbox.GroupBox( "Grid");
            this.m_content.setContentPadding( 0, 0, 0, 0 );
            this._add( this.m_content );
            
            this.m_content.setLayout(new qx.ui.layout.HBox(2));
            this.m_axes = new skel.widgets.Grid.GridAxes();
            this.m_content.add( this.m_axes );
        },
        
        /**
         * Set the grid controls based on an update from the server.
         * @param grid {Object} - the server side grid controls.
         */
        setControls : function( grid ){
            this.m_axes.setControls( grid );
            
        },
        
        /**
         * Set the server side id of this control UI.
         * @param gridId {String} the server side id of the object that contains data for this control UI.
         */
        setId : function( gridId ){
            this.m_id = gridId;
            this.m_axes.setId( gridId );
        },
        
        m_content : null,
        m_id : null,
        m_connector : null,
        m_axes : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }


});