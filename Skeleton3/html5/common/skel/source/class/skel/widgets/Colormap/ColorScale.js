/**
 * Provides color map selection and display options (invert, reverse, etc).
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColorScale", {
    extend : qx.ui.core.Widget,

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
        CMD_INVERT_MAP : "invertColormap",
        CMD_REVERSE_MAP : "reverseColormap",
        CMD_SET_MAP : "setColormap"
    },
    
    events : {
        "colorIndexChanged" : "qx.event.type.Data"
    },

    members : {

        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            var widgetLayout = new qx.ui.layout.Grow();
            this._setLayout(widgetLayout);
            
            this.m_invertCheck = new qx.ui.form.CheckBox( "Invert");
            this.m_invertCheck.addListener( "changeValue", function(e){
                var checked = e.getData();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Colormap.ColorScale.CMD_INVERT_MAP;
                var params = "invert:"+checked;
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this );
            this.m_reverseCheck = new qx.ui.form.CheckBox( "Reverse");
            this.m_reverseCheck.addListener( "changeValue", function(e){
                var checked = e.getData();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Colormap.ColorScale.CMD_REVERSE_MAP;
                var params = "reverse:"+checked;
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this );
            
            
            var mapComposite = new qx.ui.container.Composite();
            mapComposite.setLayout(new qx.ui.layout.HBox(2));
            var mapLabel = new qx.ui.basic.Label( "Map:");
            this.m_mapCombo = new qx.ui.form.ComboBox();
            this.m_mapCombo.addListener( "changeValue", function(e){
                var mapName = e.getData();
                //Send a command to the server to let them know the map changed.
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Colormap.ColorScale.CMD_SET_MAP;
                var params = "name:"+mapName;
                this.m_connector.sendCommand( cmd, params, function(){});
            },this);
            mapComposite.add( new qx.ui.core.Spacer(50));
            mapComposite.add( mapLabel );
            mapComposite.add( this.m_mapCombo );
            mapComposite.add( this.m_reverseCheck );
            mapComposite.add( this.m_invertCheck );
            mapComposite.add( new qx.ui.core.Spacer(50));
            this._add( mapComposite );
           
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
                        var mapCount = colorMaps.colorMapCount;
                        this.m_mapCombo.removeAll();
                        for ( var i = 0; i < mapCount; i++ ){
                            var colorMapName = colorMaps.maps[i];
                            var tempItem = new qx.ui.form.ListItem( colorMapName );
                            this.m_mapCombo.add( tempItem );
                        }
                        //Try to reset the old selection
                        if ( oldName !== null ){
                            this.m_mapCombo.setValue( oldName );
                        }
                        //Select the first item
                        else if ( mapCount > 0 ){
                            var selectables = this.m_mapCombo.getChildrenContainer().getSelectables(true);
                            if ( selectables.length > 0 ){
                                this.m_mapCombo.setValue( selectables[0].getLabel());
                            }
                        }
                    }
                    catch( err ){
                        console.log( "Could not parse: "+val );
                    }
                }
            }
        },
        
        /**
         * Set whether or not to invert the color map.
         * @param invertMap {boolean} true if the map should be inverted; false otherwise.
         */
        setInvert : function( invertMap ){
            if ( invertMap != this.m_invertCheck.getValue()){
                this.m_invertCheck.setValue( invertMap );
            }
        },
        
        
        /**
         * Set the selected color map.
         * @param mapIndex {Number} the index of the selected color map.
         */
        setMapIndex : function( mapIndex ){
            var selectables = this.m_mapCombo.getChildrenContainer().getSelectables();
            if ( 0 <= mapIndex && mapIndex < selectables.length ){
               
                var mapItem = selectables[mapIndex];
                var currValue = this.m_mapCombo.getValue();
                var newValue = mapItem.getLabel();
                if ( currValue != newValue ){
                    this.m_mapCombo.setValue( newValue );
                    
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
        
        /**
         * Set whether or not to reverse the colors in the map.
         * @param reverseMap {boolean} true if the map should be reversed; false otherwise.
         */
        setReverse : function( reverseMap ){
            if ( reverseMap != this.m_reverseCheck.getValue()){
                this.m_reverseCheck.setValue(  reverseMap );
            }
        },
        
        
        m_invertCheck : null,
        m_reverseCheck : null,
        m_mapCombo : null,
        
        m_id : null,
        m_connector : null,
        m_sharedVarMaps : null
       

    }
});