/**
 * Manages caching properties for the color map.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Colormap.ColorCache", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     */
    construct : function( ) {
        this.base(arguments);
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
        }
        this._init( );
       
    },
    
    statics : {
        CMD_CACHE : "cacheColormap",
        CMD_INTERPOLATE : "interpolatedColormap",
        CMD_CACHE_SIZE : "setCacheSize"
    },
    
    members : {
        
        /**
         * Callback for a server error when setting the cache.
         * @param anObject {skel.widgets.ColorMap.ColorCache}.
         */
        _errorCacheCB : function( anObject ){
            return function( cache ){
                if ( cache ){
                    var cacheBool = skel.widgets.Util.toBool( cache );
                    if ( anObject.m_cacheCheck.getValue() != cacheBool ){
                        anObject.m_cacheCheck.setValue( cacheBool );
                    }
                }
            };
        },
    
        /**
         * Callback for a server error when setting the interpolated pixel map value.
         * @param anObject {skel.widgets.ColorMap.ColorCache}.
         */
        _errorInterpolateCB :function( anObject ){
            return function( interpolate ){
                if ( interpolate ){
                    var interpolateBool = skel.widgets.Util.toBool( interpolate );
                    if ( anObject.m_interpolateCheck.getValue() != interpolateBool ){
                        anObject.m_interpolateCheck.setValue( interpolateBool );
                    }
                }
            };
        },
        
        /**
         * Callback for a server error when setting the cache size.
         * @param anObject {skel.widgets.ColorMap.ColorCache}.
         */
        _errorSizeCB : function( anObject ){
            return function( size ){
                if ( size ){
                    var oldSize = anObject.m_cacheSizeText.getValue();
                    if ( oldSize != size ){
                        anObject.m_cacheSizeText.setValue( size );
                    }
                }
            };
        },
        
        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            var widgetLayout = new qx.ui.layout.VBox();
            this._setLayout(widgetLayout);
            
            this.m_cacheCheck = new qx.ui.form.CheckBox( "Pixel Cache");
            this.m_cacheCheck.addListener( "changeValue", function(e){
                var checked = e.getData();
                if ( this.m_connector !== null ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Colormap.ColorCache.CMD_CACHE;
                    var params = "cacheMap:"+checked;
                    this.m_connector.sendCommand( cmd, params, this._errorCacheCB( this ) );
                }
                this.m_interpolateCheck.setEnabled( checked );
                this.m_cacheSizeText.setEnabled( checked );
            }, this );
            
            this.m_interpolateCheck = new qx.ui.form.CheckBox( "Interpolation");
            this.m_interpolateCheck.addListener( "changeValue", function(e){
                if ( this.m_connector !== null ){
                    var checked = e.getData();
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Colormap.ColorCache.CMD_INTERPOLATE;
                    var params = "interpolatedCaching:"+checked;
                    this.m_connector.sendCommand( cmd, params, this._errorInterpolateCB( this ));
                }
            }, this );
            
            var mapLabel = new qx.ui.basic.Label( "Size:");
            this.m_cacheSizeText = new skel.widgets.CustomUI.NumericTextField( 0, null );
            this.m_cacheSizeText.setIntegerOnly( true );
            this.m_cacheSizeText.addListener( "textChanged", function(e){
                if ( this.m_connector !== null ){
                    var cacheSize = this.m_cacheSizeText.getValue();
                    //Send a command to the server to let them know the map changed.
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Colormap.ColorCache.CMD_CACHE_SIZE;
                    var params = "cacheSize:"+cacheSize;
                    this.m_connector.sendCommand( cmd, params, this._errorSizeCB( this));
                }
            },this);
            
            var sizeComposite = new qx.ui.container.Composite();
            sizeComposite.setLayout( new qx.ui.layout.HBox(2));
            sizeComposite.add( mapLabel );
            sizeComposite.add( this.m_cacheSizeText );
            
            this.m_interpolateCheck.setEnabled( this.m_cacheCheck.getValue() );
            this.m_cacheSizeText.setEnabled( this.m_cacheCheck.getValue() );
            
            var mapComposite = new qx.ui.groupbox.GroupBox( "Caching");
            mapComposite.setLayout(new qx.ui.layout.VBox(2));
            mapComposite.add( this.m_cacheCheck );
            mapComposite.add( this.m_interpolateCheck );
            mapComposite.add( sizeComposite );
            this._add( mapComposite );
        },
        
        
        /**
         * Set whether or not to cache the color map.
         * @param cacheMap {boolean} true if the map should be cached; false otherwise.
         */
        setCache : function( cacheMap ){
            if ( cacheMap != this.m_cacheCheck.getValue()){
                this.m_cacheCheck.setValue( cacheMap );
            }
        },
        
        
        /**
         * Set whether to interpolate the color map.
         * @param interpolateMap {boolean} true to interpolate the color map; false othewise.
         */
        setInterpolate : function( interpolateMap ){
            if ( interpolateMap != this.m_interpolateCheck.getValue()){
                this.m_interpolateCheck.setValue( interpolateMap );
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
         * Set the size of the cache.
         * @param cacheSize {Number} the size of the cache.
         */
        setCacheSize : function( cacheSize ){
            if ( cacheSize != this.m_cacheSizeText.getValue()){
                this.m_cacheSizeText.setValue( cacheSize );
            }
        },
        
        
        m_cacheCheck : null,
        m_interpolateCheck : null,
        m_cacheSizeText : null,
        
        m_id : null,
        m_connector : null,
        m_sharedVarMaps : null
       

    },
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});