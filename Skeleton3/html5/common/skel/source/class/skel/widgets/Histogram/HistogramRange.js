/**
 * Displays a file browser.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramRange", {
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
            console.log( "_init HistogramRange");
            
            var clipGroup = new qx.ui.groupbox.GroupBox( "Clip (Jy/beam)");
            this._add( clipGroup );
            clipGroup.setLayout( new qx.ui.layout.VBox(2));
            console.log("Finished clip");
            //Minimum
            var minComposite = new qx.ui.container.Composite();
            minComposite.setLayout(new qx.ui.layout.HBox(2));
            console.log( "Made min composite");
            //var unitLabel = new qx.ui.basic.Label( "Jy/beam");
            var minLabel = new qx.ui.basic.Label( "Min:");
            console.log( "made labels");
            this.m_minClipText = new qx.ui.form.TextField();
            console.log( "made minClip");
            minComposite.add( minLabel );
            console.log( "Added minLabel");
            minComposite.add( this.m_minClipText );
            //minComposite.add( unitLabel );
            console.log( "_init minimum");
            
            //Maximum
            var maxComposite = new qx.ui.container.Composite();
            maxComposite.setLayout(new qx.ui.layout.HBox(2));
            //var unitLabel = new qx.ui.basic.Label( "Jy/beam");
            var maxLabel = new qx.ui.basic.Label( "Max:");
            this.m_maxClipText = new qx.ui.form.TextField();
            maxComposite.add( maxLabel );
            maxComposite.add( this.m_maxClipText );
            //maxComposite.add( unitLabel );
            console.log( "_initMaximum");
            
            //Percentile
            var percentComposite = new qx.ui.container.Composite();
            percentComposite.setLayout( new qx.ui.layout.HBox(2) );
            var percentLabel = new qx.ui.basic.Label( "Percent:" );
            this.m_percentCombo = new qx.ui.form.ComboBox();
            percentComposite.add( percentLabel );
            percentComposite.add( this.m_percentCombo );
            console.log( "initPercent");
            
            clipGroup.add( minComposite );
            clipGroup.add( maxComposite );
            this._add( percentComposite );
            console.log( "Added percentage");
        },
        
        m_maxClipText : null,
        m_minClipText : null,
        m_percentCombo : null
    }
});