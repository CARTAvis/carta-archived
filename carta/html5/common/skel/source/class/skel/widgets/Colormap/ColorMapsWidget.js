/**
 * Provides color map selection and displays intensity bounds for the color map.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColorMapsWidget", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        
        //Initiate the shared variable containing a list of all available color maps.
        this.m_connector = mImport("connector");
        this._init( );
        
        //Initialize the shared variable that manages the list of color maps.
        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVarMaps = this.m_connector.getSharedVar(pathDict.COLORMAPS);
        this.m_sharedVarMaps.addCB(this._mapsChangedCB.bind(this));
        this._mapsChangedCB();
        
        //Initialize the shared variable that manages the list of data transforms
        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVarTransform = this.m_connector.getSharedVar(pathDict.TRANSFORMS_DATA);
        this.m_sharedVarTransform.addCB(this._transformChangedCB.bind(this));
        this._transformChangedCB();
        
        //Initialize the shared variable that manages the units.
        this.m_sharedVarUnits = this.m_connector.getSharedVar(pathDict.INTENSITY_UNITS);
        this.m_sharedVarUnits.addCB(this._unitsChangedCB.bind(this));
        this._unitsChangedCB();
    },
    
    statics : {
        CMD_SET_MAP : "setColormap"
    },

    members : {
        
        /**
         * Callback for when the intensity bounds change for the color map.
         */
        _colormapDataCB : function(){
            var val = this.m_sharedVarData.get();
            if ( val ){
                try {
                    var colorData = JSON.parse( val );
                    this._setIntensityRange( colorData.intensityMin, colorData.intensityMax );
                }
                catch( err ){
                    console.log( "ColorMapData could not parse: "+val+" error: "+err );
                }
            }
        },
        
        
        /**
         * Callback for a server error when setting the map index.
         * @param anObject {skel.widgets.ColorMap.ColorScale}.
         */
        _errorMapIndexCB :function( anObject ){
            return function( mapName ){
                if ( mapName ){
                    anObject.setMapName( mapName );
                }
            };
        },
        
        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            var widgetLayout = new qx.ui.layout.HBox(2);
            this._setLayout(widgetLayout);
            
            this.m_intensityLowText = new skel.widgets.CustomUI.NumericTextField( null, null);
            skel.widgets.TestID.addTestId( this.m_intensityLowText, "clipMinIntensity" ); 
            this.m_intensityLowListenId = this.m_intensityLowText.addListener( "textChanged",
                    this._intensityChanged, this );
            this.m_intensityLowText.setToolTipText( "Set the lower intensity bound.");
            this.m_intensityLowText.setIntegerOnly( false );
            this._add( this.m_intensityLowText );
            this._add( new qx.ui.core.Spacer(), {flex:1});
            
            this.m_mapCombo = new skel.widgets.CustomUI.SelectBox( skel.widgets.Colormap.ColorMapsWidget.CMD_SET_MAP, "name");
            skel.widgets.TestID.addTestId( this.m_mapCombo, "colorMapName" ); 
            this.m_mapCombo.setToolTipText( "Select a color map.");
            this._add( this.m_mapCombo );
            
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
            this._add( this.m_dataCombo );
            
            this.m_imageUnitsCombo = new skel.widgets.CustomUI.SelectBox( "setImageUnits", "imageUnits");
            skel.widgets.TestID.addTestId( this.m_imageUnitsCombo, "colorImageUnits" ); 
            this.m_imageUnitsCombo.setToolTipText( "Select units for the intensity bounds.");
            this._add( this.m_imageUnitsCombo );
            
            this._add( new qx.ui.core.Spacer(), {flex:1});
            this.m_intensityHighText = new skel.widgets.CustomUI.NumericTextField(null, null);
            skel.widgets.TestID.addTestId( this.m_intensityHighText, "clipMinIntensity" ); 
            this.m_intensityHighText.setToolTipText( "Set the upper intensity bound.");
            this.m_intensityHighListenId = this.m_intensityHighText.addListener( "textChanged",
                    this._intensityChanged, this );
            this.m_intensityHighText.setIntegerOnly( false );
            this._add( this.m_intensityHighText );
        },
        
        /**
         * The user has changed either the lower or upper colormap intensity
         * bound.
         */
        _intensityChanged : function(){
            if ( this.m_id !== null ){
                var minInt = this.m_intensityLowText.getValue();
                var maxInt = this.m_intensityHighText.getValue();
                  
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setIntensityRange";
                var params = "intensityMin:"+minInt+",intensityMax:"+maxInt;
                this.m_connector.sendCommand( cmd, params, null);
            }
        },
        
        /**
         * Callback for a change in the available color maps on the server.
         */
        _mapsChangedCB : function(){
            if ( this.m_sharedVarMaps ){
                var val = this.m_sharedVarMaps.get();
                if ( val ){
                    try {
                        var oldName = this.m_mapCombo.getValue();
                        var colorMaps = JSON.parse( val );
                        this.m_mapCombo.setSelectItems( colorMaps.maps );
                       
                        //Try to reset the old selection
                        if ( oldName !== null ){
                            this.m_mapCombo.setSelectValue( oldName );
                        }
                    }
                    catch( err ){
                        console.log( "Could not parse color map list: "+val );
                        console.log( "Err="+err );
                    }
                }
            }
        },
        
        /**
         * Set the minimum and maximum intensity values.
         * @param minValue {Number} - the minimum intensity for the color map.
         * @param maxValue {Number} - the maximum intensity for the color map.
         */
        _setIntensityRange : function( minValue, maxValue ){
            var oldMin = this.m_intensityLowText.getValue();
            if ( oldMin != minValue ){
                if ( this.m_intensityLowListenId !== null ){
                    this.m_intensityLowText.removeListenerById( this.m_intensityLowListenId );
                }
                this.m_intensityLowText.setValue( minValue.toString() );
                this.m_intensityLowListenId = this.m_intensityLowText.addListener( "textChanged",
                        this._intensityChanged, this );
            }
            var oldMax = this.m_intensityHighText.getValue();
            if ( oldMax != maxValue ){
                if ( this.m_intensityHighListenId !== null ){
                    this.m_intensityHighText.removeListenerById( this.m_intensityHighListenId );
                }
                this.m_intensityHighText.setValue( maxValue.toString() );
                this.m_intensityHighListenId = this.m_intensityHighText.addListener( "textChanged",
                        this._intensityChanged, this );
            }
        },
        
        /**
         * Set the type of data transform based on server side values.
         * @param transform {String} - the data transform.
         */
        setDataTransform : function( transform ){
            var dSelectables = this.m_dataCombo.getChildrenContainer().getSelectables(true);
            for ( var i = 0; i < dSelectables.length; i++ ){
                if ( dSelectables[i].getLabel() == transform ){
                    this.m_dataCombo.setValue( dSelectables[i].getLabel() );
                    break;
                }
            }
        },
        
        /**
         * Set the selected color map.
         * @param mapName {String} the name of the selected color map.
         */
        setMapName : function( mapName ){
            var selectables = this.m_mapCombo.setSelectValue( mapName );
        },
        
        /**
         * Set the server side id of the color map.
         * @param id {String} the unique server side id of this color map.
         */
        setId : function( id ){
            this.m_id = id;
            this.m_mapCombo.setId( id );
            this.m_imageUnitsCombo.setId( id );
            
            var path = skel.widgets.Path.getInstance();
            var dataPath = this.m_id + path.SEP + path.DATA;
            this.m_sharedVarData = this.m_connector.getSharedVar( dataPath );
            this.m_sharedVarData.addCB( this._colormapDataCB.bind( this));
            this._colormapDataCB();
        },
        
        /**
         * The list of available data transforms has changed on the server.
         */
        _transformChangedCB : function(){
            if ( this.m_sharedVarTransform ){
                var val = this.m_sharedVarTransform.get();
                if ( val ){
                    try {
                        var oldData = this.m_dataCombo.getValue();
                        var transforms = JSON.parse( val );
                        var transformCount = transforms.dataTransforms.length;
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
         * The list of available image units has changed on the server.
         */
        _unitsChangedCB : function(){
            if ( this.m_sharedVarUnits ){
                var val = this.m_sharedVarUnits.get();
                if ( val ){
                    try {
                        var imageUnits = JSON.parse( val );
                        this.m_imageUnitsCombo.setSelectItems(imageUnits.units);
                    }
                    catch( err ){
                        console.log( "Could not parse image units: "+val );
                    }
                }
            }
        },
        
        m_intensityLowText : null,
        m_intensityHighText : null,
        m_intensityLowListenId : null,
        m_intensityHighListenId : null,
       
        m_mapCombo : null,
        m_dataCombo : null,
        m_imageUnitsCombo : null,
        
        m_id : null,
        m_connector : null,
        m_sharedVarData : null,
        m_sharedVarMaps : null,
        m_sharedVarTransform : null,
        m_sharedVarUnits : null
    }
});