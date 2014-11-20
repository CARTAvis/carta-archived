/**
 * Displays a file browser.
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
            //This will contain nothing for a logarithmic scale or a grid for setting
            //gamma parameters.
            this.setWidth( 50 );
            this.setHeight( 50 );
            this.m_gammaGrid = new skel.widgets.Colormap.TwoDSlider();
            this._add( this.m_gammaGrid );
        },
        
        
        m_gammaGrid : null

    }
});