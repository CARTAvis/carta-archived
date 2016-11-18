/**
 * Created by pfederl on 04/01/15.
 *
 * Widget for controling layers in a layered view.
 *
 * @asset(qx/icon/${qx.icontheme}/16/actions/list-add.png)
 * @asset(qx/icon/${qx.icontheme}/16/actions/go-up.png)
 * @asset(qx/icon/${qx.icontheme}/16/actions/go-down.png)
 * @asset(qx/icon/${qx.icontheme}/16/actions/edit-delete.png)
 *
 * @ignore( mImport)
 */

qx.Class.define( "skel.hacks.LayeredViewManager", {

    extend: qx.ui.container.Composite,

    events: {
        "viewRefreshed": "qx.event.type.Data"
    },

    /**
     * Constructor
     */
    construct: function( viewName )
    {
        this.base( arguments );
        this.m_connector = mImport( "connector" );
        this.m_viewName = viewName;
        this.setLayout( new qx.ui.layout.VBox( 5 ) );
        // create a list widget
        this.m_listWidget = new qx.ui.form.List();
        this.m_listWidget.setSelectionMode( "multi");

        for (var i = 0; i < 3; i++) {
            this.m_listWidget.add( new qx.ui.form.ListItem( "item" + i));
        }

        this.add( this.m_listWidget, {flex: 1} );

        // create buttons
        var m_row1Widget = new qx.ui.container.Composite( new qx.ui.layout.HBox( 3 ) );
        this.add( m_row1Widget );
        var addButton = new qx.ui.form.Button( "Add", "icon/16/actions/list-add.png" );
        addButton.set( {toolTipText: "Add new layer", show: "icon", padding: 1} );
        addButton.addListener( "execute", this._addButtonCB.bind(this));
        m_row1Widget.add( addButton );
        var upButton = new qx.ui.form.Button( "Up", "icon/16/actions/go-up.png" );
        upButton.set( {toolTipText: "Move layer up", show: "icon", padding: 1} );
        upButton.addListener( "execute", this._upButtonCB.bind(this));
        m_row1Widget.add( upButton );
        var downButton = new qx.ui.form.Button( "Down", "icon/16/actions/go-down.png" );
        downButton.set( {toolTipText: "Move layer down", show: "icon", padding: 1} );
        downButton.addListener( "execute", this._downButtonCB.bind(this));
        m_row1Widget.add( downButton );
        var deleteButton = new qx.ui.form.Button( "Down", "icon/16/actions/edit-delete.png" );
        deleteButton.set( {toolTipText: "Delete layer", show: "icon", padding: 1} );
        deleteButton.addListener( "execute", this._deleteButtonCB.bind(this));
        m_row1Widget.add( deleteButton );

        // create rename textbox
        this.m_renameTextField = new qx.ui.form.TextField("");
        this.m_renameTextField.set({ toolTipText: "Type new name and press enter"});
        this.add( this.m_renameTextField);

        // alpha slider
        var alphaRow = new qx.ui.container.Composite( new qx.ui.layout.HBox());
        this.add( alphaRow);
        this.m_alphaLabel = new qx.ui.basic.Label( "");
        this.m_alphaLabel.set({ rich: true, font: "monospace" });
        alphaRow.add( this.m_alphaLabel);
        this.m_alphaSlider = new qx.ui.form.Slider();
        this.m_alphaSlider.set({ minimum: 0, maximum: 255, value: 10});
        alphaRow.add( this.m_alphaSlider, { flex: 1 });
        this.m_alphaSlider.addListener( "changeValue", this._sliderCB.bind(this));
        this.m_alphaSlider.set({ toolTipText: "Change alpha for selected layers."});

        // mask toggles
        var maskToggles = new qx.ui.container.Composite( new qx.ui.layout.HBox(2));
        this.add( maskToggles);
        var CLS = qx.ui.form.CheckBox;
        this.m_redToggle = new CLS( "R");
        maskToggles.add( this.m_redToggle);
        this.m_greenToggle = new CLS( "G");
        maskToggles.add( this.m_greenToggle);
        this.m_blueToggle = new CLS( "B");
        maskToggles.add( this.m_blueToggle);
        this.m_noMaskToggle = new CLS( "None");
        maskToggles.add( this.m_noMaskToggle);

        this.m_redToggle.addListener( "changeValue", this._maskToggleCB.bind(this, "R"));
        this.m_greenToggle.addListener( "changeValue", this._maskToggleCB.bind(this, "G"));
        this.m_blueToggle.addListener( "changeValue", this._maskToggleCB.bind(this, "B"));
        this.m_noMaskToggle.addListener( "changeValue", this._maskToggleCB.bind(this, "X"));

        // update the slider label
        this._sliderCB();

        // listen for state updates
        this.m_sharedVar = this.m_connector.getSharedVar( "/hacks/LayeredViewController/" + this.m_viewName);
        this.m_sharedVar.addCB(this._sharedVarCB.bind(this));
        this._sharedVarCB(this.m_sharedVar.get());

        this.m_listWidget.addListener( "changeSelection", this._listSelectionCB.bind(this));

        this.m_selectedLayerIDs = [];
    },

    members: {

        m_viewName  : null,
        m_listWidget: null,
        m_connector : null,
        m_selectedLayerIDs : null,

        /**
         * Get the overlay widget
         */
        viewName: function()
        {
            return this.m_viewName;
        },

        /**
         * Callback for 'add' button
         * @private
         */
        _addButtonCB: function (action) {
            console.log( "Add");
        },

        _upButtonCB: function (action) {
            console.log( "Up");

            this._updateLayerSelection();
            this._sendCommand( "up", this.m_selectedLayerIDs);
        },

        _downButtonCB: function (action) {
            console.log( "Down");

            this._updateLayerSelection();
            this._sendCommand( "down", this.m_selectedLayerIDs);
        },

        _deleteButtonCB: function (action) {
            console.log( "Delete");

            this._updateLayerSelection();
            this._sendCommand( "delete", this.m_selectedLayerIDs);
        },


        _sharedVarCB : function(val)
        {
            this.m_insideSharedVar = true;
            try {
                console.log( "Layers:", val);
                var obj = JSON.parse( val);
                console.log( "Layers obj:", obj);

                // if we didn't receive a valid state, make a default one
                if( ! obj) obj = { list: []};

                var list = obj.list;

                this.m_listWidget.removeAll();
                var selectedItems = [];
                list.forEach( function(entry) {
                    var item = new qx.ui.form.ListItem( entry.name);
                    if( entry.input) selectedItems.push( item);
                    item.setUserData( "layerID", entry.id);
                    this.m_listWidget.add( item);
                }.bind(this));
                this.m_listWidget.setSelection( selectedItems);

                this._updateLayerSelection();

            } catch(e) {

            }
            this.m_insideSharedVar = false;

        },

        _sliderCB: function() {
            var num = "" + this.m_alphaSlider.getValue();
            while( num.length < 3) { num = " " + num; }
            this.m_alphaLabel.setValue( "&alpha;" + num);
        },

        _maskToggleCB : function( which, e)
        {
            console.log( which, e);
            var flag = e.getData();
            if( which === "R" && flag) {
                this.m_noMaskToggle.setValue( false);
            }
            if( which === "G" && flag) {
                this.m_noMaskToggle.setValue( false);
            }
            if( which === "B" && flag) {
                this.m_noMaskToggle.setValue( false);
            }
            if( which === "X" && flag) {
                this.m_redToggle.setValue( false);
                this.m_greenToggle.setValue( false);
                this.m_blueToggle.setValue( false);
            }

        },

        _updateLayerSelection : function ()
        {
            console.log( "...", this.m_listWidget.getSelection() );
            var selected = [];
            this.m_listWidget.getSelection().forEach( function( item )
            {
                console.log( "Selected:" + item.getUserData( "layerID" ) );
                selected.push( item.getUserData( "layerID" ) );
            }.bind( this ) );
            console.log( "selected layers:", selected );

            this.m_selectedLayerIDs = selected;
        },

        _sendCommand: function( cmd, data)
        {
            if( ! data ) {
                data = null;
            }
            var data = {
                command: cmd,
                data   : data
            };
            this.m_connector.sendCommand(
                "/hacks/LayeredViewController/" + this.m_viewName + "/command",
                JSON.stringify( data ) );
        },

        _listSelectionCB : function ( e)
        {
            if( this.m_insideSharedVar ) return;

            this._updateLayerSelection();

            //this.m_connector.sendCommand(
            //    "/hacks/LayeredViewController/" + this.m_viewName + "/setSelection",
            //    JSON.stringify( this.m_selectedLayerIDs ) );

            this._sendCommand( "setSelection", this.m_selectedLayerIDs);
        }

    }
} );

