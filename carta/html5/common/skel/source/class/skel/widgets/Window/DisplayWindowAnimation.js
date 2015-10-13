/**
 * A display window specialized for controlling data animators.
 */
/*global mImport */
/*******************************************************************************
 * @ignore( mImport)
 
 ******************************************************************************/

qx.Class.define(
"skel.widgets.Window.DisplayWindowAnimation",
{
    extend : skel.widgets.Window.DisplayWindow,

    /**
     * Constructor.
     */
    construct : function(index, detached ) {
        this.base(arguments, skel.widgets.Path.getInstance().ANIMATOR, index, detached );
        this.m_content.setLayout(new qx.ui.layout.VBox(5));
        this.m_links = [];
    },

    members : {
        
        /**
         * Update the list of available animators from the server.
         * @param animators {Array} - a list of available animators.
         */
        _addSupportedAnimations : function( animators ){
            //Initialize the supported animators
            if ( this.m_animators === null ){
                this.m_animators = [];
            }
            
            for (var i = 0; i < animators.length; i++ ){
                if ( animators[i].visible ){
                    var animId = animators[i].type;
                    //Go through the existing animators and see if there is already one
                    //with the given title.
                    var animator = this._getAnimator( animId );
                    //Add a new animator
                    if ( animator === null ) {
                        var anim = new skel.boundWidgets.Animator(animId, this.m_identifier);
                        anim.addListener( "movieStart", this._movieStarted, this );
                        anim.addListener( "movieStop", this._movieStopped, this );
                        this.m_animators.push( anim );
                    }
                }
            }
        },
        
        /**
         * Returns the animator of the corresponding type or null if there is no
         * such animator.
         * @param type {String} - the type of axis being animated.
         * @return {skel.boundWidgets.Animator} - the corresponding animator.
         */
        _getAnimator : function( type ){
            var target = null;
            if ( this.m_animators !== null ){
                for ( var i = 0; i < this.m_animators.length; i++ ){
                    if ( this.m_animators[i].getTitle() == type ){
                        target = this.m_animators[i];
                    }
                }
            }
            return target;
        },
        
        /**
         * Returns the names of the existing animators.
         * @return {Array}- the names of existing animators.
         */
        _getSupportedAnimators : function(){
            var supportedAnims = [];
            if ( this.m_animators != null ){
                for ( var i = 0; i < this.m_animators.length; i++ ){
                    supportedAnims.push( this.m_animators[i].getTitle() );
                }
            }
            return supportedAnims;
        },
        
        /**
         * Initialize the window specific commands that are supported.
         */
        _initSupportedCommands : function(){
            var animCmd = skel.Command.Animate.CommandAnimations.getInstance();
            if ( this.m_supportedCmds.length == 0 ){
                var linksCmd = skel.Command.Link.CommandLink.getInstance();
                this.m_supportedCmds.push( linksCmd.getLabel() );
                this.m_supportedCmds.push( animCmd.getLabel() );
                arguments.callee.base.apply(this, arguments);
            }
            //this.updateCmds();
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
                var animator = this._getAnimator( animId );
                if ( animator !== null ){
                    if ( this.m_content.indexOf( animator ) >= 0 ){
                        visible = true;
                    }
                }
            }
            return visible;
        },
        
        /**
         * Notify all the animators that the movie has stopped playing.
         * @param ev {qx.event.type.Data}.
         */
        _movieStopped : function( ev ){
            var data = ev.getData();
            for (var i = 0; i < this.m_animators.length; i++) {
                this.m_animators[i].movieStopped( data.title );
            }
        },
        
        /**
         * Notify all the animators that a movie has started playing.
         * @parm ev {qx.event.type.Data}.
         */
        _movieStarted : function( ev ){
            var data = ev.getData();
            for ( var i = 0; i < this.m_animators.length; i++ ){
                this.m_animators[i].movieStarted( data.title );
            }
        },
        
        
        /**
         * Remove animators that are for axes that do not exist for currently loaded
         * images.
         * @param animators {Array} - a list of currently supported animators.
         */
        _removeUnsupportedAnimations : function( animators ){
            if ( this.m_animators !== null ){
                //Determine if the animator still exists.
                for ( var i = this.m_animators.length-1; i >= 0; i-- ){
                    var animIndex = -1;
                    for ( var j = 0; j < animators.length; j++ ){
                        if ( animators[j].type == this.m_animators[i].getTitle() ){
                            if ( !animators[j].visible ){
                                animIndex = j;
                                break;
                            }
                        }
                    }
                   
                    //The animator does not still exists
                    if ( animIndex >= 0 ){
                        //Remove the animator from the view
                        if ( this.m_content.indexOf( this.m_animators[i] ) >= 0 ){
                           this.m_content.remove( this.m_animators[i] );
                        }
                        //Remove it from the list
                        this.m_animators.splice(i, 1 );
                    }
                }
            }
        },
        
        /**
         * Set the appearance of this window based on whether or not it is selected.
         * @param selected {boolean} true if the window is selected; false otherwise.
         * @param multiple {boolean} true if multiple windows can be selected; false otherwise.
         */
        setSelected : function(selected, multiple) {
            this.updateCmds();
            arguments.callee.base.apply(this, arguments, selected, multiple );
        },
        
        
        /**
         * Adds or removes a specific animator from the display.
         * @param animators {Array} - list of currently supported animators.
         */
        _showHideAnimation : function( animators ) {
            if ( this.m_animators === null ){
                this.m_animators = [];
            }
            if ( this.m_identifier === null || this.m_identifier.length === 0 ){
                return;
            }
            
            //Go through the passed in animators from the server and decide if they
            //should be visible.
            for ( var i = 0; i < animators.length; i++ ){
                var animId = animators[i].type;
                var visible = animators[i].visible;
                var animator = this._getAnimator( animId );
                if ( animator != null ){
                    var contentIndex = this.m_content.indexOf( animator );
                    if ( visible ){
                        if ( contentIndex < 0 ){
                            this.m_content.add( animator );
                        }
                    }
                    else {
                        if ( contentIndex >= 0 ){
                            this.m_content.remove( animator );
                        }
                    }
                }
            }
        },
        
        
        /**
         * Update the commands about which animation is visible..
         */
        updateCmds : function(){
            var animAllCmd = skel.Command.Animate.CommandAnimations.getInstance();
            var supportedAnims = this._getSupportedAnimators();
            animAllCmd.setAnimations( supportedAnims );
            if ( this.m_animators !== null ){
                for (var i = 0; i < this.m_animators.length; i++) {
                    var animId = this.m_animators[i].getTitle();
                    var animCmd = animAllCmd.getCmd( animId );
                    var visible = this.isVisible( animId );
                    animCmd.setValue( visible );
                    supportedAnims.push( animId );
                }
            }
        },
        
        /**
         * Show/hide animators based on server information.
         * @param animObj {Object} server side information about the animators that
         *      are visible.
         */
        windowSharedVarUpdate : function( animObj ){
            this._removeUnsupportedAnimations( animObj.animators );
            this._addSupportedAnimations( animObj.animators );
            this._showHideAnimation( animObj.animators );
            this.updateCmds();
        },
        
        //Tape deck widgets
        m_animators : null
    }

});
