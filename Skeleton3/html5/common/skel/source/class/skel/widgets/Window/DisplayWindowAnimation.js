/**
 * A display window specialized for controlling data animators.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 
 ******************************************************************************/

qx.Class
        .define(
                "skel.widgets.Window.DisplayWindowAnimation",
                {
                    extend : skel.widgets.Window.DisplayWindow,

                    /**
                     * Constructor.
                     */
                    construct : function(row, col, index, detached ) {
                        this.base(arguments, skel.widgets.Path.getInstance().ANIMATOR, row, col, index, detached );
                        this.setLayout(new qx.ui.layout.VBox(5));
                        this.m_links = [];
                        this._initSupportedAnimations();
                    },

                    members : {
                        /**
                         * Send one or more commands to the server to update it on what animators are on
                         * display.
                         */
                        _addRemoveAnimators : function(){
                            var val = this.m_sharedVar.get();
                            if ( val ){
                                try {
                                    var emptyFunction = function(){};
                                    var animObj = JSON.parse( this.m_sharedVar.get() );
                                    for ( var i = 0; i < this.m_checks.length; i++ ){
                                        var checkName = this.m_checks[i].getLabel();
                                        var selected = this.m_checks[i].getValue();
                                        var alreadyThere = false;
                                        if ( animObj.animators[checkName] ){
                                            alreadyThere = true;
                                        }
                                        
                                        //Decide if the animator's visibility has changed with respect to what the serve thinks.
                                        var changedValue = false;
                                        if ( ( !alreadyThere && selected) || (alreadyThere && !selected) ){
                                           changedValue = true;
                                        }
                                        
                                        //If the visibility of the animator has changed issue a command to tell the server to sync up.
                                        if ( changedValue ){
                                           var addRemVal = "addAnimator";
                                           if ( !selected ){
                                               addRemVal = "removeAnimator";
                                           }
                                           var path = skel.widgets.Path.getInstance();
                                           var cmd = this.m_identifier + path.SEP_COMMAND + addRemVal;
                                           var params = "type:"+this.m_checks[i].getLabel();
                                           this.m_connector.sendCommand( cmd, params, emptyFunction);
                                        }
                                    }
                                }
                                catch( err ){
                                    console.log( "Could not parse: "+val );
                                }
                            }
                        },
                        
                        
                        
                        /**
                         * Callback for a state change; update the animators that are displayed.
                         */
                        _animationCB : function( ){
                            if ( this.m_sharedVar ){
                                var val = this.m_sharedVar.get();
                                if ( val ){
                                    try {
                                        var animObj = JSON.parse( val );
                                        //Go through the supported animations.  If it is in the list, mark
                                        //as visible; otherwise mark as invisible.
                                        for ( var j = 0; j < this.m_checks.length; j++ ){
                                            var checkName = this.m_checks[j].getLabel();
                                            var checkIndex = animObj.animators.indexOf( checkName );
                                            var visible = false;
                                            if ( checkIndex >= 0 ){
                                                visible = true;
                                            }
                                            if ( this.m_checks[j].getValue() != visible ){
                                                this.m_checks[j].setValue( visible );
                                            }
                                        }
                                        this._showHideAnimation(animObj);
                                    }
                                    catch( err ){
                                        console.log( "Could not parse: "+val );
                                    }
                                }
                            }
                        },
                        
                        /**
                         * Add a callback to the main window shared variable to deal with animation
                         * related changes.
                         */
                        _initSharedVarAnim : function(){
                            this.m_sharedVar.addCB( this._animationCB.bind( this ));
                            this._animationCB();
                        },
                        
                        /**
                         * Get server side information about the list of all possible
                         * animators that are available.
                         */
                        _initSupportedAnimations : function(){
                            var path = skel.widgets.Path.getInstance();
                            this.m_sharedVarAnimations = this.m_connector.getSharedVar( path.ANIMATOR_TYPES );
                            this.m_sharedVarAnimations.addCB( this._sharedVarAnimationsCB.bind( this ));
                            this._sharedVarAnimationsCB( this.m_sharedVarAnimations.get());
                        },
                        
                        /**
                         * Initialize the window specific commands that are supported.
                         */
                        _initSupportedCommands : function(){
                            arguments.callee.base.apply(this, arguments);
                            var animCmd = skel.Command.Animate.CommandAnimations.getInstance();
                            this.m_supportedCmds.push( animCmd.getLabel() );
                        },
                        
                        /**
                         * Initialize the check boxes that indicate which animators
                         * are displayed from a server side array listing all available animators.
                         * @param animArray {Array} a list of available animators.
                         */
                        _initChecks : function( animArray ){
                            this.m_checks = [];
                            for (var i = 0; i < animArray.length; i++) {
                                var animId = animArray[i];
                                if ( this.m_checks.length <= i ){
                                    var check = new qx.ui.menu.CheckBox( animId);
                                    check.setValue( false );
                                    this.m_checks.push( check );
                                    this.m_checks[i].listener = this.m_checks[i].addListener(skel.widgets.Path.CHANGE_VALUE, this._addRemoveAnimators, this);
                                }
                            }
                        },

                        
                        /**
                         * Set the appearance of this window based on whether or not it is selected.
                         * @param selected {boolean} true if the window is selected; false otherwise.
                         * @param multiple {boolean} true if multiple windows can be selected; false otherwise.
                         */
                        setSelected : function(selected, multiple) {
                            arguments.callee.base.apply(this, arguments, selected, multiple );
                            var animAllCmd = skel.Command.Animate.CommandAnimations.getInstance();
                            for (var i = 0; i < this.m_checks.length; i++) {
                                var animId = this.m_checks[i].getLabel();
                                var animCmd = animAllCmd.getCmd( animId );
                                animCmd.setValue( this.m_checks[i].getValue());
                            }
                        },
                        
                        
                        /**
                         * Update the list of available animators from the server.
                         */
                        _sharedVarAnimationsCB : function( ){
                            var val = this.m_sharedVarAnimations.get();
                            if ( val ){
                                try {
                                    var animObj = JSON.parse( val );
                                    //Initialize the check boxes
                                    this._initChecks( animObj.animators);
                                    //Update which check boxes should be checked based on the
                                    //state of THIS animator.
                                    this._animationCB();
                                    //Show/hide individual animations based on the checked status.
                                    this._showHideAnimation();
                                }
                                catch( err ){
                                    console.log( "Could not parse: "+val );
                                }
                            }
                        },
                        

                        
                        /**
                         * Adds or removes a specific animator from the display
                         * based on what the user has selected from the menu.
                         */
                        _showHideAnimation : function( ) {
                            if ( this.m_animators === null ){
                                this.m_animators = {};
                            }

                            for (var i = 0; i < this.m_checks.length; i++) {

                                var animId = this.m_checks[i].getLabel();
                                var check = this.m_checks[i];
                                var animVisible = check.getValue();
                                if (animVisible) {
                                    if (this.m_animators[animId] === undefined ) {
                                        this.m_animators[animId] = new skel.boundWidgets.Animator(animId, this.m_identifier);
                                    }
                                      
                                    if (this.m_content.indexOf(this.m_animators[animId]) < 0) {
                                        this.m_content.add(this.m_animators[animId]);
                                    }
                                } 
                                else {
                                    if (this.m_content.indexOf(this.m_animators[animId]) >= 0) {
                                        this.m_content.remove(this.m_animators[animId]);
                                    }
                                }
                            }
                        },
                        
                        /**
                         * Implemented to remove the title.
                         */
                        windowIdInitialized : function() {
                            arguments.callee.base.apply(this, arguments );
                            this._initSharedVarAnim();
                        },
                        
                        //List of all animators that are available.
                        m_sharedVarAnimations : null,
                       
                        m_checks : null,
                        m_animators : null
                    }

                });
