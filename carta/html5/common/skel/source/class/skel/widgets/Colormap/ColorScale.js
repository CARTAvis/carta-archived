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
    },
    
    statics : {
        CMD_INVERT_MAP : "invertColormap",
        CMD_REVERSE_MAP : "reverseColormap"
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
         * Whether or not color map changes should be applied to all color maps
         * has changed in the UI.
         */
        _globalChanged : function(){
            if ( this.m_id !== null ){
                var global = this.m_globalCheck.getValue();
                //Send a command to the server to let them know the map changed.
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + "setGlobal";
                var params = "global:"+global;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        
        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            var widgetLayout = new qx.ui.layout.VBox();
            this._setLayout(widgetLayout);
            
            this._initInvertReverse();
            this._initApplyAll();
        },
        
        
        /**
         * Initialize the 'Apply All' functionality.
         */
        _initApplyAll : function(){
            this.m_globalCheck = new qx.ui.form.CheckBox( "Apply All");
            this.m_globalCheck.setToolTipText( "Apply color map settings to all images in the stack.");
            skel.widgets.TestID.addTestId( this.m_globalCheck, "colorMapGlobal");
            this.m_globalListenId = this.m_globalCheck.addListener( skel.widgets.Path.CHANGE_VALUE, this._globalChanged, this );
            var allComposite = new qx.ui.container.Composite();
            allComposite.setLayout(new qx.ui.layout.HBox(1));
            allComposite.add( new qx.ui.core.Spacer(1), {flex:1});
            allComposite.add( this.m_globalCheck );
            allComposite.add( new qx.ui.core.Spacer(1), {flex:1});
            this._add( allComposite );
        },
        
        /**
         * Initialize the revert & invert controls.
         */
        _initInvertReverse : function(){
            this.m_invertCheck = new qx.ui.form.CheckBox( "Invert");
            this.m_invertCheck.setToolTipText( "Invert the colors in the map.");
            skel.widgets.TestID.addTestId( this.m_invertCheck, "colorMapInvert");
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
            mapComposite.setLayout(new qx.ui.layout.HBox(1));
            mapComposite.add( new qx.ui.core.Spacer(1), {flex:1});
            mapComposite.add( this.m_reverseCheck );
            mapComposite.add( this.m_invertCheck );
            mapComposite.add( new qx.ui.core.Spacer(1), {flex:1});
            this._add( mapComposite );
        },
        
        /**
         * Server update as to whether this is a global colormap.
         * @param globalMap {boolean} - true if this is a global color map;
         *      false otherwise.
         */
        setGlobal : function( globalMap ){
            if ( this.m_globalCheck.getValue() != globalMap ){
                if ( this.m_globalListenId != null ){
                    this.m_globalCheck.removeListenerById( this.m_globalListenId );
                }
                this.m_globalCheck.setValue( globalMap );
                this.m_globalListenId = this.m_globalCheck.addListener( skel.widgets.Path.CHANGE_VALUE, this._globalChanged, this );
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
        
        m_globalCheck : null,
        m_globalListenId : null,
        m_invertCheck : null,
        m_reverseCheck : null,
        
        m_id : null,
        m_connector : null
    }
});