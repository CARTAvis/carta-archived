/**
 * Displays a file browser.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramCube", {
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
            this._initPlane();
        },
        
        _initPlane : function(){
            var planeGroup = new qx.ui.groupbox.GroupBox( "Plane");
            planeGroup.setLayout( new qx.ui.layout.VBox(2));
            
            var singleComposite = new qx.ui.container.Composite();
            singleComposite.setLayout(new qx.ui.layout.HBox(2));
            this.m_singlePlaneCheck = new qx.ui.form.CheckBox( "Single");
            singleComposite.add( this.m_singlePlaneCheck );
            this.m_singlePlaneText = new qx.ui.form.TextField();
            singleComposite.add( this.m_singlePlaneText );
            
            var rangeComposite = new qx.ui.container.Composite();
            rangeComposite.setLayout( new qx.ui.layout.HBox(2));
            this.m_rangeMinSpin = new qx.ui.form.Spinner();
            rangeComposite.add( this.m_rangeMinSpin );
            var rangeLabel = new qx.ui.basic.Label( "<-Range->");
            rangeComposite.add( rangeLabel );
            this.m_rangeMaxSpin = new qx.ui.form.Spinner();
            rangeComposite.add( this.m_rangeMaxSpin );
            
            planeGroup.add( singleComposite );
            planeGroup.add( rangeComposite );
            this._add( planeGroup );
        },
        
        m_singlePlaneCheck : null,
        m_singlePlaneText : null,
        m_rangeMinSpin : null,
        m_rangeMaxSpin : null
    }
});