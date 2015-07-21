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
         * Callback for a server error when setting the invert flag.
         * @param anObject {skel.widgets.ColorMap.ColorScale}.
         */
        _errorInvertCB : function( anObject ){
            return function( invertMap ){
                if ( invertMap ){
                    var invertMapBool = skel.widgets.Util.toBool( invertMap );
                    anObject.setInvert( invertMapBool );
                }
            };
        },
        
        /**
         * Callback for a server error when setting the reverse flag.
         * @param anObject {skel.widgets.ColorMap.ColorScale}.
         */
        _errorReverseCB : function( anObject ){
            return function( reverseMap ){
                if ( reverseMap ){
                    var reverseMapBool = skel.widgets.Util.toBool( reverseMap );
                    anObject.setReverse( reverseMapBool );
                }
            };
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
            var widgetLayout = new qx.ui.layout.VBox();
            this._setLayout(widgetLayout);
            this._add( new qx.ui.core.Spacer(), {flex:1});
            
            this.m_invertCheck = new qx.ui.form.CheckBox( "Invert");
            this.m_invertCheck.setToolTipText( "Invert the colors in the map.");
            this.m_invertCheck.addListener( skel.widgets.Path.CHANGE_VALUE, function(e){
                var checked = e.getData();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Colormap.ColorScale.CMD_INVERT_MAP;
                var params = "invert:"+checked;
                this.m_connector.sendCommand( cmd, params, this._errorInvertCB( this ));
            }, this );
            this.m_reverseCheck = new qx.ui.form.CheckBox( "Reverse");
            this.m_reverseCheck.setToolTipText( "Reverse the colors in the map.");
            this.m_reverseCheck.addListener( skel.widgets.Path.CHANGE_VALUE, function(e){
                var checked = e.getData();
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Colormap.ColorScale.CMD_REVERSE_MAP;
                var params = "reverse:"+checked;
                this.m_connector.sendCommand( cmd, params, this._errorReverseCB( this ));
            }, this );
            
            var mapComposite = new qx.ui.container.Composite();
            mapComposite.setLayout(new qx.ui.layout.VBox(1));
            this.m_mapCombo = new qx.ui.form.ComboBox();
            this.m_mapCombo.setToolTipText( "Select a color map.");
            this.m_mapCombo.addListener( skel.widgets.Path.CHANGE_VALUE, function(e){
                if ( this.m_id !== null ){
                    var mapName = e.getData();
                    //Send a command to the server to let them know the map changed.
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Colormap.ColorScale.CMD_SET_MAP;
                    var params = "name:"+mapName;
                    this.m_connector.sendCommand( cmd, params, this._errorMapIndexCB( this ));
                }
            },this);
            var mapLayout = new qx.ui.layout.HBox(2);
            var comboComp = new qx.ui.container.Composite();
            comboComp.setLayout( mapLayout );
            comboComp.add( this.m_mapCombo );
            
            
            var revComp = new qx.ui.container.Composite();
            revComp.setLayout( new qx.ui.layout.HBox());
            revComp.add( this.m_reverseCheck );
            revComp.add( new qx.ui.core.Spacer(1), {flex:1});
            mapComposite.add( revComp );
            
            var invertComp = new qx.ui.container.Composite();
            invertComp.setLayout( new qx.ui.layout.HBox());
            invertComp.add( this.m_invertCheck );
            invertComp.add( new qx.ui.core.Spacer(1), {flex:1});
            mapComposite.add( invertComp );
            
            mapComposite.add( comboComp );
            this._add( mapComposite );
            this._add( new qx.ui.core.Spacer(), {flex:1});
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
            var checkVal = this.m_invertCheck.getValue();
            if ( invertMap != checkVal ){
                this.m_invertCheck.setValue( invertMap );
            }
        },
        
        
        /**
         * Set the selected color map.
         * @param mapName {String} the name of the selected color map.
         */
        setMapName : function( mapName ){
            var selectables = this.m_mapCombo.getChildrenContainer().getSelectables();
            var currValue = this.m_mapCombo.getValue();
            for ( var i = 0; i < selectables.length; i++ ){
                var mapItem = selectables[i];
                var newValue = selectables[i].getLabel();
                if ( newValue == mapName ){
                    if ( currValue != newValue ){
                        this.m_mapCombo.setValue( newValue );
                    }
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