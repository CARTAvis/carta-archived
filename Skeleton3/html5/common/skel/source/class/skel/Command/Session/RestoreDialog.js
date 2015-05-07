/**
 * Mixin for registering snapshot.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/
qx.Class.define("skel.Command.Session.RestoreDialog", {
    extend : qx.ui.core.Widget,
    include : [skel.Command.Session.SnapRegistration],
    
    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        
        this.registerSnapshot( this._updateSnapshots );

        var pane = new qx.ui.splitpane.Pane( "horizontal");
        this.m_snapshotTable = new skel.Command.Session.StateTable();
        this.m_snapshotTable.setToolTipText( "Select the state to restore.");
        this.m_snapshotTable.setHeight(200);
        this.m_snapshotTable.addListener( "selectionChanged", function(){
            this._updateDescription();
        }, this );
        pane.add( this.m_snapshotTable, 3 );
        this.m_detailsArea = new qx.ui.form.TextArea();
        this.m_detailsArea.setEnabled( false );
        this.m_detailsArea.setToolTipText( "Additional notes about the saved state information if it was provided when the state was saved.");
        pane.add( this.m_detailsArea, 1 );
        
        var butContainer = new qx.ui.container.Composite();
        butContainer.setLayout( new qx.ui.layout.HBox(2));
        butContainer.add( new qx.ui.core.Spacer(1), {flex:1});
        var closeButton = new qx.ui.form.Button( "Close");
        closeButton.setToolTipText( "Close this dialog without saving state.");
        closeButton.addListener( "execute", function(){
            this.fireDataEvent("closeSessionRestore", "");
        }, this);
        var restoreButton = new qx.ui.form.Button("Restore");
        restoreButton.setToolTipText( "Restore the selected state.");
        restoreButton.addListener( "execute", function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_identifier + path.SEP_COMMAND + "restoreSnapshot";
            var stateName = this.m_snapshotTable.getSnapshotName();
            var params = "fileName:"+stateName;
            this.m_connector.sendCommand( cmd, params, function(val){} );
            
        }, this );
        var deleteButton = new qx.ui.form.Button( "Delete");
        deleteButton.setToolTipText( "Delete the selected state.");
        deleteButton.addListener( "execute", function(){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_identifier + path.SEP_COMMAND + "deleteSnapshot";
            var stateName = this.m_snapshotTable.getSnapshotName();
            var params = "fileName:"+stateName;
            this.m_connector.sendCommand( cmd, params, function(val){} );
        }, this);
        butContainer.add( restoreButton );
        butContainer.add( deleteButton );
        butContainer.add( closeButton );
        
        this._setLayout( new qx.ui.layout.VBox(2));
        this._add( pane );
        this._add( butContainer );
    },

    events : {
        "closeSessionRestore" : "qx.event.type.Data"
    },

    members : {
        

        
        /**
         * Update the snapshot description based on the name of the
         * selected snapshot.
         */
        _updateDescription : function(){
            var snapName = this.m_snapshotTable.getSnapshotName();
            if ( snapName !== null && snapName.length > 0 ){
                var descript = this.m_snapMap[snapName];
                this.m_detailsArea.setValue( descript );
            }
        },
        
        /**
         * Update the UI with the new list of snapshots from the server.
         * @param val {String} a string representation of user state that has been saved.
         */
        _updateSnapshots : function(){
            var val = this.m_sharedVar.get();
            if ( val !== null ){
                try {
                    var jsonObj = JSON.parse( val );
                    
                  //See if there is one that should be selected.
                    var selectedName = jsonObj.selected;
                    this.m_snapshotTable.setSelectedName( selectedName );
                    
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
        m_snapMap : {}
    },

    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }

    }

});
