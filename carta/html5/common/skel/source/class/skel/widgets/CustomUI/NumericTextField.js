/**
 * Allows only numeric input
 */


qx.Class.define( "skel.widgets.CustomUI.NumericTextField",
    {
        extend: qx.ui.container.Composite,

        /**
         * Constructor.
         * @param minValue {Number} the minimum value allowed in the text field.
         * @param maxValue {Number} the maximum value allowed in the text field.
         */
        construct: function ( minValue, maxValue ) {
            this.base(arguments);
            this.m_minValue = minValue;
            this.m_maxValue = maxValue;
            this.m_text = new skel.widgets.CustomUI.ErrorTextField();
            this.m_textChangedValue = false;
            this.setIntegerOnly( true );
            //Only set the value in the textfield if it passes numeric validation.

            this.m_text.addListener( "keyinput", function( evt ){
                var charKey = evt.getChar();
                if ( charKey !== '.' || charKey !== '0' || charKey !=='1'||charKey !=='2' || charKey !=='3'|| 
                        charKey !=='4'|| charKey !== '5'|| charKey !== '6' || charKey !== '7' || charKey !== '8'||
                    charKey !== '9' ){
                        var tValue = this.m_text.getValue();
                        var textValue = charKey;
                        if ( tValue !== null){
                            textValue = tValue + charKey;
                        }
                        if ( textValue !== "-"){
                            var numValue = parseFloat( textValue );
                            if ( ! this._isValidValue( numValue) ){
                                evt.preventDefault();
                            }
                        }
                }
            }, this );
            this.m_text.addListener( "keypress", function( evt){
                var enterKey = false;
                if ( evt.getKeyIdentifier().toLowerCase() =="enter"){
                    enterKey = true;
                }
                var textVal = this.m_text.getValue();
                this.setValueFire( textVal, enterKey );
            }, this );
            
            this.m_text.addListener( "focusout", function( evt ){
                if ( this.m_textChangedValue ){
                    var textVal = this.m_text.getValue();
                    this.fireDataEvent( "textChanged", textVal );
                    this.m_textChangedValue = false;
                }
            }, this );

            //Range validation when field changes value
            this.m_text.addListener( skel.widgets.Path.CHANGE_VALUE, function( evt ){
                var numValue = this.getValue();
                this._checkValue( numValue );
            }, this );
            this.m_warning = new qx.ui.basic.Label();
            this.m_warning.setTextColor(skel.theme.Color.colors.error);

            this._layoutControls();
        },
        
        events : {
            "textChanged" : "qx.event.type.Data"
        },

        members: {
            /**
             * Returns true if the passed in value is in bounds; otherwise returns false and posts an error.
             * @param num {Number} a value to check.
             * @return {boolean} true if the value is valid; false otherwise.
             */
            _checkValue: function ( num ) {
                var valid = false;
                var validRange = this._isValidRange( num );
                if ( validRange && !this.m_text.isError()){
                   this.clearWarning();
                   valid = true;
                }
                else if (!validRange ){
                    var warningText = "Range error:"+num;
                    if ( this.m_minValue !== null ){
                        warningText = warningText +" is not at least "+this.m_minValue;
                    }
                    if ( this.m_maxValue !== null ){
                        if ( this.m_minValue !== null ){
                            warningText = warningText + " and ";
                        }
                        warningText = warningText + " at most "+this.m_maxValue;
                    }
                    this.postWarning(warningText);
                }
                return valid;
            },
            
            /**
             * Remove the invalid value warning.
             */
            clearWarning : function(){
                if ( this.indexOf( this.m_warning) >= 0 ){
                    this.remove( this.m_warning);
                }
                this.m_text.setError( false );
            },
            
            /**
             * Return the value of the text field as a Number or NaN if the value is not a valid number.
             * @return {Number} the numeric value of the text field.
             */
            getValue : function(){
                var valueStr = this.m_text.getValue();
                var num = null;
                if ( valueStr !== null ){
                   num = parseFloat(valueStr);
                }
                return num;
            },
            
            /**
             * Returns whether or not there is an error in the text field.
             * @return {boolean} true if there is an error; false otherwise.
             */
            isError : function(){
                return this.m_text.isError();
            },
            
            /**
             * Returns whether or not the number is an integer.
             * @param num {Number} the number to check.
             * @return {boolean} true if the number is a valid integer; false otherwise.
             */
            _isInteger : function( num ){
                return num==Math.floor( num );
            },
            
            /**
             * Returns true if the parameter is a valid number; false otherwise.
             * @param num {Number}, which could be a NaN.
             * @return {boolean} true if num is a valid number; false otherwise.
             */
            _isNumeric : function( num ){
               return !isNaN(parseFloat(num)) && isFinite(num);
            },
            
            /**
             * Returns true if the parameter lies within any preset range restrictions;
             * false if there are not restrictions or the parameter is outside the preset range.
             * @param num {Number} the value whose range should be checked.
             * @return {boolean} true if there is an out-of-bounds problem; false otherwise.
             */
            _isValidRange : function( num ){
                var validRange = false;
                if ( this.m_minValue === null || this.m_minValue <= num ){
                    if ( this.m_maxValue === null ||  num <= this.m_maxValue ){
                        validRange = true;
                    }
                }
                return validRange;
            },
            
            /**
             * Returns true if the parameter is numeric; false otherwise.
             * @param value {Number} the value to check.
             * @return {boolean} true if the value is numeric; false otherwise.
             */
            _isValidValue : function( value ){
                var validValue = false;
                if ( this._isNumeric( value ) ){
                    if ( this.m_acceptFloat || this._isInteger( value )){
                       validValue = true;
                    }
                }
                return validValue;
            },
            
            /**
             * Returns whether or not the text field is showing a warning.
             * @return {boolean} - true if the text field is showing an error; false,
             * 		otherwise.
             */
            _isWarning : function(){
            	var warning = false;
            	if ( this.indexOf( this.m_warning) >= 0 ){
                    warning = true;
                }
            	return warning;
            },
            
            /**
             * Layout the text field.
             */
            _layoutControls : function(){
                this.setLayout( new qx.ui.layout.VBox(2));
                this.add( this.m_text );
            },
            
            /**
             * Add a warning to the display.
             * @param warningStr {String} the contents of the warning.
             */
            postWarning : function( warningStr ){
                this.remove( this.m_text );
                if ( this.indexOf( this.m_warning) == -1 ){
                    this.add( this.m_warning);
                }
                this.m_warning.setValue( warningStr );
                this.add( this.m_text);
            },
            
            /**
             * Set the numeric text field enabled/disabled.
             * @param enabled {boolean} - true to enable the text field; 
             *          false to disable it.
             */
            setTextEnabled : function( enabled ){
                this.m_text.setEnabled( enabled );
            },
            
            /**
             * Set or clear the error status of the textfield.
             * @param inError {boolean} true if the there are no errors; false otherwise.
             */
            setError : function( inError ){
                this.m_text.setError( inError );
            },
            
            /**
             * Set whether or not this numeric field should accept floating point numbers or just integers.
             * @param onlyIntegers {boolean} true if only integers are acceptable; 
             *          false if decimal numbers are allowed.
             */
            setIntegerOnly : function( onlyIntegers ){
                this.m_acceptFloat = !onlyIntegers;
                if ( this.m_acceptFloat ){
                    this.m_text.setFilter( /([0-9]|\.|[+-])/);
                }
                else {
                    this.m_text.setFilter( /([0-9]|[+-])/ );
                }
            },
            
            /**
             * Set a lower (inclusive) bound for valid values.
             * @param bound {Number} a lower bound.
             */
            setLowerBound : function( bound ){
                this.m_minValue = bound;
            },
            
            /**
             * Set an id for the text box.
             * @param id {String} a testing identifier.
             */
            setTextId : function( id ){
                skel.widgets.TestID.addTestId( this.m_text, id);
            },
            
            /**
             * Set an upper (inclusive) bound for valid values.
             * @param bound {Number} an upper bound.
             */
            setUpperBound : function( bound ){
                this.m_maxValue = bound;
            },
            
            /**
             * Set the value into the text field, assuming it meets all specifications; otherwise,
             * do nothing.
             * @param value {Number} the value to be set into the text field.
             */
            _setValidValue : function ( value ){
                if ( this._isValidValue(value)){
                    this.m_text.setValue( value.toString());
                    this.fireDataEvent("textChanged", value);
                }
            },
            
            /**
             * Set the value of the text field programmatically.
             * @param val {Number} the new text field value.
             */
            setValue : function( val ){
                this._setValidValue( val );
            },
            
            setValueFire : function( val, fireEvent ){
                if ( this._isValidValue( val )){
                    this.m_text.setValue( val.toString());
                    if ( fireEvent ){
                        this.fireDataEvent( "textChanged", val);
                        this.m_textChangedValue = false;
                    }
                    else {
                        this.m_textChangedValue = true;
                    }
                }
            },
            
            m_minValue : null,
            m_maxValue : null,
            m_text : null,
            m_textChangedValue : null,
            m_acceptFloat : false,
            m_warning : null
        }

    } );

