/**
 * A widget that allows user to select a sky coordinate system.
 *
 * @ignore( mImport)
 */

/*global mImport,qx */

qx.Class.define( "skel.hacks.BoundSkyCSSelector", {
    extend: qx.ui.container.Composite,

    construct: function( pconfig )
    {
        // figure out options
        var defaultConfig = {
            label    : "CS",
            knownSkyCSPathPrefix: "/hacks/knownSkyCS",
            sharedVar: null
        };
        this.m_config = qx.lang.Object.mergeWith( defaultConfig, pconfig, true );

        // call inherited constructor
        this.base( arguments, new qx.ui.layout.HBox( 5 ).set( {alignY: "middle"} ) );

        // setup our own UI
        this.setBackgroundColor( "#aaaaaa" );
        this.setPadding( 2 );
        this.m_label = new qx.ui.basic.Label( this.m_config.label );
        this.add( this.m_label );
        this.m_selBox = new qx.ui.form.SelectBox();
        this.add( this.m_selBox );

        // retrieve connector
        this.m_connector = mImport( "connector");

        // setup the selection box after know sky coordinate systems are available
        var knownSkyCsSharedVar = this.m_connector.getSharedVar( this.m_config.knownSkyCSPathPrefix + "/count");
        knownSkyCsSharedVar.addCB( this._csSharedVarCB.bind(this));
        this._csSharedVarCB();

        // listen and react to state changes
        this.m_config.sharedVar.addCB( this._sharedVarCB.bind( this ) );
        this._sharedVarCB( this.m_config.sharedVar.get() );

        // listen to UI events
        this.m_selBox.addListener( "changeSelection", this._changeSelectionCB.bind( this ) );
    },

    members: {

        // callback for chanhging the current coordinate system
        _sharedVarCB: function( val )
        {
            console.log( "BoundSkyCSSelector::_sharedVarCB", val );
            if( this.m_ignoreSharedVarCB ) {
                return;
            }
            // skip nulls, or if the items are not yet populated
            if( ! val || ! this.m_items) {
                return;
            }
            var symbol = val;
            // find the symbol in our list of items
            var item = null;
            for( var i = 0 ; i < this.m_items.length ; i ++ ) {
                console.log( "cmp", this.m_items[i].getUserData("symbol"), symbol);
                if( this.m_items[i].getUserData("symbol") === symbol) {
                    item = this.m_items[i];
                    break;
                }
            }
            if( ! item) {
                console.error( "Could not find symbol", symbol, "in list", this.m_items);
                return;
            }
            this.m_ignoreChangeSelectionCB = true;
            this.m_selBox.setSelection( [item] );
            this.m_ignoreChangeSelectionCB = false;
        },

        // callback for reading in the list of known coordinate systems
        _csSharedVarCB: function() {
            if( this.m_items) {
                console.error( "List of known sky CS updated??? Should be static, no?");
                throw "Cannot handle dynamically changing list of known SKY CSs";
            }
            var c = this.m_connector;
            var pf = this.m_config.knownSkyCSPathPrefix;
            var countSV = c.getSharedVar( pf + "/count" );
            if( countSV.get() == null) {
                console.log( "Waiting for known sky cs");
                return;
            }

            this.m_ignoreChangeSelectionCB = true;
            this.m_items = [];
            var count = Number( c.getSharedVar( pf + "/count" ).get());
            for( var i = 0 ; i < count ; i ++ ) {
                var entry = c.getSharedVar( pf + "/s" + i).get();
                console.log( "entry", i, entry);
                var a = entry.split( " ");
                var sym = a[0];
                var text = a[1];
                var item = new qx.ui.form.ListItem( text);
                item.setUserData( "symbol", sym);
                this.m_items.push( item);
                this.m_selBox.add( item);
            }
            console.log("m_items:", this.m_items);
            this.m_ignoreChangeSelectionCB = false;
            this._changeSelectionCB();
        },

        // this gets called when UI is manipulated by user
        _changeSelectionCB: function()
        {
            if( this.m_ignoreChangeSelectionCB ) {
                return;
            }
            var selectedWidget = this.m_selBox.getSelection()[0];
            var newVal = selectedWidget.getUserData( "symbol");
            this.m_ignoreSharedVarCB = true;
            this.m_config.sharedVar.set( newVal );
            this.m_ignoreSharedVarCB = false;
        },

        m_connector: null,
        m_ignoreChangeSelectionCB: false,
        m_ignoreSharedVarCB      : false,
        m_items                  : null

    }

} );