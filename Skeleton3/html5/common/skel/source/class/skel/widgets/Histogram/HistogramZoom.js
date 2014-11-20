/**
 * Displays a file browser.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramZoom", {
    extend : qx.ui.core.Widget,

    construct : function( ) {
        this.base(arguments);
        this._init( );
    },

    members : {


        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);

            var zoomGroup = new qx.ui.groupbox.GroupBox( "Zoom Range");
            zoomGroup.setLayout( new qx.ui.layout.VBox(2));
            
            this._add( zoomGroup );
            this.m_fullRange = new qx.ui.form.Button( "Full" );
            this.m_selectedRange = new qx.ui.form.Button( "Selected");
            zoomGroup.add( this.m_fullRange );
            zoomGroup.add( this.m_selectedRange );
        },
        
        m_fullRange : null,
        m_selectedRange : null
    }
});