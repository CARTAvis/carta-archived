/**
 * Controls for the color map transform settings.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColorTransform", {
    extend : qx.ui.core.Widget,

    
    /**
     * Constructor.
     */
    construct : function(  ) {
        this.base(arguments);
        this._init( );
        
        this.m_connector = mImport("connector");
        
        //Initialize the shared variable that manages the list of data transforms
        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVarData = this.m_connector.getSharedVar(pathDict.TRANSFORMS_DATA);
        this.m_sharedVarData.addCB(this._dataChangedCB.bind(this));
        this._dataChangedCB();
        
        //Initialize the shared variable that manages the list of image transforms.
        this.m_sharedVarImage = this.m_connector.getSharedVar(pathDict.TRANSFORMS_IMAGE);
        this.m_sharedVarImage.addCB(this._imageChangedCB.bind(this));
        this._imageChangedCB();
    },

    members : {


        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            this.setAllowGrowX( true );
            this.setAllowGrowY( true );
            var widgetLayout = new qx.ui.layout.VBox(1);
            this._setLayout(widgetLayout);
            this._add( new qx.ui.core.Spacer(), {flex:1});
            var comp = new qx.ui.container.Composite();
            var gridLayout = new qx.ui.layout.Grid();
            gridLayout.setColumnAlign( 0, "right", "middle");
            comp.setLayout( gridLayout );
            
            this.m_imageCombo = new qx.ui.form.ComboBox();
            this.m_imageCombo.setToolTipText( "Select an image transformation.");
            this.m_imageCombo.setEnabled( false );
            var imageLabel = new qx.ui.basic.Label( "Image:");
            comp.add( imageLabel, {row:0,column:0} );
            comp.add( this.m_imageCombo, {row:0,column:1} );
            
            this.m_dataCombo = new qx.ui.form.ComboBox();
            this.m_dataCombo.setToolTipText( "Select a data transformation.");
            this.m_dataCombo.addListener( skel.widgets.Path.CHANGE_VALUE, function(e){
                if ( this.m_id !== null ){
                    var transformName = e.getData();
                    //Send a command to the server to let them know the map changed.
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + "setDataTransform";
                    var params = "dataTransform:"+transformName;
                    this.m_connector.sendCommand( cmd, params, function(){});
                }
            },this);
            var dataLabel = new qx.ui.basic.Label( "Data:");
            comp.add( dataLabel, {row:1,column:0} );
            comp.add( this.m_dataCombo, {row:1, column:1} );
            
            this._add( comp );
            this._add( new qx.ui.core.Spacer(), {flex:1});
        },
        
        /**
         * Updates the UI when the data transform changes on the server.
         */
        _dataChangedCB : function(){
            if ( this.m_sharedVarData ){
                var val = this.m_sharedVarData.get();
                if ( val ){
                    try {
                        var oldData = this.m_dataCombo.getValue();
                        var transforms = JSON.parse( val );
                        var transformCount = transforms.dataTransformCount;
                        this.m_dataCombo.removeAll();
                        for ( var i = 0; i < transformCount; i++ ){
                            var transformName = transforms.dataTransforms[i];
                            var tempItem = new qx.ui.form.ListItem( transformName );
                            this.m_dataCombo.add( tempItem );
                        }
                        //Try to reset the old selection
                        if ( oldData !== null ){
                            this.m_dataCombo.setValue( oldData );
                        }
                        //Select the first item
                        else if ( transformCount > 0 ){
                            var selectables = this.m_dataCombo.getChildrenContainer().getSelectables(true);
                            if ( selectables.length > 0 ){
                                this.m_dataCombo.setValue( selectables[0].getLabel());
                            }
                        }
                    }
                    catch( err ){
                        console.log( "Could not parse data transforms: "+val );
                    }
                }
            }
        },
        
        /**
         * Updates the UI when the image transform changes on the server.
         */
        _imageChangedCB : function(){
            if ( this.m_sharedVarImage ){
                var val = this.m_sharedVarImage.get();
                if ( val ){
                    try {
                        var oldData = this.m_imageCombo.getValue();
                        var transforms = JSON.parse( val );
                        var transformCount = transforms.imageTransformCount;
                        this.m_imageCombo.removeAll();
                        for ( var i = 0; i < transformCount; i++ ){
                            var transformName = transforms.imageTransforms[i];
                            var tempItem = new qx.ui.form.ListItem( transformName );
                            this.m_imageCombo.add( tempItem );
                        }
                        //Try to reset the old selection
                        if ( oldData !== null ){
                            this.m_imageCombo.setValue( oldData );
                        }
                        //Select the first item
                        else if ( transformCount > 0 ){
                            var selectables = this.m_imageCombo.getChildrenContainer().getSelectables(true);
                            if ( selectables.length > 0 ){
                                this.m_imageCombo.setValue( selectables[0].getLabel());
                            }
                        }
                    }
                    catch( err ){
                        console.log( "Could not parse image transforms: "+val );
                    }
                }
            }
        },
        
        /**
         * Update from the server for the color map controls to set.
         * @param controls {Object} - server-side color map state.
         */
        setControls : function( controls ){
            var dSelectables = this.m_dataCombo.getChildrenContainer().getSelectables(true);
            for ( var i = 0; i < dSelectables.length; i++ ){
                if ( dSelectables[i].getLabel() == controls.dataTransform ){
                    this.m_dataCombo.setValue( dSelectables[i].getLabel() );
                    break;
                }
            }
            var iSelectables = this.m_imageCombo.getChildrenContainer().getSelectables(true);
            for ( i = 0; i < iSelectables.length; i++ ){
                if ( iSelectables[i].getLabel() == controls.imageTransform ){
                    this.m_imageCombo.setValue( iSelectables[i].getLabel() );
                    break;
                }
            }
        },
        
        /**
         * Set the server side id of the color map.
         * @param id {String} the unique server side id of this color map.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        m_id : null,
        m_imageCombo : null,
        m_dataCombo : null,
        m_connector : null,
        m_sharedVarData : null,
        m_sharedVarImage : null
    }
});