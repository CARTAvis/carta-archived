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

        // font size
        this.add( sliderRow( "Font size:", "numText1FontSize", {
            val2slider: function(x) { return linMap( x, 3, 30, 0, 1000 ) },
            slider2val: function(x) { return linMap( x, 0, 1000, 3, 30 ) }
        }));

        // font index
        this.add( sliderRow( "Font index:", "numText1FontIndex", {
            val2slider: function(x) { return linMap( x, 0, 3, 0, 3 ) },
            slider2val: function(x) { return linMap( x, 0, 3, 0, 3 ) },
            maximum: 3,
            pageStep: 1
        }));

        // grid density
        this.add( sliderRow( "Grid spacing:", "gridDensity", {
            val2slider: function(x) { return linMap( x, 0.25, 3, 0, 1000 ) },
            slider2val: function(x) { return linMap( x, 0, 1000, 0.25, 3 ) }
        }));

        box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        //box.add( new qx.ui.basic.Label( "Internal labels:"));
        box.add( new skel.boundWidgets.Toggle( "Internal labels", this.m_statePrefix + "internalLabels"));
        this.add( box);

        var pens = [
            ["Grid lines 1", "gridLines1"],
            ["Grid lines 2", "gridLines2"],
            ["Axis lines 1", "axisLines1"],
            ["Axis lines 2", "axisLines2"],
            ["Tick lines 1", "tickLines1"],
            ["Tick lines 2", "tickLines2"],
            ["Border", "borderLines"],
            ["Labels 1", "numText1pen"],
            ["Labels 2", "numText2pen"],
            ["Axis Labels 1", "labelText1pen"],
            ["Axis Labels 2", "labelText2pen"],
            ["Shadow", "shadowPen"],
            ["Margin dim", "marginDimPen"]
        ];
        box = new qx.ui.container.Composite( new qx.ui.layout.Flow( 2, 2));
        pens.forEach( function(e,ind) {
            box.add( new skel.hacks.BoundQPen( {
                sharedVar: this.m_connector.getSharedVar( this.m_statePrefix + e[1] ),
                title: e[0]
            }), { lineBreak: (ind + 1) % 3 == 0});
        }, this);
        this.add(box);



        // system
        box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        box.add( new skel.hacks.BoundSkyCSSelector({
            sharedVar: this.m_connector.getSharedVar( this.m_statePrefix + "skyCS")
        }));
        this.add( box);


        //// line color
        //var colButt;
        //var colpop = new qx.ui.control.ColorPopup();
        //colpop.exclude();
        //
        //// line color
        //box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        //colButt = new qx.ui.form.Button( "Line color");
        //box.add( colButt);
        //colButt.addListener( "execute", function(e) {
        //    colpop.placeToWidget( colButt);
        //    colpop.setValue( "#ff0000");
        //    colpop.show();
        //});
        //this.add( box);
        //
        //// font color
        //box = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        //colButt = new qx.ui.form.Button( "Font color");
        //box.add( colButt);
        //colButt.addListener( "execute", function(e) {
        //    colpop.placeToWidget( colButt);
        //    colpop.setValue( "#ff00ff");
        //    colpop.show();
        //});
        //this.add( box);
        //

        this.setMinWidth( 100);
        this.setMinHeight( 100);
    },

    members: {

        m_connector: null,
        m_statePrefix: "/"
    }

});