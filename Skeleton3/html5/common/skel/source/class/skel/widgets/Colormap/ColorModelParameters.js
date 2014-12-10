/**
 * Controls that allow the user to set color model parameters.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColorModelParameters", {
    extend : qx.ui.core.Widget,

    construct : function(  ) {
        this.base(arguments);
        this._init( );
    },

    members : {


        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
            
            var scaleGroup = new qx.ui.groupbox.GroupBox( "Parameters");
            scaleGroup.setLayout( new qx.ui.layout.VBox(2));
            this._add(scaleGroup);
            
            //This will contain nothing for a logarithmic scale or a grid for setting
            //gamma parameters.
            this.setWidth( 100 );
            this.setHeight( 100 );
            this.m_gammaGrid = new skel.widgets.Colormap.TwoDSlider();
            scaleGroup.add( this.m_gammaGrid );
        },
        
        /**
         * Set the server side id of the color map.
         * @param id {String} the unique server side id of this color map.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        m_id : null,
        m_gammaGrid : null

    }
});