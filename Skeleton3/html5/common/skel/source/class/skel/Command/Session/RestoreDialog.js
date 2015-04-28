/**
 * Dialog for saving state.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/
qx.Class.define("skel.Command.Session.RestoreDialog", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this.m_connector = mImport( "connector");

        var pane = new qx.ui.splitpane.Pane( "horizontal");
        this.m_snapshotTable = new skel.Command.Session.StateTable();
        pane.add( this.m_snapshotTable, 1 );
        this.m_detailsArea = new qx.ui.form.TextArea();
        pane.add( this.m_detailsArea, 1 );
        
        var butContainer = new qx.ui.container.Composite();
        butContainer.setLayout( new qx.ui.layout.HBox(2));
        butContainer.add( new qx.ui.core.Spacer(1), {flex:1});
        var closeButton = new qx.ui.form.Button( "Close");
        closeButton.addListener( "execute", function(){
            this.fireDataEvent("closeSessionRestore", "");
        }, this);
        var restoreButton = new qx.ui.form.Button("Restore");
        restoreButton.addListener( "execute", function(){
            //Restoring the layout, could change it, and we don't want the pop-up
            var path = skel.widgets.Path.getInstance();
            var cmd = path.getCommandRestoreState();
            var stateName = this.m_snapshotTable.getSnapshotName();
            var params = "fileName:"+stateName;
            this.m_connector.sendCommand( cmd, params, function(val){} );
            
        }, this );
        butContainer.add( restoreButton );
        butContainer.add( closeButton );
        
        this._setLayout( new qx.ui.layout.VBox(2));
        this._add( pane );
        this._add( butContainer );
       //Send a command to update the available snapshots
        this.addListener( "appear", function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = path.SNAPSHOTS + path.SEP_COMMAND + "getSnapshots";
            var params = "";
            this.m_connector.sendCommand( cmd, params, this._loadSnapshots(this));
        }, this );
    },

    events : {
        "closeSessionRestore" : "qx.event.type.Data"
    },

    members : {
        _loadSnapshots : function( anObject ){
            return function( snapshotList ){
                anObject._updateSnapshots( snapshotList );
            };
        },
        
        
        _updateSnapshots : function(val){
            if ( val !== null ){
                try {
                    var jsonObj = JSON.parse( val );
                    var snapshotStrArray = jsonObj.Snapshots;
                    var count = snapshotStrArray.length;
                    this.m_snapshotTable.clear();
                    for ( var i = 0; i < count; i++ ){
                        var snap = JSON.parse( snapshotStrArray[i]);
                        this.m_snapshotTable.addSnapshot( snap.Snapshot, snap.dateCreated, snap.layout, snap.preferences, snap.data );
                        //TODO:  Add clickable description
                    }
                }
                catch( err ){
                    console.log( "UpdateSnapshots could not parse: "+val+" error: "+err );
                }
            }
        },
        
        m_snapshotTable : null,
        m_connector : null
    },

    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }

    }

});
