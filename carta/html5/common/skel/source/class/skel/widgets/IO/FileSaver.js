/**
 * Saves images of active windows.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.IO.FileSaver", {
    extend : qx.ui.core.Widget,

    construct : function() {
        this.base(arguments);

        this._init();
        
        // save preferences shared variable
        this.m_connector = mImport("connector");
        var path = skel.widgets.Path.getInstance();
        this.m_sharedVar = this.m_connector.getSharedVar( path.PREFERENCES_SAVE );
        this.m_sharedVar.addCB( this._savePrefsChanged.bind( this ));
        this._savePrefsChanged();
    },

    members : {
        
        /**
         * Update the file to save based on a user-specified path.
         * @param fullPath {String} - path for saving a file.
         */
        fileChanged : function( fullPath ){
            this.m_saveText.setValue( fullPath );
        },

        /**
         * Initializes the UI.
         */
        _init : function() {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);

            //File location
            var saveLabel = new qx.ui.basic.Label( "Save Name:");
            this.m_saveText = new qx.ui.form.TextField();
            this.m_saveText.setToolTipText( "Full path to the file save location.");
            this.m_saveText.setValue( "/tmp/saveImage.png");
            var browseButton = new qx.ui.form.Button( "Browse...");
            browseButton.setToolTipText( "Browse the file system to select a file save location.");
            browseButton.addListener( "execute", function(){
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                                    "showSaveBrowser", this));
            }, this );
            var locContainer = new qx.ui.container.Composite();
            locContainer.setLayout( new qx.ui.layout.HBox(2));
            locContainer.add( saveLabel );
            locContainer.add( this.m_saveText, {flex:1} );
            locContainer.add( browseButton );
            
            //Aspect Ratio
            var aspectLabel = new qx.ui.basic.Label( "Aspect Ratio:");
            var radioGroup = new qx.ui.form.RadioButtonGroup();
            this.m_aspectKeepRadio = new qx.ui.form.RadioButton( "Keep");
            this.m_aspectKeepRadio.setToolTipText( "The size is scaled as large as possible while preserving the aspect ratio and using the width and height as upper bounds.");
            this.m_aspectKeepRadio.setValue( true );
            this.m_aspectExpandRadio = new qx.ui.form.RadioButton( "Expand");
            this.m_aspectExpandRadio.setToolTipText( "This size is scaled as small as possible while preserving the aspect ratio and using the width and height as lower bounds");
            this.m_aspectIgnoreRadio = new qx.ui.form.RadioButton( "Ignore");
            this.m_aspectIgnoreRadio.setToolTipText( "Scale the size to the specified width and height.");
            radioGroup.add( this.m_aspectKeepRadio );
            radioGroup.add( this.m_aspectExpandRadio );
            radioGroup.add( this.m_aspectIgnoreRadio );
            this.m_keepListenerId = this.m_aspectKeepRadio.addListener( skel.widgets.Path.CHANGE_VALUE, this._sendAspectRatioMode, this );
            this.m_expandListenerId = this.m_aspectExpandRadio.addListener( skel.widgets.Path.CHANGE_VALUE, this._sendAspectRatioMode, this );
            this.m_ignoreListenerId = this.m_aspectIgnoreRadio.addListener( skel.widgets.Path.CHANGE_VALUE, this._sendAspectRatioMode, this );
            var aspectContainer = new qx.ui.container.Composite();
            aspectContainer.setLayout( new qx.ui.layout.HBox(2) );
            aspectContainer.add( new qx.ui.core.Spacer(), {flex:1});
            aspectContainer.add( aspectLabel );
            aspectContainer.add( this.m_aspectKeepRadio );
            aspectContainer.add( this.m_aspectExpandRadio );
            aspectContainer.add( this.m_aspectIgnoreRadio );
            aspectContainer.add( new qx.ui.core.Spacer(), {flex:1});
            
            //Size
            var widthLabel = new qx.ui.basic.Label( "Width:");
            this.m_widthText = new skel.widgets.CustomUI.NumericTextField(0, null);
            this.m_widthText.setToolTipText( "Desired width, in pixels, of the saved image.");
            this.m_widthText.addListener( "keypress", function( evt){
                var enterKey = false;
                if ( evt.getKeyIdentifier().toLowerCase() =="enter"){
                    this._sendWidth();
                }
            }, this );
            var heightLabel = new qx.ui.basic.Label( "Height:");
            this.m_heightText = new skel.widgets.CustomUI.NumericTextField(0,null);
            this.m_heightText.setToolTipText( "Desired height, in pixels, of the saved image.");
            this.m_heightText.addListener( "keypress", function( evt){
                var enterKey = false;
                if ( evt.getKeyIdentifier().toLowerCase() =="enter"){
                    this._sendHeight();
                }
            }, this );
            var sizeContainer = new qx.ui.container.Composite();
            var gridLayout = new qx.ui.layout.Grid();
            gridLayout.setColumnFlex( 0, 1 );
            gridLayout.setColumnFlex( 3, 1 );
            sizeContainer.setLayout( gridLayout );
            sizeContainer.add( new qx.ui.core.Spacer(), {row:0, rowSpan:2, column:0});
            sizeContainer.add( widthLabel, {row:0,column:1});
            sizeContainer.add( this.m_widthText, {row:0, column:2});
            sizeContainer.add( heightLabel, {row:1, column:1});
            sizeContainer.add( this.m_heightText, {row:1, column:2});
            sizeContainer.add( new qx.ui.core.Spacer(), {row:0, rowSpan:2, column:3});

            //Buttons
            var closeButton = new qx.ui.form.Button("Close");
            closeButton.setToolTipText( "Close this dialog.");
            skel.widgets.TestID.addTestId( closeButton, "closeFileSaverButton");
            closeButton.addListener("execute", function() {
                qx.event.message.Bus.dispatch(new qx.event.message.Message(
                        "closeFileSaver", ""));
            });
            var saveButton = new qx.ui.form.Button("Save");
            saveButton.setToolTipText( "Save the image.");
            skel.widgets.TestID.addTestId( saveButton, "saveFileButton");
            saveButton.addListener("execute", this._saveImage, this);
            var buttonComposite = new qx.ui.container.Composite();
            var buttonLayout = new qx.ui.layout.HBox(2);
            buttonComposite.setLayout(buttonLayout);
            buttonComposite.add(new qx.ui.core.Spacer(), {flex : 1});
            buttonComposite.add(saveButton);
            buttonComposite.add(closeButton);

            //Add containers to main one
            this._add( locContainer );
            this._add( aspectContainer );
            this._add( sizeContainer );
            this._add(buttonComposite);
        },
        
        
        /**
         * Save the specified image.
         */
        _saveImage : function(){
            var errorMan = skel.widgets.ErrorHandler.getInstance();
            errorMan.clearErrors();
            if ( this.m_connector !== null && this.m_target !== null ){
                //Make sure with width and height are up-to date and error free (which they may
                //not be if the user has changed them, but not pressed enter.
                var widthOK = this._sendWidth();
                if ( widthOK ){
                    var heightOK = this._sendHeight();
                    if ( heightOK ){
                        var imagePath = this.m_saveText.getValue();
                        var path = skel.widgets.Path.getInstance();
                        var cmd = this.m_target.getIdentifier() + path.SEP_COMMAND + "saveImage";
                        var params = "dataPath:"+imagePath;
                        this.m_connector.sendCommand( cmd, params, null );
                    }
                }
            }
        },
        
        /**
         * Update from the server that the preferred save preferences have changed.
         */
        _savePrefsChanged : function(){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var savePrefs = JSON.parse( val );
                    this._setWidth( savePrefs.width );
                    this._setHeight( savePrefs.height );
                    this._setAspectRatioMode( savePrefs.aspectMode );
                }
                catch( err ){
                    console.log( "Could not parse file save preferences: "+val );
                    console.log( err );
                }
            }
        },
        
        /**
         * Notify the server that the aspect ratio has changed.
         */
        _sendAspectRatioMode : function(){
            if ( this.m_connector !== null ){
                var mode = this.m_aspectKeepRadio.getLabel();
                if ( this.m_aspectExpandRadio.getValue() ){
                    mode = this.m_aspectExpandRadio.getLabel();
                }
                else if ( this.m_aspectIgnoreRadio.getValue() ){
                    mode = this.m_aspectIgnoreRadio.getLabel();
                }
                var path = skel.widgets.Path.getInstance();
                var cmd = path.PREFERENCES_SAVE + path.SEP_COMMAND + "setAspectRatioMode";
                var params = "aspectMode:"+mode;
                this.m_connector.sendCommand( cmd, params, null );
            }
        },

        /**
         * Notify the server that the preferred height of the saved image has changed.
         */
        _sendHeight : function(){
            var errorMan = skel.widgets.ErrorHandler.getInstance();
            var heightValid = false;
            if ( this.m_connector !== null ){
                var value = this.m_heightText.getValue();
                if ( value > 0 ){
                    heightValid = true;
                    errorMan.clearErrors();
                    var path = skel.widgets.Path.getInstance();
                    var cmd = path.PREFERENCES_SAVE + path.SEP_COMMAND +"setHeight";
                    var params = "height:"+value;
                    this.m_connector.sendCommand( cmd, params, null);
                }
                else {
                    errorMan.updateErrors( "Warn: Height must be positive.");
                }
            }
            return heightValid;
        },
        
        /**
         * Notify the server that the preferred width of the saved image has changed.
         */
        _sendWidth : function(){
            var errorMan = skel.widgets.ErrorHandler.getInstance();
            var widthValid = false;
            if ( this.m_connector !== null ){
                var value = this.m_widthText.getValue();
                if ( value > 0 ){
                    errorMan.clearErrors();
                    widthValid = true;
                    var path = skel.widgets.Path.getInstance();
                    var cmd = path.PREFERENCES_SAVE + path.SEP_COMMAND +"setWidth";
                    var params = "width:"+value;
                    this.m_connector.sendCommand( cmd, params, null);
                }
                else {
                    console.log( "Update width error");
                    errorMan.updateErrors( "Warn: Width must be positive.");
                }
            }
            return widthValid;
        },
        
        /**
         * Update the UI based on the server specified aspect ratio mode.
         * @param mode {String} - the server specified aspect ratio mode.
         */
        _setAspectRatioMode : function( mode ){
            this.m_aspectKeepRadio.removeListenerById( this.m_keepListenerId );
            this.m_aspectExpandRadio.removeListenerById( this.m_expandListenerId );
            this.m_aspectIgnoreRadio.removeListenerById( this.m_ignoreListenerId );
            if ( mode == this.m_aspectKeepRadio.getLabel() ){
                this.m_aspectKeepRadio.setValue( true );
            }
            else if ( mode == this.m_aspectExpandRadio.getLabel() ){
                this.m_aspectExpandRadio.setValue( true );
            }
            else if ( mode == this.m_aspectIgnoreRadio.getLabel() ){
                this.m_aspectIgnoreRadio.setValue( true );
            }
            else {
                console.log( "Unrecognized aspect ratio mode:"+ mode );
            }
            this.m_keepListenerId = this.m_aspectKeepRadio.addListener( 
                    skel.widgets.Path.CHANGE_VALUE, this._sendAspectRatioMode, this );
            this.m_expandListenerId = this.m_aspectExpandRadio.addListener( 
                    skel.widgets.Path.CHANGE_VALUE, this._sendAspectRatioMode, this );
            this.m_ignoreListenerId = this.m_aspectIgnoreRadio.addListener( 
                    skel.widgets.Path.CHANGE_VALUE, this._sendAspectRatioMode, this );
        },
        
        /**
         * Update the UI based on the server specified height.
         * @param height {Number} - the preferred height for saved images.
         */
        _setHeight : function( height ){
            var oldHeight = this.m_heightText.getValue();
            if ( oldHeight != height ){
                this.m_heightText.setValue( height.toString() );
            }
        },

        /**
         * Stores the window that wants to add data.
         * @param source {String} an identifier for the window that needs to load data.
         */
        setTarget : function(source) {
            this.m_target = source;
        },
        
        /**
         * Update the UI based on the server specified width.
         * @param width {Number} - the preferred width for saved images.
         */
        _setWidth : function( width ){
            var oldWidth = this.m_widthText.getValue();
            if ( oldWidth != width ){
                this.m_widthText.setValue( width.toString() );
            }
        },

        m_connector : null,
        m_aspectIgnoreRadio : null,
        m_aspectKeepRadio : null,
        m_aspectExpandRadio : null,
        m_widthText : null,
        m_heightText : null,
        m_sharedVar : null,
        m_saveText : null,
        m_target : null,
        
        //Listeners
        m_ignoreListenerId : null,
        m_keepListenerId : null,
        m_expandListenerId : null
    },
    properties : {
        appearance : {
            refine : true,
            init : "popup-dialog"
        }

    }

});