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
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
            
            var comp = new qx.ui.groupbox.GroupBox( "Transform");
            comp.setLayout( new qx.ui.layout.VBox(2));
            
            var imageComposite = new qx.ui.container.Composite();
            imageComposite.setLayout( new qx.ui.layout.HBox(2));
            this.m_imageCombo = new qx.ui.form.ComboBox();
            this.m_imageCombo.setEnabled( false );
            var imageLabel = new qx.ui.basic.Label( "Image:");
            imageComposite.add( imageLabel );
            imageComposite.add( this.m_imageCombo );
            
            var dataComposite = new qx.ui.container.Composite();
            dataComposite.setLayout( new qx.ui.layout.HBox(2));
            this.m_dataCombo = new qx.ui.form.ComboBox();
            var dataLabel = new qx.ui.basic.Label( "Data:");
            dataComposite.add( dataLabel );
            dataComposite.add( this.m_dataCombo );
            
            comp.add( dataComposite);
            comp.add( imageComposite );
            this._add( comp );
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
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});