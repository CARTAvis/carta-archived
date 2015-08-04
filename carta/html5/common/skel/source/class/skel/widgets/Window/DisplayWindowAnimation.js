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
                    construct : function(index, detached ) {
                        this.base(arguments, skel.widgets.Path.getInstance().ANIMATOR, index, detached );
                        this.setLayout(new qx.ui.layout.VBox(5));
                        this.m_links = [];
                        this._initSupportedAnimations();
                    },

                    members : {
                        
                        /**
                         * Get server side information about the list of all possible
                         * animators that are available.
                         */
                        _initSupportedAnimations : function(){
                            var path = skel.widgets.Path.getInstance();
                            this.m_sharedVarAnimations = this.m_connector.getSharedVar( path.ANIMATOR_TYPES );
                            this.m_sharedVarAnimations.addCB( this._sharedVarAnimationsCB.bind( this ));
                            this._sharedVarAnimationsCB();
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
                         * Notify all the animators that the movie has stopped playing.
                         * @param ev {qx.event.type.Data}.
                         */
                        _movieStopped : function( ev ){
                            var data = ev.getData();
                            for (var i = 0; i < this.m_supportedAnimations.length; i++) {
                                var animId = this.m_supportedAnimations[i];
                                if ( this.m_animators[animId] !== undefined ){
                                    this.m_animators[animId].movieStopped( data.title );
                                }
                            }
                        },
                        
                        /**
                         * Notify all the animators that a movie has started playing.
                         * @parm ev {qx.event.type.Data}.
                         */
                        _movieStarted : function( ev ){
                            var data = ev.getData();
                            for ( var i = 0; i < this.m_supportedAnimations.length; i++ ){
                                var animId = this.m_supportedAnimations[i];
                                if ( this.m_animators[animId] !== undefined ){
                                    this.m_animators[animId].movieStarted( data.title );
                                }
                            }
                        },
                        
                        /**
                         * Returns true if the animator widget with the given identifier is visible;
                         *      false otherwise.
                         * @param animId {String} an identifier for an animator.
                         * @return {boolean} true if the animator is visible; false otherwise.
                         */
                        isVisible : function( animId ){
                            var visible = false;
                            if ( this.m_animators !== null ){
                                if ( this.m_content.indexOf( this.m_animators[animId] ) >= 0 ){
                                    visible = true;
                                }
                            }
                            return visible;
                        },
                        
                        /**
                         * Set the appearance of this window based on whether or not it is selected.
                         * @param selected {boolean} true if the window is selected; false otherwise.
                         * @param multiple {boolean} true if multiple windows can be selected; false otherwise.
                         */
                        setSelected : function(selected, multiple) {
                            arguments.callee.base.apply(this, arguments, selected, multiple );
                            this.updateCmds();
                        },
                        
                        /**
                         * Update the list of available animators from the server.
                         */
                        _sharedVarAnimationsCB : function( ){
                            var val = this.m_sharedVarAnimations.get();
                            if ( val ){
                                try {
                                    var animObj = JSON.parse( val );
                                    //Initialize the supported animations
                                    this.m_supportedAnimations = [];
                                    for (var i = 0; i < animObj.animators.length; i++ ){
                                        this.m_supportedAnimations[i] = animObj.animators[i];
                                    }
                                    //Update which animators should appear based on the state of this animator
                                    this._showHideAnimation( animObj );
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
                        _showHideAnimation : function( animObj ) {
                            if ( this.m_animators === null ){
                                this.m_animators = {};
                            }
                            if ( this.m_supportedAnimations === null ){
                                return;
                            }
                            if ( this.m_identifier === null || this.m_identifier.length === 0 ){
                                return;
                            }
                            for (var i = 0; i < this.m_supportedAnimations.length; i++) {
                                var animId = this.m_supportedAnimations[i];
                                var animVisible = false;
                                var index = animObj.animators.indexOf( animId );
                                if ( index >= 0 ){
                                    animVisible = true;
                                }
                                var oldVisible = this.isVisible( animId );
                                if (animVisible) {
                                    if (this.m_animators[animId] === undefined ) {
                                        this.m_animators[animId] = new skel.boundWidgets.Animator(animId, this.m_identifier);
                                        this.m_animators[animId].addListener( "movieStart", this._movieStarted, this );
                                        this.m_animators[animId].addListener( "movieStop", this._movieStopped, this );
                                    }
                                      
                                    if ( !oldVisible) {
                                        this.m_content.add(this.m_animators[animId]);
                                    }
                                } 
                                else {
                                    if ( oldVisible ) {
                                        this.m_content.remove(this.m_animators[animId]);
                                    }
                                }
                            }
                        },
                        
                        
                        /**
                         * Update the commands about which animation is visible..
                         */
                        updateCmds : function(){
                            var animAllCmd = skel.Command.Animate.CommandAnimations.getInstance();
                            if ( this.m_supportedAnimations !== null ){
                                for (var i = 0; i < this.m_supportedAnimations.length; i++) {
                                    var animId = this.m_supportedAnimations[i];
                                    var animCmd = animAllCmd.getCmd( animId );
                                    var visible = this.isVisible( animId );
                                    animCmd.setValue( visible );
                                }
                            }
                        },
                        
                        /**
                         * Show/hide animators based on server information.
                         * @param animObj {Object} server side information about the animators that
                         *      are visible.
                         */
                        windowSharedVarUpdate : function( animObj ){
                            this._showHideAnimation( animObj );
                        },
                        
                        //Shared variable containing all animations.
                        m_sharedVarAnimations : null,
                        
                        //List of all animators that are available
                        m_supportedAnimations : null,
                        
                        //Tape deck widgets
                        m_animators : null
                    }

                });
