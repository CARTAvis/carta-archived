/**
 * Displays a file browser.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramDisplay", {
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
            this._initStyle();
            this._initOptions();
        },
        
        _initOptions : function(){
            this.m_logCheck = new qx.ui.form.CheckBox( "Log(Count)");
            this.m_coloredCheck = new qx.ui.form.CheckBox( "Colored");
            this._add( this.m_logCheck );
            this._add( this.m_coloredCheck );
        },
        
        _initStyle : function(){
            var styleGroup = new qx.ui.groupbox.GroupBox( "Graph");
            styleGroup.setLayout( new qx.ui.layout.VBox(2));
            console.log( "_initStyle");
            
            this.m_lineRadio = new qx.ui.form.RadioButton( "Line");
            this.m_barRadio = new qx.ui.form.RadioButton( "Outline");
            this.m_fillRadio = new qx.ui.form.RadioButton( "Fill");
           
            var styleGroupRadio = new qx.ui.form.RadioGroup();
            styleGroupRadio.add( this.m_lineRadio, this.m_barRadio, this.m_fillRadio );
         
            styleGroup.add( this.m_lineRadio );
            styleGroup.add( this.m_barRadio );
            styleGroup.add( this.m_fillRadio );
            console.log( "Display added radios");
            this._add( styleGroup);
            console.log( "Display added style");
        },
        
        m_lineRadio : null,
        m_barRadio : null,
        m_fillRadio : null,
        m_logCheck : null,
        m_coloredCheck : null

    }
});