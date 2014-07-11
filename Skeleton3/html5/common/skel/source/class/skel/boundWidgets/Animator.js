/**
 * A 'tape deck' UI for animating data.
 */
/*global mImport */
/**
 @ignore( mImport)
 
 @asset(skel/icons/movie-next-frame16.png)
 @asset(skel/icons/movie-pause16.png)
 @asset(skel/icons/movie-play-reverse16.png)
 @asset(skel/icons/movie-play16.png)
 @asset(skel/icons/movie-previous-frame16.png)
 @asset(skel/icons/movie-stop16.png)
 @asset(skel/icons/dblarrowleft.png)
 @asset(skel/icons/dblarrowright.png)
 ************************************************************************ */


qx.Class.define("skel.boundWidgets.Animator",
    {
        extend: qx.ui.core.Widget,

        /**
         * Constructor.
         * @param title {String} descriptor for what will be animated (Channel, Region, Image, etc).
         */
        construct: function ( title, baseIdentifier, winId ) {
            this.base(arguments);
                        
            this.m_title = title;
            this.m_winId = winId;
            this.m_baseIdentifier = baseIdentifier;
            
            this._initSharedVars();
            
            //UI
            this._initUI();
        },   

        members: {
        	/**
        	 * Return the path for animator commands.
        	 * @param command {String} a specific command such as "setFrame".
        	 */
        	_getSharedPath : function( command ){
        		return "/carta/"+this.m_baseIdentifier+"/"+this.m_title.toLowerCase()+"/"+command+"/"+this.m_winId;
        	},
        	
        	/**
        	 * Show or hide the less-used additional animator settings.
        	 * @param maximize {Boolean} true if all the additional settings should be shown; false otherwise.
        	 */
           _minMaxSettings : function( maximize ){
        	   if ( maximize ){
        		   this._add( this.m_settingsComposite);
        	   }
        	   else {
        		   this._remove( this.m_settingsComposite );
        	   }
           },
           
           /**
            * Initialize the UI.
            */
           _initUI : function(){
        	   var locationComposite = this._initLocation();
               var buttonComposite = this._initToolBar();
               var sliderComposite = this._initSliderControls();
               this._initSettings();
             
               this._setLayout( new qx.ui.layout.VBox(5));
               this._add( locationComposite );
               this._add( sliderComposite );
               this._add( buttonComposite );
               
               this.m_timer = new qx.event.Timer(1000); 
               this._setTimerSpeed();
           },
           
           /**
            * Initialize the part of the UI concerned with the current location of the animator.
            */
           _initLocation : function(){
        	   var titleLabel = new qx.ui.basic.Label( this.m_title );
        	   this.m_indexText = new qx.ui.form.TextField();
        	   this.m_indexText.addListener("input", function(e) {
        		   var value = this.m_indexText.getValue();
        		   var valueInt = parseInt( value );
        		   if ( !isNaN( valueInt) ){
        			   if ( valueInt <= this.m_slider.getMaximum() && valueInt >= this.m_slider.getMinimum()){
        				   this._setFrame( valueInt );
        			   }
        		   }
        	   }, this); 
        	  
        	   var endLabel = new qx.ui.basic.Label( "out of ");
        	   this.m_endLabel = new qx.ui.basic.Label("");
        	   var locationComposite = new qx.ui.container.Composite();
        	   locationComposite.setLayout( new qx.ui.layout.HBox(5));
        	   locationComposite.add( titleLabel );
        	   locationComposite.add( this.m_indexText );
        	   locationComposite.add( endLabel );
        	   locationComposite.add( this.m_endLabel );
        	   locationComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
        	   var settingsCheck = new qx.ui.form.CheckBox( "Settings...");
        	   settingsCheck.addListener( "changeValue", function(){
        		   this._minMaxSettings( settingsCheck.getValue());
        	   }, this);
        	   locationComposite.add( settingsCheck );
        	   return locationComposite;
           },
        	
           /**
            * Initialize the additional less-used settings in the UI.
            */
           _initSettings : function(){  
        	   this.m_endWrapRadio = new qx.ui.form.RadioButton( "Wrap");
        	   this.m_endWrapRadio.addListener( "changeValue", function(){
        		   if ( this.m_endWrapRadio.getValue() ){
        			   this._setEndBehavior( "Wrap");
        		   }
        	   }, this);
        	   this.m_endReverseRadio = new qx.ui.form.RadioButton( "Reverse");
        	   this.m_endReverseRadio.addListener( "changeValue", function(){
        		   if ( this.m_endReverseRadio.getValue() ){
        			   this._setEndBehavior( "Reverse");
        		   }
        	   }, this );
        	   this.m_endJumpRadio = new qx.ui.form.RadioButton( "Jump");
        	   this.m_endJumpRadio.addListener( "changeValue", function(){
        		   if ( this.m_endJumpRadio.getValue() ){
        			   this._setEndBehavior( "Jump");
        		   }
        	   }, this );
        	   var endRadioGroup = new qx.ui.form.RadioGroup();
        	   endRadioGroup.add( this.m_endWrapRadio );
        	   endRadioGroup.add( this.m_endReverseRadio );
        	   endRadioGroup.add( this.m_endJumpRadio );
        	   if ( this.m_stateEndBehavior != null ){
        		   this._endBehaviorCB( this.m_stateEndBehavior.get());
        	   }   
        	   
        	   var speedLabel = new qx.ui.basic.Label( "Rate:");
        	   this.m_speedSpinBox = new qx.ui.form.Spinner(1, 10, 100);
        	   if ( this.m_stateFrameRate != null ){
        		   this._frameRateCB( this.m_stateFrameRate.get());
        	   }
        	   this.m_speedSpinBox.addListener( "changeValue", function(){
        		   
        		   this._setTimerSpeed();
        		   this.m_stateFrameRate.set( this.m_speedSpinBox.getValue());
        	   }, this );
        	   var stepLabel = new qx.ui.basic.Label( "Step:");
        	   var stepSpinBox = new qx.ui.form.Spinner(1, 1, 100 );
        	   this.bind( "frameUpperBound", stepSpinBox, "maximum");
        	   stepSpinBox.bind( "value", this, "frameStep");
        	   if ( this.m_stateFrameStep != null ){
        		   this._frameStepCB( this.m_stateFrameStep.get());
        	   }
        	   
        	   this.m_settingsComposite = new qx.ui.container.Composite();
        	   this.m_settingsComposite.setLayout( new qx.ui.layout.HBox(5));
        	   this.m_settingsComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
        	   this.m_settingsComposite.add( stepLabel );
        	   this.m_settingsComposite.add( stepSpinBox );
        	   this.m_settingsComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
        	   this.m_settingsComposite.add( this.m_endWrapRadio);
        	   this.m_settingsComposite.add( this.m_endReverseRadio );
        	   this.m_settingsComposite.add( this.m_endJumpRadio );
        	   this.m_settingsComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
        	   this.m_settingsComposite.add( speedLabel );
        	   this.m_settingsComposite.add( this.m_speedSpinBox );
        	   this.m_settingsComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
           },
           
           /**
            * Initialize the tape recorder part of the UI.
            */
           _initToolBar : function(){
        	   //Toolbar
               var toolbar = new qx.ui.toolbar.ToolBar();
               toolbar.setSpacing( 2 );
              
               var startButton = new qx.ui.toolbar.Button( "","skel/icons/dblarrowleft.png");
               startButton.addListener( "execute", this._goToStart, this);
               
               this.m_revPlayButton = new qx.ui.form.ToggleButton( "","skel/icons/movie-play-reverse16.png");
               this.m_revPlayButton.addListener( "execute", function(){
            	   this.m_playButton.setValue( false );
            	   this._play(false);
               }, this );
               
               var revStepButton = new qx.ui.toolbar.Button( "","skel/icons/movie-previous-frame16.png");
               revStepButton.addListener( "execute", this._decrementValue, this );
               
               var stopButton = new qx.ui.toolbar.Button( "", "skel/icons/movie-stop16.png");
               stopButton.addListener( "execute", this._stop, this );
               
               var stepButton = new qx.ui.toolbar.Button( "", "skel/icons/movie-next-frame16.png");
               stepButton.addListener( "execute", this._incrementValue, this );
                            
               this.m_playButton = new qx.ui.form.ToggleButton( "", "skel/icons/movie-play16.png");
               this.m_playButton.addListener( "execute", function(){
            	   this.m_revPlayButton.setValue( false );
            	   this._play( true );
               }, this );
       
               var endButton = new qx.ui.toolbar.Button( "", "skel/icons/dblarrowright.png");
               endButton.addListener( "execute", this._goToEnd, this );
               
               toolbar.add( startButton );
               toolbar.add( this.m_revPlayButton );
               toolbar.add( revStepButton );
               toolbar.add( stopButton );
               toolbar.add( stepButton );
               toolbar.add( this.m_playButton );
               toolbar.add( endButton );           
               var buttonComposite = new qx.ui.container.Composite();
               buttonComposite.setLayout( new qx.ui.layout.HBox(5));
               buttonComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
               buttonComposite.add( toolbar );
               buttonComposite.add( new qx.ui.core.Spacer(10,10), {flex:1});
               return buttonComposite;
           
        },
        
       /**
        * Initialize the slider controls.
        */
        _initSliderControls : function(){
        	var lowBoundSpinner = new qx.ui.form.Spinner(0, 0, 100);
        	this.m_slider = new qx.ui.form.Slider();
        	this.m_slider.addListener("changeValue", function () {
        		if ( this.m_inUpdateState ){
        			return;
        		}
        		this._setFrame( this.m_slider.getValue());
             }, this);
        	
        	var highBoundSpinner = new qx.ui.form.Spinner(0, 100, 100);
        	var sliderComposite = new qx.ui.container.Composite();
        	sliderComposite.setLayout( new qx.ui.layout.HBox(5));
        	sliderComposite.add( lowBoundSpinner );
        	sliderComposite.add( this.m_slider, {flex:1});
        	sliderComposite.add( highBoundSpinner );
        	this.bind( "frame", this.m_slider, "value");
        	this.bind( "frameLowerBound", lowBoundSpinner, "minimum");
        	this.bind( "frameUpperBound", highBoundSpinner, "maximum");
        	lowBoundSpinner.bind( "value", this.m_slider, "minimum");
        	highBoundSpinner.bind( "value", this.m_slider, "maximum");
        	lowBoundSpinner.bind( "value", highBoundSpinner, "minimum");
        	highBoundSpinner.bind( "value", lowBoundSpinner, "maximum");
        	return sliderComposite;
        },
        
        /**
         * Initialize the shared variables.
         */
        _initSharedVars : function(){
        	this.m_connector = mImport( "connector");
        	
        	 //Frame
        	var framePath = this._getSharedPath("frame");
            this.m_stateFrame = this.m_connector.getSharedVar( framePath );
            this.m_stateFrame.addCB( this._frameCB.bind(this) );
            
            //Lower bound for frame
            var frameStartPath = this._getSharedPath("frameStart");
            this.m_stateFrameStart = this.m_connector.getSharedVar( frameStartPath );
            this.m_stateFrameStart.addCB( this._frameStartCB.bind(this));
            
            //Upper bound for frame
            var frameEndPath = this._getSharedPath("frameEnd");
            this.m_stateFrameEnd = this.m_connector.getSharedVar( frameEndPath );
            this.m_stateFrameEnd.addCB( this._frameEndCB.bind(this));
            
            //End behavior
            var frameEndBehaviorPath = this._getSharedPath("endBehavior");
            this.m_stateEndBehavior = this.m_connector.getSharedVar( frameEndBehaviorPath );
            this.m_stateEndBehavior.addCB( this._endBehaviorCB.bind( this ));
            
            //Step size
            var frameStepPath = this._getSharedPath("frameStep");
            this.m_stateFrameStep = this.m_connector.getSharedVar( frameStepPath );
            this.m_stateFrameStep.addCB( this._frameStepCB.bind( this ));
            
            //Animation Rate
            var frameRatePath = this._getSharedPath("frameRate");
            this.m_stateFrameRate = this.m_connector.getSharedVar( frameRatePath);
            this.m_stateFrameRate.addCB( this._frameRateCB.bind( this ));
            
            //Initial values
            this._frameCB( this.m_stateFrame.get());   
            this._frameStartCB( this.m_stateFrameStart.get());
            this._frameEndCB( this.m_stateFrameEnd.get());
        },
        
        /**
         * Update the new frame position.
         */
        _applyFrame: function(){
        	if ( this.m_indexText != null ){
        		this.m_indexText.setValue( this.getFrame()+"");
        	}
        },
        
        /**
         * Update the maximum number of frames available.
         */
        _applyFrameUpperBound: function(){
        	if ( this.m_endLabel != null ){
        		this.m_endLabel.setValue( this.getFrameUpperBound()+"");
        	}
        },
        
        /**
         * Stop the animation.
         */
        _stop: function(){
        	if ( this.m_timer ){
        		this.m_playButton.setValue( false );
        		this.m_revPlayButton.setValue( false );
        		this.m_timer.stop();
        	}
        }, 
        
        /**
         * Start the animation.
         * @param forward {Boolean} true if the frame position should be increased; false,
         * 			if the frame position should be decreased.
         */
        _play: function( forward ){
        	if ( this.m_timer ){
        		this.m_timer.stop();
        		if ( this.m_timer.hasListener( "interval")){
        			this.m_timer.removeListenerById( this.m_timerListener );
        		}
    
        		if ( forward ){
        			this.m_timerListener = this.m_timer.addListener( "interval", this._increaseValue, this );
        		}
        		else {
        			this.m_timerListener = this.m_timer.addListener( "interval", this._decreaseValue, this );
        		}
        		this.m_timer.start();
        	}
        },
        
        /**
         * Move to the smallest available frame.
         */
        _goToStart: function(){
        	this._stop();
        	this._setFrame( this.getFrameLowerBound() );
        },
        
        /**
         * Move to the largest available frame.
         */
        _goToEnd: function(){
        	this._stop();
        	this._setFrame( this.getFrameUpperBound());
        },
        
        /**
         * Increment the frame.
         */
        _incrementValue: function(){
        	this._stop();
        	this._increaseValue();
        },
        
        /**
         * Increase the frame value taking into account end behavior.
         */
        _increaseValue: function(){
        	var val = this.getFrame()+this.getFrameStep();
        	
        	var upperBound = this.m_slider.getMaximum();
        	if ( this.m_endJumpRadio.getValue() ){
        		if ( this.getFrame() < upperBound ){
        			val = upperBound;
        		}
        		else {
        			val = this.m_slider.getMinimum();
        		}
        	}
        	
        	//May need to do a wrap.  	
        	if ( val > upperBound ){
        		if ( this.m_endWrapRadio.getValue()){
        			val = val % upperBound;
        		}
        		else if ( this.m_endReverseRadio.getValue()){
        			//If we are playing, reverse play. otherwise decrease the value
        			if ( this.m_playButton.getValue() ){
        				this.m_revPlayButton.execute();
        			}
        			else {
        				val = upperBound - 1;
        			}
        		}
        		else {
        			console.log( "Unhandled wrap val="+val);
        		}
        	}
          	this._setFrame( val );
        },
        
        /**
         * Notify the server of the new frame value.
         * @param frameIndex {Number} the new frame value.
         */
        _setFrame : function ( frameIndex ){  
        	var paramMap = "path:"+this._getSharedPath("setFrame")+",frame:"+frameIndex;
        	var setFramePath = this._getSharedPath( "setFrame");
        	this.m_connector.sendCommand( setFramePath, paramMap, function(val){} );
        },
        
        /**
         * Adjust the speed of the animation.
         */
        _setTimerSpeed : function(){
        	var rate = this.m_speedSpinBox.getValue();
        	//High values should be fast.
        	rate = rate - this.m_speedSpinBox.getMaximum();
 		   	rate = rate * 100;
 		   	if ( this.m_timer ){
 			   this.m_timer.setInterval( rate );
 		   	}
        },
        
        /**
         * Set whether frames at upper or lower bounds should wrap, reverse, etc.
         * @param behavior {String} a descriptor for the behavior.
         */
        _setEndBehavior : function( behavior ){
        	if ( this.m_stateEndBehavior != null ){
        		this.m_stateEndBehavior.set( behavior );
        	}
        },
        
        /**
         * Decrease the frame value.
         */
        _decrementValue: function(){
        	this._stop();
        	this._decreaseValue();
        },
        
        /**
         * Decrease the frame value taking into account end behavior.
         */
        _decreaseValue : function(){
        	var val = this.getFrame() - this.getFrameStep();
        	
        	var lowerBound = this.m_slider.getMinimum();
        	if ( this.m_endJumpRadio.getValue()){
        		if ( this.getFrame() > lowerBound ){
        			val = lowerBound;
        		}
        		else {
        			val = this.m_slider.getMaximum();
        		}
        	}
        	
        	//May need to do a wrap.
        	if ( val < lowerBound ){
        		if ( this.m_endWrapRadio.getValue()){
        			val = val + this.m_slider.getMaximum();
        		}
        		else if ( this.m_endReverseRadio.getValue()){
        			if ( this.m_revPlayButton.getValue() ){
        				this.m_playButton.execute();
        			}
        			else {
        				val = lowerBound + 1;
        			}
        		}
        	}
        	this._setFrame( val);
        },
        
        /**
         * Shared var callback. Read in the position of the animator and render it.
         * @param val {String} Number of plugins.
         * @private
         */
        _frameCB: function( val) {
        	if ( val ){
        		var frameNum = parseInt( val );
        		if ( !isNaN( frameNum )){
        			this.setFrame( frameNum );
        		}
        	}       	
        },
        
        /**
         * Callback for the frame lower bound.
         */
        _frameStartCB : function( val ){
        	if ( val ){
        		var frameStart = parseInt( val );
        		if ( !isNaN( frameStart )){
        			this.setFrameLowerBound( frameStart );
        		}
        	}
        },
        
        /**
         * Callback for the frame end behavior.
         */
        _frameEndCB : function( val ){
        	if ( val ){
        		var frameEnd = parseInt( val );
        		if ( !isNaN( frameEnd )){
        			this.setFrameUpperBound( frameEnd );
        		}
        	}
        },
        
        /**
         * Callback for the frame step size.
         */
        _frameStepCB : function( val ){
        	if ( val ){
        		var frameStep = parseInt( val );
        		if ( !isNaN( frameStep )){
        			this.setFrameStep( frameStep );
        		}
        	}
        },
        
        /**
         * Callback for the speed of the animation.
         */
        _frameRateCB : function( val ){
        	if ( val ){
        		var frameRate = parseInt( val );
        		if ( !isNaN( frameRate)){
        			this.m_speedSpinBox.setValue( frameRate );
        		}
        	}
        }, 
        
        /**
         * Callback for behavior when the frame is at the lower or upper extreme
         * of its range.
         */
        _endBehaviorCB : function( val ){
        	if ( val ){
        		if ( val == "Wrap"){
        			this.m_endWrapRadio.setValue( true );
        		}
        		else if ( val == "Reverse"){
        			this.m_endReverseRadio.setValue( true );
        		}
        		else if ( val == "Jump"){
        			this.m_endJumpRadio.setValue( true );
        		}
        		else {
        			console.log( "Unrecognized end behavior: "+val);
        		}
        	}
        },
        
        //ID
        m_title : "",
        m_baseIdentifier: "",
        m_winId: "",
        
        //UI Widgets
        m_settingsComposite : null,
        m_endLabel : null,
        m_timer : null,
        m_slider : null,
        m_timerListener : null,
        m_indexText : null,
        m_endWrapRadio : null,
        m_endReverseRadio : null,
        m_endJumpRadio : null,
        m_playButton : null,
        m_revPlayButton : null,
        m_speedSpinBox : null,
        
        //State variables
        m_connector : null,
        m_stateFrame : null,
        m_stateFrameStart : null,
        m_stateFrameEnd : null,
        m_stateFrameStep : null,
        m_stateEndBehavior : null,
        m_stateFrameRate : null,
        
        m_inUpdateState : false
        },
        

        properties: {
            appearance: {
                refine: true,
                init: "internal-area"
            },
        	frame : {
        		init : 0,
        		apply : "_applyFrame",
        		event : "changeFrame"
        	},
            frameUpperBound : {
            	init : 100,
            	apply : "_applyFrameUpperBound",
            	event : "changeFrameUpperBound"
            },
            frameLowerBound : {
            	init : 0,
            	event : "changeFrameLowerBound"
            },
            frameStep : {
            	init : 1,
            	event : "changeFrameStep"
            }
            
        }

    });

