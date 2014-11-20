/**
 * Displays a file browser.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramBin", {
    extend : qx.ui.core.Widget,

    construct : function( title ) {
        this.base(arguments);
        this._init( title );
    },

    members : {


        /**
         * Initializes the UI.
         */
        _init : function( title ) {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
            
            var binComposite = new qx.ui.container.Composite();
            binComposite.setLayout(new qx.ui.layout.HBox(2));
            var binCountLabel = new qx.ui.basic.Label( "Bin Count: ");
            this.m_binCountText = new qx.ui.form.TextField();
            binComposite.add( binCountLabel );
            binComposite.add( this.m_binCountText );
            this._add( binComposite );
            
            this.m_binCountSlider = new qx.ui.form.Slider();
            this.m_binCountSlider.setMinimum( 0 );
            this.m_binCountSlider.setMaximum( 100 );
            this.m_binCountSlider.setValue( 25 );
            this._add( this.m_binCountSlider );
        },
    
        m_binCountText : null,
        m_binCountSlider : null
        
        

    }
});