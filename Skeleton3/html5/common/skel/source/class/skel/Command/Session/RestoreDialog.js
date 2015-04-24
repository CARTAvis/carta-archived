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

        var nameContainer = new qx.ui.container.Composite();
        nameContainer.setLayout(new qx.ui.layout.HBox(2));
        var nameLabel = new qx.ui.basic.Label( "Restore Name:");
        nameContainer.add( new qx.ui.core.Spacer(1), {flex:1});
        nameContainer.add(nameLabel);
        this.m_restoreCombo = new qx.ui.form.ComboBox();
        skel.widgets.TestID.addTestId( this.m_restoreCombo, "snapshotRestoreName");
        nameContainer.add( this.m_restoreCombo);
        nameContainer.add( new qx.ui.core.Spacer(1), {flex:1});
        
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
            var stateName = this.m_restoreCombo.getValue();
            var params = "fileName:"+stateName;
            this.m_connector.sendCommand( cmd, params, function(val){} );
            
        }, this );
        butContainer.add( restoreButton );
        butContainer.add( closeButton );
        
        this._setLayout( new qx.ui.layout.VBox(2));
        this._add( nameContainer );
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
                var oldName = this.m_restoreCombo.getValue();
                var snapshots = val.split( ",");
                this.m_restoreCombo.removeAll();
                for ( var i = 0; i < snapshots.length; i++ ){
                    var tempItem = new qx.ui.form.ListItem( snapshots[i] );
                    this.m_restoreCombo.add( tempItem );
                }
                //Try to reset the old selection
                if ( oldName !== null ){
                    this.m_restoreCombo.setValue( oldName );
                }
                //Select the first item
                else if ( snapshots.length > 0 ){
                    var selectables = this.m_restoreCombo.getChildrenContainer().getSelectables(true);
                    if ( selectables.length > 0 ){
                        this.m_restoreCombo.setValue( selectables[0].getLabel());
                    }
                }
            }
        },
        
        m_restoreCombo : null,
        m_connector : null
    },

    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }

    }

});
