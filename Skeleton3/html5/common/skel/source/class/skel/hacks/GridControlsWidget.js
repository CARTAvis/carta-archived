/**
 * A widget that allows user to manipulate settings related to WCS grids.
 *
 * @ignore( mImport)
 */

/*global mImport,qx */

qx.Class.define( "skel.hacks.GridControlsWidget", {
    extend: qx.ui.container.Composite,

    construct: function( statePrefix) {
        this.base( arguments);
        this.m_statePrefix = statePrefix;
        if( ! qx.lang.String.endsWith( this.m_statePrefix, "/")) {
            this.m_statePrefix += "/";
        }

        this.m_connector = mImport( "connector");

        this.setLayout( new qx.ui.layout.VBox( 3));
        this.m_controls = {};
        var box;

        // line opacity
        box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        box.add( new qx.ui.basic.Label( "Line opacity:"));
        this.m_controls.lineOpacitySlider = new skel.hacks.BoundSlider(
            {
                orientation: "horizontal",
                sharedVar: this.m_connector.getSharedVar( this.m_statePrefix + "lineOpacity"),
                min: 0,
                max: 1
            } ).set( { minWidth: 100 });
        box.add( this.m_controls.lineOpacitySlider, { flex: 1});
        this.add( box);

        // line thickness
        box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        box.add( new qx.ui.basic.Label( "Line thickness:"));
        this.m_controls.lineThicknessSlider = new skel.hacks.BoundSlider(
            {
                orientation: "horizontal",
                sharedVar: this.m_connector.getSharedVar( this.m_statePrefix + "lineThickness"),
                min: 0,
                max: 10

            });
        box.add( this.m_controls.lineThicknessSlider, { flex: 1});
        this.add( box);

        // grid density
        box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        box.add( new qx.ui.basic.Label( "Density:"));
        this.m_controls.gridDensitySlider = new skel.hacks.BoundSlider(
            {
                orientation: "horizontal",
                sharedVar: this.m_connector.getSharedVar( this.m_statePrefix + "gridDensity"),
                min: 0,
                max: 10

            });
        box.add( this.m_controls.gridDensitySlider, { flex: 1});
        this.add( box);

        // font size
        box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        box.add( new qx.ui.basic.Label( "Font size:"));
        this.m_controls.fontSizeSlider = new skel.hacks.BoundSlider(
            {
                orientation: "horizontal",
                sharedVar: this.m_connector.getSharedVar( this.m_statePrefix + "fontSize"),
                min: 0,
                max: 10

            });
        box.add( this.m_controls.fontSizeSlider, { flex: 1});
        this.add( box);

        // system
        box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        box.add( new qx.ui.basic.Label( "System:"));
        this.add( box);

        // font color
        box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        box.add( new qx.ui.basic.Label( "Font color:"));
        this.add( box);

        // line color
        box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        box.add( new qx.ui.basic.Label( "Line color:"));
        this.add( box);




        this.setMinWidth( 100);
        this.setMinHeight( 100);
    },

    members: {

        m_connector: null,
        m_statePrefix: "/"
    }

});