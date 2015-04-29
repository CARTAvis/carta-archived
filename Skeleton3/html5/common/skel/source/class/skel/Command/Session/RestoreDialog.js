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
        this.m_snapshotTable.setHeight(200);
        this.m_snapshotTable.addListener( "selectionChanged", function(){
            this._updateDescription();
        }, this );
        pane.add( this.m_snapshotTable, 3 );
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
        
        _updateDescription : function(){
            var snapName = this.m_snapshotTable.getSnapshotName();
            if ( snapName !== null && snapName.length > 0 ){
                var descript = this.m_snapMap[snapName];
                this.m_detailsArea.setValue( descript );
            }
        },
        
        _updateSnapshots : function(val){
            if ( val !== null ){
                try {
                    var jsonObj = JSON.parse( val );
                    var snapshotStrArray = jsonObj.Snapshots;
                    var count = snapshotStrArray.length;
                    this.m_snapshotTable.clear();
                    this.m_snapMap = {};
                    for ( var i = 0; i < count; i++ ){
                        var snap = JSON.parse( snapshotStrArray[i]);
                        var name = snap.Snapshot;
                        if ( (typeof snap.description ==="undefined") || (snap.description === "") ){
                            this.m_snapMap[name] = "No Details";
                        }
                        else {
                            this.m_snapMap[name] = snap.description;
                        }
                        this.m_snapshotTable.addSnapshot( name, snap.dateCreated, snap.layout, 
                                snap.preferences, snap.data );
                    }
                }
                catch( err ){
                    console.log( "UpdateSnapshots could not parse: "+val+" error: "+err );
                }
            }
        },
        
        m_snapshotTable : null,
        m_connector : null,
        m_snapMap : {}
    },

    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }

    }

});
