/**
 * Displays a file browser.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.Histogram2D", {
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
            this._initFootPrint();
        },
        
        _initFootPrint : function(){
            var footPrintGroup = new qx.ui.groupbox.GroupBox( "2D Foot Print");
            footPrintGroup.setLayout( new qx.ui.layout.VBox(2));
            
            this.m_imageRadio = new qx.ui.form.RadioButton( "Image");
            this.m_regionRadio = new qx.ui.form.RadioButton( "Selected Region");
            this.m_regionAllRadio = new qx.ui.form.RadioButton( "All Regions");
            
            var footPrintRadio = new qx.ui.form.RadioGroup();
            footPrintRadio.add( this.m_imageRadio, this.m_regionRadio, this.m_regionAllRadio );
            
            footPrintGroup.add( this.m_imageRadio );
            footPrintGroup.add( this.m_regionRadio );
            footPrintGroup.add( this.m_regionAllRadio );
            this._add( footPrintGroup );
        },
        
        m_imageRadio : null,
        m_regionRadio : null,
        m_regionAllRadio : null
    }
});