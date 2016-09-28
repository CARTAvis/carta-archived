/**
 * Dialog for saving state.
 */

/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/
qx.Class.define("skel.Command.Session.SaveDialog", {
    extend : qx.ui.core.Widget,
    
    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        this.registerSnapshot();
        var nameContainer = new qx.ui.container.Composite();
        nameContainer.setLayout(new qx.ui.layout.HBox(2));
        var nameLabel = new qx.ui.basic.Label( "Save Name:");
        nameContainer.add( new qx.ui.core.Spacer(1), {flex:1});
        nameContainer.add(nameLabel);
        this.m_saveText = new qx.ui.form.TextField();
        this.m_saveText.setToolTipText( "Name for the state that will be saved, state in 'default_session' will automatically be loaded on start-up.");
        this.m_saveText.setValue( this.m_DEFAULT_SAVE );
        skel.widgets.TestID.addTestId( this.m_saveText, "snapshotSaveName");
        nameContainer.add( this.m_saveText);
        nameContainer.add( new qx.ui.core.Spacer(1), {flex:1});
        
        var path = skel.widgets.Path.getInstance();
        var checkContainer = new qx.ui.container.Composite();
        checkContainer.setLayout( new qx.ui.layout.VBox(2));
        this.m_layoutCheck = new qx.ui.form.CheckBox(path.STATE_LAYOUT);
        this.m_layoutCheck.setToolTipText( "Save the layout information which includes which plug-ins are loaded and links between plug-ins.");
        this.m_layoutCheck.setValue( true );
        this.m_preferencesCheck = new qx.ui.form.CheckBox(path.STATE_PREFERENCES);
        this.m_preferencesCheck.setToolTipText( "Save user preferences which includes things like whether the toolbar should be shown/hidden.");
        this.m_allCheck = new qx.ui.form.CheckBox(path.STATE_SESSION);
        this.m_allCheck.setToolTipText( "Save the entire session, including images that are loaded, so it can be restored at a later date.");
        this.m_allCheck.addListener( "changeValue", function(){
            var saveAll = this.m_allCheck.getValue();
            if ( saveAll ){
                this.m_layoutCheck.setEnabled( false );
                this.m_layoutCheck.setValue( true );
                this.m_preferencesCheck.setEnabled( false );
                this.m_preferencesCheck.setValue( true );
            }
            else {
                this.m_layoutCheck.setEnabled( true );
                this.m_preferencesCheck.setEnabled( true );
            }
        }, this );
        checkContainer.add( this.m_allCheck );
        var subCheckContainer1 = new qx.ui.container.Composite();
        subCheckContainer1.setLayout( new qx.ui.layout.HBox(2));
        subCheckContainer1.add( new qx.ui.core.Spacer(20));
        subCheckContainer1.add( this.m_preferencesCheck );
        subCheckContainer1.add( new qx.ui.core.Spacer(20), {flex:1});
        var subCheckContainer2 = new qx.ui.container.Composite();
        subCheckContainer2.setLayout( new qx.ui.layout.HBox(2));
        subCheckContainer2.add( new qx.ui.core.Spacer(20));
        subCheckContainer2.add( this.m_layoutCheck );
        subCheckContainer2.add( new qx.ui.core.Spacer(20), {flex:1});
        
        var descriptContainer = new qx.ui.container.Composite();
        descriptContainer.setLayout( new qx.ui.layout.VBox(2));
        var labelContainer = new qx.ui.container.Composite();
        labelContainer.setLayout( new qx.ui.layout.HBox(2));
        var descriptLabel = new qx.ui.basic.Label( "Description:");
        labelContainer.add( descriptLabel );
        labelContainer.add( new qx.ui.core.Spacer(20), {flex:1});
        descriptContainer.add( labelContainer );
        this.m_descriptArea = new qx.ui.form.TextArea();
        this.m_descriptArea.setToolTipText( "Add additional (optional) information as a reminder of what this state represents.");
        descriptContainer.add( this.m_descriptArea );
        
        checkContainer.add( subCheckContainer1);
        checkContainer.add( subCheckContainer2);
        var butContainer = new qx.ui.container.Composite();
        butContainer.setLayout( new qx.ui.layout.HBox(5));
        butContainer.add( new qx.ui.core.Spacer(1), {flex:1});
        var closeButton = new qx.ui.form.Button( "Close");
        closeButton.setToolTipText( "Close this window without saving state.");
        closeButton.addListener( "execute", function(){
            this.fireDataEvent("closeSessionSave", "");
        }, this);
        
        var saveButton = new qx.ui.form.Button( "Save");
        saveButton.addListener( "execute", this._saveSnapshot, this );
        saveButton.setToolTipText( "Save the state so that it can be restored at a later date.");
        butContainer.add( saveButton );
        butContainer.add( closeButton );
        
        this._setLayout( new qx.ui.layout.VBox(2));
        this._add( nameContainer );
        this._add( checkContainer );
        this._add( descriptContainer );
        this._add( butContainer );     
    },
    
    events : {
        "closeSessionSave" : "qx.event.type.Data"
    },

    members : {
    	
    	/**
         * Send the command to get the shared variable containing snapshot information.
         * @param callback {Function} the function to be called when the shared variable changes.
         */
        registerSnapshot : function(  ){
            this.m_connector = mImport( "connector");
            var params = "";
            var pathDict = skel.widgets.Path.getInstance();
            var regCmd = pathDict.getCommandRegisterSnapshots();
            this.m_connector.sendCommand( regCmd, params, this._registrationCallback(this));
        },
        
        /**
         * Send a command to the server to save a snapshot.
         */
        _saveSnapshot : function(){
            var path = skel.widgets.Path.getInstance();
            var errMan = skel.widgets.ErrorHandler.getInstance();
            var fileName = this.m_saveText.getValue();
            if ( fileName.trim().length === 0 ){
                errMan.updateErrors( "The name of the state to save cannot be blank.");
                return;
            }
            var description = this.m_descriptArea.getValue();
            if ( description === null ){
                description = "";
            }
            var saveLayout = this.m_layoutCheck.getValue();
            var savePreferences = this.m_preferencesCheck.getValue();
            var saveData = this.m_allCheck.getValue();
            //Only save if we are actually saving something; otherwise, post an
            //error.
            if ( saveLayout || savePreferences || saveData ){
                errMan.clearErrors();
                
                var cmd = this.m_identifier + path.SEP_COMMAND + "saveSnapshot";
                var params = "fileName:"+fileName+",layoutSnapshot:"+saveLayout+
                    ",preferencesSnapshot:"+savePreferences+",dataSnapshot:"+saveData+",description:"+description;
                this.m_connector.sendCommand( cmd, params, function(val){} );
            }
            else {
                errMan.updateErrors( "Please select the type of state to save.");
            }
        },
        
        /**
         * Callback for obtaining the snapshot server id.
         * @param anObject {skel.Command.Session.SaveDialog} -this object.
         */
        _registrationCallback : function( anObject ){
            return function( id ){
                if ( id && id.length > 0 ){
                    if ( id != anObject.m_identifier ){
                        anObject.m_identifier = id;                       
                    }
                }
            };
        },
        
        m_connector : null,
        m_identifier : null,
        m_DEFAULT_SAVE : "session_default",
        m_layoutCheck : null,
        m_preferencesCheck : null,
        m_descriptArea : null,
        m_allCheck : null,
        m_saveText : null
    },

    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }

    }

});
