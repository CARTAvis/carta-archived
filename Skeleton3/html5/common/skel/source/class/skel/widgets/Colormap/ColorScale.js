/**
 * Displays a file browser.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColorScale", {
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
            
            var scaleGroup = new qx.ui.groupbox.GroupBox( "Map");
            scaleGroup.setLayout( new qx.ui.layout.VBox(2));
            this.m_invertCheck = new qx.ui.form.CheckBox( "Invert");
            this.m_reverseCheck = new qx.ui.form.CheckBox( "Reverse");
            scaleGroup.add( this.m_invertCheck );
            scaleGroup.add( this.m_reverseCheck );
            this._add( scaleGroup );
            
            var mapComposite = new qx.ui.container.Composite();
            mapComposite.setLayout(new qx.ui.layout.HBox(2));
            var mapLabel = new qx.ui.basic.Label( "Map:");
            this.m_mapCombo = new qx.ui.form.ComboBox();
            mapComposite.add( mapLabel );
            mapComposite.add( this.m_mapCombo );
            this._add( mapComposite );
        },
        
        m_invertCheck : null,
        m_reverseCheck : null,
        m_mapCombo : null
    }
});