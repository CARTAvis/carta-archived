/**
 * A 'tape deck' UI for animating data.
 */
/*global mImport */
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

    members : {
        /**
         * Callback for when the animator settings shared variable has changed value.
         * @param val {String} the new settings.
         */
        _animationCB : function( ){
            var val = this.m_sharedVar.get();
            try {
                var animObj = JSON.parse( val );
                this._endBehaviorCB( animObj.endBehavior );
                this._frameStepCB( animObj.frameStep );
                this._frameRateCB( animObj.frameRate );
            }
            catch( err ){
                console.log( "Could not parse: "+val );
            }
        },
        
        /**
         * Update the new frame position.
         */
        _applyFrame : function() {
            if (this.m_indexText !== null) {
                this.m_indexText.setValue(this.getFrame() + "");
            }
        },

        /**
         * Update the maximum number of frames available.
         */
        _applyFrameUpperBound : function() {
            if (this.m_endLabel !== null) {
                this.m_endLabel.setValue(this.getFrameUpperBound() + "");
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
            var val = this.getFrame() - this.getFrameStep();

            var lowerBound = this.m_slider.getMinimum();
            if (this.m_endJumpRadio.getValue()) {
                if (this.getFrame() > lowerBound) {
                    val = lowerBound;
                } else {
                    val = this.m_slider.getMaximum();
                }
            }

            // May need to do a wrap.
            if (val < lowerBound) {
                if (this.m_endWrapRadio.getValue()) {
                    val = val + this.m_slider.getMaximum();
                } else if (this.m_endReverseRadio.getValue()) {
                    if (this.m_revPlayButton.getValue()) {
                        this.m_playButton.execute();
                    } else {
                        val = lowerBound + 1;
                    }
                }
            }
            this._setFrame(val);
        },

        /**
         * Callback for behavior when the frame is at the lower or upper extreme
         * of its range.
         * @param val {String} the new end behavior.
         */
        _endBehaviorCB : function(val) {
            if (val) {
                if (val == "Wrap") {
                    this.m_endWrapRadio.setValue(true);
                } else if (val == "Reverse") {
                    this.m_endReverseRadio.setValue(true);
                } else if (val == "Jump") {
                    this.m_endJumpRadio.setValue(true);
                } else {
                    console.log("Unrecognized end behavior: " + val);
                }
            }
        },

        /**
         * Shared var callback. Read in the position of the animator and render
         * it.
         * 
         * @param val
         *                {String} Number of plugins.
         * @private
         */
        _frameCB : function(val) {
            if (val) {
                this.setFrame( val );
            }
        },

        /**
         * Callback for the frame end behavior.
         * @param val {Number} the new upper bound.
         */
        _frameEndCB : function(val) {
            if (val) {
                this.setFrameUpperBound( val );
            }
        },

        /**
         * Callback for the frame step size.
         * @param val {Number} the new step size.
         */
        _frameStepCB : function(val) {
            if (val) {
                this.setFrameStep( val );
            }
        },

        /**
         * Callback for the speed of the animation.
         * @param val {Number} the new speed.
         */
        _frameRateCB : function(val) {
            if (val) {
                this.m_speedSpinBox.setValue(val);
            }
        },

        /**
         * Callback for the frame lower bound.
         * @param val {Number} the new lower bound.
         */
        _frameStartCB : function(val) {
            if (val) {
                this.setFrameLowerBound( val );
            }
        },

        /**
         * Move to the smallest available frame.
         */
        _goToStart : function() {
            this._stop();
            var lowBound = this.getFrameLowerBound();
            if ( lowBound != this.getFrame() ){
                this._setFrame( lowBound );
            }
        },

        /**
         * Move to the largest available frame.
         */
        _goToEnd : function() {
            this._stop();
            var highBound = this.getFrameUpperBound();
            if ( highBound != this.getFrame()){
                this._setFrame( highBound );
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
            var val = this.getFrame() + this.getFrameStep();

            var upperBound = this.m_slider.getMaximum();
            if (this.m_endJumpRadio.getValue()) {
                if (this.getFrame() < upperBound) {
                    val = upperBound;
                } else {
                    val = this.m_slider.getMinimum();
                }
            }

            // May need to do a wrap.
            if (val > upperBound) {
                if (this.m_endWrapRadio.getValue()) {
                    val = val % upperBound;
                } else if (this.m_endReverseRadio.getValue()) {
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
            this._setFrame(val);
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
         */
        _initLocation : function() {
            var titleLabel = new qx.ui.basic.Label(this.m_title);
            this.m_indexText = new qx.ui.form.TextField();
            this.m_indexText.addListener("input", function(e) {
                var value = this.m_indexText.getValue();
                var valueInt = parseInt(value);
                if (!isNaN(valueInt)) {
                    if (valueInt <= this.m_slider.getMaximum() && valueInt >= this.m_slider.getMinimum()) {
                        if ( valueInt != this.getFrame() ){
                            this._setFrame(valueInt);
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
            settingsCheck.addListener("changeValue", function() {
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
            this.m_endWrapRadio.addListener("changeValue", function() {
                if (this.m_endWrapRadio.getValue()) {
                    this._setEndBehavior("Wrap");
                }
            }, this);
            this.m_endReverseRadio = new qx.ui.form.RadioButton("Reverse");
            this.m_endReverseRadio.addListener("changeValue", function() {
                if (this.m_endReverseRadio.getValue()) {
                    this._setEndBehavior("Reverse");
                }
            }, this);
            this.m_endJumpRadio = new qx.ui.form.RadioButton("Jump");
            this.m_endJumpRadio.addListener("changeValue", function() {
                if (this.m_endJumpRadio.getValue()) {
                    this._setEndBehavior("Jump");
                }
            }, this);
            var endRadioGroup = new qx.ui.form.RadioGroup();
            endRadioGroup.add(this.m_endWrapRadio);
            endRadioGroup.add(this.m_endReverseRadio);
            endRadioGroup.add(this.m_endJumpRadio);
            /*if (this.m_stateEndBehavior != null) {
                this._endBehaviorCB(this.m_stateEndBehavior.get());
            }*/

            var speedLabel = new qx.ui.basic.Label("Rate:");
            this.m_speedSpinBox = new qx.ui.form.Spinner(1, 10, 100);
            /*if (this.m_stateFrameRate != null) {
                this._frameRateCB(this.m_stateFrameRate.get());
            }*/
            /*this.m_speedSpinBox.addListener("changeValue", function() {

                this._setTimerSpeed();
                this.m_stateFrameRate.set(this.m_speedSpinBox.getValue());
            }, this);*/
            var stepLabel = new qx.ui.basic.Label("Step:");
            var stepSpinBox = new qx.ui.form.Spinner(1, 1, 100);
            this.bind("frameUpperBound", stepSpinBox, "maximum");
            stepSpinBox.bind("value", this, "frameStep");
            /*if (this.m_stateFrameStep != null) {
                this._frameStepCB(this.m_stateFrameStep.get());
            }*/

            this.m_settingsComposite = new qx.ui.container.Composite();
            this.m_settingsComposite.setLayout(new qx.ui.layout.HBox(5));
            this.m_settingsComposite.add(new qx.ui.core.Spacer(10, 10), {
                flex : 1
            });
            this.m_settingsComposite.add(stepLabel);
            this.m_settingsComposite.add(stepSpinBox);
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
           
            var regCmd = this.m_animId + pathDict.SEP_COMMAND + "getSelection";
            this.m_connector.sendCommand( regCmd, "", this._selectionCB(this));
        },
        
 
        

        /**
         * Initialize the slider controls.
         */
        _initSliderControls : function() {
            var lowBoundSpinner = new qx.ui.form.Spinner(0, 0, 100);
            this.m_slider = new qx.ui.form.Slider();
            this.m_slider.addListener("changeValue", function() {
                if (this.m_inUpdateState) {
                    return;
                }
                if ( this.getFrame() != this.m_slider.getValue()){
                    this._setFrame(this.m_slider.getValue());
                }
            }, this);

            var highBoundSpinner = new qx.ui.form.Spinner(0, 100, 100);
            var sliderComposite = new qx.ui.container.Composite();
            sliderComposite.setLayout(new qx.ui.layout.HBox(5));
            sliderComposite.add(lowBoundSpinner);
            sliderComposite.add(this.m_slider, {
                flex : 1
            });
            sliderComposite.add(highBoundSpinner);
            this.bind("frame", this.m_slider, "value");
            this.bind("frameLowerBound", lowBoundSpinner, "minimum");
            this.bind("frameUpperBound", highBoundSpinner, "maximum");
            //this.bind("frameUpperBound", highBoundsSpinner, "value");
            lowBoundSpinner.bind("value", this.m_slider, "minimum");
            highBoundSpinner.bind("value", this.m_slider, "maximum");
            lowBoundSpinner.bind("value", highBoundSpinner, "minimum");
            highBoundSpinner.bind("value", lowBoundSpinner, "maximum");
            return sliderComposite;
        },

        /**
         * Initialize the tape recorder part of the UI.
         */
        _initToolBar : function() {
            // Toolbar
            var toolbar = new qx.ui.toolbar.ToolBar();
            toolbar.setSpacing(2);

            var startButton = new qx.ui.toolbar.Button("",
                    "skel/icons/dblarrowleft.png");
            startButton.addListener("execute", this._goToStart, this);

            this.m_revPlayButton = new qx.ui.form.ToggleButton("",
                    "skel/icons/movie-play-reverse16.png");
            this.m_revPlayButton.addListener("execute", function() {
                this.m_playButton.setValue(false);
                this._play(false);
            }, this);

            var revStepButton = new qx.ui.toolbar.Button("",
                    "skel/icons/movie-previous-frame16.png");
            revStepButton.addListener("execute", this._decrementValue, this);

            var stopButton = new qx.ui.toolbar.Button("",
                    "skel/icons/movie-stop16.png");
            stopButton.addListener("execute", this._stop, this);

            var stepButton = new qx.ui.toolbar.Button("",
                    "skel/icons/movie-next-frame16.png");
            stepButton.addListener("execute", this._incrementValue, this);

            this.m_playButton = new qx.ui.form.ToggleButton("",
                    "skel/icons/movie-play16.png");
            this.m_playButton.addListener("execute", function() {
                this.m_revPlayButton.setValue(false);
                this._play(true);
            }, this);

            var endButton = new qx.ui.toolbar.Button("",
                    "skel/icons/dblarrowright.png");
            endButton.addListener("execute", this._goToEnd, this);

            toolbar.add(startButton);
            toolbar.add(this.m_revPlayButton);
            toolbar.add(revStepButton);
            toolbar.add(stopButton);
            toolbar.add(stepButton);
            toolbar.add(this.m_playButton);
            toolbar.add(endButton);
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
         * Start the animation.
         * @param forward {Boolean} true if the frame position should be increased; false,
         * 			if the frame position should be decreased.
         */
        _play : function(forward) {
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
         */
        _selectionCB : function( anObject ){
            return function( id ){
                anObject.m_sharedVarSelection = anObject.m_connector.getSharedVar( id );
                anObject.m_sharedVarSelection.addCB( anObject._selectionResetCB.bind( anObject ));
                anObject._selectionResetCB( anObject.m_sharedVarSelection.get());
            };
        },
        
        /**
         * Callback for a change in the selection.
         */
        _selectionResetCB : function( val ){
            if ( val ){
                if ( this._frameCB ){
                    try {
                        var frameObj = JSON.parse( val );
                        this._frameCB( frameObj.frame );
                        this._frameStartCB( frameObj.frameStart );
                        this._frameEndCB( frameObj.frameEnd );
                    }
                    catch( err ){
                        console.log( "Could not parse: "+val );
                    }
                }
            }
        },

        /**
         * Set whether frames at upper or lower bounds should wrap, reverse, etc.
         * @param behavior {String} a descriptor for the behavior.
         */
        _setEndBehavior : function(behavior) {
            if (this.m_stateEndBehavior !== null) {
                this.m_stateEndBehavior.set(behavior);
            }
        },

        /**
         * Notify the server of the new frame value.
         * @param frameIndex {Number} the new frame value.
         */
        _setFrame : function(frameIndex) {
            if (this.m_connector !== null) {
                var paramMap = frameIndex.toString();
                var path = skel.widgets.Path.getInstance();
                var setFramePath = this.m_animId + path.SEP_COMMAND + "setFrame";
                this.m_connector.sendCommand(setFramePath, paramMap, function(val) {});
            }
        },

        /**
         * Adjust the speed of the animation.
         */
        _setTimerSpeed : function() {
            var rate = this.m_speedSpinBox.getValue();
            //High values should be fast.
            rate = rate - this.m_speedSpinBox.getMaximum();
            rate = rate * 100;
            if (this.m_timer) {
                this.m_timer.setInterval(rate);
            }
        },

        /**
         * Stop the animation.
         */
        _stop : function() {
            if (this.m_timer) {
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
        m_endReverseRadio : null,
        m_endJumpRadio : null,
        m_playButton : null,
        m_revPlayButton : null,
        m_speedSpinBox : null,

        //State variables
        m_connector : null,
        m_sharedVar : null,
        m_sharedVarSelection : null,
        m_identifier : null,
        m_inUpdateState : false
    },

    properties : {
        appearance : {
            refine : true,
            init : "internal-area"
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
