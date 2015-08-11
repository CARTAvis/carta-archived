/**
 * A slider and text field that work together to display/set the same value.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 ******************************************************************************/

//Event Path : TextField -> Slider -> Server -> TextField

qx.Class.define("skel.widgets.CustomUI.TextSlider", {
    extend : qx.ui.core.Widget, 
    
    /**
     * Constructor.
     * @param serverCmd {String} - the command to set the value on the server.
     * @param paramId {String} - the identifier for the value to be set on the server.
     * @param minValue {Number} - the minimum slider value.
     * @param maxValue {Number} - the maximum slider value.
     * @param defaultValue {Number} - the default value of the slider.
     * @param textLabel {String} - a user displayable label for the text field.
     * @param labelLeft {boolean} - true if the label should be displayed to the left of
     *          the text field; false if the label will be above the text field.
     * @param textHelp {String} - tool tip help for the text field.
     * @param sliderHelp {String} - tool tip help for the slider.
     * @param textTestId {String} - a test identifier for the text field.
     * @param sliderTestId {String} - a test identifier for the slider.
     * @param normalize {boolean} - make the value into a percentage before sending to server.
     */
    construct : function( serverCmd, paramId, minValue, maxValue, defaultValue,
            textLabel, labelLeft, textHelp, sliderHelp, textTestId, sliderTestId, normalize ) {
        this.base(arguments);
        this.m_cmd = serverCmd;
        this.m_paramId = paramId;
        this.m_normalize = normalize;
        if ( typeof mImport !== "undefined"){
            this.m_connector = mImport("connector");
        }
        this._init( defaultValue, minValue, maxValue, textLabel, labelLeft, textHelp, sliderHelp,
                textTestId, sliderTestId);
    },
    
    statics : {
        MAX_SLIDER : 1000
    },

    members : {
        /**
         * Return the value of the text/slider.
         * @return {String} the value of this widget.
         */
        getValue : function(){
            return this.m_text.getValue();
        },
        
        /**
         * Callback for a server error when setting the value.
         * @param anObject {skel.widgets.CustomUI.TextSlider}.
         */
        _errorCB : function( anObject ){
            return function( msg ){
                if ( msg !== null && msg.length > 0 ){
                    anObject.m_text.setError( true );
                }
                else {
                    var oldError = anObject.m_text.isError();
                    if ( oldError ){
                        anObject.m_text.setError( false );
                        var errorMan = skel.widgets.ErrorHandler.getInstance();
                        errorMan.clearErrors();
                    }
                }
            };
        },

        /**
         * Initializes the UI.
         * @param minValue {Number} - the minimum slider value.
         * @param maxValue {Number} - the maximum slider value.
         * @param defaultValue {Number} - the default value of the slider.
         * @param textLabel {String} - a user displayable label for the text field.
         * @param labelLeft {boolean} - true if the label should be displayed to the left of
         *          the text field; false if the label will be above the text field.
         * @param textHelp {String} - tool tip help for the text field.
         * @param sliderHelp {String} - tool tip help for the slider.
         * @param textTestId {String} - a test identifier for the text field.
         * @param sliderTestId {String} - a test identifier for the slider.
         */
        _init : function( defaultValue, minValue, maxValue, textLabel, labelLeft, textHelp, sliderHelp,
                textTestId, sliderTestId ) {
            var widgetLayout = new qx.ui.layout.VBox(2);
            this._setLayout(widgetLayout);
            
            //container
            var container = new qx.ui.container.Composite();
            var gridLayout = new qx.ui.layout.Grid();
            gridLayout.setColumnAlign( 0, "center", "middle");
            gridLayout.setSpacing( 1 );
            container.setLayout( gridLayout );
            
            //TextField
            this.m_text = new skel.widgets.CustomUI.NumericTextField(minValue, maxValue);
            this.m_text.setToolTipText( textHelp );
            this.m_text.addListener("textChanged",
                    function(ev) {
                        var value = this.m_text.getValue();
                        if ( !isNaN( value ) ){
                            this.m_slider.setValue( value );
                        }
                }, this);
            this.m_text.setTextId( textTestId );
            var widgetColumn = 0;
            var widgetRow = 1;
            if ( labelLeft ){
                widgetRow = 0;
                widgetColumn = 1;
            }
            container.add( this.m_text, {row:widgetRow, column:widgetColumn} );
            
            //Slider
            this.m_slider = new qx.ui.form.Slider();
            this.m_slider.setMinimum( minValue );
            this.m_slider.setMaximum( maxValue );
            this.m_slider.setValue( defaultValue );
            this.m_slider.setToolTipText( sliderHelp );
            this.m_listenerId = this.m_slider.addListener( skel.widgets.Path.CHANGE_VALUE, this._sendCmd, this);
            skel.widgets.TestID.addTestId( this.m_slider, sliderTestId );
            container.add( this.m_slider, {row:widgetRow+1, column:widgetColumn} );
            
            //Label
            var label = new qx.ui.basic.Label( textLabel + ":");
            
            if ( labelLeft  ){
                container.add( label, {row:0,column:0, rowSpan:2});
            }
            else {
                container.add( label, {row:0,column:0});
                label.setAlignX( "left");
            }
            this._add( container );
        },
        
        /**
         * Send a value change to the server.
         */
        _sendCmd : function(){
            if ( this.m_connector !== null ){
                //Notify the server of the new value.
                var value = this.m_slider.getValue();
                var percentValue = value;
                if ( this.m_normalize ){
                    percentValue = ( value - this.m_slider.getMinimum() ) / 
                        (this.m_slider.getMaximum() - this.m_slider.getMinimum());
                }
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_id + path.SEP_COMMAND + this.m_cmd;
                var params = this.m_paramId + ":"+percentValue;
                this.m_connector.sendCommand( cmd, params, this._errorCB(this));
            }
        },
        
        /**
         * Set an upper bound for the text box and the slider.
         * @param value {Number} - the maximum value for this widget.
         */
        setMax : function( value ){
            this.m_slider.setMaximum( value );
            this.m_text.setUpperBound( value );
        },
        
        
        /**
         * Set a new value.
         * @param value {Number} the new value.
         */
        setValue : function( value ){
            if ( value !== undefined ){
                var oldValue = this.m_text.getValue();
                this.m_slider.removeListenerById( this.m_listenerId);
                if ( value != oldValue ){
                    this.m_text.setValue( value );
                }
                //Someone may have moved the slider and got an error on the 
                //server so we may have to reset the slider.
                else {
                    if ( value != this.m_slider.getValue()){
                        this.m_slider.setValue( value );
                    }
                }
                this.m_listenerId = this.m_slider.addListener( skel.widgets.Path.CHANGE_VALUE, this._sendCmd, this);
            }
        },
        

        
        /**
         * Set the server side id.
         * @param id {String} the server side id.
         */
        setId : function( id ){
            this.m_id = id;
        },

        m_cmd : null,
        m_paramId : null,
        m_normalize : false,
        m_text : null,
        m_slider : null,
        m_listenerId : null,
        m_id : null,
        m_connector : null
        
    }
});