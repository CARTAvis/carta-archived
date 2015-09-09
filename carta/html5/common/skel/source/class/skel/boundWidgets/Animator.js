/**
 * A 'tape deck' UI for animating data.
 */

/*******************************************************************************
 * @ignore( mImport)
 * 
 * @asset(skel/icons/movie-next-frame16.png)
 * @asset(skel/icons/movie-pause16.png)
 * @asset(skel/icons/movie-play-reverse16.png)
 * @asset(skel/icons/movie-play16.png)
 * @asset(skel/icons/movie-previous-frame16.png)
 * @asset(skel/icons/movie-stop16.png)
 * @asset(skel/icons/dblarrowleft.png)
 * @asset(skel/icons/dblarrowright.png)
 ******************************************************************************/

/* global mImport, qx, skel, console */

qx.Class.define("skel.boundWidgets.Animator", {
    extend : qx.ui.core.Widget,

    /**
     * Constructor.
     * 
     * @param title {String} descriptor for what will be animated (Channel,Region, Image, etc).
     * @param winId {String} the unique server id.
     */
    construct : function(title,  winId) {
        this.base(arguments);
        this.m_title = title;
        this.m_winId = winId;

        // Create the GUI
        this._initUI();
        
        // Create the shared variable for the settings
        this._initSharedVars();
    },
    
    events : {
        "movieStart" : "qx.event.type.Data",
        "movieStop" : "qx.event.type.Data"
    },

    members : {
        /**
         * Callback for when the animator settings shared variable has changed value.
         */
        _animationCB : function( ){
            var val = this.m_sharedVar.get();
            if ( val ){
                try {
                    var animObj = JSON.parse( val );
                    this._endBehaviorCB( animObj.endBehavior );
                    this._frameStepCB( animObj.frameStep );
                    this._frameRateCB( animObj.frameRate );
                }
                catch( err ){
                    console.log( "Could not parse: "+val );
                }
            }
        },
        

        /**
         * Decrease the frame value.
         */
        _decrementValue : function() {
            this._stop();
            this._decreaseValue();
        },

        /**
         * Decrease the frame value taking into account end behavior.
         */
        _decreaseValue : function() {
            var val = this.m_frame - this.getFrameStep();

            var lowerBound = this.m_lowBoundsSpinner.getValue();
            if (this.m_endJumpRadio.getValue()) {
                if (this.m_frame > lowerBound) {
                    val = lowerBound;
                } 
                else {
                    val = this.m_highBoundsSpinner.getValue();
                }
            }

            // May need to do a wrap.
            if (val < lowerBound) {
                if (this.m_endWrapRadio.getValue()) {
                    val = this.m_highBoundsSpinner.getValue();
                } else if (this.m_endReverseRadio.getValue()) {
                    if (this.m_revPlayButton.getValue()) {
                        this.m_playButton.execute();
                    } else {
                        val = lowerBound + 1;
                    }
                }
            }
            this._sendFrame(val);
        },

        /**
         * Callback for behavior when the frame is at the lower or upper extreme
         * of its range.
         * @param val {String} the new end behavior.
         */
        _endBehaviorCB : function(val) {
            if (val !== null) {
                if ( val == this.m_endWrapRadio.getLabel()){
                    if ( ! this.m_endWrapRadio.getValue()){
                        this.m_endWrapRadio.setValue( true );
                    }
                }
                else if ( val == this.m_endReverseRadio.getLabel()){
                    if ( ! this.m_endReverseRadio.getValue()){
                        this.m_endReverseRadio.setValue( true );
                    }
                }
                else if ( val == this.m_endJumpRadio.getLabel()){
                    if ( ! this.m_endJumpRadio.getValue()){
                        this.m_endJumpRadio.setValue( true );
                    }
                }
                else {
                    console.log("Unrecognized end behavior: " + val);
                }
            }
        },
        
        /**
         * Updates the user set lower and upper bounds based on server values.
         * @param start {Number} - the user settable lower bound.
         * @param end {Number} - the user settable upper bound.
         */
        _frameBoundsUserCB : function( start, end ){
            var lowValue = this.m_lowBoundsSpinner.getValue();
            if ( start !=  lowValue ){
                this.m_lowBoundsSpinner.setValue( parseInt(start) );
            }
            
            var highValue = this.m_highBoundsSpinner.getValue();
            if ( end !=  highValue ){
                this.m_highBoundsSpinner.setValue( parseInt(end) );
            }
        },

        /**
         * Shared var callback. Read in the position of the animator and render it.
         * @private
         */
        _frameCB : function() {
           if ( this.m_slider.getValue() != this.m_frame ){
                this.m_slider.setValue( this.m_frame );
           }
           var valStr = this.m_frame + "";
           if ( this.m_indexText.getValue() !== valStr ){
                this.m_indexText.setValue( valStr );
           }
        },

        /**
         * Callback for the frame end behavior.
         */
        _frameEndCB : function() {
            var limit = this.m_frameHigh - 1;
            if ( limit < 0  ){
                limit = 0;
            }
            if ( limit != this.m_slider.getMaximum() ){
                this.m_slider.setMaximum( limit );
            }
            if ( limit != this.m_highBoundsSpinner.getMaximum()){
                this.m_highBoundsSpinner.setMaximum( limit );
            }
            //var highSpinValue = this.m_highBoundsSpinner.getValue();
            //this.m_highBoundsSpinner.setValue( limit );
            var valStr = this.m_frameHigh + "";
            if ( this.m_endLabel.getValue() !== valStr ){
                this.m_endLabel.setValue( valStr);
            }
        },

        /**
         * Callback for the frame step size.
         * @param val {Number} the new step size.
         */
        _frameStepCB : function(val) {
            if (val !== null ) {
                if ( val !== this.getFrameStep()){
                    this.setFrameStep( val );
                }
            }
        },

        /**
         * Callback for the speed of the animation.
         * @param val {Number} the new speed.
         */
        _frameRateCB : function(val) {
            if (val !== null ) {
                var oldValue = this.m_speedSpinBox.getValue();
                if ( oldValue != val){
                    this.m_speedSpinBox.setValue(val);
                }
            }
        },

        /**
         * Callback for the frame lower bound.
         */
        _frameStartCB : function() {
            if ( this.m_frameLow != this.m_lowBoundsSpinner.getMinimum() ){
                this.m_lowBoundsSpinner.setMinimum( this.m_frameLow );
            }
            /*if ( this.m_frameLow > this.m_lowBoundsSpinner.getValue()){
                this.m_lowBoundsSpinner.setValue( this.m_frameLow );
            }*/
            if ( this.m_frameLow != this.m_slider.getMinimum() ){
                this.m_slider.setMinimum( this.m_frameLow );
            }
        },

        /**
         * Move to the smallest available frame.
         */
        _goToStart : function() {
            this._stop();
            var lowBound = this.m_lowBoundsSpinner.getValue();
            if ( lowBound != this.m_frame ){
                this._sendFrame( lowBound );
            }
        },

        /**
         * Move to the largest available frame.
         */
        _goToEnd : function() {
            this._stop();
            var highBound = this.m_highBoundsSpinner.getValue();
            if ( highBound != this.m_frame){
                this._sendFrame( highBound );
            }
        },

        /**
         * Increment the frame.
         */
        _incrementValue : function() {
            this._stop();
            this._increaseValue();
        },

        /**
         * Increase the frame value taking into account end behavior.
         */
        _increaseValue : function() {
            var val = this.m_frame + this.getFrameStep();

            var upperBound = this.m_highBoundsSpinner.getValue();
            if (this.m_endJumpRadio.getValue()) {
                if (this.m_frame < upperBound) {
                    val = upperBound;
                } else {
                    val = this.m_lowBoundsSpinner.getValue();
                }
            }

            // May need to do a wrap.
            if (val > upperBound) {
                if (this.m_endWrapRadio.getValue()) {
                    val = this.m_lowBoundsSpinner.getValue();
                } 
                else if (this.m_endReverseRadio.getValue()) {
                    // If we are playing, reverse play. otherwise decrease the
                    // value
                    if (this.m_playButton.getValue()) {
                        this.m_revPlayButton.execute();
                    } else {
                        val = upperBound - 1;
                    }
                } else {
                    console.log("Unhandled wrap val=" + val);
                }
            }
            this._sendFrame(val);
        },

        /**
         * Initialize the UI.
         */
        _initUI : function() {
            var locationComposite = this._initLocation();
            var buttonComposite = this._initToolBar();
            var sliderComposite = this._initSliderControls();
            this._initSettings();

            this._setLayout(new qx.ui.layout.VBox(5));
            this._add(locationComposite);
            this._add(sliderComposite);
            this._add(buttonComposite);
            this.m_timer = new qx.event.Timer(1000);
            this._setTimerSpeed();
        },

        /**
         * Initialize the part of the UI concerned with the current location of
         * the animator.
         * @return {qx.ui.container.Composite} a container containing the location UI.
         */
        _initLocation : function() {
            var titleLabel = new qx.ui.basic.Label(this.m_title);
            
            this.m_indexText = new skel.widgets.CustomUI.NumericTextField(0,null);
            this.m_indexText.setIntegerOnly( true );
            this.m_indexText.setToolTipText( "Set the current value.");
            this.m_indexText.setTextId( this.m_title +"IndexText");
            this.m_indexText.setValue( "0");
            this.m_indexText.addListener("textChanged", function(e) {
                var value = this.m_indexText.getValue();
                var valueInt = parseInt(value);
                if (!isNaN(valueInt)) {
                    if (valueInt <= this.m_slider.getMaximum() && valueInt >= this.m_slider.getMinimum()) {
                        if ( valueInt != this.m_frame ){
                            this._sendFrame(valueInt);
                        }
                    }
                }
            }, this);

            var endLabel = new qx.ui.basic.Label("out of ");
            this.m_endLabel = new qx.ui.basic.Label("");
            var locationComposite = new qx.ui.container.Composite();
            locationComposite.setLayout(new qx.ui.layout.HBox(5));
            locationComposite.add(titleLabel);
            locationComposite.add(this.m_indexText);
            locationComposite.add(endLabel);
            locationComposite.add(this.m_endLabel);
            locationComposite.add(new qx.ui.core.Spacer(10, 10), {
                flex : 1
            });
            var settingsCheck = new qx.ui.form.CheckBox("Settings...");
            settingsCheck.setToolTipText( "Show additional animator settings.");
            settingsCheck.addListener(skel.widgets.Path.CHANGE_VALUE, function() {
                this._minMaxSettings(settingsCheck.getValue());
            }, this);
            locationComposite.add(settingsCheck);
            return locationComposite;
        },

        /**
         * Initialize the additional less-used settings in the UI.
         */
        _initSettings : function() {
            this.m_endWrapRadio = new qx.ui.form.RadioButton("Wrap");
            skel.widgets.TestID.addTestId( this.m_endWrapRadio, this.m_title+"WrapRadioButton");
            this.m_endWrapRadio.setToolTipText( "Wrap around when reaching an end value.");
            this.m_endWrapRadio.addListener(skel.widgets.Path.CHANGE_VALUE, function() {
                if (this.m_endWrapRadio.getValue()) {
                    this._sendEndBehavior(this.m_endWrapRadio.getLabel());
                }
            }, this);
            
            this.m_endReverseRadio = new qx.ui.form.RadioButton("Reverse");
            skel.widgets.TestID.addTestId( this.m_endReverseRadio, this.m_title+"ReverseRadioButton");
            this.m_endReverseRadio.setToolTipText( "Change direction when reaching an end value.");
            this.m_endReverseRadio.addListener(skel.widgets.Path.CHANGE_VALUE, function() {
                if (this.m_endReverseRadio.getValue()) {
                    this._sendEndBehavior(this.m_endReverseRadio.getLabel());
                }
            }, this);
            
            this.m_endJumpRadio = new qx.ui.form.RadioButton("Jump");
            skel.widgets.TestID.addTestId( this.m_endJumpRadio, this.m_title+"JumpRadioButton");
            this.m_endJumpRadio.setToolTipText( "Move from one end to the other end.");
            this.m_endJumpRadio.addListener(skel.widgets.Path.CHANGE_VALUE, function() {
                if (this.m_endJumpRadio.getValue()) {
                    this._sendEndBehavior(this.m_endJumpRadio.getLabel());
                }
            }, this);
           
            var endRadioGroup = new qx.ui.form.RadioGroup();
            endRadioGroup.add(this.m_endWrapRadio, this.m_endReverseRadio, this.m_endJumpRadio);

            var speedLabel = new qx.ui.basic.Label("Rate:");
            this.m_speedSpinBox = new qx.ui.form.Spinner(1, 10, 100);
            skel.widgets.TestID.addTestId( this.m_speedSpinBox, this.m_title+"Rate");
            this.m_speedSpinBox.setToolTipText( "Set the speed of the animation.");

            this.m_speedSpinBox.addListener(skel.widgets.Path.CHANGE_VALUE, function() {
                this._setTimerSpeed();
                this._sendFrameRate();
            }, this);
            var stepLabel = new qx.ui.basic.Label("Step:");
            this.m_stepSpin = new qx.ui.form.Spinner(1, 1, 100);
            skel.widgets.TestID.addTestId( this.m_stepSpin, this.m_title+"StepIncrement");
            this.m_stepSpin.setToolTipText( "Set the step increment.");
            this.m_stepSpin.bind("value", this, "frameStep");
            this.m_stepSpin.addListener( skel.widgets.Path.CHANGE_VALUE, function(){
                this._sendFrameStep();
            });

            this.m_settingsComposite = new qx.ui.container.Composite();
            this.m_settingsComposite.setLayout(new qx.ui.layout.HBox(5));
            this.m_settingsComposite.add(new qx.ui.core.Spacer(10, 10), {
                flex : 1
            });
            this.m_settingsComposite.add(stepLabel);
            this.m_settingsComposite.add(this.m_stepSpin);
            this.m_settingsComposite.add(new qx.ui.core.Spacer(10, 10), {
                flex : 1
            });
            this.m_settingsComposite.add(this.m_endWrapRadio);
            this.m_settingsComposite.add(this.m_endReverseRadio);
            this.m_settingsComposite.add(this.m_endJumpRadio);
            this.m_settingsComposite.add(new qx.ui.core.Spacer(10, 10), {
                flex : 1
            });
            this.m_settingsComposite.add(speedLabel);
            this.m_settingsComposite.add(this.m_speedSpinBox);
            this.m_settingsComposite.add(new qx.ui.core.Spacer(10, 10), {
                flex : 1
            });
        },
        
 
        

        /**
         * Initialize the shared variable for the settings.
         */
        _initSharedVars : function() {
            this.m_connector = mImport("connector");

            //Kick off a commad to get step size, wrap, etc (that change less frequently).
            var paramMap = "type:"+this.m_title;
            var pathDict = skel.widgets.Path.getInstance();
            var regCmd = this.m_winId + pathDict.SEP_COMMAND + "addAnimator";
            this.m_connector.sendCommand( regCmd, paramMap, this._registrationCB(this));
        },
        
        /**
         * Initialize the shared variable for the selection.
         */
        _initSharedVarsSelection : function(){
             
            //Kick off a command to get frame index, lower bound, and upper bound.
            var pathDict = skel.widgets.Path.getInstance();
           
            var regCmd = this.m_animId +pathDict.SEP_COMMAND + "getSelection";
            this.m_connector.sendCommand( regCmd, "", this._selectionCB(this));
        },
        
 
        

        /**
         * Initialize the slider controls.
         * @return {qx.ui.container.Composite} a container containing the slider controls.
         */
        _initSliderControls : function() {
            this.m_lowBoundsSpinner = new qx.ui.form.Spinner(0, 0, 100);
            this.m_lowBoundsSpinner.addListener( skel.widgets.Path.CHANGE_VALUE, function(ev){
                //Low bound spinner value must be less than high bound spinner value.
                var newVal = ev.getData();
                if ( newVal > this.m_highBoundsSpinner.getValue() ){
                    this.m_lowBoundsSpinner.setValue( ev.getOldData());
                }
                else {
                    this._sendUserLowerBound();
                }
            }, this );
            skel.widgets.TestID.addTestId( this.m_lowBoundsSpinner, this.m_title+"LowerBoundSpin");
            this.m_lowBoundsSpinner.setToolTipText( "Set a lower bound for valid values.");
            this.m_slider = new qx.ui.form.Slider();
            this.m_slider.addListener(skel.widgets.Path.CHANGE_VALUE, function( ev ) {
                if (this.m_inUpdateState) {
                    return;
                }
                
                if ( this.m_frame != this.m_slider.getValue()){
                    this._sendFrame(this.m_slider.getValue());
                }
            }, this);

            this.m_highBoundsSpinner = new qx.ui.form.Spinner(0, 100, 100);
            this.m_highBoundsSpinner.addListener( skel.widgets.Path.CHANGE_VALUE, function(ev){
                var newVal = ev.getData();
                if ( newVal < this.m_lowBoundsSpinner.getValue() ){
                    this.m_highBoundsSpinner.setValue( ev.getOldData());
                }
                else {
                    this._sendUserUpperBound();
                }
            }, this );
            skel.widgets.TestID.addTestId( this.m_highBoundsSpinner, this.m_title+"UpperBoundSpin");
            this.m_highBoundsSpinner.setToolTipText( "Set an upper bound for valid values");
            var sliderComposite = new qx.ui.container.Composite();
            sliderComposite.setLayout(new qx.ui.layout.HBox(5));
            sliderComposite.add(this.m_lowBoundsSpinner);
            sliderComposite.add(this.m_slider, {
                flex : 1
            });
            sliderComposite.add(this.m_highBoundsSpinner);
            return sliderComposite;
        },

        /**
         * Initialize the tape recorder part of the UI.
         * @return {qx.ui.container.Composite} a container containing the tape recorder.
         */
        _initToolBar : function() {
            // Toolbar
            var toolbar = new qx.ui.toolbar.ToolBar();
            toolbar.setSpacing(2);

            this.m_startButton = new qx.ui.toolbar.Button("",
                    "skel/icons/dblarrowleft.png");
            skel.widgets.TestID.addTestId( this.m_startButton, this.m_title+"TapeDeckFirstValue");
            this.m_startButton.addListener("execute", this._goToStart, this);
            this.m_startButton.setToolTipText( "Go to the first valid value");

            this.m_revPlayButton = new qx.ui.form.ToggleButton("",
                    "skel/icons/movie-play-reverse16.png");
            skel.widgets.TestID.addTestId( this.m_revPlayButton, this.m_title+"TapeDeckReversePlay");
            this.m_revPlayButton.setToolTipText( "Animate in a reverse direction.");
            this.m_revPlayButton.addListener("execute", function() {
                this.m_playButton.setValue(false);
                this._play(false);
            }, this);

            this.m_revStepButton = new qx.ui.toolbar.Button("",
                    "skel/icons/movie-previous-frame16.png");
            skel.widgets.TestID.addTestId( this.m_revStepButton, this.m_title+"TapeDeckDecrement");
            this.m_revStepButton.setToolTipText( "Decrease by one step value.");
            this.m_revStepButton.addListener("execute", this._decrementValue, this);
            
            this.m_stopButton = new qx.ui.toolbar.Button("",
                    "skel/icons/movie-stop16.png");
            skel.widgets.TestID.addTestId( this.m_stopButton, this.m_title+"TapeDeckStopAnimation");
            this.m_stopButton.addListener("execute", this._stop, this);
            this.m_stopButton.setToolTipText( "Stop the animation.");

            this.m_stepButton = new qx.ui.toolbar.Button("",
                    "skel/icons/movie-next-frame16.png");
            skel.widgets.TestID.addTestId( this.m_stepButton, this.m_title +"TapeDeckIncrement");
            this.m_stepButton.addListener("execute", this._incrementValue, this);
            this.m_stepButton.setToolTipText( "Increase by one step value.");

            this.m_playButton = new qx.ui.form.ToggleButton("",
                    "skel/icons/movie-play16.png");
            skel.widgets.TestID.addTestId( this.m_playButton, this.m_title+"TapeDeckPlayButton");
            this.m_playButton.setToolTipText( "Animate in a forward direction.");
            this.m_playButton.addListener("execute", function() {
                this.m_revPlayButton.setValue(false);
                this._play(true);
            }, this);

            this.m_endButton = new qx.ui.toolbar.Button("",
                    "skel/icons/dblarrowright.png");
            skel.widgets.TestID.addTestId( this.m_endButton, this.m_title+"TapeDeckLastValue");
            this.m_endButton.addListener("execute", this._goToEnd, this);
            this.m_endButton.setToolTipText( "Go to the last valid value.");
            toolbar.add(this.m_startButton);
            toolbar.add(this.m_revPlayButton);
            toolbar.add(this.m_revStepButton);
            toolbar.add(this.m_stopButton);
            toolbar.add(this.m_stepButton);
            toolbar.add(this.m_playButton);
            toolbar.add(this.m_endButton);
            var buttonComposite = new qx.ui.container.Composite();
            buttonComposite.setLayout(new qx.ui.layout.HBox(5));
            buttonComposite.add(new qx.ui.core.Spacer(10, 10), {
                flex : 1
            });
            buttonComposite.add(toolbar);
            buttonComposite.add(new qx.ui.core.Spacer(10, 10), {
                flex : 1
            });
            return buttonComposite;

        },


        /**
         * Show or hide the less-used additional animator settings.
         * 
         * @param maximize
         *                {Boolean} true if all the additional settings should
         *                be shown; false otherwise.
         */
        _minMaxSettings : function(maximize) {
            if (maximize) {
                this._add(this.m_settingsComposite);
            } else {
                this._remove(this.m_settingsComposite);
            }
        },
        
        /**
         * One of the animators has started playing a movie; set movie widgets disabled
         * if it is not us.
         * @param title {String} - the name of the animator playing the movie.
         */
        movieStarted : function( title ){
            if ( this.m_title !== title ){
                this._setWidgetsEnabled( false );
            }
        },
        
        /**
         * One of the animators has stopped playing a movie; set movie widgets disabled
         * if it is not us.
         * @param title {String} - the name of the animator that has stopped the movie.
         */
        movieStopped : function( title ){
            if ( this.m_title !== title ){
                this._setWidgetsEnabled( true );
            }
        },

        /**
         * Start the animation.
         * @param forward {Boolean} true if the frame position should be increased; false,
         *        if the frame position should be decreased.
         */
        _play : function(forward) {
            var data = {
                title : this.m_title
            };
            this.fireDataEvent( "movieStart", data );
            if (this.m_timer) {
                this.m_timer.stop();
                if (this.m_timer.hasListener("interval")) {
                    this.m_timer.removeListenerById(this.m_timerListener);
                }

                if (forward) {
                    this.m_timerListener = this.m_timer.addListener("interval",
                            this._increaseValue, this);
                } else {
                    this.m_timerListener = this.m_timer.addListener("interval",
                            this._decreaseValue, this);
                }
                this.m_timer.start();
            }
        },
        
        /**
         * Initialize the shared variable that controls the settings.
         * @param anObject {skel.boundWidgets.Animator}.
         * @return {Function} the registration callback.
         */
        _registrationCB : function( anObject ){
            return function( id ){
                //Initialize the shared variable that manages the rate, endBehavior and step.
                anObject.m_sharedVar = anObject.m_connector.getSharedVar( id );
                anObject.m_sharedVar.addCB( anObject._animationCB.bind( anObject ));
                anObject._animationCB();
                anObject.m_animId = id;
                anObject._initSharedVarsSelection();
            };
        },
        
        
        /**
         * Initialization of the shared variable that controls the selection.
         * @param anObject {skel.boundWidgets.Animator}.
         * @return {Function} the selection callback.
         */
        _selectionCB : function( anObject ){
            return function( id ){
                anObject.m_sharedVarSelection = anObject.m_connector.getSharedVar( id );
                anObject.m_sharedVarSelection.addCB( anObject._selectionResetCB.bind( anObject ));
                anObject._selectionResetCB();
            };
        },
        
        /**
         * Callback for a change in the selection.
         * @param val {String} the JSON representing the animation selection.
         */
        _selectionResetCB : function(){
            if ( this.m_sharedVarSelection !== null ){
                var val = this.m_sharedVarSelection.get();
                if ( val ){
                    if ( this._frameCB ){
                        try {
                            var frameObj = JSON.parse( val );
                            this.m_frame = frameObj.frame;
                            this.m_frameLow = frameObj.frameStart;
                            this.m_frameHigh = frameObj.frameEnd;
                            this._frameStartCB();
                            this._frameEndCB( );
                            this._frameCB( );
                            this._frameBoundsUserCB( frameObj.frameStartUser, frameObj.frameEndUser );
                        }
                        catch( err ){
                            console.log( "Selection resetCB, could not parse: "+val );
                            console.log( err );
                        }
                    }
                }
            }
        },

        /**
         * Send a command to the server indicating the new end behavior (wrap, jump, etc).
         * @param behavior {String} a descriptor for the end behavior.
         */
        _sendEndBehavior : function(behavior) {
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_animId + path.SEP_COMMAND + "setEndBehavior";
                var params = "endBehavior:"+behavior;
                
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },

        /**
         * Notify the server of the new frame value.
         * @param frameIndex {Number} the new frame value.
         */
        _sendFrame : function(frameIndex) {
            if (this.m_connector !== null) {
                var paramMap = frameIndex;
                var path = skel.widgets.Path.getInstance();
                var setFramePath = this.m_animId  + path.SEP_COMMAND + "setFrame";
                this.m_connector.sendCommand(setFramePath, paramMap, function(val) {});
            }
        },
        
        /**
         * Send a command to the server indicating the new frame rate.
         */
        _sendFrameRate : function() {
            if ( this.m_connector !== null ){
                if ( this.m_animId !== null && this.m_animId.length > 0 ){
                    var path = skel.widgets.Path.getInstance();
                    var cmd = this.m_animId + path.SEP_COMMAND + "setFrameRate";
                    var params = "frameRate:"+this.m_speedSpinBox.getValue();
                    this.m_connector.sendCommand( cmd, params, function(){});
                }
            }
        },
        
        /**
         * Send a command to the server indicating the new frame step size.
         */
        _sendFrameStep : function() {
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_animId + path.SEP_COMMAND + "setFrameStep";
                var params = "frameStep:"+this.m_stepSpin.getValue();
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Send the user set lower bound to the server.
         */
        _sendUserLowerBound : function(){
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_animId + path.SEP_COMMAND + "setLowerBoundUser";
                var params = this.m_lowBoundsSpinner.getValue();
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },
        
        /**
         * Send the user set upper bound to the server.
         */
        _sendUserUpperBound : function(){
            if ( this.m_connector !== null ){
                var path = skel.widgets.Path.getInstance();
                var cmd = this.m_animId + path.SEP_COMMAND + "setUpperBoundUser";
                var params = this.m_highBoundsSpinner.getValue();
                this.m_connector.sendCommand( cmd, params, function(){});
            }
        },

        /**
         * Adjust the speed of the animation.
         */
        _setTimerSpeed : function() {
            var rate = this.m_speedSpinBox.getValue();
            //High values should be fast.
            var maxRate = this.m_speedSpinBox.getMaximum();
            var ratePercent = rate / maxRate;
            ratePercent = 1 - ratePercent;
            var interval  = ratePercent * 2000;
            if ( interval < 10 ){
                interval = 10;
            }
            interval = Math.round( interval );
            if (this.m_timer !== null ) {
                this.m_timer.setInterval(interval);
            }
        },
        
        /**
         * Change the enabled status of the movie widgets.
         * @param enable {boolean} - true if the widgets should be enabled; false otherwise.
         */
        _setWidgetsEnabled : function( enable ){
            this.m_slider.setEnabled( enable );
            this.m_indexText.setEnabled( enable );
            this.m_playButton.setEnabled( enable );
            this.m_revPlayButton.setEnabled( enable );
            this.m_startButton.setEnabled( enable );
            this.m_revStepButton.setEnabled( enable );
            this.m_stopButton.setEnabled( enable );
            this.m_stepButton.setEnabled( enable );
            this.m_endButton.setEnabled( enable );
        },

        /**
         * Stop the animation.
         */
        _stop : function() {
            var data = {
                title : this.m_title
            };
            this.fireDataEvent( "movieStop", data );
            if (this.m_timer !== null) {
                this.m_playButton.setValue(false);
                this.m_revPlayButton.setValue(false);
                this.m_timer.stop();
            }
        },

        //Title will be something like 'channel','image',etc.
        m_title : "",
        //m_winId is the object id of the containing DisplayWindowAnimation, something like '/CartaObjects/8'.
        m_winId : "",
        //The object id of the look up to use for finding updates; corresponds to a C++ AnimatorType object id.
        m_animId : "",

        //UI Widgets
        m_settingsComposite : null,
        m_endLabel : null,
        m_timer : null,
        m_slider : null,
        m_timerListener : null,
        m_indexText : null,
        m_endWrapRadio : null,
        m_stepSpin : null,
        m_endReverseRadio : null,
        m_endJumpRadio : null,
        m_lowBoundsSpinner : null,
        m_highBoundsSpinner : null,
        m_playButton : null,
        m_revPlayButton : null,
        m_speedSpinBox : null,
        m_startButton : null,
        m_revStepButton : null,
        m_stopButton : null,
        m_stepButton : null,
        m_endButton : null,

        //State variables
        m_connector : null,
        m_sharedVar : null,
        m_sharedVarSelection : null,
        m_identifier : null,
        m_inUpdateState : false,
        
        m_frame : null,
        m_frameLow : null,
        m_frameHigh : null
    },

    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
        },
        frameStep : {
            init : 1,
            event : "changeFrameStep"
        }

    }

});