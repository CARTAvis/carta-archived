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
        this.add( this.m_listWidget, {flex: 1} );

        // create buttons
        var m_row1Widget = new qx.ui.container.Composite( new qx.ui.layout.HBox( 3 ) );
        this.add( m_row1Widget );
        var addButton = new qx.ui.form.Button( "Add", "icon/16/actions/list-add.png" );
        addButton.set( {toolTipText: "Add new layer", show: "icon", padding: 1} );
        m_row1Widget.add( addButton );
        var upButton = new qx.ui.form.Button( "Up", "icon/16/actions/go-up.png" );
        upButton.set( {toolTipText: "Move layer up", show: "icon", padding: 1} );
        m_row1Widget.add( upButton );
        var downButton = new qx.ui.form.Button( "Down", "icon/16/actions/go-down.png" );
        downButton.set( {toolTipText: "Move layer down", show: "icon", padding: 1} );
        m_row1Widget.add( downButton );
        var deleteButton = new qx.ui.form.Button( "Down", "icon/16/actions/edit-delete.png" );
        deleteButton.set( {toolTipText: "Delete layer", show: "icon", padding: 1} );
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
    },

    members: {

        m_viewName  : null,
        m_listWidget: null,
        m_connector : null,

        /**
         * Get the overlay widget
         */
        viewName: function()
        {
            return this.m_viewName;
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

        }

    }
} );

