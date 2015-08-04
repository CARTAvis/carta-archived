/**
 * A custom color chooser.
 */
/*******************************************************************************
 * @asset(skel/decoration/hue.jpg)
 * @asset(skel/decoration/brightness-field-128.png)
 * @asset(skel/decoration/brightness-handle-128.gif)
 */
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
        
                      if (this.__updateContext !== "brightnessField"){
                            var percent = value / this.m_TOTAL_BRIGHTNESS;
                            var topValue = this.m_JPG_SIZE - Math.round(percent * this.m_JPG_SIZE);
                            if (this.getChildControl("brightness-handle").getBounds()) {
                              this.getChildControl("brightness-handle").setDomTop(topValue + this.getChildControl("brightness-pane").getPaddingTop());
                            } else {
                              this.getChildControl("brightness-handle").setLayoutProperties({ top : topValue });
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
                try {
                  if (this.__updateContext === null) {
                    this.__updateContext = "hueModifier";
                  }
    
                  if (this.__updateContext !== "hsbSpinner") {
                    this.getChildControl("hsb-spinner-hue").setValue(value);
                  }
    
                  if (this.__updateContext !== "hueSaturationField"){
                          var domLeft = Math.round( value / 1.40625 );
                          var divideFactor = this.m_TOTAL / this.m_JPG_SIZE;
                          domLeft =  Math.round( domLeft / divideFactor);
                        if (this.getChildControl("hue-saturation-handle").getBounds()) {
                          this.getChildControl("hue-saturation-handle").setDomLeft(domLeft + 
                                  this.getChildControl("hue-saturation-pane").getPaddingLeft());
                        } else {
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
                  
                  this._setBrightnessGradiant();
                  if (this.__updateContext === "hueModifier") {
                    this.__updateContext = null;
                  }
                }
                catch( err ){
                    console.log( "_applyHue err");
                }
            },
            

            
            // property apply
            _applySaturation : function(value, old){
                try {
                  if (this.__updateContext === null) {
                    this.__updateContext = "saturationModifier";
                  }
    
                  if (this.__updateContext !== "hsbSpinner") {
                    this.getChildControl("hsb-spinner-saturation").setValue(value);
                  }
    
                  if (this.__updateContext !== "hueSaturationField")
                  {
                    this._setBrightnessGradiant();
                    var percent = value / this.m_TOTAL_SATURATION;
                    if (this.getChildControl("hue-saturation-handle").getBounds()) {
                      var handleVal = this.m_JPG_SIZE - Math.round(percent * this.m_JPG_SIZE );
                      this.getChildControl("hue-saturation-handle").setDomTop( handleVal + 
                              this.getChildControl("hue-saturation-pane").getPaddingTop());
                    } else {
                        var satVal = this.m_JPG_SIZE - Math.round(percent * this.m_JPG_SIZE);
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
                    control = new qx.ui.basic.Image("skel/decoration/hue.jpg");
                    control.addListener("mousedown", this._onHueSaturationFieldMouseDown, this);
                    break;
                    
                case "input-field-set":
                    //control = new qx.ui.groupbox.GroupBox(this.tr("Details"));
                    control = new qx.ui.container.Composite();
                    var controlLayout = new qx.ui.layout.VBox();
                    controlLayout.setSpacing(2);
                    control.setLayout(controlLayout);

                    control.add(this.getChildControl("hex-field-composite"));
                    control.add(this.getChildControl("rgb-spinner-composite"));
                    control.add(this.getChildControl("hsb-spinner-composite"));
                    break;    
           
                case "rgb-spinner-composite":
                    var layout = new qx.ui.layout.Grid();
                    control = new qx.ui.groupbox.GroupBox("");
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
                    //control = new qx.ui.groupbox.GroupBox(/*this.tr("Presets")*/"");
                    control = new qx.ui.container.Composite();
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
                        colorField.setBackgroundColor(this.__presetTable[colorPos]);
                        control.add(colorField, {column: colIndex, row: rowIndex});
                      }
                    }
                    break;
                
                    case "visual-pane":
                        //control = new qx.ui.groupbox.GroupBox(this.tr("Visual"));
                        control = new qx.ui.container.Composite();
                        control.setLayout(new qx.ui.layout.HBox(2));
                        control.add(this.getChildControl("hue-saturation-pane"));
                        control.add(this.getChildControl("brightness-pane"));
                        break;
                }
                return control || this.base(arguments, id, hash);
               
            },
            
        
        
        /**
         * Sets the brightness and moves the brightness handle.
         *
         * @param e {qx.event.type.Mouse} Incoming event object
         */
        _setBrightnessOnFieldEvent : function(e){
            try {
              var value = qx.lang.Number.limit(e.getDocumentTop() - this.__brightnessSubtract, 0, this.m_JPG_SIZE);
              this.__updateContext = "brightnessField";
    
              if (this.getChildControl("brightness-handle").getBounds()) {
                this.getChildControl("brightness-handle").setDomTop(value);
              } else {
                this.getChildControl("brightness-handle").setLayoutProperties({ top : value });
              }
              
              var brightVal = 100 - Math.round(value / (this.m_JPG_SIZE / 100) );
              this.setBrightness(brightVal);
    
              this.__updateContext = null;
            }
            catch( err ){
                console.log( "setBrightEvent err="+err );
            }
        },
        
        /**
         * Sets the saturation and moves the saturation handle.
         *
         * @param e {qx.event.type.Mouse} Incoming event object
         */
        _setHueSaturationOnFieldEvent : function(e){
            try {
              var vTop = qx.lang.Number.limit(e.getDocumentTop() - this.__hueSaturationSubtractTop, 0, this.m_JPG_SIZE);
              var vLeft = qx.lang.Number.limit(e.getDocumentLeft() - this.__hueSaturationSubtractLeft, 0, this.m_JPG_SIZE);
              this.getChildControl("hue-saturation-handle").setDomPosition(vLeft, vTop);
              this.__updateContext = "hueSaturationField";
              var satVal = 100 - Math.round(vTop / this.m_JPG_SIZE * 100);
              this.setSaturation( satVal );
              
              //Hue
              var hueVal = Math.round(vLeft * 1.40625);
              var multiplyFactor = this.m_TOTAL / this.m_JPG_SIZE;
              hueVal = Math.round( hueVal * multiplyFactor );
              this.setHue( hueVal );
              this.__updateContext = null;
            }
            catch( err ){
                console.log( "hue/sat err "+err);
            }
        },
        
        m_JPG_SIZE : 128,
        m_TOTAL : 256,
        m_TOTAL_BRIGHTNESS : 100,
        m_TOTAL_SATURATION : 100
        }
    } );

