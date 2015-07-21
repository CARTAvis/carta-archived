/**
 * Controls for how the histogram should be displayed.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

qx.Class.define("skel.widgets.Histogram.HistogramDisplay", {
    extend : qx.ui.core.Widget,

    construct : function( ) {
        this.base(arguments);
        this.m_connector = mImport("connector");
        this._init( );
    },
    
    statics : {
        CMD_SET_GRAPH_STYLE : "setGraphStyle",
        CMD_SET_LOG_COUNT : "setLogCount",
        CMD_SET_COLORED : "setColored"
    },

    members : {
        
        
        
        /**
         * Initializes the UI.
         */
        _init : function( ) {
            var widgetLayout = new qx.ui.layout.VBox(1);
            this._setLayout(widgetLayout);
            this.m_displayContainer = new qx.ui.groupbox.GroupBox("Display", "");
            this.m_displayContainer.setContentPadding(1,1,1,1);
            this.m_displayContainer.setLayout( new qx.ui.layout.VBox(1));
            this._add( this.m_displayContainer );
            this._initStyle();
            this._initOptions();
        },
        
        /**
         * Initialize log and color options for the histogram.
         */
        _initOptions : function(){
            this.m_logCheck = new qx.ui.form.CheckBox( "Log(Count)");
            this.m_logCheck.setToolTipText( "Use a log scale for the vertical axis.");
            this.m_logCheck.addListener( skel.widgets.Path.CHANGE_VALUE, function(){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramDisplay.CMD_SET_LOG_COUNT;
                var params = "logCount:"+this.m_logCheck.getValue();
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this);
            this.m_coloredCheck = new qx.ui.form.CheckBox( "Colored");
            this.m_coloredCheck.setToolTipText("Color the histogram based on intensity.");
            this.m_coloredCheck.addListener( skel.widgets.Path.CHANGE_VALUE, function(){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramDisplay.CMD_SET_COLORED;
                var params = "colored:"+this.m_coloredCheck.getValue();
                this.m_connector.sendCommand( cmd, params, function(){});
            }, this);
            this.m_displayContainer.add( this.m_logCheck );
            this.m_displayContainer.add( this.m_coloredCheck );
        },
        
        /**
         * Initialize whether to use a line, outline, or fill style for display points.
         */
        _initStyle : function(){
            
            this.m_lineRadio = new qx.ui.form.RadioButton( "Line");
            this.m_lineRadio.setToolTipText( "Draw using vertical lines.");
            this.m_lineRadio.addListener( skel.widgets.Path.CHANGE_VALUE, function(){
                if ( this.m_lineRadio.getValue() ){
                    this._sendStyleChangedCmd( this.m_lineRadio.getLabel());
                }
            }, this );
            this.m_barRadio = new qx.ui.form.RadioButton( "Outline");
            this.m_barRadio.setToolTipText( "Draw using an outline.");
            this.m_barRadio.addListener( skel.widgets.Path.CHANGE_VALUE, function(){
                if ( this.m_barRadio.getValue() ){
                    this._sendStyleChangedCmd( this.m_barRadio.getLabel());
                }
            }, this );
            this.m_fillRadio = new qx.ui.form.RadioButton( "Fill");
            this.m_fillRadio.setToolTipText( "Draw using solid bars.");
            this.m_fillRadio.addListener( skel.widgets.Path.CHANGE_VALUE, function(){
                if ( this.m_fillRadio.getValue() ){
                    this._sendStyleChangedCmd( this.m_fillRadio.getLabel());
                }
            }, this );
           
            var styleGroupRadio = new qx.ui.form.RadioGroup();
            styleGroupRadio.add( this.m_lineRadio, this.m_barRadio, this.m_fillRadio );
         
            this.m_displayContainer.add( this.m_lineRadio );
            this.m_displayContainer.add( this.m_barRadio );
            this.m_displayContainer.add( this.m_fillRadio );
        },
        
        /**
         * Notify the server that the histogram graph style has changed.
         * @param style {String} the new graph style.
         */
        _sendStyleChangedCmd : function( style ){
            if ( this.m_id !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + skel.widgets.Histogram.HistogramDisplay.CMD_SET_GRAPH_STYLE;
                var params = "graphStyle:"+style;
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Set whether the pen should reflect the color scale or remain a single color.
         * @param colored {boolean} true if the pen should change color based on the color
         *      scale; false otherwise.
         */
        setColored : function( colored ){
            if ( this.m_coloredCheck.getValue() != colored ){
                this.m_coloredCheck.setValue( colored );
            }
        },
        
        /**
         * Set the server side id of this histogram.
         * @param id {String} the server side id of the object that produced this histogram.
         */
        setId : function( id ){
            this.m_id = id;
        },
        
        /**
         * Set whether or not to use a logarithmic scale for the count axis.
         * @param logCount {boolean} true if a log scale should be used; false otherwise.
         */
        setLogCount : function( logCount ){
            if ( this.m_logCheck.getValue() != logCount ){
                this.m_logCheck.setValue( logCount );
            }
        },
        
        /**
         * Set the draw style for the histogram.
         * @param style {String} a description for the drawing style.
         */
        setStyle : function (style){
            if ( style == this.m_lineRadio.getLabel()){
                if ( !this.m_lineRadio.getValue()){
                    this.m_lineRadio.setValue( true );
                }
            }
            else if ( style == this.m_barRadio.getLabel()){
                if ( !this.m_barRadio.getValue()){
                    this.m_barRadio.setValue( true );
                }
            }
            else if ( style == this.m_fillRadio.getLabel()){
                if ( !this.m_fillRadio.getValue()){
                    this.m_fillRadio.setValue( true );
                }
            }
            else {
                console.log( "Unrecognized histogram style: "+style);
            }
        },
        
        m_id : null,
        m_connector : null,
        m_lineRadio : null,
        m_barRadio : null,
        m_fillRadio : null,
        m_logCheck : null,
        m_coloredCheck : null,
        m_displayContainer : null
    },
    
    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        }
    }
});