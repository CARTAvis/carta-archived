/**
 * Manages a picture of a grid surrounded with widgets that allow the grid label
 * format to be selected on each side.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Image.Grid.Settings.GridLabels", {
    extend : qx.ui.core.Widget, 

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this._init( );
    },
    
    members : {
        /**
         * Update the UI based on grid label formats that have been set on
         * the server.
         * @param formats {Object}- the server-side grid label formats.
         */
        setControls : function( formats ){
            this.m_labelNorth.setFormatValue( formats.top );
            this.m_labelSouth.setFormatValue( formats.bottom );
            this.m_labelEast.setFormatValue( formats.left );
            this.m_labelWest.setFormatValue( formats.right );
        },
        
        /**
         * Initialize the UI.
         */
        _init : function( ){
            this._setLayout( new qx.ui.layout.VBox() );
            var content = new qx.ui.container.Composite();
            var gridLayout = new qx.ui.layout.Grid();
            gridLayout.setColumnAlign( 0, "center", "middle");
            gridLayout.setColumnAlign( 1, "center", "middle");
            gridLayout.setColumnAlign( 2, "center", "middle");
            gridLayout.setRowAlign( 1, "center", "middle");
            gridLayout.setSpacing(2);
            gridLayout.setRowFlex( 1,10 );
            content.setLayout( gridLayout );
            this._add( content );
            
            this.m_centerGrid = new skel.widgets.Image.Grid.Settings.DiagonalLineCanvas();
            this.m_labelEast = new skel.widgets.Image.Grid.Settings.GridLabelFormat( "left",1 );
            this.m_labelWest = new skel.widgets.Image.Grid.Settings.GridLabelFormat( "right", 1 );
            this.m_labelNorth = new skel.widgets.Image.Grid.Settings.GridLabelFormat( "top", 0 );
            this.m_labelSouth = new skel.widgets.Image.Grid.Settings.GridLabelFormat( "bottom", 0 );
            
            content.add( this.m_labelNorth, {row:0,column:1});
            content.add( this.m_labelSouth, {row:2,column:1});
            content.add( this.m_labelEast, {row:1, column:0});
            content.add( this.m_centerGrid, {row:1, column:1});
            content.add( this.m_labelWest, {row:1,column:2});
        },
        
        
        /**
         * Set the server side id.
         * @param id {String} the server side id of the object.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_labelEast.setId( id );
            this.m_labelNorth.setId( id );
            this.m_labelSouth.setId( id );
            this.m_labelWest.setId( id );
        },
        
        m_id : null,
        m_centerGrid : null,
        m_labelEast : null,
        m_labelWest : null,
        m_labelNorth : null,
        m_labelSouth : null
    }
});