/**
 * @ignore( mImport)
 */

/*global mImport,qx */

qx.Class.define( "skel.hacks.LayeredViewHack", {
    extend: qx.ui.container.Composite,

    construct: function( viewName) {
        this.base( arguments);
        this.m_viewName = viewName;

        this.m_connector = mImport( "connector");

        this.setLayout( new qx.ui.layout.VBox( 3));
        //this.m_vgview = new skel.boundWidgets.View.View( this.m_viewName);
        //this.m_vgview = new skel.hacks.VGView( this.m_viewName);
        this.m_vgview = new skel.boundWidgets.View.VGView( this.m_viewName);

        this.add( this.m_vgview, { flex: 1 });

        this.m_status = new qx.ui.basic.Label( "status...");
        this.add( this.m_status);
        //this.m_tf = new skel.boundWidgets.TextField( "/hacks/layercommands/" + this.m_viewName);
        this.m_tf = new qx.ui.form.TextField();
        this.m_tf.addListener( "changeValue", this._tfChangeValueCB.bind( this ) );


        this.add( this.m_tf);

        this.setMinWidth( 100);
        this.setMinHeight( 100);
    },

    members: {

        _tfChangeValueCB : function( e){
            var txt = e.getData();
            this.m_connector.sendCommand( "lvgview-cmd", txt, this._commandCB.bind(this));
        },

        _commandCB : function( txt) {
            //console.log( "commandCB", arguments);
            this.m_status.setValue( "Status: " + txt);
        },

        m_connector: null,
        m_viewName: ""
    }

});