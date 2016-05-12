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
        
        var pathDict = skel.widgets.Path.getInstance();
        
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
            
            this.m_imageCombo = new qx.ui.form.ComboBox();
            this.m_imageCombo.setToolTipText( "Select an image transformation.");
            this.m_imageCombo.setEnabled( false );
            var imageLabel = new qx.ui.basic.Label( "Image:");
            this._add( imageLabel );
            this._add( this.m_imageCombo );
            
            this._add( new qx.ui.core.Spacer(), {flex:1});
        },
        
        /**

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
                        var transformCount = transforms.imageTransforms.length;
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
            var iSelectables = this.m_imageCombo.getChildrenContainer().getSelectables(true);
            for ( var i = 0; i < iSelectables.length; i++ ){
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
        m_connector : null,
        m_sharedVarData : null,
        m_sharedVarImage : null
    }
});