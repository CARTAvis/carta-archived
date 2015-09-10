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
            //Initialize the supported animations
            this.m_supportedAnimations = [];
            if ( this.m_animators === null ){
                this.m_animators = [];
            }
          
            for (var i = 0; i < animators.length; i++ ){
                var animId = animators[i].type;
                this.m_supportedAnimations.push( animId );
                if (this.m_animators.length <= i || this.m_animators[i].getTitle() != animId ) {
                    this.m_animators[i] = new skel.boundWidgets.Animator(animId, this.m_identifier);
                    this.m_animators[i].addListener( "movieStart", this._movieStarted, this );
                    this.m_animators[i].addListener( "movieStop", this._movieStopped, this );
                }
            }
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
            if ( this.m_supportedAnimations !== null ){
                animCmd.setAnimations( this.m_supportedAnimations );
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
                var animIndex = -1;
                for ( var i = 0; i < this.m_animators.length; i++ ){
                    if ( this.m_animators[i].getTitle() == animId ){
                        animIndex = i;
                        break;
                    }
                }
                
                if ( this.m_content.indexOf( this.m_animators[animIndex] ) >= 0 ){
                    visible = true;
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
            if ( this.m_supportedAnimations !== null ){
                for ( var i = this.m_supportedAnimations.length-1; i >= 0; i-- ){
                    var animIndex = -1;
                    for ( var j = 0; j < animators.length; j++ ){
                        if ( animators[j] == this.m_supportedAnimations[i] ){
                            animIndex = j;
                            break;
                        }
                    }
                   
                    if ( animIndex >= 0 ){
                        if ( animIndex < this.m_animators.length ){
                            if ( this.m_content.indexOf( this.m_animators[i] ) >= 0 ){
                                this.m_content.remove( this.m_animators[i] );
                            }
                        }
                        this.m_supportedAnimations.splice( i, 1 );
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
            if ( this.m_supportedAnimations === null ){
                return;
            }
            if ( this.m_identifier === null || this.m_identifier.length === 0 ){
                return;
            }
            
            //Go through the passed in animators from the server and decide if they
            //should be visible.
            for ( var i = 0; i < animators.length; i++ ){
                var animId = animators[i].type;
                var visible = animators[i].visible;
                var contentIndex = this.m_content.indexOf( this.m_animators[i] );
                if ( visible ){
                    if ( contentIndex < 0 ){
                        this.m_content.add( this.m_animators[i] );
                    }
                }
                else {
                    if ( contentIndex >= 0 ){
                        this.m_content.remove( this.m_animators[i] );
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
                animAllCmd.setAnimations( this.m_supportedAnimations );
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
            this._removeUnsupportedAnimations( animObj.animators );
            this._addSupportedAnimations( animObj.animators );
            this._showHideAnimation( animObj.animators );
            this.updateCmds();
        },
        
        //List of all animators that are available
        m_supportedAnimations : null,
        
        //Tape deck widgets
        m_animators : null
    }

});
