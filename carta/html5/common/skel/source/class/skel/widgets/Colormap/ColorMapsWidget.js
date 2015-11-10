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
        var pathDict = skel.widgets.Path.getInstance();
        this.m_sharedVarMaps = this.m_connector.getSharedVar(pathDict.COLORMAPS);
        this.m_sharedVarMaps.addCB(this._mapsChangedCB.bind(this));
        this._mapsChangedCB();
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
            var widgetLayout = new qx.ui.layout.HBox();
            this._setLayout(widgetLayout);
            
            this.m_intensityLowLabel = new qx.ui.basic.Label( "");
            this._add( this.m_intensityLowLabel );
            this._add( new qx.ui.core.Spacer(), {flex:1});
            
            this.m_mapCombo = new skel.widgets.CustomUI.SelectBox( skel.widgets.Colormap.ColorMapsWidget.CMD_SET_MAP, "name");
            skel.widgets.TestID.addTestId( this.m_mapCombo, "colorMapName" ); 
            this.m_mapCombo.setToolTipText( "Select a color map.");
            this._add( this.m_mapCombo );
            
            this.m_globalCheck = new qx.ui.form.CheckBox( "Global");
            skel.widgets.TestID.addTestId( this.m_globalCheck, "colorMapGlobal");
            this.m_globalCheck.addListener( skel.widgets.Path.CHANGE_VALUE, function(e){
                if ( this.m_id !== null ){
                    var global = this.m_globalCheck.getValue();
                    //Send a command to the server to let them know the map changed.
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + "setGlobal";
                    var params = "global:"+global;
                    this.m_connector.sendCommand( cmd, params, function(){});
                }
            },this);
            this._add( this.m_globalCheck );
            
            this._add( new qx.ui.core.Spacer(), {flex:1});
            this.m_intensityHighLabel = new qx.ui.basic.Label("");
            this._add( this.m_intensityHighLabel );
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
            this.m_intensityLowLabel.setValue( minValue.toString() );
            this.m_intensityHighLabel.setValue( maxValue.toString() );
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
            var path = skel.widgets.Path.getInstance();
            var dataPath = this.m_id + path.SEP + "data";
            this.m_sharedVarData = this.m_connector.getSharedVar( dataPath );
            this.m_sharedVarData.addCB( this._colormapDataCB.bind( this));
            this._colormapDataCB();
        },
        
        /**
         * Server update as to whether this is a global colormap.
         * @param globalMap {boolean} - true if this is a global color map;
         *      false otherwise.
         */
        setGlobal : function( globalMap ){
            this.m_globalCheck.setValue( globalMap );
        },
        
        m_intensityLowLabel : null,
        m_intensityHighLabel : null,
       
        m_mapCombo : null,
        m_globalCheck : null,
        
        m_id : null,
        m_connector : null,
        m_sharedVarData : null,
        m_sharedVarMaps : null
    }
});