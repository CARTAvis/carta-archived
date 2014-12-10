/**
 * Manages the controls for a histogram's 2D foot print.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.Histogram2D", {
    extend : qx.ui.core.Widget,

    construct : function(  ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init( );
    },
    
    statics : {
        CMD_SET_2D_FOOTPRINT : "set2DFootPrint"
    },

    members : {
        
        /**
         * Initializes the UI.
         */
        _init : function(  ) {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
            this._initFootPrint();
        },
        
        /**
         * Initialize the 2D foot print settings.
         */
        _initFootPrint : function(){
            var footPrintGroup = new qx.ui.groupbox.GroupBox( "2D Foot Print");
            footPrintGroup.setLayout( new qx.ui.layout.VBox(2));
            
            this.m_imageRadio = new qx.ui.form.RadioButton( "Image");
            this.m_imageRadio.addListener( "changeValue", function(){
                if ( this.m_imageRadio.getValue() ){
                    this._sendFootPrintCmd( this.m_imageRadio.getLabel());
                }
            }, this );
            this.m_regionRadio = new qx.ui.form.RadioButton( "Selected Region");
            this.m_regionRadio.addListener( "changeValue", function(){
                if ( this.m_regionRadio.getValue()){
                    this._sendFootPrintCmd( this.m_regionRadio.getLabel());
                }
            }, this );
            this.m_regionAllRadio = new qx.ui.form.RadioButton( "All Regions");
            this.m_regionAllRadio.addListener( "changeValue", function(){
                if ( this.m_regionAllRadio.getValue()){
                    this._sendFootPrintCmd( this.m_regionAllRadio.getLabel());
                }
            }, this );
            var footPrintRadio = new qx.ui.form.RadioGroup();
            footPrintRadio.add( this.m_imageRadio, this.m_regionRadio, this.m_regionAllRadio );
            
            footPrintGroup.add( this.m_imageRadio );
            footPrintGroup.add( this.m_regionRadio );
            footPrintGroup.add( this.m_regionAllRadio );
            this._add( footPrintGroup );
        },
        
        /**
         * Notify the server that the 2D footprint has changed.
         */
        _sendFootPrintCmd : function( footPrint ){
            var path = skel.widgets.Path.getInstance();
            var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.Histogram2D.CMD_SET_2D_FOOTPRINT;
            var params = "twoDFootPrint:"+footPrint;
            this.m_connector.sendCommand( cmd, params, function(){});
        },
        
        /**
         * Set a new 2D foot print.
         * @param twoDFootPrint {String} the new 2D foot print settings.
         */
        setFootPrint : function( twoDFootPrint ){
            if ( twoDFootPrint == this.m_imageRadio.getLabel()){
                if ( !this.m_imageRadio.getValue()){
                    this.m_imageRadio.setValue( true );
                }
            }
            else if ( twoDFootPrint == this.m_regionRadio.getLabel()){
                if ( ! this.m_regionRadio.getValue()){
                    this.m_regionRadio.setValue( true );
                }
            }
            else if ( twoDFootPrint == this.m_regionAllRadio.getLabel()){
                if ( ! this.m_regionAllRadio.getValue()){
                    this.m_regionAllRadio.setValue( true );
                }
            }
            else {
                console.log( "Unrecognized 2D footprint: "+twoDFootPrint );
            }
        },
        
        /**
         * Set the server side id of this histogram.
         * @param id {String} the server side id of the object that produced this histogram.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        m_id : null,
        m_connector : null,
        m_imageRadio : null,
        m_regionRadio : null,
        m_regionAllRadio : null
    }
});