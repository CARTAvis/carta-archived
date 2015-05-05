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

        function linMap( x, x1, x2, y1, y2) {
            return (x-x1)/(x2-x1) * (y2-y1) + y1;
        }

        var sliderRow = function( label, varName, cfg)
        {
            var box = new qx.ui.container.Composite( new qx.ui.layout.HBox() );
            var formatter = new qx.util.format.NumberFormat();
            formatter.setMaximumFractionDigits(3);
            formatter.setMinimumFractionDigits(3);
            function fmtNum(x) {
                var s = formatter.format(Number(x));
                while( s.length < 10) s = ' ' + s;
                return s;
            }
            box.add( new skel.boundWidgets.Label( label, fmtNum, this.m_statePrefix + varName ) );
            var defaultOptions = {
                sharedVar  : this.m_connector.getSharedVar( this.m_statePrefix + varName )
            };
            var options = qx.lang.Object.mergeWith( defaultOptions, cfg, true);
            var slider = new skel.hacks.BoundSlider( options);
            slider.setMinWidth( 100);
            box.add( slider, {flex: 1} );
            return box;
        }.bind(this);

        this.m_controls = {};
        var box;

        // line opacity
        this.add( sliderRow( "Line thickness:", "lineThickness", {
            val2slider: function(x) { return linMap( x, 0, 10, 0, 1000 ) },
            slider2val: function(x) { return linMap( x, 0, 1000, 0, 10 ) }
        }));
        // line opacity
        this.add( sliderRow( "Line opacity:", "lineOpacity", {
            val2slider: function(x) { return linMap( x, 0, 1, 0, 1000 ) },
            slider2val: function(x) { return linMap( x, 0, 1000, 0, 1 ) }
        }));
        // grid density
        this.add( sliderRow( "Grid density:", "gridDensity", {
            val2slider: function(x) { return linMap( x, 0.25, 3, 0, 1000 ) },
            slider2val: function(x) { return linMap( x, 0, 1000, 0.25, 3 ) }
        }));

        box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        //box.add( new qx.ui.basic.Label( "Internal labels:"));
        box.add( new skel.boundWidgets.Toggle( "Internal labels", this.m_statePrefix + "internalLabels"));
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