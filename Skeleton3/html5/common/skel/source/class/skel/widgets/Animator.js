/**
 * A 'tape deck' UI for animating data.
 */

/**

 @ignore(fv.assert)
 @ignore(fv.console.*)

 

 ************************************************************************ */


qx.Class.define("skel.widgets.Animator",
    {
        extend: qx.ui.core.Widget,

        /**
         * Constructor.
         * @param title {String} descriptor for what will be animated (Channel, Region, Image, etc).
         */
        construct: function ( title ) {
            this.base(arguments);
            this.m_title = title;
            var locationComposite = this._initLocation();
            var buttonComposite = this._initToolBar();
            var sliderComposite = this._initSliderControls();
            this._initSettings();
          
            this._setLayout( new qx.ui.layout.VBox(5));
            this._add( locationComposite );
            this._add( sliderComposite );
            this._add( buttonComposite );
        },   

        members: {
           _minMaxSettings : function( maximize ){
        	   if ( maximize ){
        		   this._add( this.m_settingsComposite);
        	   }
        	   else {
        		   this._remove( this.m_settingsComposite );
        	   }
           },
           
           _initLocation : function(){
        	   var titleLabel = new qx.ui.basic.Label( this.m_title );
        	   var locationText = new qx.ui.form.TextField();
        	   var endLabel = new qx.ui.basic.Label( "out of ");
        	   var locationComposite = new qx.ui.container.Composite();
        	   locationComposite.setLayout( new qx.ui.layout.HBox(5));
        	   locationComposite.add( titleLabel );
        	   locationComposite.add( locationText );
        	   locationComposite.add( endLabel );
        	   locationComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
        	   var settingsCheck = new qx.ui.form.CheckBox( "Settings...");
        	   settingsCheck.addListener( "changeValue", function(){
        		   this._minMaxSettings( settingsCheck.getValue());
        	   }, this);
        	   locationComposite.add( settingsCheck );
        	   return locationComposite;
           },
        	
           _initSettings : function(){
        	  
        	
        	   var endWrapRadio = new qx.ui.form.RadioButton( "Wrap");
        	   endWrapRadio.setValue( true );
        	   var endReverseRadio = new qx.ui.form.RadioButton( "Reverse");
        	   var endJumpRadio = new qx.ui.form.RadioButton( "Jump");
        	   var endRadioGroup = new qx.ui.form.RadioGroup();
        	   endRadioGroup.add( endWrapRadio );
        	   endRadioGroup.add( endReverseRadio );
        	   endRadioGroup.add( endJumpRadio );
        	    	   
        	   
        	   var speedLabel = new qx.ui.basic.Label( "Rate:");
        	   var speedSpinBox = new qx.ui.form.Spinner();
        	   var stepLabel = new qx.ui.basic.Label( "Step:");
        	   var stepSpinBox = new qx.ui.form.Spinner();
        	   
        	   this.m_settingsComposite = new qx.ui.container.Composite();
        	   this.m_settingsComposite.setLayout( new qx.ui.layout.HBox(5));
        	   this.m_settingsComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
        	   this.m_settingsComposite.add( stepLabel );
        	   this.m_settingsComposite.add( stepSpinBox );
        	   this.m_settingsComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
        	   var endLabel = new qx.ui.basic.Label( "End:");
        	   this.m_settingsComposite.add( endLabel );
        	   this.m_settingsComposite.add( endWrapRadio);
        	   this.m_settingsComposite.add( endReverseRadio );
        	   this.m_settingsComposite.add( endJumpRadio );
        	   this.m_settingsComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
        	   this.m_settingsComposite.add( speedLabel );
        	   this.m_settingsComposite.add( speedSpinBox );
        	   this.m_settingsComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
           },
           
           _initToolBar : function(){
        	   //Toolbar
               var toolbar = new qx.ui.toolbar.ToolBar();
               toolbar.setSpacing( 2 );
               var startButton = new qx.ui.toolbar.Button( "Start");
               var revPlayButton = new qx.ui.toolbar.Button( "Rev Play");
               var revStepButton = new qx.ui.toolbar.Button( "Rev Step");
               var stopButton = new qx.ui.toolbar.Button( "Stop");
               var stepButton = new qx.ui.toolbar.Button( "Step");
               var playButton = new qx.ui.toolbar.Button( "Play");
               var endButton = new qx.ui.toolbar.Button( "End");
               toolbar.add( startButton );
               toolbar.add( revPlayButton );
               toolbar.add( revStepButton );
               toolbar.add( stopButton );
               toolbar.add( stepButton );
               toolbar.add( playButton );
               toolbar.add( endButton );           
               var buttonComposite = new qx.ui.container.Composite();
               buttonComposite.setLayout( new qx.ui.layout.HBox(5));
               buttonComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
               buttonComposite.add( toolbar );
               buttonComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
               return buttonComposite;
           
        },
        
        _initSliderControls : function(){
        	var lowBoundSpinner = new qx.ui.form.Spinner();
        	var slider = new qx.ui.form.Slider();
        	var highBoundSpinner = new qx.ui.form.Spinner();
        	var sliderComposite = new qx.ui.container.Composite();
        	sliderComposite.setLayout( new qx.ui.layout.HBox(5));
        	sliderComposite.add( lowBoundSpinner );
        	sliderComposite.add( slider, {flex:1});
        	sliderComposite.add( highBoundSpinner );
        	return sliderComposite;
        },
        
        m_title : null,
        m_settingsComposite : null
        },
        

        properties: {
            appearance: {
                refine: true,
                init: "internal-area"
            }

        }

    });

