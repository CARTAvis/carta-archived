/**
 * A custom color chooser.
 */
/*******************************************************************************
 * @asset(skel/decoration/*)
 * @asset(skel/decoration/hue-128.png)
 * @asset(skel/decoration/brightness-field-128.png)
 * @asset(skel/decoration/brightness-handle-128.gif)
 *******************************************************************************/
qx.Class.define( "skel.widgets.CustomUI.ColorSelector",
    {
        extend: qx.ui.control.ColorSelector,

        /**
         * Constructor.
         */
        construct: function ( ) {
            
            this.base( arguments);
            this.setPadding( 0, 0, 0, 0 );
            this.getChildControl( "hex-field-composite").exclude();
            this.getChildControl( "hsb-spinner-composite").exclude();
            this.getChildControl( "preview-field-set").exclude();
        },
       

        members: {
            
             // property apply
             _applyBrightness : function(value, old){
                 try {
                      if (this.__updateContext === null) {
                          this.__updateContext = "brightnessModifier";
                      }
        
                      if (this.__updateContext !== "hsbSpinner") {
                          this.getChildControl("hsb-spinner-brightness").setValue(value);
                      }
                      
                      if (this.__updateContext !== "brightnessField" ){
                        var brightValue = this._getBrightnessPosition( value );
                        if (this.getChildControl("brightness-handle").getBounds()) {
                          this.getChildControl("brightness-handle").setDomTop(brightValue + this.getChildControl("brightness-pane").getPaddingTop());
                        } else {
                          this.getChildControl("brightness-handle").setLayoutProperties({ top : brightValue });
                        }
                      }
                      
                      switch(this.__updateContext){
                        case "hsbSpinner":
                        case "brightnessField":
                        case "brightnessModifier":
                            this._setRgbFromHue();
                      }
        
                      if (this.__updateContext === "brightnessModifier") {
                        this.__updateContext = null;
                      }
                 }
                 catch( err ){
                     console.log( "applyBright error"+err);
                 }
            },
            
            // property apply
            _applyHue : function(value, old){
                if ( this.__updateContext == "rgbSpinner"){
                    this.m_brightnessUpdate = false;
                }
                if ( this.m_brightnessUpdate ){
                   if ( value !== old ){
                       return;
                   }
                }
                try {
                   if (this.__updateContext === null) {
                      this.__updateContext = "hueModifier";
                   }
    
                   if (this.__updateContext !== "hsbSpinner") {
                      this.getChildControl("hsb-spinner-hue").setValue(value);
                   }
    
                   if (this.__updateContext !== "hueSaturationField" ){
                      //Hue in [0,360].  We must map it to [0,128].  360/128 = 2.8125, the minimum
                      //amount to actually do a move.{
                      var domLeft = this._getHuePosition( value );
                      if (this.getChildControl("hue-saturation-handle").getBounds()) {
                          var paddingLeft = this.getChildControl("hue-saturation-pane").getPaddingLeft();
                         this.getChildControl("hue-saturation-handle").setDomLeft(domLeft + paddingLeft);
     
                      } 
                      else {
                          this.getChildControl("hue-saturation-handle").setLayoutProperties({ left : domLeft });
                      }
                   }
                  
                   switch(this.__updateContext)
                   {
                    case "hsbSpinner":
                    case "hueSaturationField":
                    case "hueModifier":
                        this._setRgbFromHue();
                   }
                  
                   if (this.__updateContext === "hueModifier") {
                      this.__updateContext = null;
                   }
                }
                catch( err ){
                    console.log( "_applyHue err"+err);
                }
            },
            
            
            // property apply
            _applySaturation : function(value, old){
                if ( this.__updateContext == "rgbSpinner"){
                    this.m_brightnessUpdate = false;
                }
                if ( this.m_brightnessUpdate ){
                    if ( value !== old ){
                        //this.setSaturation( old );
                        return;
                    }
                }
                try {
                  if (this.__updateContext === null) {
                    this.__updateContext = "saturationModifier";
                  }
    
                  if (this.__updateContext !== "hsbSpinner") {
                    this.getChildControl("hsb-spinner-saturation").setValue(value);
                  }
                  
                  if (this.__updateContext !== "hueSaturationField"){
                     var satVal = this._getSaturationPosition( value );
                     if (this.getChildControl("hue-saturation-handle").getBounds()) {
                         this.getChildControl("hue-saturation-handle").setDomTop( satVal + 
                              this.getChildControl("hue-saturation-pane").getPaddingTop());
                      } 
                      else {
                          this.getChildControl("hue-saturation-handle").setLayoutProperties({ top : satVal });
                      }
                  }
                
                  switch(this.__updateContext)
                  {
                    case "hsbSpinner":
                    case "hueSaturationField":
                    case "saturationModifier":
                      this._setRgbFromHue();
                  }
    
                  if (this.__updateContext === "saturationModifier") {
                    this.__updateContext = null;
                  }
                }
                catch( err ){
                    console.log( "Apply sat err "+err);
                }
            },

            
            _createChildControlImpl : function(id, hash) {
                var control;
                switch (id) {
                
                case "brightness-field":
                    control = new qx.ui.basic.Image("skel/decoration/brightness-field-128.png");
                    control.addListener("mousedown", this._onBrightnessFieldMouseDown, this);
                    break;

                case "brightness-handle":
                    control = new qx.ui.basic.Image("skel/decoration/brightness-handle-128.gif");
                    control.addListener("mousedown", this._onBrightnessHandleMouseDown, this);
                    control.addListener("mouseup", this._onBrightnessHandleMouseUp, this);
                    control.addListener("mousemove", this._onBrightnessHandleMouseMove, this);
                    break;
                case "control-bar":
                    control = new qx.ui.container.Composite(new qx.ui.layout.HBox(1));

                    control.add(this.getChildControl("control-pane"));
                    control.add(this.getChildControl("visual-pane"));
                    
                    this._add(control);
                    break;
                    
                case "control-pane":
                    control = new qx.ui.container.Composite(new qx.ui.layout.HBox(2));
                    control.add(this.getChildControl("preset-field-set"));
                    control.add(this.getChildControl("input-field-set"));
                    control.add(this.getChildControl("preview-field-set"), {flex: 1});
                    break;
                case "hue-saturation-pane":
                    control = new qx.ui.container.Composite(new qx.ui.layout.Canvas());
                    control.setAllowGrowY(false);
                    control.addListener("mousewheel", this._onHueSaturationPaneMouseWheel, this);
                    control.add(this.getChildControl("hue-saturation-field"));
                    control.add(this.getChildControl("hue-saturation-handle"), {left: 0, top: this.m_JPG_SIZE});
                    break;
                case "hue-saturation-field":
                    control = new qx.ui.basic.Image("skel/decoration/hue-128.png");
                    control.addListener("mousedown", this._onHueSaturationFieldMouseDown, this);
                    break;
                    
                case "input-field-set":
                    //control = new qx.ui.container.Composite();
                    control = new qx.ui.groupbox.GroupBox("Details");
                    var controlLayout = new qx.ui.layout.VBox();
                    controlLayout.setSpacing(2);
                    control.setLayout(controlLayout);

                    control.add(this.getChildControl("hex-field-composite"));
                    control.add(this.getChildControl("rgb-spinner-composite"));
                    control.add(this.getChildControl("hsb-spinner-composite"));
                    break;    
           
                case "rgb-spinner-composite":
                    var layout = new qx.ui.layout.Grid();
                    //control = new qx.ui.groupbox.GroupBox("");
                    control = new qx.ui.container.Composite();
                    control.setLayout( layout );
                  
                    var redLabel = new qx.ui.basic.Label( this.tr("Red:"));
                    control.add( redLabel, {row: 0, column:0});
                    control.add( this.getChildControl( "rgb-spinner-red"), {row:0, column:1});
                    
                    var greenLabel = new qx.ui.basic.Label( this.tr( "Green:"));
                    control.add( greenLabel, {row:1, column:0} );
                    control.add( this.getChildControl("rgb-spinner-green"), {row:1, column:1});
                    
                    var blueLabel = new qx.ui.basic.Label( this.tr( "Blue:"));
                    control.add( blueLabel, {row:2, column:0} );
                    control.add( this.getChildControl( "rgb-spinner-blue"), {row:2, column:1});
                    break;
                case "preset-field-set":
                    //control = new qx.ui.container.Composite();
                    control = new qx.ui.groupbox.GroupBox( "Presets");
                    control.setLayout(new qx.ui.layout.Grow());
                    control.add(this.getChildControl("preset-grid"));
                    break;
                case "preset-grid":
                    var rowCount = 7;
                    var colCount = 3;
                    controlLayout = new qx.ui.layout.Grid(rowCount, colCount);
                    control = new qx.ui.container.Composite(controlLayout);

                    var colorField;
                    var colorPos;

                    var rowIndex = 0;
                    var colIndex = 0;
                    for (var i=0; i<2; i++){
                      for (var j=0; j<10; j++){
                        colorPos = i * 10 + j;
                        rowIndex = Math.floor( colorPos / colCount );
                        colIndex = colorPos - rowIndex * colCount;
                        colorField = this.getChildControl("colorbucket#" + colorPos);
                        if ( typeof this.__presetTable !=="undefined" ){
                        	colorField.setBackgroundColor(this.__presetTable[colorPos]);
                        }
                        control.add(colorField, {column: colIndex, row: rowIndex});
                      }
                    }
                    break;
                
                    case "visual-pane":
                        //control = new qx.ui.container.Composite();
                        control = new qx.ui.groupbox.GroupBox("Visual");
                        control.setLayout(new qx.ui.layout.HBox(2));
                        control.add(this.getChildControl("hue-saturation-pane"));
                        control.add(this.getChildControl("brightness-pane"));
                        break;
                }
                return control || this.base(arguments, id, hash);
               
            },
            
            /**
             * Return the scale factor between the original color picker and the
             * new color picker.
             * @return {Number} - the scale factor between the original color picker and
             *          the new one.
             */
            _getMultiplyFactor : function(){
                return this.m_TOTAL / this.m_JPG_SIZE;
            },
            
            /**
             * Convert a brightness value to a corresponding position on the brightness
             * scale.
             * @param val {Number} - a brightness value [0,100].
             * @return {number} a corresponding brightness position [0,128].
             */
            _getBrightnessPosition : function( val ){
                var brightPos = this.m_JPG_SIZE - Math.round( parseFloat(val) * this.m_JPG_SIZE / this.m_TOTAL_BRIGHTNESS );
                return brightPos;
            },
            
            /**
             * Convert a brightness position to a brightness value.
             * @param val {Number} - a position on the brightness scale [0,128].
             * @return {Number} - a corresponding brightness value [0,100].
             */
            _getBrightnessVal : function( val ){
                var brightVal = this.m_TOTAL_BRIGHTNESS - 
                    Math.round( parseFloat(val) * this.m_TOTAL_BRIGHTNESS / this.m_JPG_SIZE );
                if ( brightVal == 0 ){
                    brightVal = 1;
                }
                return brightVal;
            },
            
            /**
             * Convert from a hue value in the range 0 to 360 into a position on
             * the hue/saturation screen from 0 to 128.
             * @param val {Number} - a hue value in [0,360].
             * @return {Number} - a hue position in [0,128].
             */
            _getHuePosition : function( val ){
                var multiplyFactor = this._getMultiplyFactor();
                var hueValApply = Math.round( val / this.m_HUE_SCALE_FACTOR / multiplyFactor );
                return hueValApply;
            },
            
            /**
             * Convert a hue position on the hue/saturation screen from 0 to 128
             * to a hue value in the range 0 to 360.
             * @param val {Number} - a hue position in [0,128].
             * @return {Number} - a hue value in [0,360].
             */
            _getHueVal : function( val ){
                var multiplyFactor = this._getMultiplyFactor();
                var hueVal = Math.round( val * this.m_HUE_SCALE_FACTOR * multiplyFactor );
                return hueVal;
            },
            
            /**
             * Convert a saturation value to a position on the hue/saturation screen.
             * @param val {Number} - a saturation value in [0,100].
             * @return {Number} - the corresponding saturation position in [0,128].
             */
            _getSaturationPosition : function( val ){
                var remainder = val % 2;
                var evenVal = val;
                if ( remainder == 1 ){
                    evenVal = val + 1;
                }
                var satVal = this.m_JPG_SIZE - Math.floor( parseFloat(evenVal) / this.m_TOTAL_SATURATION * this.m_JPG_SIZE );
                return satVal;
            },
            
            /**
             * Convert a position on the hue/saturation pane to a saturation value.
             * @param val {Number} -a position in the hue/saturation pane in [0,128].
             * @return {Number} - the corresponding saturation value in [0,100].
             */
            _getSaturationVal : function( val ){
                var satVal = this.m_TOTAL_SATURATION - 
                    Math.round(parseFloat(val) / this.m_JPG_SIZE * this.m_TOTAL_SATURATION );
                return satVal;
            },
            
            /**
             * Generated event when a color square is clicked.
             */
            _onColorFieldClick : function(e){
                this.m_brightnessUpdate = false;
                arguments.callee.base.apply(this, arguments, e );
            },

        
        /**
         * Sets the brightness and moves the brightness handle.
         * @param e {qx.event.type.Mouse} Incoming event object
         */
        _setBrightnessOnFieldEvent : function(e){
            this.m_brightnessUpdate = true;
            try {
              var value = qx.lang.Number.limit(e.getDocumentTop() - this.__brightnessSubtract, 0, this.m_JPG_SIZE);
              this.__updateContext = "brightnessField";
    
              if (this.getChildControl("brightness-handle").getBounds()) {
                this.getChildControl("brightness-handle").setDomTop(value);
              } 
              else {
                this.getChildControl("brightness-handle").setLayoutProperties({ top : value });
              }
              
              var brightVal = this._getBrightnessVal( value );
              this.setBrightness(brightVal);
    
              this.__updateContext = null;
            }
            catch( err ){
                console.log( "setBrightEvent err="+err );
            }
        },
        
        
        /**
         * Sets the saturation and moves the saturation handle.
         * @param e {qx.event.type.Mouse} Incoming event object
         */
        _setHueSaturationOnFieldEvent : function(e){
            this.m_brightnessUpdate = false;
            try {
              var vTop = qx.lang.Number.limit(e.getDocumentTop() - this.__hueSaturationSubtractTop, 0, this.m_JPG_SIZE);
              var vLeft = qx.lang.Number.limit(e.getDocumentLeft() - this.__hueSaturationSubtractLeft, 0, this.m_JPG_SIZE);
              this.getChildControl("hue-saturation-handle").setDomPosition(vLeft, vTop);
              this.__updateContext = "hueSaturationField";
              var satVal = this._getSaturationVal( vTop );
              this.setSaturation( satVal );
              
              //Hue
              var hueVal = this._getHueVal( vLeft );
              this.setHue( hueVal );
              this.__updateContext = null;
            }
            catch( err ){
                console.log( "hue/sat err "+err);
            }
        },
        
        
        //Flag to preventing rounding drift when the brightness is set repeatedly.
        m_brightnessUpdate : false,
        m_JPG_SIZE : 128,
        //Note that this comes from 360 (max hue value) / 256 (max color value).
        m_HUE_SCALE_FACTOR : 1.40625,
        m_TOTAL : 256,
        m_TOTAL_BRIGHTNESS : 100,
        m_TOTAL_SATURATION : 100
        }
    } );

